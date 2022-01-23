/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.cpp - The Starmap module.
        Author: Keith "Daikaze" Patch
        Date: ??-??-????
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModuleStarmap.h"
#include "PlayerShipSprite.h"
#include "Script.h"
#include "Util.h"
#include "starmap_resources.h"

using namespace std;
using namespace starmap_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleStarmap")

ModuleStarmap::ModuleStarmap() : resources(STARMAP_IMAGES) {
    map_active = false;
    ratioX = 0;
    ratioY = 0;
    cursorPos.y = 0;
    cursorPos.x = 0;
    dest_active = false;
    m_destPos.y = 0;
    m_destPos.x = 0;
    star_label = NULL;
    m_bOver_Star = false;
    star_x = 0;
    star_y = 0;
}

ModuleStarmap::~ModuleStarmap() {}

void
ModuleStarmap::OnKeyPressed(int keyCode) {
    Module::OnKeyPressed(keyCode);
}

void
ModuleStarmap::OnKeyPress(int keyCode) {
    Module::OnKeyPress(keyCode);
}

void
ModuleStarmap::OnKeyReleased(int keyCode) {
    Module::OnKeyReleased(keyCode);
}

void
ModuleStarmap::OnMouseMove(int x, int y) {
    Module::OnMouseMove(x, y);
    if (y > MAP_POS_Y && y < MAP_POS_Y + MAP_HEIGHT && x > MAP_POS_X &&
        x < MAP_POS_X + MAP_WIDTH) {
        cursorPos.y = (float)(y - MAP_POS_Y) / ratioY;
        cursorPos.x = (float)(x - MAP_POS_X) / ratioX;

        // if the mouse pointer is over a starsystem, we need to remember that
        // starsystem name and coordinates to display them later at Draw() time.
        Star *starSystem = NULL;
        for (int _y = -1; _y <= 1; _y++) {
            if (starSystem) {
                break;
            }
            for (int _x = -1; _x <= 1; _x++) {
                // because of the way starsystems are positionned on the starmap
                // we need +1 to the x, and +2 to y here
                starSystem = g_game->dataMgr->GetStarByLocation(
                    (int)(cursorPos.x + _x + 1), (int)(cursorPos.y - +_y + 2));
                if (starSystem) {
                    star_x = starSystem->x;
                    star_y = starSystem->y;
                    star_label->SetText(starSystem->name);
                    m_bOver_Star = true;
                    break;
                } else {
                    m_bOver_Star = false;
                }
            }
        }
    }
}

void
ModuleStarmap::OnMouseClick(int button, int x, int y) {
    Module::OnMouseClick(button, x, y);
    if (map_active) {
        if (y > MAP_POS_Y && y < MAP_POS_Y + MAP_HEIGHT && x > MAP_POS_X &&
            x < MAP_POS_X + MAP_WIDTH) {
            if (cursorPos.y > m_destPos.y - 2 &&
                cursorPos.y < m_destPos.y + 2 &&
                cursorPos.x > m_destPos.x - 2 &&
                cursorPos.x < m_destPos.x + 2 && dest_active) {
                dest_active = false;
            } else {
                m_destPos.y = (float)(y - MAP_POS_Y) / ratioY;
                m_destPos.x = (float)(x - MAP_POS_X) / ratioX;
                dest_active = true;
            }
        }
    }
}

void
ModuleStarmap::OnMousePressed(int button, int x, int y) {
    Module::OnMousePressed(button, x, y);
}

void
ModuleStarmap::OnMouseReleased(int button, int x, int y) {
    Module::OnMouseReleased(button, x, y);
}

void
ModuleStarmap::OnMouseWheelUp(int x, int y) {
    Module::OnMouseWheelUp(x, y);
}

void
ModuleStarmap::OnMouseWheelDown(int x, int y) {
    Module::OnMouseWheelDown(x, y);
}

void
ModuleStarmap::OnEvent(Event *event) {
    Module::OnEvent(event);
    switch (event->getEventType()) {
    case 3001:
        if (!map_active) {
            map_active = true;
        } else {
            map_active = false;
        }
        break;
    }
}

