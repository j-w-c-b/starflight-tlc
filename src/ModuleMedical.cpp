/*
        STARFLIGHT - THE LOST COLONY
        ModuleMedical.cpp
        Author: Keith Patch
        Date: April 2008
*/

#include <sstream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "AudioSystem.h"
#include "Bitmap.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModuleMedical.h"
#include "ResourceManager.h"
#include "Script.h"
#include "SlidingModule.h"
#include "Util.h"
#include "medical_resources.h"

using namespace std;
using namespace medical;

#define CATSPACING 30

ALLEGRO_DEBUG_CHANNEL("ModuleMedical")

ModuleMedical::ModuleMedical() : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) {}

ModuleMedical::~ModuleMedical() {}

class MedicalTreatView : public Module {
  public:
    MedicalTreatView();
    virtual ~MedicalTreatView(){};

    void set_officer(const Officer *officer);

  private:
    std::shared_ptr<Label> m_name_label;
    std::shared_ptr<RichTextLabel> m_status_label;
    std::shared_ptr<ALLEGRO_BITMAP> m_health_bar;
    std::shared_ptr<Bitmap> m_health_bar_bitmap;
    std::shared_ptr<Label> m_percent_label;
    std::shared_ptr<TextButton> m_treat_button;
};

MedicalTreatView::MedicalTreatView() : Module(0, 85, 436 - 43, 334) {
    auto frame = make_shared<Bitmap>(
        images[I_GUI_VIEWER_RIGHT],
        get_x(),
        get_y(),
        get_width(),
        get_height(),
        ALLEGRO_FLIP_HORIZONTAL);
    add_child_module(frame);

    auto background = make_shared<Bitmap>(
        images[I_RIGHT_VIEWER_BG], get_x() + 64, get_y() + 34);
    add_child_module(background);

    const int treat_contents_x = 65;
    const int treat_contents_y = 120;
    const int treat_contents_w = 297;
    const int line_height = al_get_font_line_height(g_game->font22.get());
    int y = treat_contents_y + 10;

    m_name_label = make_shared<Label>(
        "",
        treat_contents_x + 10,
        y,
        treat_contents_w - 20,
        line_height,
        false,
        0,
        g_game->font22,
        WHITE);
    add_child_module(m_name_label);

    y += m_name_label->get_height() * 2;

    m_status_label = make_shared<RichTextLabel>(
        treat_contents_x + 10,
        y,
        treat_contents_w - 20,
        line_height,
        false,
        0,
        g_game->font22,
        WHITE,
        al_map_rgba(0, 0, 0, 0),
        vector<RichText>{});

    add_child_module(m_status_label);

    y += m_status_label->get_height() * 2;

    m_health_bar = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(treat_contents_w - 20, 50), al_destroy_bitmap);
    al_set_target_bitmap(m_health_bar.get());
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    m_health_bar_bitmap =
        make_shared<Bitmap>(m_health_bar, treat_contents_x + 10, y);
    add_child_module(m_health_bar_bitmap);

    m_percent_label = make_shared<Label>(
        "",
        treat_contents_x + 10,
        y + (50 - line_height) / 2,
        treat_contents_w - 20,
        al_get_font_line_height(g_game->font22.get()),
        false,
        ALLEGRO_ALIGN_CENTER,
        g_game->font22,
        WHITE);
    add_child_module(m_percent_label);

    y += 50 + line_height;

    m_treat_button = make_shared<TextButton>(
        "Treat",
        g_game->font24,
        BLACK,
        ALLEGRO_ALIGN_CENTER,
        treat_contents_x
            + (treat_contents_w - al_get_bitmap_width(images[I_BTN_NORM].get()))
                  / 2,
        y,
        EVENT_NONE,
        EVENT_MEDICAL_TREAT,
        images[I_BTN_NORM],
        images[I_BTN_HOV],
        images[I_BTN_DIS],
        samples[S_BUTTONCLICK]);
    m_treat_button->set_enabled(false);
    add_child_module(m_treat_button);
}

class MedicalExamineGauge : public Module {
  public:
    MedicalExamineGauge(int x, int y, Skill skill, int value = 0);
    MedicalExamineGauge(int x, int y, float value = 0);
    void set_value(int value);

