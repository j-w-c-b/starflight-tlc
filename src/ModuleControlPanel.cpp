/*
        STARFLIGHT - THE LOST COLONY
        ModuleControlPanel.cpp
        Author: coder1024
        Date: April, 07
*/

#include <allegro5/allegro.h>

#include <utility>

#include "AudioSystem.h"
#include "Bitmap.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModuleControlPanel.h"
#include "controlpanel_resources.h"

using namespace std;
using namespace controlpanel;

ALLEGRO_DEBUG_CHANNEL("ModuleControlPanel")

class OfficerButton : public Button {
  public:
    OfficerButton(
        int x,
        int y,
        EventType mouse_over_event,
        EventType click_event,
        OfficerType officer_type,
        std::shared_ptr<ALLEGRO_BITMAP> normal,
        std::shared_ptr<ALLEGRO_BITMAP> mouse_over)
        : Button(
            x,
            y,
            mouse_over_event,
            click_event,
            normal,
            mouse_over,
            nullptr,
            samples[S_OFFICER_SELECTED]),
          m_officer_type(officer_type) {}
    OfficerType get_officer_type() const { return m_officer_type; }

    virtual ALLEGRO_EVENT make_event(EventType t) override {
        ALLEGRO_EVENT e = {
            .user = {.type = t, .data1 = m_id, .data2 = m_officer_type}};
        return e;
    }

  private:
    OfficerType m_officer_type;
};

class CommandButton : public Button {
  public:
    CommandButton(
        int x,
        int y,
        EventType mouse_over_event,
        EventType click_event,
        shared_ptr<ALLEGRO_BITMAP> icon,
        const string &tooltip,
        OfficerType officer_type)
        : Button(
            x,
            y,
            mouse_over_event,
            click_event,
            images[I_COMMAND_BUTTON_BG],
            images[I_COMMAND_BUTTON_BG_SELECT],
            images[I_COMMAND_BUTTON_BG_DISABLED],
            samples[S_OFFICER_COMMAND_SELECTED]),
          m_icon(make_shared<Bitmap>(icon, x, y)), m_tooltip(tooltip),
          m_officer_type(officer_type) {
        add_child_module(m_icon);
    }
    string get_tooltip() const { return m_tooltip; }
    virtual ALLEGRO_EVENT make_event(EventType t) override {
        ALLEGRO_EVENT e = {
            .user = {.type = t, .data1 = m_id, .data2 = m_officer_type}};
        return e;
    }

  private:
    shared_ptr<Bitmap> m_icon;
    string m_tooltip;
    OfficerType m_officer_type;
};

class CommandPanel : public Module {
  public:
    CommandPanel(
        OfficerType officer_type,
        vector<tuple<string, string, EventType>> commands)
        : Module(), m_officer_type(officer_type) {
        int bx = (int)g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_X");
        int by = (int)g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_Y");
        int CMDBUTTONS_UL_X = bx + 18;
        int CMDBUTTONS_UL_Y = by + 242;

        int x = CMDBUTTONS_UL_X;
        int y = CMDBUTTONS_UL_Y;

        int count = 0;
        for (auto &[icon, tooltip, event] : commands) {
            auto cb = make_shared<CommandButton>(
                x,
                y,
                EVENT_COMMAND_MOUSE_OVER,
                event,
                images[icon],
                tooltip,
                officer_type);
            if ((++count % 3) == 0) {
                x = CMDBUTTONS_UL_X;
                y += cb->get_height();
            } else {
                x += cb->get_width();
            }
            add_child_module(cb);
        }
    }
    OfficerType get_officer_type() const { return m_officer_type; }

  private:
    OfficerType m_officer_type;
};

ModuleControlPanel::ModuleControlPanel()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
      m_selected_officer(OFFICER_NONE) {}

