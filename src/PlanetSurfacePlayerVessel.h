#pragma once

#include <string>

#include "PlanetSurfaceObject.h"

class PlanetSurfacePlayerVessel : public PlanetSurfaceObject {
  public:
    PlanetSurfacePlayerVessel(lua_State *LuaVM, const std::string &ScriptName);

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
};
// vi: ft=cpp
