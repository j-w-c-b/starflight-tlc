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
#pragma once

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Module.h"
#include "ResourceManager.h"
#include <allegro5/allegro.h>

#include <string>
#include <vector>

class ModuleControlPanel : public Module {
  public:
    ModuleControlPanel(void);
    virtual ~ModuleControlPanel(void);
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_close() override;

    ResourceManager<ALLEGRO_BITMAP> resources;

  private:
    bool bEnabled;
    ALLEGRO_BITMAP *controlPanelBackgroundImg;

    std::shared_ptr<Sample> sndOfficerSelected;
    std::shared_ptr<Sample> sndOfficerCommandSelected;

    class CommandButton {
      public:
        CommandButton(
            ModuleControlPanel &outer,
            const std::string &icon,
            const std::string &cmdName,
            int posX,
            int posY);
        virtual ~CommandButton();

        static bool InitCommon(ModuleControlPanel &outer);
        bool InitButton();
        void DestroyButton();
        static void DestroyCommon();
        static int GetCommonWidth();
        static int GetCommonHeight();

        void RenderPlain(ALLEGRO_BITMAP *canvas);
        void RenderDisabled(ALLEGRO_BITMAP *canvas);
        void RenderMouseOver(ALLEGRO_BITMAP *canvas);
        void RenderSelected(ALLEGRO_BITMAP *canvas);

        bool IsInButton(int x, int y);

        void SetEnabled(bool enabled);
        bool GetEnabled();

        // JH 5/05
        int getEventID() { return eventID; }
        void setEventID(int value) { eventID = value; }

      private:
        ModuleControlPanel &outer;

        std::string datFileCmdIcon;
        std::string cmdName;
        int posX;
        int posY;
        ALLEGRO_BITMAP *imgCmdIcon;
        bool enabled;

        // JH 5/05
        int eventID;

        static ALLEGRO_BITMAP *imgBackground;
        static ALLEGRO_BITMAP *imgBackgroundDisabled;
        static ALLEGRO_BITMAP *imgBackgroundMouseOver;
        static ALLEGRO_BITMAP *imgBackgroundSelected;

        void Render(
            ALLEGRO_BITMAP *canvas,
            ALLEGRO_BITMAP *imgBackground,
            bool down = false);
    };

    class OfficerButton {
      public:
        OfficerButton(
            ModuleControlPanel &outer,
            OfficerType officerType,
            const std::string &datFileMouseOver,
            const std::string &datFileSelected,
            int posX,
            int posY);
        virtual ~OfficerButton();

        static bool InitCommon();
        bool InitButton();
        void DestroyButton();
        static void DestroyCommon();

        void RenderMouseOver(ALLEGRO_BITMAP *canvas);
        void RenderSelected(ALLEGRO_BITMAP *canvas);

        bool IsInButton(int x, int y);

        std::vector<CommandButton *> commandButtons;

        int posX;
        int posY;
        ALLEGRO_BITMAP *imgMouseOver;

        OfficerType GetOfficerType() { return officerType; }

      private:
        ModuleControlPanel &outer;
        OfficerType officerType;

        std::string datFileMouseOver;
        std::string datFileSelected;

        ALLEGRO_BITMAP *imgSelected;
        static ALLEGRO_BITMAP *imgTipWindowBackground;
    };

    std::vector<OfficerButton *> officerButtons;

    OfficerButton *mouseOverOfficer;
    OfficerButton *selectedOfficer;

    CommandButton *mouseOverCommand;
    CommandButton *selectedCommand;
};

#endif
