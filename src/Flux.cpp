/*
        STARFLIGHT - THE LOST COLONY
        Class: Flux
        Author: Keith Patch
        Date: 10-29-2007
*/

#include "Flux.h"
#include "cmath"

static inline float
distance_between_coords(const Point2D &a, const Point2D &b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

float
Flux::distance_to_endpoint_1(const Point2D &coordinate) const {
    return distance_between_coords(m_endpoint1, coordinate);
}

float
Flux::distance_to_endpoint_2(const Point2D &coordinate) const {
    return distance_between_coords(m_endpoint2, coordinate);
}
