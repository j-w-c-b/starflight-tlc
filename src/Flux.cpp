/*
        STARFLIGHT - THE LOST COLONY
        Class: Flux
        Author: Keith Patch
        Date: 10-29-2007
*/

#include <cmath>

#include "Flux.h"

static inline float
distance_between_coords(const Point2D &a, const Point2D &b) {
    return hypot(a.x - b.x, a.y - b.y);
}

float
Flux::distance_to_endpoint_1(const Point2D &coordinate) const {
    return distance_between_coords(m_endpoint1, coordinate);
}

float
Flux::distance_to_endpoint_2(const Point2D &coordinate) const {
    return distance_between_coords(m_endpoint2, coordinate);
}
