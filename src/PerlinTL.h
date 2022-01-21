/*
        STARFLIGHT - THE LOST COLONY
*/

#ifndef _PERLIN_H
#define _PERLIN_H 1

#include "DataMgr.h"

#include "noiseutils.h"
#include <noise/noise.h>

void createPlanetSurface(int width,
                         int height,
                         int randomness,
                         PlanetType planet_type,
                         std::string filename);
void createPlanetSurface(int width,
                         int height,
                         int randomness,
                         PlanetType planet_type,
                         const char *filename);

#endif
