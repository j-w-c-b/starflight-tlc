#include <allegro5/allegro_font.h>

#include "Game.h"
#include "PersonnelSlotButton.h"
#include "crewhire_resources.h"

using namespace std;
using namespace crewhire;

PersonnelSlotButton::PersonnelSlotButton(int x, int y, OfficerType type)
    : Button(
        x,
        y,
        al_get_bitmap_width(images[I_PERSONNEL_CATBTN].get()),
        al_get_bitmap_height(images[I_PERSONNEL_CATBTN].get()),
        EVENT_NONE,
        EVENT_CREWHIRE_PERSONNEL_CLICK,
        make_normal_bitmap(type),
        make_over_bitmap(type),
        make_disabled_bitmap(type),
        samples[S_BUTTONCLICK]),
      m_name_label(make_shared<Label>(
          "",
          x + 35,
          y + al_get_font_line_height(g_game->font24.get()),
          get_width() - 35,
          al_get_font_line_height(g_game->font24.get()),
          false,
          ALLEGRO_ALIGN_LEFT,
          g_game->font24,
          WHITE)),
      m_officer_type(type) {
    add_child_module(m_name_label);
}

const Officer *
PersonnelSlotButton::set_officer(const Officer *officer) {
    auto old_officer = m_officer;
    m_officer = officer;

    if (officer) {
        if (m_officer_type != OFFICER_CAPTAIN) {
            g_game->gameState->set_officer(m_officer_type, officer);
        }
        m_name_label->set_text(m_officer->get_name());
    } else {
        if (old_officer) {
            g_game->gameState->unassign_officer(old_officer);
        }
        m_name_label->set_text("");
    }

    return old_officer;
}

const Officer *
PersonnelSlotButton::fire_officer() {
    const Officer *old_officer = m_officer;

    g_game->gameState->fire_officer(old_officer);
    m_officer = nullptr;
    m_name_label->set_text("");

    return old_officer;
}

const Officer *
PersonnelSlotButton::unassign_officer() {
    const Officer *old_officer = m_officer;

    g_game->gameState->unassign_officer(old_officer);
    m_officer = nullptr;
    m_name_label->set_text("");

    return old_officer;
}

std::shared_ptr<ALLEGRO_BITMAP>
PersonnelSlotButton::make_normal_bitmap(OfficerType officer_type) {
    if (officer_type == OFFICER_NONE) {
        auto norm_image = shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(
                al_get_bitmap_width(images[I_PERSONNEL_CATBTN_UNASSIGNED].get())
                    + 30,
                al_get_bitmap_height(
                    images[I_PERSONNEL_CATBTN_UNASSIGNED].get())
                    + 30),
            al_destroy_bitmap);
        al_set_target_bitmap(norm_image.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(images[I_PERSONNEL_CATBTN_UNASSIGNED].get(), 30, 0, 0);

        return norm_image;
    } else {
        int frame_index = static_cast<int>(officer_type) - 1;
        auto norm_image = shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(
                al_get_bitmap_width(images[I_PERSONNEL_CATBTN].get()),
                al_get_bitmap_height(images[I_PERSONNEL_CATBTN].get())),
            al_destroy_bitmap);
        al_set_target_bitmap(norm_image.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(images[I_PERSONNEL_CATBTN].get(), 0, 0, 0);
        al_draw_bitmap_region(
            images[I_ICONS_SMALL].get(), 30 * frame_index, 0, 30, 30, 0, 0, 0);
        al_draw_text(
            g_game->font24.get(),
            al_map_rgb(0, 255, 255),
            35,
            4,
            0,
            to_string(officer_type).c_str());
        return norm_image;
    }
}

std::shared_ptr<ALLEGRO_BITMAP>
PersonnelSlotButton::make_over_bitmap(OfficerType officer_type) {

    if (officer_type == OFFICER_NONE) {
        auto over_image = shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(
                al_get_bitmap_width(
                    images[I_PERSONNEL_CATBTN_UNASSIGNED_HOV].get())
                    + 30,
                al_get_bitmap_height(
                    images[I_PERSONNEL_CATBTN_UNASSIGNED_HOV].get())
                    + 30),
            al_destroy_bitmap);
        al_set_target_bitmap(over_image.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(
            images[I_PERSONNEL_CATBTN_UNASSIGNED_HOV].get(), 30, 0, 0);

        return over_image;
    } else {
        int frame_index = static_cast<int>(officer_type) - 1;
        auto over_image = shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(
                al_get_bitmap_width(images[I_PERSONNEL_CATBTN_HOV].get()),
                al_get_bitmap_height(images[I_PERSONNEL_CATBTN_HOV].get())),
            al_destroy_bitmap);
        al_set_target_bitmap(over_image.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(images[I_PERSONNEL_CATBTN_HOV].get(), 0, 0, 0);
        al_draw_bitmap_region(
            images[I_ICONS_SMALL_GREEN].get(),
            30 * frame_index,
            0,
            30,
            30,
            0,
            0,
            0);
        al_draw_bitmap_region(
            images[I_ICONS_SMALL].get(), 30 * frame_index, 0, 30, 30, 1, 1, 0);
        al_draw_text(
            g_game->font24.get(),
            al_map_rgb(0, 255, 255),
            35,
            4,
            0,
            to_string(officer_type).c_str());

        return over_image;
    }
}

std::shared_ptr<ALLEGRO_BITMAP>
PersonnelSlotButton::make_disabled_bitmap(OfficerType officer_type) {
    if (officer_type == OFFICER_NONE) {
        auto disabled_image = shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(
                al_get_bitmap_width(
                    images[I_PERSONNEL_CATBTN_UNASSIGNED_DIS].get())
                    + 30,
                al_get_bitmap_height(
                    images[I_PERSONNEL_CATBTN_UNASSIGNED_DIS].get())
                    + 30),
            al_destroy_bitmap);
        al_set_target_bitmap(disabled_image.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(
            images[I_PERSONNEL_CATBTN_UNASSIGNED_DIS].get(), 30, 0, 0);

        return disabled_image;
    } else {
        int frame_index = static_cast<int>(officer_type) - 1;
        auto disabled_image = shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(
                al_get_bitmap_width(images[I_PERSONNEL_CATBTN_DIS].get()),
                al_get_bitmap_height(images[I_PERSONNEL_CATBTN_DIS].get())),
            al_destroy_bitmap);
        al_set_target_bitmap(disabled_image.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(images[I_PERSONNEL_CATBTN_DIS].get(), 0, 0, 0);
        al_draw_bitmap_region(
            images[I_ICONS_SMALL].get(), 30 * frame_index, 0, 30, 30, 1, 1, 0);
        al_draw_bitmap_region(
            images[I_ICONS_SMALL_RED].get(),
            30 * frame_index,
            0,
            30,
            30,
            0,
            0,
            0);
        al_draw_text(
            g_game->font24.get(),
            al_map_rgb(0, 255, 255),
            35,
            4,
            0,
            to_string(officer_type).c_str());
        return disabled_image;
    }
}

// vi: ft=cpp