bool
ModuleControlPanel::on_init() {
    m_background = make_shared<Bitmap>(
        images[I_GUI_CONTROLPANEL],
        static_cast<int>(g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_X")),
        static_cast<int>(g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_Y")));
    add_child_module(m_background);

    map<OfficerType, pair<string, string>> officer_button_images = {
        {OFFICER_CAPTAIN, {I_CP_CAPTAIN_MO, I_CP_CAPTAIN_SELECT}},
        {OFFICER_SCIENCE, {I_CP_SCIENCE_MO, I_CP_SCIENCE_SELECT}},
        {OFFICER_NAVIGATION, {I_CP_NAVIGATION_MO, I_CP_NAVIGATION_SELECT}},
        {OFFICER_ENGINEER, {I_CP_ENGINEER_MO, I_CP_ENGINEER_SELECT}},
        {OFFICER_COMMUNICATION, {I_CP_COMM_MO, I_CP_COMM_SELECT}},
        {OFFICER_MEDICAL, {I_CP_MEDICAL_MO, I_CP_MEDICAL_SELECT}},
        {OFFICER_TACTICAL, {I_CP_TACTICAL_MO, I_CP_TACTICAL_SELECT}},
    };
    map<OfficerType, vector<tuple<string, string, EventType>>>
        officer_control_button_data = {
            {OFFICER_CAPTAIN,
             {
                 {I_COMMANDICON_CAPTAIN_LAUNCH,
                  "Break orbit",
                  EVENT_CAPTAIN_LAUNCH},
                 {I_COMMANDICON_CAPTAIN_DESCEND,
                  "Descend to surface",
                  EVENT_CAPTAIN_DESCEND},
                 {I_COMMANDICON_CAPTAIN_CARGO,
                  "Cargo hold",
                  EVENT_CAPTAIN_CARGO},
                 {I_COMMANDICON_COM_QUESTLOG,
                  "Quest log",
                  EVENT_CAPTAIN_QUESTLOG},
             }},
            {OFFICER_SCIENCE,
             {{I_COMMANDICON_SCIENCE_SCAN, "Sensor scan", EVENT_SCIENCE_SCAN},
              {I_COMMANDICON_SCIENCE_ANALYSIS,
               "Sensor analysis",
               EVENT_SCIENCE_ANALYSIS}}},
            {OFFICER_NAVIGATION,
             {
                 {I_COMMANDICON_NAV_ORBIT,
                  "Orbit planet",
                  EVENT_NAVIGATOR_ORBIT},
                 {I_COMMANDICON_NAV_DOCK,
                  "Dock with Starport",
                  EVENT_NAVIGATOR_DOCK},
                 {I_COMMANDICON_NAV_HYPERSPACE,
                  "Hyperspace engine",
                  EVENT_NAVIGATOR_HYPERSPACE},
                 {I_COMMANDICON_NAV_STARMAP,
                  "Starmap",
                  EVENT_NAVIGATOR_STARMAP},
             }},
            {
                OFFICER_TACTICAL,
                {
                    {I_COMMANDICON_TAC_SHIELDS,
                     "Raise/Lower Shields",
                     EVENT_TACTICAL_SHIELDS},
                    {I_COMMANDICON_TAC_WEAPONS,
                     "Arm/Disarm Weapons",
                     EVENT_TACTICAL_WEAPONS},
                },
            },
            {
                OFFICER_ENGINEER,
                {
                    {I_COMMANDICON_ENG_REPAIR,
                     "Repair systems",
                     EVENT_ENGINEER_REPAIR},
                    {I_COMMANDICON_COM_RESPOND,
                     "Inject fuel",
                     EVENT_ENGINEER_INJECT},
                },
            },
            {
                OFFICER_COMMUNICATION,
                {
                    {I_COMMANDICON_COM_HAIL,
                     "Hail or respond",
                     EVENT_COMM_HAIL},
                    {I_COMMANDICON_COM_QUESTION,
                     "Ask a question",
                     EVENT_COMM_QUESTION},
                    // Not implemented in Lua modules
                    /* {I_COMMANDICON_COM_STATEMENT, */
                    /*  "Make a statement", */
                    /*  EVENT_COMM_STATEMENT}, */
                    {I_COMMANDICON_COM_POSTURE,
                     "Change posture",
                     EVENT_COMM_POSTURE},
                    {I_COMMANDICON_COM_TERMINATE,
                     "End communication",
                     EVENT_COMM_TERMINATE},
                    {I_COMMANDICON_COM_DISTRESS,
                     "Send distress signal",
                     EVENT_COMM_DISTRESS},
                },
            },
            {
                OFFICER_MEDICAL,
                {
                    {I_COMMANDICON_MED_EXAMINE,
                     "Examine crew",
                     EVENT_DOCTOR_EXAMINE},
                    {I_COMMANDICON_MED_TREAT, "Treat crew", EVENT_DOCTOR_TREAT},
                },
            }};

    int officer_icon_x_base =
        static_cast<int>(g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_X"))
        + 45;
    int officer_icon_x = officer_icon_x_base;
    int officer_icon_y =
        static_cast<int>(g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_Y"))
        + 157;
    int officer_icon_width = 40;
    int officer_icon_height = 40;
    int officer_count = 0;

    for (auto i : OfficerIterator) {
        auto button = make_shared<OfficerButton>(
            officer_icon_x,
            officer_icon_y,
            EVENT_COMMAND_OFFICER_MOUSE_OVER,
            EVENT_COMMAND_OFFICER_CLICK,
            i,
            images[officer_button_images[i].first],
            images[officer_button_images[i].second]);
        m_officer_buttons[i] = button;
        add_child_module(button);
        if ((++officer_count % 4) == 0) {
            officer_icon_x = officer_icon_x_base;
            officer_icon_y += officer_icon_height;
        } else {
            officer_icon_x += officer_icon_width;
        }
        auto command_panel =
            make_shared<CommandPanel>(i, officer_control_button_data[i]);
        m_command_panels[i] = command_panel;
        add_child_module(command_panel);
        command_panel->set_active(false);
    }

    m_tooltip = make_shared<Label>(
        to_string(OFFICER_CAPTAIN),
        850,
        450,
        174,
        32,
        true,
        0,
        g_game->font12,
        WHITE);
    add_child_module(m_tooltip);

    m_selected_officer = g_game->gameState->getCurrentSelectedOfficer();
    if (m_selected_officer == OFFICER_NONE) {
        m_selected_officer = OFFICER_CAPTAIN;
        g_game->gameState->setCurrentSelectedOfficer(m_selected_officer);
    }
    m_officer_buttons[m_selected_officer]->set_highlight(true);
    m_command_panels[m_selected_officer]->set_active(true);
    return true;
}

bool
ModuleControlPanel::on_event(ALLEGRO_EVENT *event) {
    EventType event_type = static_cast<EventType>(event->type);

    switch (event_type) {
    case EVENT_COMMAND_OFFICER_CLICK:
        {
            OfficerType officer = static_cast<OfficerType>(event->user.data2);
            if (m_selected_officer != OFFICER_NONE
                && m_selected_officer != officer) {
                m_officer_buttons[m_selected_officer]->set_highlight(false);
                m_command_panels[m_selected_officer]->set_active(false);
            }
            m_selected_officer = officer;
            m_officer_buttons[m_selected_officer]->set_highlight(true);
            m_command_panels[m_selected_officer]->set_active(true);
            g_game->gameState->setCurrentSelectedOfficer(officer);
        }
        break;
    case EVENT_COMMAND_OFFICER_MOUSE_OVER:
        {
            OfficerType officer = static_cast<OfficerType>(event->user.data2);
            m_tooltip->set_text(to_string(officer));
        }
        break;
    case EVENT_COMMAND_MOUSE_OVER:
        {
            int button_id = static_cast<int>(event->user.data1);
            OfficerType officer = static_cast<OfficerType>(event->user.data2);
            shared_ptr<CommandButton> button =
                dynamic_pointer_cast<CommandButton>(
                    m_command_panels[officer]->get_child_module(button_id));
            ALLEGRO_ASSERT(button != nullptr);
            m_tooltip->set_text(button->get_tooltip());
        }
    default:
        break;
    }
    return true;
}

bool
ModuleControlPanel::on_close() {
    for (auto i : OfficerIterator) {
        remove_child_module(m_officer_buttons[i]);
        remove_child_module(m_command_panels[i]);
    }
    remove_child_module(m_background);
    remove_child_module(m_tooltip);
    m_officer_buttons.clear();
    m_command_panels.clear();
    m_background = nullptr;
    m_tooltip = nullptr;
    m_selected_officer = OFFICER_NONE;
    return true;
}

bool
ModuleControlPanel::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    OfficerType selected_officer = OFFICER_NONE;
    EventType selected_event = EVENT_NONE;

    switch (event->keycode) {
    case ALLEGRO_KEY_F1: // select the captain
        selected_officer = OFFICER_CAPTAIN;
        break;
    case ALLEGRO_KEY_F2: // select the science officer
        selected_officer = OFFICER_SCIENCE;
        break;
    case ALLEGRO_KEY_F3: // select the navigator
        selected_officer = OFFICER_NAVIGATION;
        break;
    case ALLEGRO_KEY_F4: // select the tactician
        selected_officer = OFFICER_TACTICAL;
        break;
    case ALLEGRO_KEY_F5: // select the engineer
        selected_officer = OFFICER_ENGINEER;
        break;
    case ALLEGRO_KEY_F6: // select the comms officer
        selected_officer = OFFICER_COMMUNICATION;
        break;
    case ALLEGRO_KEY_F7: // select the doctor
        selected_officer = OFFICER_MEDICAL;
        break;
    case ALLEGRO_KEY_M: // "map" button
        selected_officer = OFFICER_NAVIGATION;
        selected_event = EVENT_NAVIGATOR_STARMAP;
        break;
    default:
        return true;
    }
    if (m_selected_officer != OFFICER_NONE) {
        m_officer_buttons[m_selected_officer]->set_highlight(false);
        m_command_panels[m_selected_officer]->set_active(false);
    }
    m_selected_officer = selected_officer;
    m_officer_buttons[m_selected_officer]->set_highlight(true);
    m_command_panels[m_selected_officer]->set_active(true);

    if (selected_event != EVENT_NONE) {
        ALLEGRO_EVENT e = make_event(selected_event);
        g_game->broadcast_event(&e);
    }
    return true;
}
