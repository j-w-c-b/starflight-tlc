#include <allegro5/allegro_font.h>

#include "AudioSystem.h"
#include "Game.h"
#include "GameState.h"
#include "MessageBoxWindow.h"
#include "ModuleCaptainDetails.h"
#include "captaincreation_resources.h"

using namespace std;
using namespace captaincreation;

static const int TITLE_Y = 30;

static const int DETAILS_BOX_X = 72;
static const int DETAILS_BOX_WIDTH = 880;
static const int DETAILS_BOX_Y = 170;
static const int DETAILS_BOX_HEIGHT = 480;
static const int NAME_ENTRY_Y = 32;
static const int NAME_ENTRY_HEIGHT = 40;
static const int ATTR_LABEL_X = 32;
static const int ATTR_LABEL_WIDTH = 300;
static const int ATTR_LABEL_HEIGHT = 40;
static const int ATTR_VALUE_WIDTH = 48;
static const int ATTR_VALUE_PADDING = 32;
static const int ATTR_PLUS_WIDTH = 37;
static const int ATTR_PLUS_HEIGHT = 37;
static const int ATTR_PLUS_PADDING = 5;
static const int ATTR_MINUS_WIDTH = 37;
static const int ATTR_MINUS_HEIGHT = 37;
static const int ATTR_MINUS_PADDING = 32;
static const int ATTR_AVAILABLE_WIDTH = 270;

static const int CAPTAIN_NAME_MAX = 15;

#define TEXTCOL al_map_rgb(0, 255, 255)

static const int INITIAL_AVAIL_PTS = 5;
static const int INITIAL_AVAIL_PROF_PTS = 25;

const int RESET_BUTTON_X = 868;
const int RESET_BUTTON_Y = 542;

const int ACCEPT_BUTTON_X = 860;
const int ACCEPT_BUTTON_Y = 585;

static const string ATTR_NAME_DURABILITY = "Durability";
static const string ATTR_NAME_LEARN_RATE = "Learn Rate";
static const string ATTR_NAME_SCIENCE = "Science";
static const string ATTR_NAME_NAVIGATION = "Navigation";
static const string ATTR_NAME_TACTICS = "Tactics";
static const string ATTR_NAME_ENGINEERING = "Engineering";
static const string ATTR_NAME_COMMUNICATION = "Communication";
static const string ATTR_NAME_MEDICAL = "Medical";

ModuleCaptainAttribute::ModuleCaptainAttribute(
    const string &name,
    int value,
    int max,
    int x,
    int y,
    int width,
    int height,
    EventType plus_event,
    EventType minus_event)
    : Module(x, y, width, height), m_name_label(new Label(
                                       name,
                                       x,
                                       y,
                                       ATTR_LABEL_WIDTH,
                                       ATTR_LABEL_HEIGHT,
                                       false,
                                       0,
                                       g_game->font32,
                                       TEXTCOL)),
      m_value_label(new Label(
          to_string(value),
          x + ATTR_LABEL_WIDTH,
          y,
          ATTR_VALUE_WIDTH,
          ATTR_LABEL_HEIGHT,
          0,
          ALLEGRO_ALIGN_RIGHT,
          g_game->font32,
          TEXTCOL)),
      m_plus_button(new Button(
          x + ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH + ATTR_VALUE_PADDING,
          y,
          ATTR_PLUS_WIDTH,
          ATTR_PLUS_HEIGHT,
          EVENT_NONE,
          plus_event,
          images[I_CAPTAINCREATION_PLUS],
          images[I_CAPTAINCREATION_PLUS_MOUSEOVER])),
      m_minus_button(new Button(
          x + ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH + ATTR_VALUE_PADDING
              + ATTR_PLUS_WIDTH + ATTR_PLUS_PADDING,
          y,
          ATTR_MINUS_WIDTH,
          ATTR_MINUS_HEIGHT,
          EVENT_NONE,
          minus_event,
          images[I_MINUS_DISABLED],
          images[I_MINUS])),
      m_value(value), m_min(value), m_max(max) {
    add_child_module(m_name_label);
    add_child_module(m_value_label);
    add_child_module(m_plus_button);
    add_child_module(m_minus_button);
}

