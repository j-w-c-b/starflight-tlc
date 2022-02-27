#pragma once

#include "DataMgr.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <map>
#include <string>
#include <vector>

#include "lua.hpp"

class Sprite;
class Event;
class Rect;

struct Animation {
    Animation(int Start_Frame, int End_Frame, int Delay)
        : start_frame(Start_Frame), end_frame(End_Frame), delay(Delay) {}

    int start_frame;
    int end_frame;
    int delay;

    int GetStartFrame() const { return start_frame; }
    int GetEndFrame() const { return end_frame; }
    int GetDelay() const { return delay; }
};

class PlanetSurfaceObject {
  public:
    PlanetSurfaceObject();
    PlanetSurfaceObject(lua_State *LuaVM, std::string ScriptName);

    virtual ~PlanetSurfaceObject();

    void Initialize();

    int load(const char *filename);
    virtual void Animate();
    virtual void Move();

    virtual void Update();
    virtual void TimedUpdate();
    virtual void Draw();
    virtual void Draw(ALLEGRO_BITMAP *Canvas);
    virtual bool CheckCollision(PlanetSurfaceObject *otherPSO);
    virtual bool CheckCollision(int x, int y, int width, int height);
    virtual void
    AddAnimation(std::string name, int startFrame, int endFrame, int delay);
    virtual void SetActiveAnimation(std::string name);

    virtual bool OnMouseReleased(int button, int x, int y);
    virtual void getActions();
    virtual void OnEvent(int event);
    virtual void Scan();

    int Inside(int x, int y, int left, int top, int right, int bottom);
    int PointInside(int px, int py);

    static void EmptyGraphics();

    static int maxX, minX, maxY, minY;

