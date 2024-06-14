#pragma once
#include "main.h"
#include "util.hpp"

/*Global variables accossiated with the PID controller*/
extern double error, prevError, threashold, derivative, integral, cycleTime, output;
double angleDistanceToZero(double angle);
double getAngleDistance(double angle1, double angle2);
/*Turns to a certain absolute degree measure with a PID controller and IMU
* @param target desired degree the robot is reaching
* @param waitUntilSettled a boolean allowing control over settling
*/
void turn(double target, bool waitUntilSettled);
/*Moves to a certain relative encoder degree measure with a PID controller and an optical shaft encoder
* @param target desired relative encoder degree the robot is reaching
* @param waitUntilSettled a boolean allowing control over settling
*/
void move(double target, bool waitUntilSettled);
/*Turns to desired point using Odometry to determine angle and PID Control to execute
* @param desiredPoint the point you want to turn twoards; expressed in a custom struct
* @param currentPoint the current posititon of the robot; expressed in a custom struct
* @param waitUntilSettled a boolean allowing control over settling
*/
double angleBetweenPoints(odomVector desiredPoint, odomVector currentPoint);
void turnToPoint(odomVector desiredPoint, bool waitUntilSettled);
/*Turns to desired point, moves distance to disired point, and turns to heading stated in disiredPoint
* @param desiredPoint the point you want to  move to; expressed in a custom struct
* @param currentPoint the current posititon of the robot; expressed in a custom struct
* @param waitUntilSettled a boolean allowing control over settling
*/
void moveToPoint(odomVector desiredPoint, bool waitUntilSettled);