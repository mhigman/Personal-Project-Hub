#include "main.h"
#include "Auton_Drive/util.hpp"
#include "pros/motors.h"
//Drive stuff
pros::Motor topLeft(14, MOTOR_GEARSET_06, false, MOTOR_ENCODER_DEGREES);
pros::Motor bottomLeft(15, MOTOR_GEARSET_06, false, MOTOR_ENCODER_DEGREES);
pros::Motor topRight(19, MOTOR_GEARSET_06, true, MOTOR_ENCODER_DEGREES);
pros::Motor bottomRight(20, MOTOR_GEARSET_06, true, MOTOR_ENCODER_DEGREES);

pros::ADIEncoder middleEncoder('A', 'B', true);
pros::ADIEncoder rightEncoder('C', 'D', true);

pros::Imu imuSensor(2);


pros::Controller master(CONTROLLER_MASTER);

//Subsystem motors (may not use some of these).
pros::Motor intake(2, MOTOR_GEARSET_06, false, MOTOR_ENCODER_DEGREES);
pros::Motor flywheel(18, MOTOR_GEARSET_06, false, MOTOR_ENCODER_DEGREES);
pros::Motor roller(17, MOTOR_GEARSET_06, false, MOTOR_ENCODER_DEGREES);
pros::Motor indexer(15, MOTOR_GEARSET_06, false, MOTOR_ENCODER_DEGREES);

pros::Optical rollerDetector(7);