/*
        STARFLIGHT - THE LOST COLONY
        ModeMgr.cpp - ?
        Author: ?
        Date: ?
*/

#include <allegro5/allegro.h>

#include "AudioSystem.h"
#include "Game.h"
#include "GameState.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "Module.h"
#include "PauseMenu.h"
#include "Util.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("ModeMgr")

Mode::Mode(shared_ptr<Module> module, const string &path)
    : rootModule(module), musicPath(path) {}

ModeMgr::ModeMgr(Game * /* game*/)
    : m_activeRootModule(nullptr), currentMusic(nullptr), prevModeName(""),
      currentModeName(""), m_pause_menu(make_shared<PauseMenu>()),
      m_previous_modal(nullptr) {
    m_pause_menu->set_active(false);
}

ModeMgr::~ModeMgr() {
    ALLEGRO_DEBUG("[DESTROYING MODULES]\n");
    m_modes.clear();
}

void
ModeMgr::AddMode(
    const string &modeName,
    std::shared_ptr<Module> rootModule,
    const string &musicPath) {
    if (musicPath.compare("") != 0) {
        ALLEGRO_FS_ENTRY *entry =
            al_create_fs_entry(Util::resource_path(musicPath).c_str());
        if (!al_fs_entry_exists(entry)) {
            std::string error = "ModeMgr::AddMode: [ERROR] file " + musicPath
                                + " does not exist";
            g_game->fatalerror(error);
        }
        al_destroy_fs_entry(entry);
    }

    shared_ptr<Mode> newmode = make_shared<Mode>(rootModule, musicPath);
    m_modes[modeName] = newmode;
    rootModule->set_active(false);
}

void
ModeMgr::CloseCurrentModule() {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return;

    close();
    m_activeRootModule->set_active(false);
    remove_child_module(m_activeRootModule);
    m_activeRootModule = nullptr;
}

bool
ModeMgr::LoadModule(const string &newModeName) {
    bool result = false;

    // disable the Pause Menu
    m_pause_menu->set_enabled(false);

    // search the current and new modes in the m_modes associative array
    auto icurr = m_modes.find(currentModeName);
    auto inew = m_modes.find(newModeName);

    if (inew == m_modes.end()) {
        g_game->message("Error '" + newModeName + "' is not a valid mode name");
        return false;
    }

    // save module name
    prevModeName = this->currentModeName;
    currentModeName = newModeName;

    // store module name in gamestate
    g_game->gameState->setCurrentModule(newModeName);

    // the following will always be true except exactly once, at game start
    if (icurr != m_modes.end()) {
        // close active module
        ALLEGRO_DEBUG("ModeMgr: closing module '%s'\n", prevModeName.c_str());
        CloseCurrentModule();
        ALLEGRO_DEBUG("ModeMgr: module '%s' closed\n\n", prevModeName.c_str());
    }

    // launch new module
    ALLEGRO_DEBUG("ModeMgr: initializing module '%s'\n", newModeName.c_str());
    m_activeRootModule = inew->second->rootModule;
    result = m_activeRootModule->init();
    ALLEGRO_DEBUG(
        "ModeMgr: module '%s' Init(): %s\n",
        newModeName.c_str(),
        result ? "SUCCESS" : "FAILURE");

    if (!result)
        return false;
    m_activeRootModule->set_active(true);
    add_child_module(m_activeRootModule);

    // handle background music
    // if we don't want music, we are done
    if (!g_game->getGlobalBoolean("AUDIO_MUSIC"))
        return true;

    std::string currentMusicPath =
        (icurr == m_modes.end()) ? "" : icurr->second->musicPath;
    std::string newMusicPath = inew->second->musicPath;

    // if new music == current music, we do nothing (iow: we let it play)
    if (newMusicPath.compare(currentMusicPath) == 0)
        return true;
    // if new music == "", we do nothing either (iow: we let the new module
    // handle it all by itself)
    if (newMusicPath.compare("") == 0)
        return true;

    // stop the current music, unless we were told not to deal with it.
    if (currentMusicPath.compare("") != 0 && currentMusic != nullptr) {
        ALLEGRO_DEBUG(
            "ModeMgr: stop playing music %s\n", currentMusicPath.c_str());
        g_game->audioSystem->Stop(currentMusic);
        currentMusic.reset();
    }

    ALLEGRO_DEBUG("ModeMgr: start playing music %s\n", newMusicPath.c_str());
    currentMusic = g_game->audioSystem->LoadMusic(newMusicPath);
    if (currentMusic == nullptr) {
        g_game->message("Error loading music from " + newMusicPath);
        return false;
    }

    if (!g_game->audioSystem->PlayMusic(currentMusic)) {
        g_game->message("Error playing music from " + newMusicPath);
        return false;
    }

    return true;
}

bool
ModeMgr::on_event(ALLEGRO_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());

    if (event->type == EVENT_NONE) {
        return false;
    }

    // determine if messagebox or pausemenu need to be removed
    // These are events related to these pop-up controls
    // EVENT_CLOSE is always a close event for the control
    int evtype = event->type;
    switch (evtype) {
    case EVENT_CLOSE:
    case EVENT_SAVE_GAME:
    case EVENT_LOAD_GAME:
    case EVENT_QUIT_GAME:
        // hide the pause menu
        if (m_pause_menu->get_active())
            toggle_pause_menu();

        /* // hide the messagebox */
        /* if (m_message_box != nullptr) */
        /*     m_messageBox->set_active(false); */
    }

    if (m_activeRootModule == nullptr)
        return false;

    // if this is not a close event, pass it on
    if (evtype == EVENT_CLOSE)
        return false;

    return true;
}

bool
ModeMgr::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());

    if (m_pause_menu->is_enabled() && event->keycode == ALLEGRO_KEY_ESCAPE) {
        toggle_pause_menu();
        return false;
    } else {
        return true;
    }
}

void
ModeMgr::enable_pause_menu(bool enable) {
    m_pause_menu->set_enabled(enable);
}

void
ModeMgr::toggle_pause_menu() {
    if (!m_pause_menu->is_enabled())
        return;

    if (m_pause_menu->get_active()) {
        g_game->SetTimePaused(m_originally_paused);
        // hide pausemenu
        g_game->set_pause(false);
        m_pause_menu->set_active(false);
        m_activeRootModule->set_modal_child(m_previous_modal);
        m_previous_modal = nullptr;
    } else {
        m_originally_paused = g_game->getTimePaused();
        g_game->SetTimePaused(true);
        // show pausemenu
        g_game->set_pause(true);
        m_pause_menu->set_active(true);
        m_previous_modal = m_activeRootModule->set_modal_child(m_pause_menu);
    }
}
