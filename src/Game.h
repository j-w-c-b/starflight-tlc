/*
	STARFLIGHT - THE LOST COLONY
	Game.h -
	Author: D.Calkins
	Date: 2007

*/

#ifndef GAME_H
#define GAME_H 1

#include <iostream>
#include <string>
#include <vector>
#include <allegro5/allegro_font.h>
#include "ScrollBox.h"
#include "Timer.h"
#include "Sprite.h"

#include "GameState.h"
///////////////////////////////////////////
// global constants
///////////////////////////////////////////

//DO NOT MODIFY THESE
#define SCREEN_WIDTH 1024  //1280
#define SCREEN_HEIGHT 768 //960

//COMMON RGB COLORS
//here's a good source of rgb colors: http://www.pitt.edu/~nisg/cis/web/cgi/rgb.html
#define BLACK			al_map_rgb(0,0,0)
#define GRAY1			al_map_rgb(232,232,232)
#define DGRAY           al_map_rgb(120,120,120)
#define WHITE			al_map_rgb(255,255,255)
#define BLUE			al_map_rgb(0,0,255)
#define LTBLUE			al_map_rgb(150,150,255)
#define SKYBLUE			al_map_rgb(0,216,255)
#define DODGERBLUE		al_map_rgb(30,144,255)
#define ROYALBLUE		al_map_rgb(39,64,139)
#define PURPLE			al_map_rgb(212, 72,255)
#define RED				al_map_rgb(255,0,0)
#define LTRED			al_map_rgb(255,150,150)
#define ORANGE			al_map_rgb(255,165,0)
#define DKORANGE		al_map_rgb(255,140,0)
#define BRTORANGE		al_map_rgb(255,120,0)
#define YELLOW			al_map_rgb(250,250,0)
#define LTYELLOW		al_map_rgb(255,255,0)
#define GREEN			al_map_rgb(0,255,0)
#define LTGREEN			al_map_rgb(150,255,150)
#define PINEGREEN		al_map_rgb(80,170,80)
#define STEEL			al_map_rgb(159,182,205)
#define KHAKI			al_map_rgb(238,230,133)
#define DKKHAKI			al_map_rgb(139,134,78)

#define GREEN2			al_map_rgb(71,161,91)
#define RED2			al_map_rgb(110,26,15)
#define YELLOW2			al_map_rgb(232,238,106)
#define GOLD			al_map_rgb(255,216,0)
#define MASK_COLOR		al_map_rgb(255,0,255)

#define FLUX_SCANNER_ID 2

class Module;
class GameState;
class ModeMgr;
class DataMgr;
class AudioSystem;
class QuestMgr;
class Script;
class PauseMenu;

class ModulePlanetSurface;
class ModuleCargoWindow;
class MessageBoxWindow;
class ModuleEncounter;

enum MsgType {
	MSG_INFO=0,              //informative messages
	MSG_ALERT,               //messages printed in dangerous situations
	MSG_ERROR,               //command not allowed in that context and similar stuff
	MSG_ACK,                 //officer will try to execute the orders
	MSG_FAILURE,             //officer tried to execute the orders, but failed
	MSG_SUCCESS,             //officer tried to execute the orders, and succeeded
	MSG_TASK_COMPLETED,      //officer acknowledge completion of a long-running task (e.g. heal & repair)
	MSG_SKILLUP              //officer got a skill increase
};
#define NUM_MSGTYPES 8

class Game
{
public:
	Game();
	virtual ~Game();
	void Run();
	void shutdown();
	void fatalerror(const std::string &error);
	void message(const std::string &msg);
	ALLEGRO_BITMAP *GetBackBuffer() { return m_backbuffer; }
	void setVibration(int value) { vibration = value; }
	int getVibration() { return vibration; }

    MessageBoxWindow *messageBox;
	PauseMenu *pauseMenu;
	Sprite *cursor;
	float CrossModuleAngle;	//Holds entry angle for systems

	void ShowMessageBoxWindow(
        	const std::string &initHeading = "",
		const std::string &initText = "",
		int initWidth = 400,
		int initHeight = 300,
		ALLEGRO_COLOR initTextColor = WHITE,
		int initX = SCREEN_WIDTH/2,
		int initY = SCREEN_HEIGHT/2,
		bool initCentered = true,
		bool pauseGame = true);
	void KillMessageBoxWindow();

	void TogglePauseMenu();
	void setPaused(bool value) { m_pause = value; }
	void SetTimePaused(bool v);
	bool getTimePaused() 	  {return timePause;}
	int  getTimeRateDivisor() {return timeRateDivisor;}

	static QuestMgr		*questMgr;
	static GameState	*gameState;
	static ModeMgr		*modeMgr;
	static DataMgr		*dataMgr;
	static AudioSystem	*audioSystem;

	ModulePlanetSurface *PlanetSurfaceHolder;

