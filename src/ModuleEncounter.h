/*
        STARFLIGHT - THE LOST COLONY
        ModuleEncounter.h - Handles alien encounters
        Author: J.Harbour
        Date: December, 2007
*/

#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include "AudioSystem.h"
#include "CombatObject.h"
#include "Module.h"
#include "PlayerShipSprite.h"
#include "ResourceManager.h"
#include "Script.h"
#include "ScrollBox.h"
#include "Sprite.h"
#include "TileScroller.h"
#include "Util.h"
#include "lua.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <typeinfo>

class CombatObject;
enum AlienRaces;

const int NormalScreenHeight = 512;
const int FullScreenHeight = SCREEN_HEIGHT;

class ModuleEncounter : public Module {
  private:
    ResourceManager<ALLEGRO_BITMAP> resources;

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
    DropType dropitems[10];

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

    std::shared_ptr<Sample> snd_player_laser;
    std::shared_ptr<Sample> snd_player_missile;
    std::shared_ptr<Sample> snd_explosion;
    std::shared_ptr<Sample> snd_laserhit;

    Script *script;

    ScrollBox::ScrollBox *text;
    ScrollBox::ScrollBox *dialogue;
    bool bFlagDialogue; // used to draw either message output or scrollbox input

    bool bFlagLastStatementSuccess; // continue showing current statement until
                                    // player uses it
    bool bFlagLastQuestionSuccess;  // continue showing current question until
                                    // player uses it
    bool bFlagDoResponse;           // handle alien response
    bool bFlagDoAttack;             // handle attack action
    bool bFlagChatting; // tracks whether player is chatting with alien or not

    void DrawMinimap();
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
    void Encounter_Draw();
    void Combat_Draw();
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

    void createLaser(CombatObject *laser,
                     double x,
                     double y,
                     float velx,
                     float vely,
                     int angle,
                     int laserDamage);
    void createMissile(CombatObject *missile,
                       double x,
                       double y,
                       float velx,
                       float vely,
                       int angle,
                       int missileDamage);

    // shortcuts to crew last names to simplify code
    std::string com;
    std::string sci;
    std::string nav;
    std::string tac;
    std::string eng;
    std::string doc;

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
    ModuleEncounter(void);
    ~ModuleEncounter(void);
    bool Init() override;
    void Update() override;
    void Draw() override;
    void Print(std::string text, int color, long delay);
    void Print(std::string text, ALLEGRO_COLOR color, long delay);
    void OnKeyPress(int keyCode) override;
    void OnKeyReleased(int keyCode) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseClick(int button, int x, int y) override;
    void OnMousePressed(int button, int x, int y) override;
    void OnMouseReleased(int button, int x, int y) override;
    void OnMouseWheelUp(int x, int y) override;
    void OnMouseWheelDown(int x, int y) override;
    void OnEvent(Event *event) override;
    void Close() override;
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

    int
    effectiveScreenHeight() {
        return g_game->doShowControls() ? NormalScreenHeight : FullScreenHeight;
    }

    void adjustVerticalCoords(int delta);

    PlayerShipSprite *playerShip;

    int module_mode;  // 0=encounter; 1=combat
    lua_State *LuaVM; /* the Lua interpreter */

    std::vector<CombatObject *> combatObjects;
    std::vector<CombatObject *>::iterator objectIt;
    TileScroller *scroller;

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
int L_Debug(lua_State *luaVM);     // usage: L_Debug("this is a debug message")
int L_Terminate(lua_State *luaVM); // usage: L_Terminate()
int L_Attack(lua_State *luaVM);    // usage: L_Attack()

#endif