ModuleCaptainAttributeGroup::ModuleCaptainAttributeGroup(
    int x,
    int y,
    int width,
    int max)
    : Module(x, y, width, 0), m_sum(0), m_max(max) {
    m_available_label = make_shared<Label>(
        to_string(max) + " available",
        x + ATTR_LABEL_X + ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH
            + ATTR_VALUE_PADDING + ATTR_PLUS_WIDTH + ATTR_PLUS_PADDING
            + ATTR_MINUS_WIDTH + ATTR_MINUS_PADDING,
        y + get_height() / 2,
        ATTR_AVAILABLE_WIDTH,
        ATTR_LABEL_HEIGHT,
        false,
        ALLEGRO_ALIGN_RIGHT,
        g_game->font32,
        TEXTCOL);
    add_child_module(m_available_label);
}

bool
ModuleCaptainAttributeGroup::on_event(ALLEGRO_EVENT *event) {
    EventType t = static_cast<EventType>(event->type);

    auto attribute = m_attributes_by_plus.find(t);
    if (attribute != m_attributes_by_plus.end()) {
        if (m_sum >= m_max || !attribute->second->plus()) {
            g_game->audioSystem->Play(samples[S_ERROR]);
        } else {
            ++m_sum;
            g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
            m_available_label->set_text(
                to_string(m_max - m_sum) + " available");
            ALLEGRO_EVENT e = {
                .type = EVENT_CAPTAINCREATION_POINT_ASSIGNMENT_CHANGED};
            g_game->broadcast_event(&e);
        }
        return false;
    }
    attribute = m_attributes_by_minus.find(t);
    if (attribute != m_attributes_by_minus.end()) {
        if (m_sum < 0 || !attribute->second->minus()) {
            g_game->audioSystem->Play(samples[S_ERROR]);
        } else {
            --m_sum;
            g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
            m_available_label->set_text(
                to_string(m_max - m_sum) + " available");
            ALLEGRO_EVENT e = {
                .type = EVENT_CAPTAINCREATION_POINT_ASSIGNMENT_CHANGED};
            g_game->broadcast_event(&e);
        }
        return false;
    }
    return true;
}

void
ModuleCaptainAttributeGroup::add_attribute(
    const std::string &name,
    int value,
    int max,
    EventType plus_event,
    EventType minus_event) {
    auto a = make_shared<ModuleCaptainAttribute>(
        name,
        value,
        max,
        get_x() + ATTR_LABEL_X,
        get_y() + ATTR_LABEL_HEIGHT * m_attributes.size(),
        ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH + ATTR_VALUE_PADDING
            + ATTR_PLUS_WIDTH + ATTR_PLUS_PADDING + ATTR_MINUS_WIDTH
            + ATTR_MINUS_PADDING + ATTR_AVAILABLE_WIDTH,
        ATTR_LABEL_HEIGHT,
        plus_event,
        minus_event);
    m_attributes[name] = a;
    m_attributes_by_plus[plus_event] = a;
    m_attributes_by_minus[minus_event] = a;

    add_child_module(a);
}

void
ModuleCaptainAttributeGroup::clear() {
    for (auto i = m_attributes.begin(), e = m_attributes.end(); i != e;
         i = m_attributes.erase(i)) {
        remove_child_module(i->second);
    }
}

void
ModuleCaptainAttributeGroup::reset() {
    for (auto i : m_attributes) {
        i.second->reset();
    }
    m_sum = 0;
    m_available_label->set_text(to_string(m_max - m_sum) + " available");
}

void
ModuleCaptainAttributeGroup::do_resize() {
    Module::resize(get_width(), ATTR_LABEL_HEIGHT * (m_attributes.size()));

    auto [x, y] = m_available_label->get_position();

    m_available_label->move(x, static_cast<int>(get_y() + get_height() / 2.0));
}

