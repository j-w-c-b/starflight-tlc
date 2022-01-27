/*
        STARFLIGHT - THE LOST COLONY
        ModuleSettings.cpp
        Author: Keith Patch
        Date: October 2008
*/
#include "ModuleSettings.h"
#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Script.h"
#include "Util.h"
#include <allegro5/allegro.h>
#include <fstream>
#include <sstream>

#include "settings_resources.h"

using namespace std;
using namespace settings_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleSettings")

std::string old_resolution = "";

ModuleSettings::ModuleSettings() : m_resources(SETTINGS_IMAGES) {
    btn_exit = NULL;
    btn_fullscreen = NULL;
    btn_defaults = NULL;
    btn_save = NULL;
    for (int i = 0; i < 11; i++)
        btn_controls[i] = NULL;
}

ModuleSettings::~ModuleSettings() {}

bool
ModuleSettings::on_init() {
    ALLEGRO_DEBUG("  ModuleSettings Initialize\n");

    button_selected = 0;

    g_game->audioSystem->Load("data/cantina/buttonclick.ogg", "click");

    // exit button
    btn_exit = new Button(
        m_resources[I_BUTTON1],
        m_resources[I_BUTTON],
        NULL,
        10,
        SCREEN_HEIGHT - al_get_bitmap_height(m_resources[I_BUTTON1]) - 10,
        0,
        EVENT_SETTINGS_EXIT,
        g_game->font22,
        "EXIT",
        GOLD,
        "click",
        true,
        true);
    if (!btn_exit)
        return false;
    if (!btn_exit->IsInitialized())
        return false;

    // save button
    btn_save = new Button(
        m_resources[I_BUTTON1],
        m_resources[I_BUTTON],
        NULL,
        140,
        SCREEN_HEIGHT - al_get_bitmap_height(m_resources[I_BUTTON1]) - 10,
        0,
        EVENT_SETTINGS_SAVE,
        g_game->font22,
        "SAVE",
        GOLD,
        "click",
        true,
        true);
    if (!btn_save)
        return false;
    if (!btn_save->IsInitialized())
        return false;

    // key name buttons
    int x, y;
    for (int i = 0; i < 11; i++) {
        x = 680;
        y = 110 + i * 40;
        btn_controls[i] = new Button(
            m_resources[I_BUTTON1],
            m_resources[I_BUTTON],
            NULL,
            x,
            y,
            EVENT_NONE,
            EVENT_NONE,
            g_game->font22,
            "",
            GOLD,
            "click",
            true,
            true);
        if (!btn_controls[i])
            return false;
        if (!btn_controls[i]->IsInitialized())
            return false;
    }

    btn_controls[0]->SetButtonText("UP or W");
    btn_controls[1]->SetButtonText("LEFT or A");
    btn_controls[2]->SetButtonText("RIGHT or D");
    btn_controls[3]->SetButtonText("DOWN or S");
    btn_controls[4]->SetButtonText("Q");
    btn_controls[5]->SetButtonText("E");
    btn_controls[6]->SetButtonText("ALT or X");
    btn_controls[7]->SetButtonText("CTRL or Z");
    btn_controls[8]->SetButtonText("PGUP");
    btn_controls[9]->SetButtonText("PGDN");
    btn_controls[10]->SetButtonText("F1-F7");

    // create the resolution list scrollbox
    resScrollbox = new ScrollBox::ScrollBox(
        g_game->font18,
        ScrollBox::SB_LIST,
        40,
        100,
        200,
        230,
        EVENT_SETTINGS_CHANGE_RESOLUTION);
    resScrollbox->setLines(g_game->videomodes.size());
    resScrollbox->DrawScrollBar(true);
    resScrollbox->SetColorBackground(al_map_rgb(30, 30, 30));
    resScrollbox->SetColorItemBorder(al_map_rgb(40, 40, 40));
    resScrollbox->SetColorControls(al_map_rgb(130, 130, 130));
    resScrollbox->PaintNormalImage();
    resScrollbox->SetColorHover(al_map_rgb(160, 160, 160));
    resScrollbox->PaintHoverImage();
    resScrollbox->SetColorSelectedBackground(al_map_rgb(80, 80, 160));
    resScrollbox->SetColorSelectedHighlight(al_map_rgb(160, 160, 255));
    resScrollbox->PaintSelectedImage();

    chosenResolution = g_game->getGlobalString("RESOLUTION");

    for (auto mode = g_game->videomodes.begin();
         mode != g_game->videomodes.end();
         ++mode) {
        std::ostringstream os;
        os << mode->width << " x " << mode->height;
        ScrollBox::ColoredString item;
        item.Color = WHITE;
        item.String = os.str();
        resScrollbox->Write(item);
    }

    // create fullscreen toggle
    btn_fullscreen = new Button(
        m_resources[I_BUTTON32_NORMAL],
        m_resources[I_BUTTON32_OVER],
        NULL,
        40,
        360,
        0,
        EVENT_SETTINGS_TOGGLE_FULLSCREEN,
        g_game->font24,
        "",
        GOLD,
        "click",
        true,
        true);
    if (!btn_fullscreen)
        return false;
    if (!btn_fullscreen->IsInitialized())
        return false;

    string fullscreen = "";
    if (g_game->getGlobalBoolean("FULLSCREEN"))
        fullscreen = "X";
    btn_fullscreen->SetButtonText(fullscreen);

    return true;
}

