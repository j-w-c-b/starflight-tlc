/*
        STARFLIGHT - THE LOST COLONY
        Game.h -
        Author: D.Calkins
        Date: 2007

*/

#ifndef GAME_H
#define GAME_H 1

///////////////////////////////////////////
// global constants
///////////////////////////////////////////

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class Game;

#include "Constants.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "QuestMgr.h"
#include "RichTextLabel.h"
#include "Sprite.h"
#include "Timer.h"

enum MsgType
{
    MSG_INFO = 0,       // informative messages
    MSG_ALERT,          // messages printed in dangerous situations
    MSG_ERROR,          // command not allowed in that context and similar stuff
    MSG_ACK,            // officer will try to execute the orders
    MSG_FAILURE,        // officer tried to execute the orders, but failed
    MSG_SUCCESS,        // officer tried to execute the orders, and succeeded
    MSG_TASK_COMPLETED, // officer acknowledge completion of a long-running task
                        // (e.g. heal & repair)
    MSG_SKILLUP         // officer got a skill increase
};
#define NUM_MSGTYPES 8

struct VideoMode {
    int width, height;
    bool operator==(const VideoMode &mode) const {
        return mode.width == width && mode.height == height;
    }
};

inline std::string
to_string(const VideoMode &v) {
    return std::to_string(v.width) + "x" + std::to_string(v.height);
}

class Game {
  public:
    Game();
    void Run();
    void shutdown();
    void fatalerror(const std::string &error);
    void message(const std::string &msg);
    ALLEGRO_BITMAP *GetBackBuffer() { return m_backbuffer; }
    void set_vibration(int value) { vibration = value; }
    bool add_resources();

    Sprite *cursor;
    float CrossModuleAngle; // Holds entry angle for systems

    void TogglePauseMenu();
    void SetTimePaused(bool v);
    bool getTimePaused() { return timePause; }
    bool ResizeDisplay(int x, int w);

    static QuestMgr *questMgr;
    static GameState *gameState;
    static ModeMgr *modeMgr;
    static DataMgr *dataMgr;
    static AudioSystem *audioSystem;

    class ModulePlanetSurface *PlanetSurfaceHolder;

    std::shared_ptr<ALLEGRO_FONT> font10;
    std::shared_ptr<ALLEGRO_FONT> font12;
    std::shared_ptr<ALLEGRO_FONT> font18;
    std::shared_ptr<ALLEGRO_FONT> font20;
    std::shared_ptr<ALLEGRO_FONT> font22;
    std::shared_ptr<ALLEGRO_FONT> font24;
    std::shared_ptr<ALLEGRO_FONT> font32;
    std::shared_ptr<ALLEGRO_FONT> font48;
    std::shared_ptr<ALLEGRO_FONT> font60;

    void PrintDefault(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = WHITE);
    void Print(
        ALLEGRO_BITMAP *dest,
        std::shared_ptr<ALLEGRO_FONT> _font,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void Print12(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void Print18(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void Print20(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void Print22(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void Print24(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void Print32(
        ALLEGRO_BITMAP *dest,
        int x,
        int y,
        const std::string &text,
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255),
        bool shadow = false);
    void clear_printout();
    void printout(
        const std::string &text,
        ALLEGRO_COLOR color = WHITE,
        long delay = 0);
    void printout(
        OfficerType officer,
        const std::string &text,
        ALLEGRO_COLOR color = WHITE,
        long delay = 0);
    ALLEGRO_COLOR MsgColors[NUM_MSGTYPES];

    // used to retrieve global values from script file globals.lua
    std::string getGlobalString(const std::string &name);
    void setGlobalString(const std::string &name, const std::string &value);
    double getGlobalNumber(const std::string &name);
    void setGlobalNumber(const std::string &name, double value);
    bool getGlobalBoolean(const std::string &name);
    void setGlobalBoolean(const std::string &name, bool value);

    Timer globalTimer;

    // used to enable/disable the control panel (when in use)
    bool ControlPanelActivity;

    // used by the ModeMgr sanity checks
    bool IsRunning() { return m_keepRunning; }

    // on/off toggle of some gui elements to reduce vertical visual clutter
    bool doShowControls() { return showControls; }
    void toggleShowControls();

    // used to itemize detected video modes reported by the DirectX driver for
    // use in Settings
    std::list<VideoMode> videomodes;
    int desktop_width, desktop_height, desktop_colordepth;
    int actual_width, actual_height;
    bool Initialize_Graphics();
    void LoadModule(const std::string &new_module);
    void LoadModule(const std::string_view &new_module) {
        LoadModule(std::string(new_module));
    }
    void broadcast_event(ALLEGRO_EVENT *event);
    void enable_pause_menu(bool enable);
    void set_pause(bool pause) { m_pause = pause; }

    VideoMode get_video_mode() const { return m_video_mode; }
    std::list<VideoMode> get_video_modes() const { return videomodes; }

  protected:
    void Stop();
    virtual bool InitGame();
    virtual void DestroyGame();
    virtual void RunGame();

  private:
    void UpdateAlienRaceAttitudes();

    bool showControls;
    bool m_keepRunning;
    bool m_pause;
    bool timePause;      // set for modules in which game time should not update
                         // (starport, etc.)
    int timeRateDivisor; // was (static) 'update_interval' in Game::RunGame.
    int vibration;

    ALLEGRO_DISPLAY *m_display;
    // primary drawing surface for all modules
    ALLEGRO_BITMAP *m_backbuffer;
    float m_backbuffer_scale;
    int m_backbuffer_x_offset;
    int m_backbuffer_y_offset;

    // the same as the primary surface, except it doesn't have a mouse on it
    // not to be rude but who is the idiot who came up with this solution?

    // vars used for framerate calculation
    int frameCount, startTime, frameRate;
    double screen_scaling, scale_width, scale_height;

    void CalculateFramerate();
    bool InitializeModules();

    // LUA state object used to read global settings
    Script *globals;

    std::string p_title;
    std::string p_version;

    ALLEGRO_TIMER *m_fps_timer;
    ALLEGRO_EVENT_QUEUE *m_event_queue;
    ALLEGRO_EVENT_SOURCE m_user_event_source;

    std::vector<std::pair<int, int>> m_last_button_downs;
    VideoMode m_video_mode;
    std::shared_ptr<class ModuleMessageGUI> m_message_gui;
    std::shared_ptr<class ModuleAuxiliaryDisplay> m_auxiliary_display;
};

extern Game *g_game;

#endif
