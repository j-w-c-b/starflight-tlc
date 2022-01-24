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

ModuleStarmap::ModuleStarmap()
    : m_cursor_pos(0, 0), m_dest_pos(0, 0), resources(STARMAP_IMAGES) {
    map_active = false;
    ratioX = 0;
    ratioY = 0;
    dest_active = false;
    star_label = NULL;
    m_over_star = false;
    star_x = 0;
    star_y = 0;
}

ModuleStarmap::~ModuleStarmap() {}

void
ModuleStarmap::OnMouseMove(int x, int y) {
    Module::OnMouseMove(x, y);

    m_cursor_pos.x = static_cast<float>(x - MAP_X - VIEWER_X) / ratioX;
    m_cursor_pos.y = static_cast<float>(y - MAP_Y - VIEWER_Y) / ratioY;

    if (m_cursor_pos.x < 0 || m_cursor_pos.x > GALAXY_WIDTH ||
        m_cursor_pos.y < 0 || m_cursor_pos.y > GALAXY_HEIGHT) {
        return;
    }

    // if the mouse pointer is over a starsystem, we need to remember that
    // starsystem name and coordinates to display them later at Draw() time.
    m_over_star = false;

    int range[] = {0, -1, 1};
    for (auto map_y : range) {
        for (auto map_x : range) {
            const Star *star = g_game->dataMgr->GetStarByLocation(
                static_cast<int>(m_cursor_pos.x + map_x),
                static_cast<int>(m_cursor_pos.y + map_y));
            if (star) {
                star_x = star->x;
                star_y = star->y;
                star_label->SetText(star->name);
                m_over_star = true;
            }
        }
    }
}

void
ModuleStarmap::OnMouseClick(int /*button*/, int x, int y) {
    m_cursor_pos.x = static_cast<float>(x - MAP_X - VIEWER_X) / ratioX;
    m_cursor_pos.y = static_cast<float>(y - MAP_Y - VIEWER_Y) / ratioY;

    if (m_cursor_pos.x < 0 || m_cursor_pos.x > GALAXY_WIDTH ||
        m_cursor_pos.y < 0 || m_cursor_pos.y > GALAXY_HEIGHT) {
        return;
    }

    if (map_active) {
        // If we're within 2 tiles in any direction of our selected spot,
        // consider this an unselect.
        if (m_cursor_pos.y > m_dest_pos.y - 2 &&
            m_cursor_pos.y < m_dest_pos.y + 2 &&
            m_cursor_pos.x > m_dest_pos.x - 2 &&
            m_cursor_pos.x < m_dest_pos.x + 2 && dest_active) {
            dest_active = false;
        } else {
            m_dest_pos = m_cursor_pos;
            dest_active = true;
        }
    }
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
    VIEWER_WIDTH = static_cast<int>(lua.getGlobalNumber("VIEWER_WIDTH"));
    VIEWER_HEIGHT = static_cast<int>(lua.getGlobalNumber("VIEWER_HEIGHT"));
    VIEWER_X = static_cast<int>(lua.getGlobalNumber("VIEWER_X"));
    VIEWER_Y = static_cast<int>(lua.getGlobalNumber("VIEWER_Y"));
    MAP_WIDTH = static_cast<int>(lua.getGlobalNumber("MAP_WIDTH"));
    MAP_HEIGHT = static_cast<int>(lua.getGlobalNumber("MAP_HEIGHT"));
    MAP_X = static_cast<int>(lua.getGlobalNumber("MAP_X"));
    MAP_Y = static_cast<int>(lua.getGlobalNumber("MAP_Y"));
    TEXT_FIELD_Y = static_cast<int>(lua.getGlobalNumber("TEXT_FIELD_Y"));
    TEXT_WIDTH = static_cast<int>(lua.getGlobalNumber("TEXT_WIDTH"));
    TEXT_HEIGHT = static_cast<int>(lua.getGlobalNumber("TEXT_HEIGHT"));
    VIEWER_MOVE_TICKS =
        static_cast<int>(lua.getGlobalNumber("VIEWER_MOVE_TICKS"));
    ALLEGRO_ASSERT(VIEWER_MOVE_TICKS > 0);
    for (int i = 0, max = TEXT_FIELD_X.size(); i < max; ++i) {
        string key = "TEXT_FIELD_X_" + to_string(i);

        TEXT_FIELD_X[i] =
            MAP_X + static_cast<int>(lua.getGlobalNumber(key.c_str()));
    }
    ratioX = static_cast<float>(MAP_WIDTH) / GALAXY_WIDTH;
    ratioY = static_cast<float>(MAP_HEIGHT) / GALAXY_HEIGHT;

    viewer_offset_y = -VIEWER_HEIGHT;
    m_over_star = false;

    star_label = new Label("", 0, 0, 100, 22, ORANGE, g_game->font18);

    overlay = al_create_bitmap(VIEWER_WIDTH, VIEWER_HEIGHT);
    starview = al_create_bitmap(MAP_WIDTH, MAP_HEIGHT);

    al_set_target_bitmap(starview);
    al_clear_to_color(BLACK);
    draw_stars();
    draw_flux();

    return true;
}

