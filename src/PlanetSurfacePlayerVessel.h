#pragma once

#include "PlanetSurfaceObject.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <list>
#include <string>
#include <vector>

class PlanetSurfacePlayerVessel : public PlanetSurfaceObject {
  public:
    PlanetSurfacePlayerVessel(lua_State *LuaVM, std::string ScriptName);

    virtual ~PlanetSurfacePlayerVessel();

    virtual void Draw(ALLEGRO_BITMAP *Canvas) override;
    virtual void Move() override;

    void ForwardThrust(bool active) { forwardThrust = active; }
    void ReverseThrust(bool active) { reverseThrust = active; }
    void TurnRight(bool active) { turnRight = active; }
    void TurnLeft(bool active) { turnLeft = active; }
    void ResetNav() {
        forwardThrust = reverseThrust = turnRight = turnLeft = false;
    }

    bool ForwardThrust() const { return forwardThrust; }
    bool ReverseThrust() const { return reverseThrust; }
    bool TurnRight() const { return turnRight; }
    bool TurnLeft() const { return turnLeft; }

  protected:
    bool forwardThrust;
    bool reverseThrust;
    bool turnRight;
    bool turnLeft;

  private:
};