ModuleCaptainDetails::ModuleCaptainDetails()
    : Module(
        DETAILS_BOX_X,
        DETAILS_BOX_Y,
        DETAILS_BOX_WIDTH,
        DETAILS_BOX_HEIGHT),
      m_background(
          make_shared<Bitmap>(images[I_CAPTAINCREATION_DETAILSBACKGROUND])),
      m_title_label(make_shared<Label>(
          "Captain Details",
          0,
          TITLE_Y,
          SCREEN_WIDTH,
          al_get_font_line_height(g_game->font60.get()),
          false,
          ALLEGRO_ALIGN_CENTER,
          g_game->font60,
          TEXTCOL)),
      m_name_entry(make_shared<TextEntry>(
          g_game->font32,
          TEXTCOL,
          "Name: ",
          CAPTAIN_NAME_MAX,
          images[I_CAPTAINCREATION_CURSOR0],
          0.4,
          DETAILS_BOX_X,
          DETAILS_BOX_Y + NAME_ENTRY_Y,
          DETAILS_BOX_WIDTH,
          NAME_ENTRY_HEIGHT,
          samples[S_CLICK],
          samples[S_ERROR])),
      m_physical_attributes(make_shared<ModuleCaptainAttributeGroup>(
          DETAILS_BOX_X,
          DETAILS_BOX_Y + NAME_ENTRY_Y + NAME_ENTRY_HEIGHT,
          DETAILS_BOX_WIDTH,
          INITIAL_AVAIL_PTS)),
      m_professional_attributes(make_shared<ModuleCaptainAttributeGroup>(
          DETAILS_BOX_X,
          DETAILS_BOX_Y + NAME_ENTRY_Y + NAME_ENTRY_HEIGHT,
          DETAILS_BOX_WIDTH,
          INITIAL_AVAIL_PROF_PTS)),
      m_reset_button(make_shared<Button>(
          RESET_BUTTON_X,
          RESET_BUTTON_Y,
          -1,
          -1,
          EVENT_NONE,
          EVENT_CAPTAINCREATION_RESET,
          images[I_CAPTAINCREATION_RESET],
          images[I_CAPTAINCREATION_RESET_MOUSEOVER])),
      m_accept_button(make_shared<Button>(
          ACCEPT_BUTTON_X,
          ACCEPT_BUTTON_Y,
          -1,
          -1,
          EVENT_NONE,
          EVENT_CAPTAINCREATION_FINISH,
          images[I_CAPTAINCREATION_FINISH],
          images[I_CAPTAINCREATION_FINISH_MOUSEOVER],
          images[I_CAPTAINCREATION_FINISH_DISABLED])) {

    add_child_module(m_background);
    add_child_module(m_title_label);
    add_child_module(m_name_entry);
    add_child_module(m_physical_attributes);
    add_child_module(m_professional_attributes);
    add_child_module(m_reset_button);
    add_child_module(m_accept_button);
}

bool
ModuleCaptainDetails::on_init() {
    static bool help2 = true;

    // display tutorial help messages for beginners
    if ((!g_game->gameState->firstTimeVisitor
         || g_game->gameState->getActiveQuest() > 1))
        help2 = false;

    if (help2) {
        help2 = false;
        string str = "Next, you need to enter a name for your "
                     "captain, and then set "
                     "your attribute points: 5 points to "
                     "Durability or Learning "
                     "Rate, and 25 points to all the rest. You "
                     "must allocate all of "
                     "the points before continuing.";
        set_modal_child(make_shared<MessageBoxWindow>(
            "", str, 10, 250, 400, 300, YELLOW, false));
    }
    return true;
}

bool
ModuleCaptainDetails::on_event(ALLEGRO_EVENT *event) {
    EventType t = static_cast<EventType>(event->type);

    switch (t) {
    case EVENT_CLOSE:
        set_modal_child(nullptr);
        return true;
    case EVENT_CAPTAINCREATION_POINT_ASSIGNMENT_CHANGED:
        if (m_physical_attributes->get_available_points() == 0
            && m_professional_attributes->get_available_points() == 0) {
            m_accept_button->set_enabled(true);
        } else {
            m_accept_button->set_enabled(false);
        }
        return false;
    case EVENT_CAPTAINCREATION_RESET:
        reset();
        return false;
    default:
        return true;
    }
}

string
ModuleCaptainDetails::get_name() const {
    return m_name_entry->get_text();
}

