/*
        STARFLIGHT - THE LOST COLONY
        ModuleEncounter.h - Handles alien encounters
        Author: J.Harbour
        Date: December, 2007
*/

#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include <array>
#include <map>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "CombatObject.h"
#include "DialogButtonPanel.h"
#include "Game.h"
#include "GameState.h"
#include "Module.h"
#include "PlayerShipSprite.h"
#include "Script.h"
#include "ScrolledModule.h"
#include "Sprite.h"
#include "TileScroller.h"
#include "Util.h"
#include "lua.hpp"

const int NormalScreenHeight = 512;
const int FullScreenHeight = SCREEN_HEIGHT;

class ModuleEncounter : public Module {
  private:
    enum PostureStates
    {
        POSTURE_NONE = 0,
        POSTURE_OBSEQUIOUS,
        POSTURE_FRIENDLY,
        POSTURE_HOSTILE,
    };
    enum CommModes
    {
        COMM_NONE = 0,
        COMM_STATEMENT,
        COMM_QUESTION,
        COMM_POSTURE
    };
    struct DropType {
        int id;
        int rate;
        int quantity;
    };
    std::array<DropType, 10> dropitems;

    CommModes commMode;
    PostureStates commPosture;
    int shipcount;
    bool alienHailingUs;

    Point2D playerGlobal, playerScreen;

    Sprite *shield;

    int asw;
    int ash;
    int asx;
    int asy;

    std::string alienName;
    std::string portraitName;
    std::string schematicName;
    std::string shipName;

    Sprite *spr_statusbar_shield;

    Script *script;

    std::shared_ptr<ScrolledModule<RichTextLabel>> m_text;
    std::shared_ptr<DialogButtonPanel> m_dialog;

    bool bFlagDialogue; // used to draw either message output or scrollbox input

    bool bFlagLastStatementSuccess; // continue showing current statement until
                                    // player uses it
    bool bFlagLastQuestionSuccess;  // continue showing current question until
                                    // player uses it
    bool bFlagDoResponse;           // handle alien response
    bool bFlagDoAttack;             // handle attack action
    bool bFlagChatting; // tracks whether player is chatting with alien or not

    void DrawMinimap(ALLEGRO_BITMAP *target);
    std::string replaceKeyWords(std::string input);
    void applyDamageToShip(int damage, bool hullonly = false);

    int getShipCount();
    void damageAlienAttitude();

    bool Encounter_Init();
    bool Combat_Init();
    void Encounter_Close();
    void Combat_Close();
    void Encounter_Update();
    void Combat_Update();
    void Encounter_Draw(ALLEGRO_BITMAP *target);
    void Combat_Draw(ALLEGRO_BITMAP *target);
    void combatDoCollision(CombatObject *first, CombatObject *second);
    void combatDoBigExplosion(CombatObject *victim);
    void combatDoMedExplosion(CombatObject *victim);
    void combatDoSmlExplosion(CombatObject *victim, CombatObject *source);
    void combatDoBreakAsteroid(CombatObject *victim);
    void combatDoPowerup(CombatObject *victim);
    void combatTestPlayerCollision(CombatObject *other);
    void pickupRandomDropItem();
    void pickupAsteroidMineral();
    void ImpactPlayer(CombatObject *player, CombatObject *other);
    void DoAlienShipCombat(CombatObject *ship);
    Rect getBoundary();
    void enemyFireLaser(CombatObject *ship);
    void enemyFireMissile(CombatObject *ship);
    CombatObject *GetFirstAlienShip();

    void createLaser(
        CombatObject *laser,
        double x,
        double y,
        float velx,
        float vely,
        int angle,
        int laserDamage);
    void createMissile(
        CombatObject *missile,
        double x,
        double y,
        float velx,
        float vely,
        int angle,
        int missileDamage);

    bool flag_DoHyperspace;
    int hyperspaceCountdown;
    int deathState;
    bool playerAttacked;
    bool flag_greeting;
    int scanStatus;
    Timer scanTimer;

    int number_of_actions; // question counter
    int goto_question;     // if not 0, jump to this question number overriding
                           // other logic

    bool firingLaser, firingMissile;
    bool flag_thrusting;
    bool flag_nav;
    int flag_rotation;

  public:
    ModuleEncounter();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    void Print(const std::string &text, int color, long delay);
    void Print(const std::string &text, ALLEGRO_COLOR color, long delay);
    void Print(
        OfficerType type,
        const std::string &text,
        ALLEGRO_COLOR color,
        long delay = 0);
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *) override;
    virtual bool on_close() override;
    void commInitStatement();
    void commInitQuestion();
    void commInitPosture();
    void commDoStatement(int index);
    void commDoQuestion(int index);
    void commDoPosture(int index);
    void commDoGreeting();
    void commCheckCurrentAction();
    std::string commGetAction();
    void commDoAlienResponse();
    void commDoAlienAttack();
    void fireLaser();
    void fireMissile();
    void setMissileProperties(CombatObject *ship, int missileclass);
    void setLaserProperties(CombatObject *ship, int laserclass);
    void setArmorProperties(CombatObject *ship, int armorclass);
    void setShieldProperties(CombatObject *ship, int shieldclass);
    void setEngineProperties(CombatObject *ship, int engineclass);
    void sendGlobalsToScript();
    void readGlobalsFromScript();

    int effectiveScreenHeight() {
        return g_game->doShowControls() ? NormalScreenHeight : FullScreenHeight;
    }

    void adjustVerticalCoords(int delta);

    PlayerShipSprite *playerShip;

    int module_mode;  // 0=encounter; 1=combat
    lua_State *LuaVM; /* the Lua interpreter */

    std::vector<CombatObject *> combatObjects;
    std::vector<CombatObject *>::iterator objectIt;
    std::shared_ptr<TileScroller> m_scroller;

    ALLEGRO_BITMAP *minimap;

    void AddCombatObject(CombatObject *CObject);
    void RemoveCombatObject(CombatObject *CObject);

    std::map<std::string, std::string> dialogCensor;
};

/*******************************************************
 *
 * LUA functions
 *
 *******************************************************/

// NOTE: L_Debug is defined in ModulePlanetSurface.cpp
int
L_Debug(lua_State *luaVM); // usage: L_Debug("this is a debug message")
int
L_Terminate(lua_State *luaVM); // usage: L_Terminate()
int
L_Attack(lua_State *luaVM); // usage: L_Attack()

#endif
