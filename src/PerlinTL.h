/*
        STARFLIGHT - THE LOST COLONY
*/

#ifndef PERLINTL_H
#define PERLINTL_H 1

#include "DataMgr.h"

#include "noise/noise.h"
#include "noiseutils.h"

void
createPlanetSurface(
    int width,
    int height,
    int randomness,
    PlanetType planet_type,
    std::string filename);
void
createPlanetSurface(
    int width,
    int height,
    int randomness,
    PlanetType planet_type,
    const char *filename);

#endif // PERLINTL_H
// vi: ft=cpp
