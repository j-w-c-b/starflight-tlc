#include <cmath>

#include "MathTL.h"

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
Math::Distance(double x1, double y1, double x2, double y2) {
    double deltaX = (x2 - x1);
    double deltaY = (y2 - y1);
    return hypot(deltaX, deltaY);
}

double
Math::Length(double x, double y, double z) {
    return sqrt(x * x + y * y + z * z);
}
// vi: ft=cpp