ModuleCaptainDetails::Attributes
ModuleCaptainDetails::get_attributes() const {
    ModuleCaptainDetails::Attributes attributes;
    attributes[SKILL_DURABILITY] =
        m_physical_attributes->get_value(ATTR_NAME_DURABILITY);
    attributes[SKILL_LEARN_RATE] =
        m_physical_attributes->get_value(ATTR_NAME_LEARN_RATE);
    attributes[SKILL_SCIENCE] =
        m_professional_attributes->get_value(ATTR_NAME_SCIENCE);
    attributes[SKILL_NAVIGATION] =
        m_professional_attributes->get_value(ATTR_NAME_NAVIGATION);
    attributes[SKILL_TACTICAL] =
        m_professional_attributes->get_value(ATTR_NAME_TACTICS);
    attributes[SKILL_ENGINEERING] =
        m_professional_attributes->get_value(ATTR_NAME_ENGINEERING);
    attributes[SKILL_COMMUNICATION] =
        m_professional_attributes->get_value(ATTR_NAME_COMMUNICATION);
    attributes[SKILL_MEDICAL] =
        m_professional_attributes->get_value(ATTR_NAME_MEDICAL);

    return attributes;
}

void
ModuleCaptainDetails::reset() {
    m_physical_attributes->reset();
    m_professional_attributes->reset();
    m_name_entry->reset();
}

void
ModuleCaptainDetails::setup_attributes(
    const Attributes &initial_attributes,
    const Attributes &max_attributes) {
    m_physical_attributes->clear();
    m_physical_attributes->add_attribute(
        ATTR_NAME_DURABILITY,
        initial_attributes.at(SKILL_DURABILITY),
        max_attributes.at(SKILL_DURABILITY),
        EVENT_CAPTAINCREATION_PLUS_DURABILITY,
        EVENT_CAPTAINCREATION_MINUS_DURABILITY);
    m_physical_attributes->add_attribute(
        ATTR_NAME_LEARN_RATE,
        initial_attributes.at(SKILL_LEARN_RATE),
        max_attributes.at(SKILL_LEARN_RATE),
        EVENT_CAPTAINCREATION_PLUS_LEARNRATE,
        EVENT_CAPTAINCREATION_MINUS_LEARNRATE);
    m_physical_attributes->do_resize();

    m_professional_attributes->clear();
    m_professional_attributes->add_attribute(
        ATTR_NAME_SCIENCE,
        initial_attributes.at(SKILL_SCIENCE),
        max_attributes.at(SKILL_SCIENCE),
        EVENT_CAPTAINCREATION_PLUS_SCIENCE,
        EVENT_CAPTAINCREATION_MINUS_SCIENCE);
    m_professional_attributes->add_attribute(
        ATTR_NAME_NAVIGATION,
        initial_attributes.at(SKILL_NAVIGATION),
        max_attributes.at(SKILL_NAVIGATION),
        EVENT_CAPTAINCREATION_PLUS_NAVIGATION,
        EVENT_CAPTAINCREATION_MINUS_NAVIGATION);
    m_professional_attributes->add_attribute(
        ATTR_NAME_TACTICS,
        initial_attributes.at(SKILL_TACTICAL),
        max_attributes.at(SKILL_TACTICAL),
        EVENT_CAPTAINCREATION_PLUS_TACTICS,
        EVENT_CAPTAINCREATION_MINUS_TACTICS);
    m_professional_attributes->add_attribute(
        ATTR_NAME_ENGINEERING,
        initial_attributes.at(SKILL_ENGINEERING),
        max_attributes.at(SKILL_ENGINEERING),
        EVENT_CAPTAINCREATION_PLUS_ENGINEERING,
        EVENT_CAPTAINCREATION_MINUS_ENGINEERING);
    m_professional_attributes->add_attribute(
        ATTR_NAME_COMMUNICATION,
        initial_attributes.at(SKILL_COMMUNICATION),
        max_attributes.at(SKILL_COMMUNICATION),
        EVENT_CAPTAINCREATION_PLUS_COMMUNICATION,
        EVENT_CAPTAINCREATION_MINUS_COMMUNICATION);
    m_professional_attributes->add_attribute(
        ATTR_NAME_MEDICAL,
        initial_attributes.at(SKILL_MEDICAL),
        max_attributes.at(SKILL_MEDICAL),
        EVENT_CAPTAINCREATION_PLUS_MEDICAL,
        EVENT_CAPTAINCREATION_MINUS_MEDICAL);

    int newy;
    auto [x, y] = m_physical_attributes->get_position();
    newy = y + ATTR_LABEL_HEIGHT * 3;

    m_professional_attributes->move(x, newy);
    m_professional_attributes->do_resize();
    m_accept_button->set_enabled(false);
}
// vi: ft=cpp
