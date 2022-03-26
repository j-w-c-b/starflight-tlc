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
#include "SlidingModule.h"
#include "Util.h"
#include "starmap_resources.h"

using namespace std;
using namespace starmap;

ALLEGRO_DEBUG_CHANNEL("ModuleStarmap")

StarmapConfig::StarmapConfig() {
    Script lua;
    lua.load("data/starmap/starmap.lua");

    viewer_width = static_cast<int>(lua.getGlobalNumber("VIEWER_WIDTH"));
    viewer_height = static_cast<int>(lua.getGlobalNumber("VIEWER_HEIGHT"));
    viewer_x = static_cast<int>(lua.getGlobalNumber("VIEWER_X"));
    viewer_y = static_cast<int>(lua.getGlobalNumber("VIEWER_Y"));
    map_width = static_cast<int>(lua.getGlobalNumber("MAP_WIDTH"));
    map_height = static_cast<int>(lua.getGlobalNumber("MAP_HEIGHT"));
    map_x = viewer_x + static_cast<int>(lua.getGlobalNumber("MAP_X"));
    map_y = viewer_y + static_cast<int>(lua.getGlobalNumber("MAP_Y"));
    text_field_y =
        map_y + static_cast<int>(lua.getGlobalNumber("TEXT_FIELD_Y"));
    text_width = static_cast<int>(lua.getGlobalNumber("TEXT_WIDTH"));
    text_height = static_cast<int>(lua.getGlobalNumber("TEXT_HEIGHT"));
    position_x = map_x + static_cast<int>(lua.getGlobalNumber("POSITION_X_X"));
    position_y = map_x + static_cast<int>(lua.getGlobalNumber("POSITION_Y_X"));
    destination_x =
        map_x + static_cast<int>(lua.getGlobalNumber("DESTINATION_X_X"));
    destination_y =
        map_x + static_cast<int>(lua.getGlobalNumber("DESTINATION_Y_X"));
    distance_x = map_x + static_cast<int>(lua.getGlobalNumber("DISTANCE_X"));
    fuel_x = map_x + static_cast<int>(lua.getGlobalNumber("FUEL_X"));
    scale_x =
        static_cast<double>(map_width) / static_cast<double>(GALAXY_WIDTH);
    scale_y =
        static_cast<double>(map_height) / static_cast<double>(GALAXY_HEIGHT);
}

ModuleStarmap::ModuleStarmap()
    : Module(), m_config(), m_dest_active(false), m_over_star(false),
      m_cursor_pos(0, 0), m_dest_pos(0, 0), m_star_x(0), m_star_y(0) {
    move(m_config.viewer_x, m_config.viewer_y);
    resize(m_config.viewer_width, m_config.viewer_height);
}

