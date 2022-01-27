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

Mode::Mode(Module *module, const string &path)
    : rootModule(module), musicPath(path) {}

ModeMgr::ModeMgr(Game * /* game*/)
    : m_activeRootModule(nullptr), currentMusic(nullptr), prevModeName(""),
      currentModeName("") {}

ModeMgr::~ModeMgr() {
    ALLEGRO_DEBUG("[DESTROYING MODULES]\n");
    map<string, Mode *>::iterator i;
    i = m_modes.begin();

    bool isOperationsRoom,
        operationsRoomDeleted =
            false; // needed!! (cannot delete same object 3x)
    while (i != m_modes.end()) {
        if (i->first.length() > 0) {
            if ((strcmp(i->first.c_str(), "CANTINA") == 0)
                || (strcmp(i->first.c_str(), "RESEARCHLAB") == 0)
                || (strcmp(i->first.c_str(), "MILITARYOPS") == 0))
                isOperationsRoom = true;
            else
                isOperationsRoom = false;

            if ((!isOperationsRoom) || (!operationsRoomDeleted)) {
                if (isOperationsRoom)
                    operationsRoomDeleted = true;
                ALLEGRO_DEBUG("  Destroying %s\n", i->first.c_str());
                delete i->second;
            } else {
                ALLEGRO_DEBUG(
                    "  Module %s was previously deleted (object "
                    "assigned 3x)\n",
                    i->first.c_str());
            }
        }
        ++i;
    }
}

void
ModeMgr::AddMode(
    const string &modeName,
    Module *rootModule,
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

    Mode *newmode = new Mode(rootModule, musicPath);
    m_modes[modeName] = newmode;
}

void
ModeMgr::CloseCurrentModule() {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return;

    m_activeRootModule->close();
    m_activeRootModule = nullptr;
}

bool
ModeMgr::LoadModule(const string &newModeName) {
    bool result = false;

    // disable the Pause Menu
    g_game->pauseMenu->setEnabled(false);

    // search the current and new modes in the m_modes associative array
    map<string, Mode *>::iterator icurr = m_modes.find(currentModeName);
    map<string, Mode *>::iterator inew = m_modes.find(newModeName);

    if (inew == m_modes.end()) {
        g_game->message("Error '" + newModeName + "' is not a valid mode name");
        return false;
    }

    // save module name
    this->prevModeName = this->currentModeName;
    this->currentModeName = newModeName;

    // store module name in gamestate
    g_game->gameState->setCurrentModule(newModeName);

    // the following will always be true except exactly once, at game start
    if (icurr != m_modes.end()) {
        // close active module
        ALLEGRO_DEBUG(
            "ModeMgr: closing module '%s'\n", this->prevModeName.c_str());
        CloseCurrentModule();
        ALLEGRO_DEBUG(
            "ModeMgr: module '%s' closed\n\n", this->prevModeName.c_str());
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
ModeMgr::on_update() {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->update();
}

bool
ModeMgr::on_draw(ALLEGRO_BITMAP *target) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->draw(target);
}

bool
ModeMgr::on_close() {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->close();
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
        if (g_game->pauseMenu->isShowing())
            g_game->TogglePauseMenu();

        // hide the messagebox
        if (g_game->messageBox != nullptr)
            g_game->messageBox->set_active(false);
    }

    if (m_activeRootModule == nullptr)
        return false;

    // if this is not a close event, pass it on
    if (evtype != EVENT_CLOSE)
        m_activeRootModule->event(event);

    return true;
}

bool
ModeMgr::on_key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->key_down(event);
}

bool
ModeMgr::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->key_pressed(event);
}

bool
ModeMgr::on_key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->key_up(event);
}

bool
ModeMgr::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->mouse_move(event);
}

bool
ModeMgr::on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->mouse_button_down(event);
}

bool
ModeMgr::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
    if (m_activeRootModule == nullptr)
        return false;

    return m_activeRootModule->mouse_button_up(event);
}
