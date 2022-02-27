
#include "PlanetSurfacePlayerVessel.h"

PlanetSurfacePlayerVessel::PlanetSurfacePlayerVessel(
    lua_State *LuaVM,
    std::string ScriptName)
    : PlanetSurfaceObject(LuaVM, ScriptName), forwardThrust(0),
      reverseThrust(0), turnRight(0), turnLeft(0) {}

PlanetSurfacePlayerVessel::~PlanetSurfacePlayerVessel() {}

void
PlanetSurfacePlayerVessel::Move() {
    PlanetSurfaceObject::Move();
}

void
PlanetSurfacePlayerVessel::Draw(ALLEGRO_BITMAP *Canvas) {
    PlanetSurfaceObject::Draw(Canvas);
}
