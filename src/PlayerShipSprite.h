/*
        STARFLIGHT - THE LOST COLONY
        PlayerShipSprite.h
        Author: J.Harbour
        Date: 2007
*/

#ifndef PLAYERSHIPSPRITE_H
#define PLAYERSHIPSPRITE_H

#include "Script.h"
#include "Sprite.h"
#include "Timer.h"
#include <allegro5/allegro.h>
#include <string>

class PlayerShipSprite {
  private:
    Sprite ship;
    double maximum_velocity;
    double forward_thrust;
    double reverse_thrust;
    double lateral_thrust;
    double turnrate;

    Timer timer, braking_timer;

  public:
    ~PlayerShipSprite();
    PlayerShipSprite();
    void applythrust();
    void limitvelocity();
    void cruise();
    void allstop();
    void turnleft();
    void turnright();
    void starboard();
    void port();
    void applybraking();

    void draw(ALLEGRO_BITMAP *dest);
    double getX() { return ship.getX(); }
    double getY() { return ship.getY(); }
    Point2D get_screen_position() { return Point2D(ship.getX(), ship.getY()); }
    void setX(double value) { ship.setX(value); }
    void setX(int value) { ship.setX(value); }
    void setY(double value) { ship.setY(value); }
    void setY(int value) { ship.setY(value); }
    float getVelocityX();
    float getVelocityY();
    void setVelocityX(float value);
    void setVelocityY(float value);
    float getRotationAngle();
    double getCurrentSpeed();

    void reset();

    double getMaximumVelocity();
    double getForwardThrust();
    double getReverseThrust();
    double getLateralThrust();
    double getTurnRate();
};

#endif
