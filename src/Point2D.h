/*
        STARFLIGHT - THE LOST COLONY
        Point2D.h - A simple class for 2d points, consists of only a few methods
   and operations. Author: ? Date: ?
*/

#ifndef POINT2D_H
#define POINT2D_H

#include <cmath>
#include <stdexcept>
#include <string>
#include <system_error>

#include "Archive.h"

///////////////////////////////////////////////////////////////////////////////
// Point2D
//
// This class represents a basic two dimensional point.
///////////////////////////////////////////////////////////////////////////////

class Point2D {
  public:
    double x, y;
    static constexpr std::string_view ClassName = "Point2D";

    Point2D() {}
    Point2D(const Point2D &p) { *this = p; }
    Point2D(double nx, double ny) { SetPosition(nx, ny); }

    void Reset() { x = y = 0; }

    friend InputArchive &operator>>(InputArchive &archive, Point2D &point);
    friend OutputArchive &
    operator<<(OutputArchive &archive, const Point2D &point);

    void SetPosition(double nx, double ny) {
        this->x = nx;
        this->y = ny;
    }

    bool operator==(const Point2D &p) const {
        const double EPSILON = 0.0001;

        return (
            (((p.x - EPSILON) < x) && (x < (p.x + EPSILON)))
            && (((p.y - EPSILON) < y) && (y < (p.y + EPSILON))));
    }

    bool operator!=(const Point2D &p) const { return (!(*this == p)); }

    Point2D &operator=(const Point2D &p) {
        x = p.x;
        y = p.y;
        return *this;
    }
    Point2D operator+(const Point2D &p) const {
        return Point2D(x + p.x, y + p.y);
    }
    Point2D operator+(float delta) const {
        return Point2D(x + delta, y + delta);
    }
    Point2D operator-(const Point2D &p) const {
        return Point2D(x - p.x, y - p.y);
    }
    Point2D operator-(float delta) const {
        return Point2D(x - delta, y - delta);
    }
    Point2D operator/(float scale) const {
        return Point2D(x / scale, y / scale);
    }

    static double Distance(const Point2D &a, const Point2D &b) {
        return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    }
};

#endif // POINT2D_H