  private:
    optional<Skill> m_skill;
    std::shared_ptr<Bitmap> m_bar;
    std::shared_ptr<Label> m_label;
    static map<Skill, string> c_image_names;
};

map<Skill, string> MedicalExamineGauge::c_image_names = {
    {SKILL_SCIENCE, I_MED_BAR_SCIENCE},
    {SKILL_NAVIGATION, I_MED_BAR_NAVIGATION},
    {SKILL_TACTICAL, I_MED_BAR_TACTICAL},
    {SKILL_ENGINEERING, I_MED_BAR_ENGINEER},
    {SKILL_COMMUNICATION, I_MED_BAR_COMMUNICATION},
    {SKILL_MEDICAL, I_MED_BAR_MEDICAL},
    {SKILL_LEARN_RATE, I_MED_BAR_LEARN},
    {SKILL_DURABILITY, I_MED_BAR_DURABILITY},
};

MedicalExamineGauge::MedicalExamineGauge(int x, int y, Skill skill, int value)
    : Module(x, y, 251, 29), m_skill(skill),
      m_bar(make_shared<Bitmap>(images[c_image_names[skill]], x, y)),
      m_label(make_shared<Label>(
          to_string(skill),
          x,
          y,
          get_width(),
          get_height(),
          false,
          ALLEGRO_ALIGN_CENTER,
          g_game->font22,
          BLACK)) {
    set_value(value);
    add_child_module(m_bar);
    add_child_module(m_label);
}

MedicalExamineGauge::MedicalExamineGauge(int x, int y, float value)
    : Module(x, y, 251, 29), m_skill(nullopt),
      m_bar(make_shared<Bitmap>(images[I_MED_BAR_HEALTH], x, y)),
      m_label(make_shared<Label>(
          "health",
          x,
          y,
          get_width(),
          get_height(),
          false,
          ALLEGRO_ALIGN_CENTER,
          g_game->font22,
          BLACK)) {
    set_value(static_cast<int>(value));
    add_child_module(m_bar);
    add_child_module(m_label);
}

void
MedicalExamineGauge::set_value(int value) {
    if (!m_skill) {
        m_bar->set_clip_width(value * 251 / 100);
        return;
    } else {
        switch (*m_skill) {
        case SKILL_SCIENCE:
            [[fallthrough]];
        case SKILL_NAVIGATION:
            [[fallthrough]];
        case SKILL_TACTICAL:
            [[fallthrough]];
        case SKILL_ENGINEERING:
            [[fallthrough]];
        case SKILL_COMMUNICATION:
            [[fallthrough]];
        case SKILL_MEDICAL:
            m_bar->set_clip_width(value * 251 / 250);
            break;
        case SKILL_LEARN_RATE:
        case SKILL_DURABILITY:
            m_bar->set_clip_width(value * 251 / 65);
        }
    }
};

class MedicalExamineView : public Module {
  public:
    MedicalExamineView();
    void set_officer(const Officer *officer);

  private:
    std::shared_ptr<MedicalExamineGauge> m_vitality_gauge;
    map<Skill, std::shared_ptr<MedicalExamineGauge>> m_skill_gauges;
};

MedicalExamineView::MedicalExamineView() : Module(0, 10, 436 - 43, 454) {
    // GUI frame
    add_child_module(make_shared<Bitmap>(images[I_GUI_VIEWER], -43, 10));
    // gauge icons and background bars
    add_child_module(make_shared<Bitmap>(images[I_MED_WINDOW_DATA], 67, 43));

    // officer stat bars
    int x = 110;
    m_vitality_gauge = make_shared<MedicalExamineGauge>(x, 46);
    add_child_module(m_vitality_gauge);

    m_skill_gauges[SKILL_SCIENCE] =
        make_shared<MedicalExamineGauge>(x, 85, SKILL_SCIENCE);

    m_skill_gauges[SKILL_NAVIGATION] =
        make_shared<MedicalExamineGauge>(x, 129, SKILL_NAVIGATION);

    m_skill_gauges[SKILL_ENGINEERING] =
        make_shared<MedicalExamineGauge>(x, 177, SKILL_ENGINEERING);

    m_skill_gauges[SKILL_COMMUNICATION] =
        make_shared<MedicalExamineGauge>(x, 221, SKILL_COMMUNICATION);

    m_skill_gauges[SKILL_MEDICAL] =
        make_shared<MedicalExamineGauge>(x, 266, SKILL_MEDICAL);

    m_skill_gauges[SKILL_TACTICAL] =
        make_shared<MedicalExamineGauge>(x, 311, SKILL_TACTICAL);

    m_skill_gauges[SKILL_LEARN_RATE] =
        make_shared<MedicalExamineGauge>(x, 357, SKILL_LEARN_RATE);

    m_skill_gauges[SKILL_DURABILITY] =
        make_shared<MedicalExamineGauge>(x, 401, SKILL_DURABILITY);
    for (auto &i : m_skill_gauges) {
        add_child_module(i.second);
    }
}