void
ModuleStarmap::draw_stars() {
    Sprite stars;
    stars.setImage(resources[I_IS_TILES_TRANS]);
    if (!stars.getImage()) {
        g_game->message("Starmap: Error loading stars");
        return;
    }

    const int frame_size = 8;

    stars.setAnimColumns(8);
    stars.setTotalFrames(8);
    stars.setFrameWidth(frame_size);
    stars.setFrameHeight(frame_size);

    int num_stars = g_game->dataMgr->GetNumStars();
    for (int i = 0; i < num_stars; i++) {
        const Star *star = g_game->dataMgr->GetStar(i);
        // draw star image on starmap
        stars.setCurrFrame(star->spectralClass);
        stars.setX(star->x * ratioX - frame_size / 2);
        stars.setY(star->y * ratioY - frame_size / 2);
        stars.drawframe(starview);
    }
}

void
ModuleStarmap::draw_flux() {
    al_set_target_bitmap(starview);
    for (int i = 0; i < MAX_FLUX; i++) {
        const Flux *f = g_game->dataMgr->GetFlux(i);
        if (!f) {
            continue;
        }
        const FluxInfo &fi = g_game->gameState->flux_info[f->get_id()];
        const Point2D &endpoint1 = f->get_endpoint1();
        const Point2D &endpoint2 = f->get_endpoint2();
        const int frame_size = 8;

        if (fi.endpoint_1_visible) {
            al_draw_bitmap(resources[I_FLUX_TILE_TRANS],
                           endpoint1.x * ratioX - frame_size / 2,
                           endpoint1.y * ratioY - frame_size / 2,
                           0);
        }
        if (fi.endpoint_2_visible) {
            al_draw_bitmap(resources[I_FLUX_TILE_TRANS],
                           endpoint2.x * ratioX - frame_size / 2,
                           endpoint2.y * ratioY - frame_size / 2,
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
    if (overlay != NULL) {
        al_destroy_bitmap(overlay);
        overlay = NULL;
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
        } else if (viewer_offset_y < 0) {
            viewer_offset_y += VIEWER_HEIGHT / VIEWER_MOVE_TICKS;
            if (viewer_offset_y >= VIEWER_HEIGHT) {
                viewer_offset_y = VIEWER_HEIGHT;
            }
        }
    } else {
        if (viewer_offset_y > -VIEWER_HEIGHT) {
            viewer_offset_y -= VIEWER_HEIGHT / VIEWER_MOVE_TICKS;
            if (viewer_offset_y < -VIEWER_HEIGHT) {
                viewer_offset_y = -VIEWER_HEIGHT;
            }
        }
    }
}

void
ModuleStarmap::Draw() {
    Module::Draw();
    if (viewer_offset_y == -VIEWER_HEIGHT) {
        return;
    }

    ALLEGRO_COLOR font_color = BLACK;
    al_set_target_bitmap(overlay);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    Point2D playerPos = g_game->gameState->getHyperspaceCoordinates();
    string position_x = "???";
    string position_y = "???";
    int destination_x;
    int destination_y;
    float distance = -1;
    float fuel = -1;

    // position and distance if we're not lost
    if (!g_game->gameState->player->isLost()) {
        if (dest_active) {
            distance = Point2D::Distance(playerPos, m_dest_pos);
        } else {
            Point2D cursor_clamped = m_cursor_pos;
            if (cursor_clamped.x < 0) {
                cursor_clamped.x = 0;
            } else if (cursor_clamped.x >= GALAXY_WIDTH) {
                cursor_clamped.x = GALAXY_WIDTH;
            }
            if (cursor_clamped.y < 0) {
                cursor_clamped.y = 0;
            } else if (cursor_clamped.y >= GALAXY_HEIGHT) {
                cursor_clamped.y = GALAXY_HEIGHT;
            }
            distance = Point2D::Distance(playerPos, cursor_clamped);
        }

        position_x = to_string(static_cast<int>(playerPos.x));
        position_y = to_string(static_cast<int>(playerPos.y));
    }

    // destination
    if (dest_active) {
        // if player selected a spot, print the coordinates of that spot
        destination_x = m_dest_pos.x;
        destination_y = m_dest_pos.y;
    } else if (m_over_star) {
        // else if the mouse cursor is near a star, we want to print the
        // coordinates of that star instead of the actual coordinates
        // under the mouse pointer
        destination_x = star_x;
        destination_y = star_y;
    } else {
        // else print the coordinate under mouse pointer, clamping to the
        // dimensions of the galaxy
        destination_x = m_cursor_pos.x;
        destination_y = m_cursor_pos.y;
        if (destination_x < 0) {
            destination_x = 0;
        } else if (destination_x > GALAXY_WIDTH) {
            destination_x = GALAXY_WIDTH;
        }
        if (destination_y < 0) {
            destination_y = 0;
        } else if (destination_y > GALAXY_HEIGHT) {
            destination_y = GALAXY_HEIGHT;
        }
    }

    if (distance > 0) {
        fuel = g_game->gameState->getShip().get_fuel_usage(distance);
    }

    if (!g_game->gameState->player->isLost()) {
        al_draw_circle(static_cast<int>(playerPos.x * ratioX),
                       static_cast<int>(playerPos.y * ratioY),
                       4,
                       al_map_rgb(0, 255, 0),
                       1);
    }
    if (dest_active) {
        al_draw_circle(static_cast<int>(destination_x * ratioX),
                       static_cast<int>(destination_y * ratioY),
                       4,
                       al_map_rgb(255, 0, 0),
                       1);
    }
    // draw text on overlay
    al_draw_textf(g_game->font10,
                  font_color,
                  TEXT_FIELD_X[0] - MAP_X,
                  TEXT_FIELD_Y,
                  ALLEGRO_ALIGN_CENTER,
                  "%s",
                  position_x.c_str());
    al_draw_textf(g_game->font10,
                  font_color,
                  TEXT_FIELD_X[1] - MAP_X,
                  TEXT_FIELD_Y,
                  ALLEGRO_ALIGN_CENTER,
                  "%s",
                  position_y.c_str());
    al_draw_textf(g_game->font10,
                  font_color,
                  TEXT_FIELD_X[2] - MAP_X,
                  TEXT_FIELD_Y,
                  ALLEGRO_ALIGN_CENTER,
                  "%d",
                  destination_x);
    al_draw_textf(g_game->font10,
                  font_color,
                  TEXT_FIELD_X[3] - MAP_X,
                  TEXT_FIELD_Y,
                  ALLEGRO_ALIGN_CENTER,
                  "%d",
                  destination_y);
    if (distance >= 0) {
        al_draw_textf(g_game->font10,
                      font_color,
                      TEXT_FIELD_X[4] - MAP_X,
                      TEXT_FIELD_Y,
                      ALLEGRO_ALIGN_CENTER,
                      "%.1f",
                      distance);
    }
    if (fuel >= 0) {
        al_draw_textf(g_game->font10,
                      font_color,
                      TEXT_FIELD_X[5] - MAP_X,
                      TEXT_FIELD_Y,
                      ALLEGRO_ALIGN_CENTER,
                      "%.2f",
                      distance);
    }

    if (m_over_star) {
        star_label->Refresh();
        int delta_y;
        if (m_cursor_pos.y > GALAXY_HEIGHT / 2) {
            delta_y = -star_label->GetHeight();
        } else {
            delta_y = star_label->GetHeight();
        }
        star_label->SetX((int)(m_cursor_pos.x * ratioX));
        star_label->SetY((int)(m_cursor_pos.y * ratioY + delta_y));
        star_label->Draw(overlay);
    }
    // draw map
    al_set_target_bitmap(g_game->GetBackBuffer());
    al_draw_bitmap(resources[I_STARMAP_VIEWER], VIEWER_X, viewer_offset_y, 0);
    al_draw_bitmap_region(starview,
                          0,
                          -viewer_offset_y,
                          MAP_WIDTH,
                          MAP_HEIGHT + viewer_offset_y,
                          VIEWER_X + MAP_X,
                          VIEWER_Y + MAP_Y,
                          0);
    // draw overlay
    al_set_target_bitmap(g_game->GetBackBuffer());
    al_draw_bitmap_region(overlay,
                          0,
                          -viewer_offset_y,
                          VIEWER_WIDTH,
                          VIEWER_HEIGHT + viewer_offset_y,
                          VIEWER_X + MAP_X,
                          VIEWER_Y + MAP_Y,
                          0);
}