bool
ModuleStarmap::Init() {
    ALLEGRO_DEBUG("  ModuleStarmap Initialize\n");

    // load the datafile
    if (!resources.load()) {
        g_game->message("Starmap: Error loading resources");
        return false;
    }

    // initialize constants
    Script lua;
    lua.load("data/starmap/starmap.lua");
    FUEL_PER_UNIT = (float)lua.getGlobalNumber("FUEL_PER_UNIT");
    VIEWER_WIDTH = (int)lua.getGlobalNumber("VIEWER_WIDTH");
    VIEWER_HEIGHT = (int)lua.getGlobalNumber("VIEWER_HEIGHT");
    MAP_WIDTH = (int)lua.getGlobalNumber("MAP_WIDTH");
    MAP_HEIGHT = (int)lua.getGlobalNumber("MAP_HEIGHT");
    X_OFFSET = (int)lua.getGlobalNumber("X_OFFSET");
    Y_OFFSET = (int)lua.getGlobalNumber("Y_OFFSET");
    MAP_POS_X = (int)lua.getGlobalNumber("MAP_POS_X");
    MAP_POS_Y = (int)lua.getGlobalNumber("MAP_POS_Y");
    VIEWER_TARGET_OFFSET = (int)lua.getGlobalNumber("VIEWER_TARGET_OFFSET");
    viewer_offset_y = -VIEWER_TARGET_OFFSET;
    VIEWER_MOVE_RATE = (int)lua.getGlobalNumber("VIEWER_MOVE_RATE");

    // load starmap GUI
    gui_starmap = resources[I_STARMAP_VIEWER];

    m_bOver_Star = false;
    star_label = new Label("", 0, 0, 100, 22, ORANGE, g_game->font18);

    starview = al_create_bitmap(MAP_WIDTH, MAP_HEIGHT);
    al_set_target_bitmap(starview);
    al_clear_to_color(BLACK);

    flux_view = al_create_bitmap(MAP_WIDTH, MAP_HEIGHT);
    al_set_target_bitmap(flux_view);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    text = al_create_bitmap(VIEWER_WIDTH, VIEWER_HEIGHT);
    al_set_target_bitmap(text);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    ratioX = (float)MAP_WIDTH / 250.0f;
    ratioY = (float)MAP_HEIGHT / 220.0f;

    al_set_target_bitmap(starview);
    al_clear_to_color(BLACK);

    Sprite stars;

    stars.setImage(resources[I_IS_TILES_TRANS]);
    if (!stars.getImage()) {
        g_game->message("Starmap: Error loading stars");
        return false;
    }

    stars.setAnimColumns(8);
    stars.setTotalFrames(8);
    stars.setFrameWidth(8);
    stars.setFrameHeight(8);

    for (int i = 0; i < g_game->dataMgr->GetNumStars(); i++) {
        const Star *star = g_game->dataMgr->GetStar(i);
        // draw star image on starmap
        stars.setCurrFrame(star->spectralClass);
        stars.setX(star->x * ratioX);
        stars.setY(star->y * ratioY);
        stars.drawframe(starview);
    }
    return true;
}

void
ModuleStarmap::draw_flux() {
    al_set_target_bitmap(flux_view);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    for (int i = 0; i < MAX_FLUX; i++) {
        const Flux *f = g_game->dataMgr->GetFlux(i);
        if (!f) {
            continue;
        }
        const FluxInfo &fi = g_game->gameState->flux_info[f->get_id()];
        const Point2D &endpoint1 = f->get_endpoint1();
        const Point2D &endpoint2 = f->get_endpoint2();

        if (fi.endpoint_1_visible) {
            al_draw_bitmap(resources[I_FLUX_TILE_TRANS],
                           endpoint1.x * ratioX,
                           endpoint1.y * ratioY,
                           0);
        }
        if (fi.endpoint_2_visible) {
            al_draw_bitmap(resources[I_FLUX_TILE_TRANS],
                           endpoint2.x * ratioX,
                           endpoint2.y * ratioY,
                           0);
        }
        if (fi.path_visible) {

            al_draw_line((int)(endpoint1.x * ratioX),
                         (int)(endpoint1.y * ratioY),
                         (int)(endpoint2.x * ratioX),
                         (int)(endpoint2.y * ratioY),
                         al_map_rgb(0, 170, 255),
                         1);
        }
    }
}

void
ModuleStarmap::Close() {
    if (starview != NULL) {
        al_destroy_bitmap(starview);
        starview = NULL;
    }
    if (text != NULL) {
        al_destroy_bitmap(text);
        text = NULL;
    }

    if (flux_view != NULL) {
        al_destroy_bitmap(flux_view);
        flux_view = NULL;
    }

    if (star_label != NULL) {
        delete star_label;
        star_label = NULL;
    }

    // unload the resources
    resources.unload();
}

void
ModuleStarmap::Update() {
    Module::Update();
    if (map_active) {
        if (g_game->gameState->getCurrentSelectedOfficer() !=
            OFFICER_NAVIGATION) {
            map_active = false;
        }
    }
}

