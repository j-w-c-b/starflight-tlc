/*
        STARFLIGHT - THE LOST COLONY
        ModeMgr.h - A mode consists of a tree of modules.  Multiple modes are
   configured at application startup. One mode is active at any given time.  The
   modules in the active mode are the ones which are run. Switching modes will
   deactivate all modules in the current mode and then activate all modules in
        the new mode and begin running them.

        Author: ?
        Date: ?

*/

#ifndef MODEMGR_H
#define MODEMGR_H

#include <map>
#include <string>

#include "AudioSystem.h"
#include "Game.h"
#include "Module.h"
#include "PauseMenu.h"

// mode names defined here
constexpr std::string_view MODULE_STARTUP = "STARTUP";
constexpr std::string_view MODULE_TITLESCREEN = "TITLESCREEN";
constexpr std::string_view MODULE_CREDITS = "CREDITS";
constexpr std::string_view MODULE_CAPTAINCREATION = "CAPTAINCREATION";
constexpr std::string_view MODULE_CAPTAINSLOUNGE = "CAPTAINSLOUNGE";
constexpr std::string_view MODULE_STARMAP = "STARMAP";
constexpr std::string_view MODULE_HYPERSPACE = "HYPERSPACE";
constexpr std::string_view MODULE_INTERPLANETARY = "INTERPLANETARY";
constexpr std::string_view MODULE_ORBIT = "PLANETORBIT";
constexpr std::string_view MODULE_SURFACE = "PLANETSURFACE";
constexpr std::string_view MODULE_PORT = "STARPORT";
constexpr std::string_view MODULE_SHIPCONFIG = "SHIPCONFIG";
constexpr std::string_view MODULE_STARPORT = "STARPORT";
constexpr std::string_view MODULE_CREWBUY = "CREWHIRE";
constexpr std::string_view MODULE_BANK = "BANK";
constexpr std::string_view MODULE_TRADEDEPOT = "TRADEDEPOT";
constexpr std::string_view MODULE_GAMEOVER = "GAMEOVER";
constexpr std::string_view MODULE_CANTINA = "CANTINA";
constexpr std::string_view MODULE_RESEARCHLAB = "RESEARCHLAB";
constexpr std::string_view MODULE_MILITARYOPS = "MILITARYOPS";
constexpr std::string_view MODULE_ENCOUNTER = "ENCOUNTER";
constexpr std::string_view MODULE_AUXILIARYDISPLAY = "AUXILIARYDISPLAY";
constexpr std::string_view MODULE_SETTINGS = "SETTINGS";
constexpr std::string_view MODULE_MESSAGEGUI = "MESSAGEGUI";

class Mode final {
  public:
    Mode(std::shared_ptr<Module> module, const std::string &path);

  private:
    std::shared_ptr<Module> rootModule;
    std::string musicPath;

    friend class ModeMgr;
};

class ModeMgr : public Module {
  public:
    explicit ModeMgr(Game *game);
    virtual ~ModeMgr();

    void AddMode(
        const std::string_view &modeName,
        std::shared_ptr<Module> rootModule,
        const std::string &musicPath);
    bool LoadModule(const std::string &moduleName);
    void CloseCurrentModule();

    std::string GetCurrentModuleName() { return currentModeName; }
    std::string GetPrevModuleName() { return prevModeName; }

    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;

    void enable_pause_menu(bool enable);
    void toggle_pause_menu();

  private:
    std::shared_ptr<Module> m_activeRootModule;
    std::shared_ptr<Sample> currentMusic;

    std::string prevModeName;
    std::string currentModeName;

    // this is the list of game modes in the game
    std::map<std::string, std::shared_ptr<Mode>> m_modes;
    std::shared_ptr<PauseMenu> m_pause_menu;
    std::shared_ptr<Module> m_previous_modal;
    bool m_originally_paused;
};

#endif
