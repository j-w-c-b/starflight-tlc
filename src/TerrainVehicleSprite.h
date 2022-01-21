/*
        STARFLIGHT - THE LOST COLONY
        TerrainVehicleSprite.h
        Author: J.Harbour
        Date: Nov, 2007
*/

#ifndef TERRAINVEHICLESPRITE_H
#define TERRAINVEHICLESPRITE_H

#include "Sprite.h"
#include <allegro5/allegro.h>
#include <string>

class TerrainVehicleSprite {
  private:
    Sprite vehicle;
    float forward_thrust;
    float reverse_thrust;
    float turnrate;

  public:
    ~TerrainVehicleSprite();
    TerrainVehicleSprite();
    void applythrust();
    void limitvelocity();
    void cruise();
    void allstop();
    void turnleft();
    void turnright();
    void applybraking();

    void draw(ALLEGRO_BITMAP *dest);
    float getVelocityX();
    float getVelocityY();
    float getRotationAngle();

    void reset();
};

#endif
