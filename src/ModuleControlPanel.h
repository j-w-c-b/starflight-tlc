/*
        STARFLIGHT - THE LOST COLONY
        ModuleControlPanel.h - control panel module, provides a tab for each
   officer.  Each of these tabs shows info about the officer and all the
   commands for that officer.  Individual commands may be enabled/disabled based
   on the current context.  When commands are used, events are broadcast to the
   other active modules, allowing them to handle the commands.

        Author: coder1024
        Date: April, 07
*/

#ifndef MODULECONTROLPANEL_H
#define MODULECONTROLPANEL_H

#include <allegro5/allegro.h>

#include <map>
#include <memory>

#include "Bitmap.h"
#include "Button.h"
#include "Module.h"
#include "Officer.h"

class ModuleControlPanel : public Module {
  public:
    ModuleControlPanel();
    virtual bool on_init() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_close() override;

  private:
    OfficerType m_selected_officer;

    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Label> m_tooltip;
    std::map<OfficerType, std::shared_ptr<class OfficerButton>>
        m_officer_buttons;
    std::map<OfficerType, std::shared_ptr<Module>> m_command_panels;
};

#endif
// vi: ft=cpp