void
ModuleStarmap::Draw() {
    al_set_target_bitmap(g_game->GetBackBuffer());
    Module::Draw();
    if (viewer_offset_y > -VIEWER_TARGET_OFFSET) {
        al_draw_bitmap(gui_starmap, 120, viewer_offset_y, 0);

        // Update the flux layer, which may change as navigation
        // occurs.
        draw_flux();

#pragma region Draw Starmap
        int new_x_offset = 120 + X_OFFSET;
        int new_y_offset = Y_OFFSET + viewer_offset_y;
        int text_y = 480;
        ALLEGRO_COLOR fontColor = al_map_rgb(0, 0, 0);
        al_set_target_bitmap(text);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));

        al_set_target_bitmap(g_game->GetBackBuffer());
        al_draw_bitmap(starview, new_x_offset, new_y_offset, 0);
        al_draw_bitmap(flux_view, new_x_offset, new_y_offset, 0);
        al_set_target_bitmap(text);

        // display status info
        if (!g_game->gameState->player->isLost()) {
            Point2D playerPos = g_game->gameState->getHyperspaceCoordinates();

            PlayerShipSprite shipSprite;
            float distance = Point2D::Distance(playerPos, cursorPos);

            if (dest_active) {
                distance = Point2D::Distance(playerPos, m_destPos);
            }
            float max_vel = shipSprite.getMaximumVelocity();
            float fuel = distance * max_vel / 100 /
                         g_game->gameState->getShip().getEngineClass();

            // position
            al_draw_textf(g_game->font10,
                          fontColor,
                          115,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.0f",
                          playerPos.x);
            al_draw_textf(g_game->font10,
                          fontColor,
                          189,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.0f",
                          playerPos.y);

            // distance
            al_draw_textf(g_game->font10,
                          fontColor,
                          505,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.1f",
                          distance);

            // fuel
            al_draw_textf(g_game->font10,
                          fontColor,
                          620,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.2f",
                          fuel);

            al_set_target_bitmap(g_game->GetBackBuffer());
            al_draw_circle((int)(playerPos.x * ratioX + new_x_offset),
                           (int)(new_y_offset + (playerPos.y) * ratioY),
                           4,
                           al_map_rgb(0, 255, 0),
                           1);
            al_set_target_bitmap(text);
        }

        // destination

        // if player selected a spot, print the coordinates of that spot
        if (dest_active) {
            al_draw_textf(g_game->font10,
                          fontColor,
                          310,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.0f",
                          m_destPos.x);
            al_draw_textf(g_game->font10,
                          fontColor,
                          380,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.0f",
                          m_destPos.y);
            al_set_target_bitmap(g_game->GetBackBuffer());
            al_draw_circle((int)(m_destPos.x * ratioX + new_x_offset),
                           (int)(new_y_offset + (m_destPos.y) * ratioY),
                           4,
                           al_map_rgb(255, 0, 0),
                           1);
            al_set_target_bitmap(text);
        }
        // else if the mouse cursor is near a starsystem, we want to print the
        // coordinates of that starsystem instead of the actual coordinates
        // under the mouse pointer
        else if (m_bOver_Star) {
            al_draw_textf(g_game->font10,
                          fontColor,
                          310,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%i",
                          star_x);
            al_draw_textf(g_game->font10,
                          fontColor,
                          380,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%i",
                          star_y);
            star_label->Refresh();
            star_label->SetX((int)(cursorPos.x * ratioX + new_x_offset + 10));
            star_label->SetY((int)(cursorPos.y * ratioY + new_y_offset));
            star_label->Draw(g_game->GetBackBuffer());
        }
        // else print the the coordinate under mouse pointer
        else {
            al_draw_textf(g_game->font10,
                          fontColor,
                          310,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.0f",
                          cursorPos.x);
            al_draw_textf(g_game->font10,
                          fontColor,
                          380,
                          text_y,
                          ALLEGRO_ALIGN_CENTER,
                          "%.0f",
                          cursorPos.y);
        }
    }
    // draw generated text
    al_set_target_bitmap(g_game->GetBackBuffer());
    al_draw_bitmap(text, 120 + X_OFFSET / 2, viewer_offset_y, 0);

#pragma endregion
    if (map_active) {
        if (viewer_offset_y < -30) {
            viewer_offset_y += VIEWER_MOVE_RATE;
        }
    } else {
        if (viewer_offset_y > -VIEWER_TARGET_OFFSET) {
            viewer_offset_y -= VIEWER_MOVE_RATE;
        }
    }
}
