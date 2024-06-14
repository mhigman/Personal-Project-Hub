#include "main.h"
#include "math.h"
double clamp(double min, double max, double value){
    return value < min ? min : value > max ? max : value;
}


double normalizeAngle(double rotation){
    double normalizedAngle = PI/180 * (rotation);

    return ((std::atan2(std::sin(normalizedAngle), std::cos(normalizedAngle))) * 180/PI);
}