bool
ModuleSettings::on_close() {
    ALLEGRO_DEBUG("*** ModuleSettings closing\n");
    if (btn_fullscreen != NULL) {
        btn_fullscreen->Destroy();
        btn_fullscreen = NULL;
    }
    if (btn_exit != NULL) {
        btn_exit->Destroy();
        btn_exit = NULL;
    }
    for (int i = 0; i < 10; i++) {
        if (btn_controls[i] != NULL) {
            btn_controls[i]->Destroy();
            btn_controls[i] = NULL;
        }
    }
    if (btn_save != NULL) {
        btn_save->Destroy();
        btn_save = NULL;
    }
    if (btn_fullscreen) {
        btn_fullscreen->Destroy();
        btn_fullscreen = NULL;
    }
    return true;
}

bool
ModuleSettings::on_draw(ALLEGRO_BITMAP *target) {
    // draw background
    al_set_target_bitmap(target);
    al_draw_bitmap(m_resources[I_BACKGROUND], 0, 0, 0);

    btn_save->Run(target);
    btn_exit->Run(target);

    // Show screen resolutions
    g_game->Print32(target, 40, 30, "VIDEO MODES");
    g_game->Print24(target, 40, 70, "MODE: " + chosenResolution);
    resScrollbox->SetX(40);
    resScrollbox->SetY(110);
    resScrollbox->Draw(target);

    // text
    g_game->Print32(target, 680, 30, "CONTROLS");
    int x = btn_controls[0]->GetX() + btn_controls[0]->GetWidth() + 10;
    int y = btn_controls[0]->GetY() + 5;

    g_game->Print20(target, x, y, "Forward", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Turn left", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Turn right", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Reverse", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Strafe left", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Strafe right", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Fire laser or stunner", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Fire missile", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Toggle shield", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Toggle weapons", WHITE);
    y += 40;
    g_game->Print20(target, x, y, "Select bridge station", WHITE);

    // draw buttons for each command
    for (int i = 0; i < 11; i++) {
        btn_controls[i]->Run(target);
    }

    // draw fullscreen toggle
    btn_fullscreen->Run(target);
    x = btn_fullscreen->GetX() + 60;
    y = btn_fullscreen->GetY();
    g_game->Print24(target, x, y, "FULLSCREEN", WHITE);

    return true;
}

#pragma region INPUT

bool
ModuleSettings::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ESCAPE) {
        g_game->LoadModule(MODULE_TITLESCREEN);
        return false;
    }
    return true;
}

bool
ModuleSettings::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    resScrollbox->OnMouseMove(x, y);
    btn_exit->OnMouseMove(x, y);
    btn_save->OnMouseMove(x, y);
    btn_fullscreen->OnMouseMove(x, y);

    if (is_mouse_wheel_down(event)) {
        resScrollbox->OnMouseWheelDown(x, y);
    } else if (is_mouse_wheel_up(event)) {
        resScrollbox->OnMouseWheelUp(x, y);
    }
    return true;
}

bool
ModuleSettings::on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    resScrollbox->OnMousePressed(button, x, y);

    return true;
}

bool
ModuleSettings::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    resScrollbox->OnMouseReleased(button, x, y);
    btn_exit->OnMouseReleased(button, x, y);
    btn_save->OnMouseReleased(button, x, y);
    btn_fullscreen->OnMouseReleased(button, x, y);

    if (is_mouse_click(event)) {
        resScrollbox->OnMouseClick(button, x, y);
    }
    return true;
}

#pragma endregion

// Save configuration settings back INTO the Config.lua file
bool
ModuleSettings::SaveConfigurationFile() {
    string audio_global = "false";
    if (g_game->getGlobalBoolean("AUDIO_GLOBAL"))
        audio_global = "true";

    string audio_music = "false";
    if (g_game->getGlobalBoolean("AUDIO_MUSIC"))
        audio_music = "true";

    string fullscreen = "false";
    if (btn_fullscreen->GetButtonText() == "X")
        fullscreen = "true";

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
    configfile << "RESOLUTION = \"" << chosenResolution << "\"" << std::endl;
    configfile << "FULLSCREEN = " << fullscreen << std::endl;
    configfile << "AUDIO_GLOBAL = " << audio_global << std::endl;
    configfile << "AUDIO_MUSIC = " << audio_music << std::endl;
    configfile.close();

    // use new video mode
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;
    g_game->ShowMessageBoxWindow(
        "SETTINGS SAVED",
        "Please restart the program for changes to take effect...",
        400,
        250,
        WHITE,
        cx,
        cy,
        true);

    return true;
}

bool
ModuleSettings::on_event(ALLEGRO_EVENT *event) {
    string text;
    switch (event->type) {
    case EVENT_SETTINGS_CHANGE_RESOLUTION:
        if (resScrollbox->GetSelectedItem().length() > 0) {
            old_resolution = chosenResolution;
            chosenResolution = resScrollbox->GetSelectedItem();
            ALLEGRO_DEBUG("Resolution change: %s\n", chosenResolution.c_str());
        }
        break;

    case EVENT_SETTINGS_TOGGLE_FULLSCREEN:
        text = btn_fullscreen->GetButtonText();
        if (text == "X")
            text = "";
        else
            text = "X";
        btn_fullscreen->SetButtonText(text);
        break;

    case EVENT_SETTINGS_SAVE:
        // save resolution
        if (chosenResolution != old_resolution) {
            old_resolution = chosenResolution;
            SaveConfigurationFile();
        }
        break;

    case EVENT_SETTINGS_EXIT:
        g_game->LoadModule(MODULE_TITLESCREEN);
        break;
    }

    return true;
}
