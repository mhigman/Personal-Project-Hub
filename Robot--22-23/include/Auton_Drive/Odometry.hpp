#pragma once

#include "main.h"
#include "util.hpp"
/*
class odomTwoEncoderImuController{
    public:
    std::function<double()> getImuHeading;
    pros::ADIEncoder mid;
    pros::ADIEncoder right;
    odomTwoEncoderImuController();
    
    //void setSensors(std::function<double()> &getImuHeading, pros::ADIEncoder imid, pros::ADIEncoder iright);
    //Representing a position vector (x, y, rotation) relative to origin.
    struct odomVector {
        double x;
        double y;
        double heading;
    };

    odomVector get_robot_position();

    void step();

    void zero_everything();

    odomVector stepMath();
    private:

    odomVector totalVector, prevVector, localOffset, globalOffset;
    double offsetR;
    double offsetM;
    double r, m, prevL, prevR, prevM, deltaR, deltaM, totalR, totalM;
    double totalHeading, prevHeading, deltaHeading, headingAtLastReset, averageHeading, polarRadius, polarAngle;
    odomVector addVector(odomVector vector1, odomVector vector2);
    
};
*/

extern odomVector totalVector, prevVector, localOffset, globalOffset;
extern const double offsetR;
extern const double offsetM;
extern double r, m, prevL, prevR, prevM, deltaR, deltaM, totalR, totalM;
extern double totalHeading, prevHeading, deltaHeading, headingAtLastReset, averageHeading; 
extern double polarRadius, polarAngle;
odomVector get_robot_position();
odomVector stepMath();
odomVector addVector(odomVector vector1, odomVector vector2);
void step();
void zero_everything();
