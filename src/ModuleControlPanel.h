/*
	STARFLIGHT - THE LOST COLONY
	ModuleControlPanel.h - control panel module, provides a tab for each officer.  Each of these tabs
	shows info about the officer and all the commands for that officer.  Individual commands may be
	enabled/disabled based on the current context.  When commands are used, events are broadcast to
	the other active modules, allowing them to handle the commands.
	
	Author: coder1024
	Date: April, 07
*/

#ifndef MODULECONTROLPANEL_H
#define MODULECONTROLPANEL_H
#pragma once

#include <allegro5/allegro.h>
#include "Module.h"
#include "DataMgr.h"
#include "AudioSystem.h"
#include "ResourceManager.h"

#include <string>
#include <vector>

#define EVENT_CAPTAIN_LAUNCH 1000
#define EVENT_CAPTAIN_DESCEND 1002
//#define EVENT_CAPTAIN_DISEMBARK 1003
#define EVENT_CAPTAIN_CARGO 1004
//#define EVENT_CAPTAIN_LOG 1005
#define EVENT_CAPTAIN_QUESTLOG 1007
#define EVENT_SCIENCE_SCAN 2000
#define EVENT_SCIENCE_ANALYSIS 2001
//#define EVENT_NAVIGATOR_MANEUVER 3000
#define EVENT_NAVIGATOR_STARMAP 3001
#define EVENT_NAVIGATOR_ORBIT 3002
#define EVENT_NAVIGATOR_HYPERSPACE 3003
#define EVENT_NAVIGATOR_DOCK 3005
#define EVENT_TACTICAL_SHIELDS 4000
#define EVENT_TACTICAL_WEAPONS 4002
#define EVENT_TACTICAL_COMBAT 4004
//#define EVENT_ENGINEER_DAMAGE 5000
#define EVENT_ENGINEER_REPAIR 5001
#define EVENT_ENGINEER_INJECT 5002
#define EVENT_COMM_HAIL 6000
#define EVENT_COMM_DISTRESS 6002
#define EVENT_COMM_STATEMENT 6003
#define EVENT_COMM_QUESTION 6004
#define EVENT_COMM_POSTURE 6005
#define EVENT_COMM_TERMINATE 6006
#define EVENT_DOCTOR_EXAMINE 7000
#define EVENT_DOCTOR_TREAT 7001



class ModuleControlPanel : public Module
{
public:
	ModuleControlPanel(void);
	virtual ~ModuleControlPanel(void);
	virtual bool Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnKeyReleased(int keyCode) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseClick(int button, int x, int y) override;
	virtual void OnMousePressed(int button, int x, int y) override;
	virtual void OnMouseReleased(int button, int x, int y) override;
	virtual void OnMouseWheelUp(int x, int y) override;
	virtual void OnMouseWheelDown(int x, int y) override;
	virtual void Close() override;

	ResourceManager<ALLEGRO_BITMAP>         resources;
	
private:

	bool bEnabled;
	ALLEGRO_BITMAP			*controlPanelBackgroundImg;

        std::shared_ptr<Sample> sndOfficerSelected;
        std::shared_ptr<Sample> sndOfficerCommandSelected;

	class CommandButton
	{
	public:
		CommandButton(ModuleControlPanel& outer, const std::string &icon, const std::string &cmdName, int posX, int posY);
		virtual ~CommandButton();
		
		

		static bool InitCommon(ModuleControlPanel &outer);
		bool InitButton();
		void DestroyButton();
		static void DestroyCommon();
		static int GetCommonWidth();
		static int GetCommonHeight();

		void RenderPlain(ALLEGRO_BITMAP	*canvas);
		void RenderDisabled(ALLEGRO_BITMAP *canvas);
		void RenderMouseOver(ALLEGRO_BITMAP	*canvas);
		void RenderSelected(ALLEGRO_BITMAP *canvas);

		bool IsInButton(int x, int y);

		void SetEnabled(bool enabled);
		bool GetEnabled();

		//JH 5/05
		int getEventID() { return eventID; }
		void setEventID(int value) { eventID = value; }

	private:
		ModuleControlPanel	&outer;
		
                std::string				datFileCmdIcon;
		std::string				cmdName;
		int					posX;
		int					posY;
		ALLEGRO_BITMAP				*imgCmdIcon;
		bool				enabled;

		//JH 5/05
		int					eventID;

		static ALLEGRO_BITMAP		*imgBackground;
		static ALLEGRO_BITMAP		*imgBackgroundDisabled;
		static ALLEGRO_BITMAP		*imgBackgroundMouseOver;
		static ALLEGRO_BITMAP		*imgBackgroundSelected;

		void Render(ALLEGRO_BITMAP *canvas, ALLEGRO_BITMAP *imgBackground, bool down = false);
	};

	class OfficerButton
	{
	public:
		OfficerButton(ModuleControlPanel& outer, OfficerType officerType, const std::string &datFileMouseOver, const std::string &datFileSelected, int posX, int posY);
		virtual ~OfficerButton();

		static bool InitCommon();
		bool InitButton();
		void DestroyButton();
		static void DestroyCommon();

		void RenderMouseOver(ALLEGRO_BITMAP *canvas);
		void RenderSelected(ALLEGRO_BITMAP *canvas);

		bool IsInButton(int x, int y);

		std::vector<CommandButton*> commandButtons;

		int					posX;
		int					posY;
		ALLEGRO_BITMAP				*imgMouseOver;

      OfficerType GetOfficerType() { return officerType; }

	private:
		ModuleControlPanel	&outer;
		OfficerType			officerType;
		
		std::string			datFileMouseOver;
		std::string			datFileSelected;
		
		ALLEGRO_BITMAP			*imgSelected;
		static ALLEGRO_BITMAP		*imgTipWindowBackground;

	};

	std::vector<OfficerButton*>		officerButtons;
	
	OfficerButton					*mouseOverOfficer;
	OfficerButton					*selectedOfficer;

	CommandButton					*mouseOverCommand;
	CommandButton					*selectedCommand;
};

#endif

