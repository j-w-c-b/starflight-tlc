#pragma once
#include <string>

#include <allegro5/allegro.h>

#include "Sprite.h"

const short BEHAVIOR_WANDER = 0;
const short BEHAVIOR_FLEE = 1;
const short BEHAVIOR_ATTACK = 2;

class CombatObject : public Sprite {
  public:
    CombatObject();
    ~CombatObject();

    void Update();
    void TimedUpdate();
    bool CheckCollision(CombatObject *otherPSO);
    void ApplyImpact(CombatObject *incoming);
    void AllStop() {
        velX = 0.0;
        velY = 0.0;
    }
    void TurnLeft();
    void TurnRight();
    void LimitVelocity();
    void ApplyThrust();
    void ApplyBraking();
    double getRelativeSpeed();

    // accessors
    int getID() const { return id; }
    std::string getName() const { return name; }
    double getDamage() const { return damage; }
    int getHealth() const { return health; }
    double getMass() const { return mass; }
    double getMaxVelocity() const { return maxVelocity; }
    double getTurnRate() const { return turnrate; }
    double getAcceleration() const { return accel; }
    int getBehavior() const { return behavior; }
    int getLaserFiringRate() const { return laserFiringRate; }
    int getLaserFiringTimer() const { return laserFiringTimer; }
    int getLaserDamage() const { return laserDamage; }
    int getLaserModifier() const { return laserModifier; }
    int getMissileFiringRate() const { return missileFiringRate; }
    int getMissileFiringTimer() const { return missileFiringTimer; }
    int getMissileDamage() const { return missileDamage; }
    int getMissileModifier() const { return missileModifier; }
    int getShieldStrength() const { return shieldStrength; }
    int getArmorStrength() const { return armorStrength; }

    // mutators
    void setID(int initID) { id = initID; }
    void setName(const std::string &initName) { name = initName; }
    void setDamage(double initDamage) { damage = initDamage; }
    void setHealth(int initHealth) { health = initHealth; }
    void setAlpha(bool initAlpha) { UseAlpha = initAlpha; }
    void setExpireDuration(int time) { expireDuration = time; }
    void setMass(double value) { mass = value; }
    void setRotation(double value) { rotation = value; }
    void setMaxVelocity(double value) { maxVelocity = value; }
    void setTurnRate(double value) { turnrate = value; }
    void setAcceleration(double value) { accel = value; }
    void setBehavior(int value) { behavior = value; }
    void setLaserFiringRate(int value) { laserFiringRate = value; }
    void setLaserFiringTimer(int value) { laserFiringTimer = value; }
    void setLaserDamage(int value) { laserDamage = value; }
    void setLaserModifier(int value) { laserModifier = value; }
    void setMissileFiringRate(int value) { missileFiringRate = value; }
    void setMissileFiringTimer(int value) { missileFiringTimer = value; }
    void setMissileDamage(int value) { missileDamage = value; }
    void setMissileModifier(int value) { missileModifier = value; }
    void setShieldStrength(int value) { shieldStrength = value; }
    void setArmorStrength(int value) { armorStrength = value; }

  protected:
    int behavior;
    int id;
    std::string name;
    double damage;
    double mass;
    int health;
    bool UseAlpha;
    int expireStartTime;
    int expireDuration;
    double rotation;
    double rotateAngle;
    double maxVelocity;
    double collisionRadius;
    double turnrate;
    double accel;
    int laserFiringRate;
    int laserDamage;
    int laserModifier;
    int laserFiringTimer;
    int missileFiringRate;
    int missileDamage;
    int missileModifier;
    int missileFiringTimer;
    int shieldStrength;
    int armorStrength;
};
// vi: ft=cpp
