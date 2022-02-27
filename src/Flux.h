#ifndef STLC_FLUX_H
#define STLC_FLUX_H

#include "Point2D.h"

class Flux {
  public:
    Flux() : m_id(-1), m_endpoint1(-1, -1), m_endpoint2(-1, -1) {}
    Flux(int id, const Point2D &endpoint1, const Point2D &endpoint2)
        : m_id(id), m_endpoint1(endpoint1), m_endpoint2(endpoint2) {}
    Flux(const Flux &other);
    int get_id() const { return m_id; }

    Point2D get_endpoint1() const { return m_endpoint1; }
    Point2D get_endpoint2() const { return m_endpoint2; }
    float distance_to_endpoint_1(const Point2D &coordinate) const;
    float distance_to_endpoint_2(const Point2D &coordinate) const;

  private:
    // Unique identifier of the flux in galaxy.yaml
    int m_id;
    // (x, y) tile coordinates of the two endpoints of the flux.
    Point2D m_endpoint1;
    Point2D m_endpoint2;
};
#endif // STLC_FLUX_H
// vi: ft=cpp
