/*
        STARFLIGHT - THE LOST COLONY
        ModuleSettings.cpp
        Author: Keith Patch
        Date: October 2008
*/
#include <fstream>
#include <sstream>

#include <allegro5/allegro.h>

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleSettings.h"
#include "Script.h"
#include "Util.h"

#include "settings_resources.h"

using namespace std;
using namespace settings;

ALLEGRO_DEBUG_CHANNEL("ModuleSettings")

VideoModeSelector::VideoModeSelector(
    int x,
    int y,
    int width,
    int height,
    VideoMode video_mode)
    : Module(x, y, width, height) {
    m_video_mode = video_mode;
    int line_height = al_get_font_line_height(g_game->font22.get());
    int space = al_get_text_width(g_game->font22.get(), " ");

    m_title = make_shared<Label>(
        "Available Video Modes:",
        x + space,
        y,
        width - space,
        line_height,
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font22,
        YELLOW);
    add_child_module(m_title);

    m_video_modes = make_shared<ScrolledModule<Module>>(
        x + space,
        y + line_height,
        width - 16 - space,
        height - 3 * line_height,
        line_height,
        al_map_rgb(40, 40, 40),
        al_map_rgb(30, 30, 30),
        al_map_rgb(40, 40, 40),
        al_map_rgb(160, 160, 160),
        al_map_rgb(30, 30, 30));
    add_child_module(m_video_modes);

    m_current_label = make_shared<Label>(
        "Current: " + to_string(m_video_mode),
        x + space,
        y + height - line_height,
        width - space,
        line_height,
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font22,
        WHITE);
    add_child_module(m_current_label);
}

bool
VideoModeSelector::on_init() {
    ALLEGRO_FONT *f = g_game->font22.get();
    int space = al_get_text_width(f, " ");
    int x = get_x() + space;
    int w = get_width() - 2 * space;
    int lineh = al_get_font_line_height(f);
    int y = get_y() + lineh;
    int y_offset = 0;

    auto background = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(w, lineh), al_destroy_bitmap);

    al_set_target_bitmap(background.get());
    al_clear_to_color(al_map_rgb(30, 30, 30));

    auto highlight = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(w, lineh), al_destroy_bitmap);
    al_set_target_bitmap(highlight.get());
    al_clear_to_color(al_map_rgb(64, 64, 64));

    for (auto &mode : g_game->get_video_modes()) {
        auto b = make_shared<TextButton>(
            to_string(mode),
            g_game->font22,
            WHITE,
            ALLEGRO_ALIGN_LEFT,
            x,
            y + y_offset,
            EVENT_NONE,
            EVENT_SETTINGS_VIDEO_MODE_CLICK,
            background,
            highlight);
        m_video_mode_map[b->get_id()] = mode;
        m_video_modes->add_child_module(b);
        y_offset += lineh;
        if (mode == m_video_mode) {
            m_mode_id = b->get_id();
            b->set_highlight(true);
        }
    }
    m_video_modes->resize(m_video_modes->get_width(), y_offset);
    return true;
}

bool
VideoModeSelector::on_event(ALLEGRO_EVENT *event) {
    EventType t = static_cast<EventType>(event->type);

    switch (t) {
    case EVENT_SETTINGS_VIDEO_MODE_CLICK:
        {
            if (m_mode_id) {
                auto old_highlight = dynamic_pointer_cast<TextButton>(
                    m_video_modes->get_child_module(*m_mode_id));
                if (old_highlight) {
                    old_highlight->set_highlight(false);
                }
            }
            m_mode_id = static_cast<int>(event->user.data1);
            m_video_mode = m_video_mode_map[*m_mode_id];
            auto new_highlight = dynamic_pointer_cast<TextButton>(
                m_video_modes->get_child_module(*m_mode_id));
            if (new_highlight) {
                new_highlight->set_highlight(true);
            }

            ALLEGRO_EVENT e = make_event(EVENT_SETTINGS_CHANGE_RESOLUTION);
            g_game->broadcast_event(&e);
            break;
        }
    default:
        break;
    }
    return true;
}

bool
VideoModeSelector::on_close() {
    m_video_modes->clear_child_modules();
    m_video_mode_map.clear();
    return true;
}

