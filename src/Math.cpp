#include "MathTL.h"
#include "Vector3.h"
#include <cmath>

double
Math::toDegrees(double radians) {
    return radians * PI_under_180;
}
double
Math::wrapAngleDegs(double degs) {
    double result = fmod(degs, 360.0);
    if (result < 0)
        result += 360.0f;
    return result;
}

double
Math::AngleToTarget(double x1, double y1, double x2, double y2) {
    double deltaX = (x2 - x1);
    double deltaY = (y2 - y1);
    return atan2(deltaY, deltaX);
}

double
Math::AngleToTarget(Vector3 &source, Vector3 &target) {
    return AngleToTarget(
        source.getX(), source.getY(), target.getX(), target.getY());
}

double
Math::Distance(double x1, double y1, double x2, double y2) {
    double deltaX = (x2 - x1);
    double deltaY = (y2 - y1);
    return sqrt(deltaX * deltaX + deltaY * deltaY);
}

double
Math::Distance(Vector3 &vec1, Vector3 &vec2) {
    return Distance(vec1.getX(), vec1.getY(), vec2.getX(), vec2.getY());
}

double
Math::Length(double x, double y, double z) {
    return sqrt(x * x + y * y + z * z);
}

double
Math::Length(Vector3 &vec) {
    return Length(vec.getX(), vec.getY(), vec.getZ());
}

double
Math::DotProduct(
    double x1,
    double y1,
    double z1,
    double x2,
    double y2,
    double z2) {
    return (x1 * x2 + y1 * y2 + z1 * z2);
}

double
Math::DotProduct(Vector3 &vec1, Vector3 &vec2) {
    return DotProduct(
        vec1.getX(),
        vec1.getY(),
        vec1.getZ(),
        vec2.getX(),
        vec2.getY(),
        vec2.getZ());
}

Vector3
Math::CrossProduct(
    double x1,
    double y1,
    double z1,
    double x2,
    double y2,
    double z2) {
    double nx = (y1 * z2) - (z1 * y2);
    double ny = (z1 * y2) - (x1 * z2);
    double nz = (x1 * y2) - (y1 * x2);
    return Vector3(nx, ny, nz);
}

Vector3
Math::CrossProduct(Vector3 &vec1, Vector3 &vec2) {
    return CrossProduct(
        vec1.getX(),
        vec1.getY(),
        vec1.getZ(),
        vec2.getX(),
        vec2.getY(),
        vec2.getZ());
}

Vector3
Math::Normal(double x, double y, double z) {
    double length = Length(x, y, z);
    if (length != 0)
        length = 1 / length;
    double nx = x * length;
    double ny = y * length;
    double nz = z * length;
    return Vector3(nx, ny, nz);
}

Vector3
Math::Normal(Vector3 &vec) {
    return Normal(vec.getX(), vec.getY(), vec.getZ());
}