bool
ModuleStarmap::on_init() {
    ALLEGRO_DEBUG("  ModuleStarmap Initialize\n");

    m_over_star = false;
    m_viewer_active = false;

    m_viewer = make_shared<SlidingModule<Bitmap>>(
        SLIDE_FROM_TOP,
        EVENT_NONE,
        0.6,
        images[I_STARMAP_VIEWER],
        m_config.viewer_x,
        m_config.viewer_y);
    m_starview = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(m_config.map_width, m_config.map_height),
        al_destroy_bitmap);
    m_starview_bitmap = make_shared<Bitmap>(
        m_starview,
        m_config.map_x,
        m_config.map_y,
        m_config.map_width,
        m_config.map_height);
    m_star_label =
        make_shared<Label>("", 0, 0, 100, 22, false, 0, g_game->font18, ORANGE);
    m_position_x_label = make_shared<Label>(
        "",
        m_config.position_x,
        m_config.text_field_y
            - (m_config.text_height
               - al_get_font_line_height(g_game->font10.get()))
                  / 2,
        m_config.text_width,
        m_config.text_height,
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font10,
        BLACK);
    m_position_y_label = make_shared<Label>(
        "",
        m_config.position_y,
        m_config.text_field_y
            - (m_config.text_height
               - al_get_font_line_height(g_game->font10.get()))
                  / 2,
        m_config.text_width,
        m_config.text_height,
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font10,
        BLACK);
    m_destination_x_label = make_shared<Label>(
        "",
        m_config.destination_x,
        m_config.text_field_y
            - (m_config.text_height
               - al_get_font_line_height(g_game->font10.get()))
                  / 2,
        m_config.text_width,
        m_config.text_height,
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font10,
        BLACK);
    m_destination_y_label = make_shared<Label>(
        "",
        m_config.destination_y,
        m_config.text_field_y
            - (m_config.text_height
               - al_get_font_line_height(g_game->font10.get()))
                  / 2,
        m_config.text_width,
        m_config.text_height,
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font10,
        BLACK);
    m_distance_label = make_shared<Label>(
        "",
        m_config.distance_x,
        m_config.text_field_y
            - (m_config.text_height
               - al_get_font_line_height(g_game->font10.get()))
                  / 2,
        m_config.text_width,
        m_config.text_height,
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font10,
        BLACK);
    m_fuel_label = make_shared<Label>(
        "",
        m_config.fuel_x,
        m_config.text_field_y
            - (m_config.text_height
               - al_get_font_line_height(g_game->font10.get()))
                  / 2,
        m_config.text_width,
        m_config.text_height,
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font10,
        BLACK);
    m_circles_bitmap = make_shared<Bitmap>(
        shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(m_config.viewer_width, m_config.viewer_height),
            al_destroy_bitmap),
        m_config.viewer_x,
        m_config.viewer_y,
        m_config.viewer_width,
        m_config.viewer_height);

    add_child_module(m_viewer);

    m_viewer->add_child_module(m_starview_bitmap);
    m_viewer->add_child_module(m_star_label);
    m_viewer->add_child_module(m_position_x_label);
    m_viewer->add_child_module(m_position_y_label);
    m_viewer->add_child_module(m_destination_x_label);
    m_viewer->add_child_module(m_destination_y_label);
    m_viewer->add_child_module(m_distance_label);
    m_viewer->add_child_module(m_fuel_label);
    m_viewer->add_child_module(m_circles_bitmap);

    if (m_viewer->is_open()) {
        m_viewer->close_slider();
    }
    m_flux_image = images[I_FLUX_TILE_TRANS];

    al_set_target_bitmap(m_starview.get());
    al_clear_to_color(BLACK);

    draw_stars();
    draw_flux();

    return true;
}

bool
ModuleStarmap::on_close() {
    ALLEGRO_DEBUG("  ModuleStarmap Close\n");

    m_starview = nullptr;

    m_viewer->remove_child_module(m_starview_bitmap);
    m_starview_bitmap = nullptr;

    m_viewer->remove_child_module(m_star_label);
    m_star_label = nullptr;

    m_viewer->remove_child_module(m_position_x_label);
    m_position_x_label = nullptr;

    m_viewer->remove_child_module(m_position_y_label);
    m_position_y_label = nullptr;

    m_viewer->remove_child_module(m_destination_x_label);
    m_destination_x_label = nullptr;

    m_viewer->remove_child_module(m_destination_y_label);
    m_destination_y_label = nullptr;

    m_viewer->remove_child_module(m_distance_label);
    m_distance_label = nullptr;

    m_viewer->remove_child_module(m_fuel_label);
    m_fuel_label = nullptr;

    m_viewer->remove_child_module(m_circles_bitmap);
    m_circles_bitmap = nullptr;

    remove_child_module(m_viewer);
    m_viewer = nullptr;

    m_flux_image = nullptr;

    return true;
}

