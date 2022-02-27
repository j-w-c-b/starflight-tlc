#pragma once

const double PI = 3.1415926535;
const double PI_over_180 = PI / 180.0f;
const double PI_under_180 = 180.0f / PI;

class Math {
  public:
    static double toDegrees(double radian);
    static double wrapAngleDegs(double degs);
    static double AngleToTarget(double x1, double y1, double x2, double y2);
    static double Distance(double x1, double y1, double x2, double y2);
    static double Length(double x, double y, double z);
};
// vi: ft=cpp