ModuleSettings::ModuleSettings() {
    m_background = make_shared<Bitmap>(images[I_BACKGROUND]);
    add_child_module(m_background);

    // exit button
    m_exit_button = make_shared<TextButton>(
        "EXIT",
        g_game->font22,
        GOLD,
        ALLEGRO_ALIGN_CENTER,
        10,
        SCREEN_HEIGHT - al_get_bitmap_height(images[I_BUTTON1].get()) - 10,
        EVENT_NONE,
        EVENT_SETTINGS_EXIT,
        images[I_BUTTON1],
        images[I_BUTTON],
        nullptr,
        samples[S_BUTTONCLICK]);
    add_child_module(m_exit_button);

    // save button
    m_save_button = make_shared<TextButton>(
        "SAVE",
        g_game->font22,
        GOLD,
        ALLEGRO_ALIGN_CENTER,
        140,
        SCREEN_HEIGHT - al_get_bitmap_height(images[I_BUTTON1].get()) - 10,
        EVENT_NONE,
        EVENT_SETTINGS_SAVE,
        images[I_BUTTON1],
        images[I_BUTTON],
        nullptr,
        samples[S_BUTTONCLICK]);
    add_child_module(m_save_button);

    // key name labels
    m_control_keys = make_shared<Label>(
        "UP or W\n"
        "LEFT or A\n"
        "RIGHT or D\n"
        "DOWN or S\n"
        "Q\n"
        "E\n"
        "ALT or X\n"
        "CTRL or Z\n"
        "PGUP\n"
        "PGDN\n"
        "F1-F7\n",
        580,
        110,
        150,
        11 * al_get_font_line_height(g_game->font22.get()),
        true,
        ALLEGRO_ALIGN_CENTER,
        g_game->font22,
        GOLD);
    add_child_module(m_control_keys);

    m_control_effect = make_shared<Label>(
        "Forward\n"
        "Turn left\n"
        "Turn right\n"
        "Reverse\n"
        "Strafe left\n"
        "Strafe right\n"
        "Fire laser or stunner\n"
        "Fire missile\n"
        "Toggle shield\n"
        "Toggle weapons\n"
        "Select bridge station\n",
        740,
        110,
        250,
        11 * al_get_font_line_height(g_game->font22.get()),
        true,
        ALLEGRO_ALIGN_LEFT,
        g_game->font22,
        GOLD);
    add_child_module(m_control_effect);

    // create the resolution list scrollbox
    m_video_mode_selector = make_shared<VideoModeSelector>(
        40, 100, 400, 230, g_game->get_video_mode());
    add_child_module(m_video_mode_selector);

    // create fullscreen toggle
    m_fullscreen_button = make_shared<CheckBox>(
        40,
        360,
        400,
        "Fullscreen",
        g_game->font24,
        GOLD,
        EVENT_SETTINGS_TOGGLE_FULLSCREEN);
    add_child_module(m_fullscreen_button);

    // create sound toggle
    m_enable_sound = make_shared<CheckBox>(
        40,
        400,
        400,
        "Enable Sound Effects",
        g_game->font24,
        GOLD,
        EVENT_SETTINGS_TOGGLE_SOUND);
    add_child_module(m_enable_sound);

    // create music toggle
    m_enable_music = make_shared<CheckBox>(
        40,
        440,
        400,
        "Enable Music",
        g_game->font24,
        GOLD,
        EVENT_SETTINGS_TOGGLE_MUSIC);
    add_child_module(m_enable_music);
}

ModuleSettings::~ModuleSettings() {}

bool
ModuleSettings::on_init() {
    ALLEGRO_DEBUG("  ModuleSettings Initialize\n");

    m_fullscreen_button->set_checked(g_game->getGlobalBoolean("FULLSCREEN"));
    m_enable_sound->set_checked(g_game->getGlobalBoolean("AUDIO_GLOBAL"));
    m_enable_music->set_checked(g_game->getGlobalBoolean("AUDIO_MUSIC"));

    return true;
}

bool
ModuleSettings::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    // Show screen resolutions
    g_game->Print32(target, 40, 30, "VIDEO MODES");

    // text
    g_game->Print32(target, 680, 30, "CONTROLS");

    return true;
}

bool
ModuleSettings::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ESCAPE) {
        g_game->LoadModule(MODULE_TITLESCREEN);
        return false;
    }
    return true;
}

// Save configuration settings back INTO the Config.lua file
bool
ModuleSettings::SaveConfigurationFile() {
    string audio_global = m_enable_sound->get_checked() ? "true " : "false";
    string audio_music = m_enable_music->get_checked() ? "true " : "false";
    string fullscreen = m_fullscreen_button->get_checked() ? "true" : "false";

    std::ofstream configfile;
    configfile.open(
        "data/config.lua", std::ofstream::out | std::ofstream::trunc);
    if (!configfile.is_open()) {
        ALLEGRO_DEBUG("ModuleSettings: error opening config.lua\n");
        return false;
    }

    configfile << "--CONFIGURATION SETTINGS" << std::endl;
    configfile << "--This file is overwritten by the settings screen but can "
                  "be manually edited."
               << std::endl;
    configfile << std::endl;
    configfile << "RESOLUTION = \""
               << to_string(m_video_mode_selector->get_video_mode()) << "\""
               << std::endl;
    configfile << "FULLSCREEN = " << fullscreen << std::endl;
    configfile << "AUDIO_GLOBAL = " << audio_global << std::endl;
    configfile << "AUDIO_MUSIC = " << audio_music << std::endl;
    configfile.close();

    // use new video mode
    g_game->setGlobalString(
        "RESOLUTION", to_string(m_video_mode_selector->get_video_mode()));
    g_game->setGlobalBoolean("FULLSCREEN", m_fullscreen_button->get_checked());
    g_game->setGlobalBoolean("AUDIO_GLOBAL", m_enable_sound->get_checked());
    g_game->setGlobalBoolean("AUDIO_MUSIC", m_enable_music->get_checked());

    g_game->Initialize_Graphics();

    if (!m_enable_music->get_checked()) {
        g_game->audioSystem->StopMusic();
    } else {
        g_game->audioSystem->StartMusic();
    }
    return true;
}

bool
ModuleSettings::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_CLOSE:
        set_modal_child(nullptr);
        break;

    case EVENT_SETTINGS_CHANGE_RESOLUTION:
        break;

    case EVENT_SETTINGS_SAVE:
        SaveConfigurationFile();
        break;

    case EVENT_SETTINGS_EXIT:
        g_game->LoadModule(MODULE_TITLESCREEN);
        break;
    }

    return true;
}
// vi: ft=cpp
