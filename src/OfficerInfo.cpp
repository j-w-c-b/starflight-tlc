#include <memory>
#include <string>

#include "Bitmap.h"
#include "Game.h"
#include "OfficerInfo.h"
#include "crewhire_resources.h"

using namespace std;
using namespace crewhire;

class OfficerInfoGauge : public Module {
  public:
    OfficerInfoGauge(int x, int y, Skill skill);
    virtual ~OfficerInfoGauge(){};

    void set_value(int value);
    static constexpr int spacing = 50;

  private:
    Skill m_skill;
    shared_ptr<Bitmap> m_gauge_fill;
    shared_ptr<Label> m_gauge_title;
    shared_ptr<Label> m_gauge_title_shadow;
    static map<Skill, const string> c_skill_bar_names;
};

map<Skill, const string> OfficerInfoGauge::c_skill_bar_names = {
    {SKILL_SCIENCE, I_PERSONNEL_SCIBAR},
    {SKILL_NAVIGATION, I_PERSONNEL_NAVBAR},
    {SKILL_TACTICAL, I_PERSONNEL_TACBAR},
    {SKILL_ENGINEERING, I_PERSONNEL_ENGBAR},
    {SKILL_COMMUNICATION, I_PERSONNEL_COMBAR},
    {SKILL_MEDICAL, I_PERSONNEL_MEDBAR},
    {SKILL_LEARN_RATE, I_PERSONNEL_LRBAR},
    {SKILL_DURABILITY, I_PERSONNEL_DURBAR},
};

OfficerInfoGauge::OfficerInfoGauge(int x, int y, Skill skill)
    : Module(
        x,
        y,
        al_get_bitmap_width(images[c_skill_bar_names[skill]].get()),
        al_get_bitmap_height(images[c_skill_bar_names[skill]].get())),
      m_skill(skill),
      m_gauge_fill(
          make_shared<Bitmap>(images[c_skill_bar_names[skill]], x + 50, y)) {
    add_child_module(m_gauge_fill);
    m_gauge_title_shadow = make_shared<Label>(
        to_string(skill),
        x + 57,
        y + 2,
        get_width(),
        get_height(),
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font24,
        BLACK);
    add_child_module(m_gauge_title_shadow);
    m_gauge_title = make_shared<Label>(
        to_string(skill),
        x + 55,
        y,
        get_width(),
        get_height(),
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font24,
        WHITE);
    add_child_module(m_gauge_title);
    set_value(0);
}

void
OfficerInfoGauge::set_value(int value) {
    float visible;
    if (m_skill == SKILL_DURABILITY || m_skill == SKILL_LEARN_RATE) {
        visible = value / 65.0;
    } else {
        visible = value / 250.0;
    }
    m_gauge_fill->set_clip_width(get_width() * visible);
}

OfficerInfo::OfficerInfo(int x, int y)
    : Module(x, y, 480, 510),
      m_name_label(make_shared<Label>(
          "",
          x + 15,
          y + 15,
          al_get_bitmap_width(images[I_PERSONNEL_MINIPOSITIONS].get()),
          al_get_font_line_height(g_game->font32.get()),
          false,
          ALLEGRO_ALIGN_LEFT,
          g_game->font24,
          WHITE)),
      m_preferred_job_label(make_shared<Label>(
          "",
          x + 15,
          y + 45,
          al_get_bitmap_width(images[I_PERSONNEL_MINIPOSITIONS].get()),
          al_get_font_line_height(g_game->font32.get()),
          false,
          ALLEGRO_ALIGN_LEFT,
          g_game->font24,
          WHITE)) {
    auto background =
        make_shared<Bitmap>(images[I_PERSONNEL_MINIPOSITIONS], x + 15, y + 80);
    add_child_module(background);
    add_child_module(m_name_label);
    add_child_module(m_preferred_job_label);

    int count = 0;
    for (auto skill : SkillIterator) {
        auto gauge = make_shared<OfficerInfoGauge>(
            x + 15, y + 100 + count * OfficerInfoGauge::spacing, skill);
        add_child_module(gauge);
        m_gauges[skill] = gauge;
        count++;
    }
}

void
OfficerInfo::set_officer(const Officer *officer) {
    if (officer != nullptr) {
        m_name_label->set_text(officer->get_name());
        if (officer->get_officer_type() != OFFICER_NONE) {
            m_preferred_job_label->set_text(
                to_string(officer->get_officer_type()));
            m_preferred_job_label->set_color(WHITE);
        } else {
            m_preferred_job_label->set_text(
                to_string(officer->get_preferred_profession()));
            m_preferred_job_label->set_color(GOLD);
        }
        for (auto skill : SkillIterator) {
            auto gauge = m_gauges[skill];
            gauge->set_value(officer->get_skill(skill));
        }
    } else {
        m_name_label->set_text("");
        m_preferred_job_label->set_text("");
        for (auto skill : SkillIterator) {
            auto gauge = m_gauges[skill];
            gauge->set_value(0);
        }
    }
}
// vi: ft=cpp