    // accessors
    int getID() const { return id; }
    std::string getName() const { return name; }
    double getValue() const { return value; }
    double getSize() const { return size; }
    double getDangerLvl() const { return danger; }
    double getDamage() const { return damage; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool IsShipRepairMetal() const { return shipRepairMetal; }
    bool IsBlackMarketItem() const { return blackMarketItem; }
    std::string GetScriptName() const { return scriptName; }
    bool IsScanned() const { return scanned; }
    int getStunCount() const { return stunCount; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getXOffset() const { return x + (frameWidth * scale) / 2; }
    double getYOffset() const { return y + (frameHeight * scale) / 2; }
    double getSpeed() const { return speed; }
    double getVelX() const { return velX; }
    double getVelY() const { return velY; }
    float getFaceAngle() const { return faceAngle; }
    float getMoveAngle() const { return moveAngle; }
    int isAlive() const { return alive; }
    int getState() const { return state; }
    int getObjectType() const { return objectType; }
    int getColHalfWidth() const { return (int)(colHalfWidth * scale); }
    int getColHalfHeight() const { return (int)(colHalfHeight * scale); }
    double getScale() const { return scale; }
    int getCurrFrame() const { return currFrame; }
    int getTotalFrames() const { return totalFrames; }
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }
    int getCounter1() const { return counter1; }
    int getCounter2() const { return counter2; }
    int getCounter3() const { return counter3; }
    int getThreshold1() const { return threshold1; }
    int getThreshold2() const { return threshold2; }
    int getThreshold3() const { return threshold3; }

    ALLEGRO_COLOR
    getMinimapColor() const { return minimapColor; }
    int getMinimapSize() const { return minimapSize; }

    // mutators
    void setID(int initID) { id = initID; }
    void setItemType(ItemType initItemType) { itemType = initItemType; }
    void setName(const std::string &initName) { name = initName; }
    void setScriptName(const std::string &initName) { scriptName = initName; }
    void setValue(double initValue) { value = initValue; }
    void setSize(double initSize) { size = initSize; }
    void setDangerLvl(double initDangerLvl) { danger = initDangerLvl; }
    void setDamage(double initDamage) { damage = initDamage; }
    void setHealth(int initHealth) { health = initHealth; }
    void setMaxHealth(int initHealth) { maxHealth = initHealth; }
    void setItemAge(ItemAge initItemAge) { itemAge = initItemAge; }
    void setAsShipRepairMetal(bool initRepairMetal) {
        shipRepairMetal = initRepairMetal;
    }
    void setAsBlackMarketItem(bool initBlackMarket) {
        blackMarketItem = initBlackMarket;
    }
    void setScanned(bool initScanned) { scanned = initScanned; }
    void setStunCount(int initStunCount) { stunCount = initStunCount; }
    void setSelected(bool initSelected) { selected = initSelected; }
    // The set positions are in the cpp
    void setX(double initX);
    void setY(double initY);
    void setPos(double initX, double initY);
    void setPosOffset(double initX, double initY);

    void setSpeed(double initSpeed) { speed = initSpeed; }
    void setVelX(double initVelX) { velX = initVelX; }
    void setVelY(double initVelY) { velY = initVelY; }
    void setFaceAngle(float initFaceAngle) { faceAngle = initFaceAngle; }
    void setMoveAngle(float initMoveAngle) { moveAngle = initMoveAngle; }
    void setAngleOffset(double initAngleOffset) {
        angleOffset = initAngleOffset;
    }
    void setAlive(int initAlive) { alive = initAlive; }
    void setState(int initState) { state = initState; }
    void setObjectType(int initObjectType) { objectType = initObjectType; }
    void setWidth(int initWidth) { width = initWidth; }
    void setHeight(int initHeight) { height = initHeight; }
    void setColHalfWidth(int initHalfWidth) { colHalfWidth = initHalfWidth; }
    void setColHalfHeight(int initHalfHeight) {
        colHalfHeight = initHalfHeight;
    }
    void setScale(double initScale) { scale = initScale; }
    void setCurrFrame(int initCurrFrame) { currFrame = initCurrFrame; }
    void setTotalFrames(int initTotalFrames) { totalFrames = initTotalFrames; }
    void setFrameDelay(int initFrameDelay) { frameDelay = initFrameDelay; }
    void setFrameWidth(int initFrameWidth) { frameWidth = initFrameWidth; }
    void setFrameHeight(int initFrameHeight) { frameHeight = initFrameHeight; }
    void setAnimColumns(int initAnimColumns) { animColumns = initAnimColumns; }
    void setCounter1(int initCounter1) { counter1 = initCounter1; }
    void setCounter2(int initCounter2) { counter2 = initCounter2; }
    void setCounter3(int initCounter3) { counter3 = initCounter3; }
    void setThreshold1(int initThreshold1) { threshold1 = initThreshold1; }
    void setThreshold2(int initThreshold2) { threshold2 = initThreshold2; }
    void setThreshold3(int initThreshold3) { threshold3 = initThreshold3; }

    void setMinimapColor(ALLEGRO_COLOR initColor) { minimapColor = initColor; }
    void setMinimapSize(int initSize) { minimapSize = initSize; }

    std::string description;

  protected:
    std::string scriptName;
    lua_State *luaVM;

    std::map<std::string, Animation *> regAnimations;
    std::map<std::string, Animation *>::iterator animationsIt;

    Animation *defaultAnim;
    Animation *activeAnim;

    int id;
    ItemType itemType; // the type of item
    std::string name;
    double value;
    double size; // size (m^3)
    double danger;
    double damage;
    int health;
    int maxHealth;
    ItemAge itemAge;
    bool shipRepairMetal; // is this a ship repair metal?
    bool blackMarketItem; // is this a blackmarket item?
    bool selected;
    bool scanned;
    ALLEGRO_COLOR minimapColor; // Minimap dot color
    int minimapSize;            // Size of the minimap dot

    static std::map<std::string, ALLEGRO_BITMAP *> graphics;

    ALLEGRO_BITMAP *image;
    bool moved;
    double x;
    double y;
    int stunCount;
    double speed;
    double velX;
    double velY;
    float faceAngle;
    float moveAngle;
    float angleOffset;
    int alive;
    int state;
    int objectType;
    int direction;
    int width;
    int height;
    int colHalfWidth;
    int colHalfHeight;
    double scale;
    int delayX;
    int delayY;
    int countX;
    int countY;
    int currFrame;
    int totalFrames;
    int animDir;
    int frameCount;
    int frameDelay;
    int frameWidth;
    int frameHeight;
    int animColumns;
    int animStartX;
    int animStartY;
    int counter1;
    int counter2;
    int counter3;
    int threshold1;
    int threshold2;
    int threshold3;

  private:
};
// vi: ft=cpp
