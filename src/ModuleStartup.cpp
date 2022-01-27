/*
        STARFLIGHT - THE LOST COLONY
        ModuleStartup.cpp - Handles opening sequences, videos, copyrights, prior
   to titlescreen. The purpose of this module is to free up resources and reduce
   the logic in titlescreen which was having to deal with the startup sequence.
   This is just easier. Author: J.Harbour Date: Jan,2008
*/

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "ModeMgr.h"
#include "ModuleStartup.h"
#include "Util.h"
#include "startup_resources.h"

using namespace startup_resources;

void
showOpeningStory(int page);
int storypage = 0;

ALLEGRO_DEBUG_CHANNEL("ModuleStartup")

ModuleStartup::ModuleStartup() : m_resources(STARTUP_IMAGES) {
    display_mode = 3;
}

ModuleStartup::~ModuleStartup() {}

bool
ModuleStartup::on_close() {
    m_resources.unload();
    return true;
}

int
ModuleStartup::fadein(ALLEGRO_BITMAP *dest, ALLEGRO_BITMAP *source, int speed) {
    int retval = 0;
    static int loop = 0;

    al_set_target_bitmap(dest);
    if (loop < 256 - speed) {
        loop += speed;
        al_draw_filled_rectangle(
            0,
            0,
            al_get_bitmap_width(source),
            al_get_bitmap_height(source),
            al_map_rgba(0, 0, 0, 255 - loop));
    } else {
        loop = 0;
        retval = 1;
    }

    return retval;
}

int
ModuleStartup::fadeout(
    ALLEGRO_BITMAP *dest,
    ALLEGRO_BITMAP *source,
    int speed) {
    int retval = 0;
    static int loop = 255;

    al_set_target_bitmap(dest);
    if (loop > speed) {
        loop -= speed;
        al_draw_filled_rectangle(
            0,
            0,
            al_get_bitmap_width(source),
            al_get_bitmap_height(source),
            al_map_rgba(0, 0, 0, loop));
    } else {
        al_draw_filled_rectangle(
            0,
            0,
            al_get_bitmap_width(source),
            al_get_bitmap_height(source),
            al_map_rgba(0, 0, 0, 255));
        loop = 255;
        retval = 1;
    }

    return retval;
}

bool
ModuleStartup::on_draw(ALLEGRO_BITMAP *target) {
    static bool title_done = false;
    al_set_target_bitmap(target);

    al_draw_bitmap(m_resources[I_SPACE_1280], 0, 0, 0);

    switch (display_mode) {

    case 0: // initial blank period to slow down the intro
        if (Util::ReentrantDelay(4000))
            display_mode = 1;
        break;

    case 1: // copyright fadein
        if (!title_done) {
            if (fadein(target, m_resources[I_STARTUP_COPYRIGHTS], 1)) {
                title_done = true;
            }

        } else {
            al_draw_bitmap(m_resources[I_STARTUP_COPYRIGHTS], 0, 0, 0);
            if (Util::ReentrantDelay(4000))
                display_mode = 2;
        }
        break;

    case 2: // copyright fadeout
        title_done = false;
        if (fadeout(target, m_resources[I_STARTUP_COPYRIGHTS], 2)) {
            display_mode = 3;
        }
        break;

    case 3: // opening story
        showOpeningStory(storypage);
        break;

    case 100: // done, transition to TitleScreen
        if (Util::ReentrantDelay(1000)) {
            g_game->LoadModule(MODULE_TITLESCREEN);
            return false;
        }
        break;
    }
    return true;
}

bool
ModuleStartup::on_key_pressed(ALLEGRO_KEYBOARD_EVENT * /*event*/) {
    switch (display_mode) {
    // pressing any key will fast forward the slideshow
    case 1: // fade in copyright
        display_mode = 2;
        break;

    case 2: // fade out copyright
        display_mode = 3;
        break;

    case 3: // opening story
        storypage++;
        if (storypage > 4)
            display_mode = 100;
        break;

    case 100: // done
        break;
    }
    return true;
}

void
showOpeningStory(int page) {
    int y = 50, x = 100, spacing = 28;
    for (int j = 0; j < lines; j++) {
        g_game->Print24(g_game->GetBackBuffer(), x, y, story[page][j]);
        y += spacing;
    }
}
