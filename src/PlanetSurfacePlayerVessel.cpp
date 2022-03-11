#include "PlanetSurfacePlayerVessel.h"

PlanetSurfacePlayerVessel::PlanetSurfacePlayerVessel(
    lua_State *LuaVM,
    const std::string &ScriptName)
    : PlanetSurfaceObject(LuaVM, ScriptName), forwardThrust(false),
      reverseThrust(false), turnRight(false), turnLeft(false) {}

// vi: ft=cpp