	ALLEGRO_FONT *font10;
	ALLEGRO_FONT *font12;
	ALLEGRO_FONT *font18;
	ALLEGRO_FONT *font20;
	ALLEGRO_FONT *font22;
	ALLEGRO_FONT *font24;
	ALLEGRO_FONT *font32;
	ALLEGRO_FONT *font48;
	ALLEGRO_FONT *font60;

	void PrintDefault(ALLEGRO_BITMAP *dest,int x,int y, const std::string &text,ALLEGRO_COLOR color = WHITE);
	void Print(ALLEGRO_BITMAP *dest,ALLEGRO_FONT *_font, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow = false);
	void Print12(ALLEGRO_BITMAP *dest, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow=false);
	void Print18(ALLEGRO_BITMAP *dest, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow=false);
	void Print20(ALLEGRO_BITMAP *dest, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow=false);
	void Print22(ALLEGRO_BITMAP *dest, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow=false);
	void Print24(ALLEGRO_BITMAP *dest, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow=false);
	void Print32(ALLEGRO_BITMAP *dest, int x,int y, const std::string &text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), bool shadow=false);

	//shared print to ScrollBox in GUI modules
	struct TimedText
	{
		std::string text;
		ALLEGRO_COLOR color;
		long delay;
	};
	std::vector<TimedText> messages;
    ScrollBox::ScrollBox *g_scrollbox;
	void printout(ScrollBox::ScrollBox *scroll, const std::string &text, ALLEGRO_COLOR color=WHITE, long delay=0);
	ALLEGRO_COLOR MsgColors[NUM_MSGTYPES];

   //used to retrieve global values from script file globals.lua
	std::string getGlobalString(const std::string &name);
    void setGlobalString(const std::string &name, const std::string &value);
	double getGlobalNumber(const std::string &name);
	void setGlobalNumber(const std::string &name, double value);
	bool getGlobalBoolean(const std::string &name);

	int getFrameRate() { return frameRate; }
	Timer globalTimer;

	//used to enable/disable the control panel (when in use)
	bool ControlPanelActivity;

	//used by the ModeMgr sanity checks
	bool IsRunning() { return m_keepRunning; }

	//on/off toggle of some gui elements to reduce vertical visual clutter
	bool doShowControls() { return showControls; }
	void toggleShowControls();

    //used to itemize detected video modes reported by the DirectX driver for use in Settings
    struct VideoMode
    {
        int width,height;
    };
    std::list<VideoMode> videomodes;
    int desktop_width, desktop_height, desktop_colordepth;
    int actual_width, actual_height;
    bool Initialize_Graphics();


protected:
	void Stop();
	virtual bool InitGame();
	virtual void DestroyGame();
	virtual void RunGame();
	virtual void OnKeyPress(int keyCode);
	virtual void OnKeyPressed(int keyCode);
	virtual void OnKeyReleased(int keyCode);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseClick(int button, int x, int y);
	virtual void OnMousePressed(int button, int x, int y);
	virtual void OnMouseReleased(int button, int x, int y);
	virtual void OnMouseWheelUp(int x, int y);
	virtual void OnMouseWheelDown(int x, int y);
    
private:

    void UpdateAlienRaceAttitudes();

	bool showControls;
	bool m_keepRunning;
	bool m_pause;
	bool timePause;			//set for modules in which game time should not update (starport, etc.)
	int  timeRateDivisor;	//was (static) 'update_interval' in Game::RunGame.
	int vibration;

	ALLEGRO_DISPLAY *m_display;
	//primary drawing surface for all modules
	ALLEGRO_BITMAP *m_backbuffer;

	//the same as the primary surface, except it doesn't have a mouse on it
    //not to be rude but who is the idiot who came up with this solution?

	//vars used for framerate calculation
	int frameCount, startTime, frameRate;
    double screen_scaling, scale_width, scale_height;

	int m_numMouseButtons;
	// array of bools which are true for pressed buttons, false otherwise
	bool *m_mouseButtons;

	// same as m_mouseButtons, but used to indicate the prior state
	bool *m_prevMouseButtons;

	// used to record the position at which a mouse button was pressed; used
	// to detect clicks (mouse pressed and released at the same location
	struct MousePos
	{
		int x;
		int y;
	};
	MousePos *m_mousePressedLocs;

	// previous mouse position
	int m_prevMouseX;
	int m_prevMouseY;
	int m_prevMouseZ;

	// holds the state of the keys in the previous loop; used to detect kb events
	ALLEGRO_KEYBOARD_STATE m_prevKeyState;

	void CalculateFramerate();
	void UpdateKeyboard();
	void UpdateMouse();
	bool InitializeModules();

	//LUA state object used to read global settings
	Script *globals;

	std::string p_title;
	std::string p_version;

};


extern Game *g_game;


#endif