void
MedicalExamineView::set_officer(const Officer *officer) {
    if (officer) {
        m_vitality_gauge->set_value(officer->get_vitality());

        for (auto i : SkillIterator) {
            m_skill_gauges[i]->set_value(officer->get_skill(i));
        }
    }
}

void
MedicalTreatView::set_officer(const Officer *officer) {
    if (officer == nullptr) {
        m_name_label->set_text("");
        m_status_label->set_text("");
        al_set_target_bitmap(m_health_bar.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        m_percent_label->set_text("");
        m_treat_button->set_active(false);
        return;
    }

    auto vitality = officer->get_vitality();
    ALLEGRO_COLOR status_color;
    string status;
    m_name_label->set_text(officer->get_name());

    if (vitality <= 0) {
        status_color = DGRAY;
        status = "DEAD";
    } else if (vitality < 30) {
        status_color = RED2;
        status = "CRITICAL";
    } else if (vitality < 70) {
        status_color = YELLOW;
        status = "INJURED";
    } else {
        status_color = GREEN2;
        status = "HEALTHY";
    }

    m_status_label->set_text(
        {RichText("STATUS: ", WHITE), RichText(status, status_color)});

    al_set_target_bitmap(m_health_bar.get());
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_filled_rectangle(
        0,
        0,
        vitality * al_get_bitmap_width(m_health_bar.get()) / 100,
        al_get_bitmap_height(m_health_bar.get()),
        status_color);

    string percent = to_string(static_cast<int>(vitality)) + "%";
    m_percent_label->set_text(percent);

    m_treat_button->set_active(true);
    if (vitality > 0 && vitality < 100) {
        m_treat_button->set_enabled(true);
    }
}

bool
ModuleMedical::on_init() {
    ALLEGRO_DEBUG("  ModuleMedical::Init()\n");

    m_mode = MEDICAL_MODE_NONE;
    m_selected_officer = nullopt;

    m_viewer_treat = make_shared<SlidingModule<MedicalTreatView>>(
        SLIDE_FROM_LEFT, EVENT_NONE, 0.6);
    add_child_module(m_viewer_treat);

    m_viewer_examine = make_shared<SlidingModule<MedicalExamineView>>(
        SLIDE_FROM_LEFT, EVENT_NONE, 0.6);
    add_child_module(m_viewer_examine);

    // GUI stuff
    m_viewer_crewlist = make_shared<SlidingModule<Module>>(
        SLIDE_FROM_RIGHT, EVENT_NONE, 0.6, 1024 - 436, 85, 436, 334);
    m_viewer_crewlist->add_child_module(make_shared<Bitmap>(
        images[I_GUI_VIEWER_RIGHT],
        m_viewer_crewlist->get_x(),
        m_viewer_crewlist->get_y()));
    m_viewer_crewlist->add_child_module(make_shared<Bitmap>(
        images[I_RIGHT_VIEWER_BG],
        m_viewer_crewlist->get_x() + 34,
        m_viewer_crewlist->get_y() + 34));
    add_child_module(m_viewer_crewlist);

    // Create and initialize the crew buttons
    for (auto i : OfficerIterator) {
        static const map<OfficerType, string> abbrev = {
            {OFFICER_CAPTAIN, "CAP. "},
            {OFFICER_SCIENCE, "SCI. "},
            {OFFICER_NAVIGATION, "NAV. "},
            {OFFICER_TACTICAL, "TAC. "},
            {OFFICER_ENGINEER, "ENG. "},
            {OFFICER_COMMUNICATION, "COM. "},
            {OFFICER_MEDICAL, "DOC. "},
        };
        static const map<OfficerType, EventType> events = {
            {OFFICER_CAPTAIN, EVENT_MEDICAL_CAPTAIN_SELECT},
            {OFFICER_SCIENCE, EVENT_MEDICAL_SCIENCE_SELECT},
            {OFFICER_NAVIGATION, EVENT_MEDICAL_NAVIGATION_SELECT},
            {OFFICER_TACTICAL, EVENT_MEDICAL_TACTICAL_SELECT},
            {OFFICER_ENGINEER, EVENT_MEDICAL_ENGINEER_SELECT},
            {OFFICER_COMMUNICATION, EVENT_MEDICAL_COMMUNICATIONS_SELECT},
            {OFFICER_MEDICAL, EVENT_MEDICAL_MEDICAL_SELECT},
        };

        m_officer_buttons[i] = make_shared<TextButton>(
            "",
            g_game->font22,
            WHITE,
            ALLEGRO_ALIGN_LEFT,
            m_viewer_crewlist->get_x() + 34,
            m_viewer_crewlist->get_y() + 34
                + (m_officer_buttons.size() * CATSPACING),
            EVENT_NONE,
            events.at(i),
            images[I_MEDICAL_CAPTBTN],
            images[I_MEDICAL_CAPTBTN_HOV],
            images[I_MEDICAL_CAPTBTN_DIS],
            samples[S_BUTTONCLICK]);
        auto o = g_game->gameState->get_officer(i);
        m_officer_buttons[i]->set_text(abbrev.at(i) + o->get_name());

        m_viewer_crewlist->add_child_module(m_officer_buttons[i]);
    }

    return true;
}

bool
ModuleMedical::on_event(ALLEGRO_EVENT *event) {
    string other = "";
    bool treat_officer = false;
    optional<OfficerType> select_officer;

    switch (event->type) {
    case EVENT_MEDICAL_CAPTAIN_SELECT:
        select_officer = OFFICER_CAPTAIN;
        break;
    case EVENT_MEDICAL_SCIENCE_SELECT:
        select_officer = OFFICER_SCIENCE;
        break;
    case EVENT_MEDICAL_NAVIGATION_SELECT:
        select_officer = OFFICER_NAVIGATION;
        break;
    case EVENT_MEDICAL_TACTICAL_SELECT:
        select_officer = OFFICER_TACTICAL;
        break;
    case EVENT_MEDICAL_ENGINEER_SELECT:
        select_officer = OFFICER_ENGINEER;
        break;
    case EVENT_MEDICAL_COMMUNICATIONS_SELECT:
        select_officer = OFFICER_COMMUNICATION;
        break;
    case EVENT_MEDICAL_MEDICAL_SELECT:
        select_officer = OFFICER_MEDICAL;
        break;

    case EVENT_MEDICAL_TREAT:
        treat_officer = true;
        break;

    case EVENT_DOCTOR_TREAT:
        m_mode = MEDICAL_MODE_TREAT;
        break;

    case EVENT_DOCTOR_EXAMINE:
        m_mode = MEDICAL_MODE_EXAMINE;
        break;
    }

    if (select_officer) {
        if (m_selected_officer) {
            m_officer_buttons[*m_selected_officer]->set_highlight(false);
        }
        if (m_selected_officer && *m_selected_officer == *select_officer) {
            m_selected_officer = nullopt;
            m_viewer_treat->set_officer(nullptr);
            m_viewer_examine->set_officer(nullptr);
        } else {
            auto officer = g_game->gameState->get_officer(*select_officer);
            m_viewer_treat->set_officer(officer);
            m_viewer_examine->set_officer(officer);
            m_officer_buttons[*select_officer]->set_highlight(true);
            m_selected_officer = *select_officer;
        }
    }
    if (treat_officer && m_selected_officer) {
        auto med_officer =
            g_game->gameState->get_effective_officer(OFFICER_MEDICAL);
        auto officer = g_game->gameState->get_officer(*m_selected_officer);
        auto vitality = officer->get_vitality();

        ALLEGRO_ASSERT(vitality < 100 && vitality > 0);

        g_game->gameState->cease_healing();
        g_game->gameState->set_healing(*m_selected_officer, true);
        if (officer == med_officer) {
            g_game->printout(
                OFFICER_MEDICAL, "Okay, I'll patch myself up", GREEN, 1000);
        } else {
            g_game->printout(
                OFFICER_MEDICAL,

                "Okay, I'm treating " + officer->get_abbreviated_title(),
                GREEN,
                1000);
        }
    }
    return true;
}

bool
ModuleMedical::on_close() {
    ALLEGRO_DEBUG("*** ModuleMedical::Close()\n");

    m_officer_buttons.clear();
    remove_child_module(m_viewer_treat);
    m_viewer_treat = nullptr;

    remove_child_module(m_viewer_crewlist);
    m_viewer_crewlist = nullptr;

    m_selected_officer = nullopt;

    return true;
}

bool
ModuleMedical::on_update() {
    auto medic = g_game->gameState->get_effective_officer(OFFICER_MEDICAL);
    auto current_officer = g_game->gameState->getCurrentSelectedOfficer();

    if (current_officer != OFFICER_MEDICAL) {
        m_mode = MEDICAL_MODE_NONE;
    }
    switch (m_mode) {
    case MEDICAL_MODE_NONE:
        if (m_viewer_crewlist->is_open()) {
            m_viewer_crewlist->toggle();
        }
        if (m_viewer_treat->is_open()) {
            m_viewer_treat->toggle();
        }
        if (m_viewer_examine->is_open()) {
            m_viewer_examine->toggle();
        }
        break;
    case MEDICAL_MODE_TREAT:
        if (!m_viewer_crewlist->is_open()) {
            m_viewer_crewlist->toggle();
        }
        if (!m_viewer_treat->is_open()) {
            m_viewer_treat->toggle();
        }
        if (m_viewer_examine->is_open()) {
            m_viewer_examine->toggle();
        }
        break;
    case MEDICAL_MODE_EXAMINE:
        if (!m_viewer_crewlist->is_open()) {
            m_viewer_crewlist->toggle();
        }
        if (m_viewer_treat->is_open()) {
            m_viewer_treat->toggle();
        }
        if (!m_viewer_examine->is_open()) {
            m_viewer_examine->toggle();
        }
        break;
    }
    if (current_officer != OFFICER_MEDICAL) {
        return true;
    }

    // heal medical_skill/50 vitality point every iteration
    float heal_rate =
        g_game->gameState->CalcEffectiveSkill(SKILL_MEDICAL) / 50.0;

    for (auto i : OfficerIterator) {
        auto officer = g_game->gameState->get_officer(i);
        if (officer->get_vitality() <= 0) {
            m_officer_buttons[i]->set_color(DGRAY);
        } else if (officer->isBeingHealed()) {
            m_officer_buttons[i]->set_color(LTGREEN);
            if (g_game->gameState->CanSkillCheck(SKILL_MEDICAL)) {
                if (g_game->gameState->add_experience(SKILL_MEDICAL, 1)) {
                    g_game->printout(
                        OFFICER_MEDICAL,

                        "I think I'm getting better at this.",
                        PURPLE,
                        5000);
                }
                g_game->gameState->heal(i, heal_rate);
                m_viewer_treat->set_officer(officer);
                m_viewer_examine->set_officer(officer);
            }
            if (officer->get_vitality() == 100) {
                g_game->gameState->cease_healing(i);
                if (officer == medic) {
                    g_game->printout(
                        OFFICER_MEDICAL,

                        "I'm feeling much better now.",
                        BLUE,
                        1000);
                } else {
                    g_game->printout(
                        OFFICER_MEDICAL,

                        officer->get_abbreviated_title()
                            + " has fully recovered.",
                        BLUE,
                        1000);
                }
            }
        } else if (officer->get_vitality() < 30) {
            m_officer_buttons[i]->set_color(RED2);
        } else if (officer->get_vitality() < 70) {
            m_officer_buttons[i]->set_color(YELLOW2);
        } else {
            m_officer_buttons[i]->set_color(WHITE);
        }
    }

    return true;
}
