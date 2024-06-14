#pragma once
#define PI 3.14159265
double clamp(double min, double max, double value);

struct odomVector{
    double x;
    double y;
    double heading;
};


double normalizeAngle(double rotation);