bool
ModuleStarmap::on_draw(ALLEGRO_BITMAP * /*target*/) {
    Point2D playerPos = g_game->gameState->getHyperspaceCoordinates();
    string position_x = "???";
    string position_y = "???";
    int destination_x;
    int destination_y;
    float distance = -1;
    float fuel = -1;

    // this may change during exploration
    draw_flux();

    ALLEGRO_TRANSFORM translate;
    al_identity_transform(&translate);
    al_translate_transform(
        &translate, -m_circles_bitmap->get_x(), -m_circles_bitmap->get_y());

    al_set_target_bitmap(m_circles_bitmap->get_bitmap().get());
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_use_transform(&translate);

    // position and distance if we're not lost
    if (!g_game->gameState->player.isLost()) {
        if (m_dest_active) {
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
    if (m_dest_active) {
        // if player selected a spot, print the coordinates of that spot
        destination_x = m_dest_pos.x;
        destination_y = m_dest_pos.y;
    } else if (m_over_star) {
        // else if the mouse cursor is near a star, we want to print the
        // coordinates of that star instead of the actual coordinates
        // under the mouse pointer
        destination_x = m_star_x;
        destination_y = m_star_y;
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

    if (!g_game->gameState->player.isLost()) {
        int ship_x =
            static_cast<int>(playerPos.x * m_config.scale_x + m_config.map_x);
        int ship_y =
            static_cast<int>(playerPos.y * m_config.scale_y + m_config.map_y);
        al_draw_circle(ship_x, ship_y, 4, al_map_rgb(0, 255, 0), 1);
    }
    if (m_dest_active) {
        al_draw_circle(
            static_cast<int>(destination_x * m_config.scale_x + m_config.map_x),
            static_cast<int>(destination_y * m_config.scale_y + m_config.map_y),
            4,
            al_map_rgb(255, 0, 0),
            1);
    }
    m_position_x_label->set_text(position_x);
    m_position_y_label->set_text(position_y);
    m_destination_x_label->set_text(to_string(destination_x));
    m_destination_y_label->set_text(to_string(destination_y));

    if (distance >= 0) {
        stringstream distancestr;
        distancestr << fixed << setprecision(1) << distance;
        m_distance_label->set_text(distancestr.str());
    }

    if (fuel >= 0) {
        stringstream fuelstr;
        fuelstr << fixed << setprecision(2) << fuel;
        m_fuel_label->set_text(fuelstr.str());
    }
    return true;
}

bool
ModuleStarmap::on_event(ALLEGRO_EVENT *event) {
    EventType type = static_cast<EventType>(event->type);

    switch (type) {
    case EVENT_NAVIGATOR_STARMAP:
        m_viewer->toggle();
        if (!m_viewer_active) {
            m_viewer_active = true;
        }
        return true;
    case EVENT_NAVIGATOR_ORBIT:
        [[fallthrough]];
    case EVENT_NAVIGATOR_HYPERSPACE:
        [[fallthrough]];
    case EVENT_NAVIGATOR_DOCK:
        if (!m_viewer->is_closed()) {
            m_viewer_active = false;
            m_viewer->toggle();
        }
    default:
        return true;
    }
}
bool
ModuleStarmap::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    m_cursor_pos.x = static_cast<float>(x - m_config.map_x) / m_config.scale_x;
    m_cursor_pos.y = static_cast<float>(y - m_config.map_y) / m_config.scale_y;
    if (m_cursor_pos.x < 0 || m_cursor_pos.x > GALAXY_WIDTH
        || m_cursor_pos.y < 0 || m_cursor_pos.y > GALAXY_HEIGHT) {
        return true;
    }

    // if the mouse pointer is over a star, we need to remember that
    // star name and coordinates to display them later at Draw() time.
    m_over_star = false;

    int range[] = {0, -1, 1};
    for (auto map_y : range) {
        for (auto map_x : range) {
            const Star *star = g_game->dataMgr->GetStarByLocation(
                static_cast<int>(m_cursor_pos.x + map_x),
                static_cast<int>(m_cursor_pos.y + map_y));
            if (star) {
                m_star_x = star->x;
                m_star_y = star->y;
                m_star_label->set_text(star->name);
                m_over_star = true;

                int delta_x;
                int delta_y;
                int height;

                if (m_cursor_pos.x > GALAXY_WIDTH / 2) {
                    delta_x = -m_star_label->get_text_width();
                } else {
                    delta_x = 0;
                }

                height = m_star_label->get_height();

                if (m_cursor_pos.y > GALAXY_HEIGHT / 2) {
                    delta_y = -height;
                } else {
                    delta_y = height;
                }

                m_star_label->move(
                    static_cast<int>(
                        m_cursor_pos.x * m_config.scale_x + m_config.map_x
                        + delta_x),
                    static_cast<int>(
                        m_cursor_pos.y * m_config.scale_y + m_config.map_y
                        + delta_y));
            }
        }
    }
    if (!m_over_star) {
        m_star_label->set_text("");
    }
    return true;
}

bool
ModuleStarmap::on_mouse_button_click(ALLEGRO_MOUSE_EVENT *event) {
    if (event->button != 1) {
        return true;
    }
    if (!m_viewer->is_open()) {
        return true;
    }
    m_cursor_pos.x =
        static_cast<float>(event->x - m_config.map_x) / m_config.scale_x;
    m_cursor_pos.y =
        static_cast<float>(event->y - m_config.map_y) / m_config.scale_y;

    if (m_cursor_pos.x < 0 || m_cursor_pos.x > GALAXY_WIDTH
        || m_cursor_pos.y < 0 || m_cursor_pos.y > GALAXY_HEIGHT) {
        return true;
    }

    // If we're within 2 tiles in any direction of our selected spot,
    // consider this an unselect.
    if (m_cursor_pos.y > m_dest_pos.y - 2 && m_cursor_pos.y < m_dest_pos.y + 2
        && m_cursor_pos.x > m_dest_pos.x - 2
        && m_cursor_pos.x < m_dest_pos.x + 2 && m_dest_active) {
        m_dest_active = false;
    } else {
        m_dest_pos = m_cursor_pos;
        m_dest_active = true;
    }
    return false;
}

bool
ModuleStarmap::on_update() {
    auto current_officer = g_game->gameState->getCurrentSelectedOfficer();

    if (current_officer != OFFICER_NAVIGATION) {
        if (!m_viewer->is_closed()) {
            m_viewer->toggle();
        }
        m_viewer_active = false;
    } else if (m_viewer_active && !m_viewer->is_visible()) {
        m_viewer_active = false;
    }
    return true;
}

void
ModuleStarmap::draw_flux() {
    al_set_target_bitmap(m_starview.get());
    for (auto i = g_game->dataMgr->flux_begin(),
              e = g_game->dataMgr->flux_end();
         i != e;
         ++i) {
        auto f = *i;
        const FluxInfo &fi = g_game->gameState->flux_info[f->get_id()];
        const Point2D &endpoint1 = f->get_endpoint1();
        const Point2D &endpoint2 = f->get_endpoint2();
        const int frame_size = 8;

        if (fi.endpoint_1_visible) {
            al_draw_bitmap(
                m_flux_image.get(),
                endpoint1.x * m_config.scale_x - frame_size / 2,
                endpoint1.y * m_config.scale_y - frame_size / 2,
                0);
        }
        if (fi.endpoint_2_visible) {
            al_draw_bitmap(
                m_flux_image.get(),
                endpoint2.x * m_config.scale_x - frame_size / 2,
                endpoint2.y * m_config.scale_y - frame_size / 2,
                0);
        }
        if (fi.path_visible) {

            al_draw_line(
                (int)(endpoint1.x * m_config.scale_x),
                (int)(endpoint1.y * m_config.scale_y),
                (int)(endpoint2.x * m_config.scale_x),
                (int)(endpoint2.y * m_config.scale_y),
                al_map_rgb(0, 170, 255),
                1);
        }
    }
}

void
ModuleStarmap::draw_stars() {
    Sprite stars;
    stars.setImage(images[I_IS_TILES_TRANS]);
    if (!stars.getImage()) {
        g_game->message("Starmap: Error loading stars");
        return;
    }

    const int frame_size = 8;

    stars.setAnimColumns(8);
    stars.setTotalFrames(8);
    stars.setFrameWidth(frame_size);
    stars.setFrameHeight(frame_size);

    for (auto i = g_game->dataMgr->stars_begin(),
              e = g_game->dataMgr->stars_end();
         i != e;
         ++i) {
        auto star = *i;
        // draw star image on starmap
        stars.setCurrFrame(star->spectralClass);
        stars.setX(star->x * m_config.scale_x - frame_size / 2);
        stars.setY(star->y * m_config.scale_y - frame_size / 2);
        stars.drawframe(m_starview.get());
    }
}

void
ModuleStarmap::save_map(const std::string &filename) {
    draw_flux();

    al_save_bitmap(filename.c_str(), m_starview.get());
}
