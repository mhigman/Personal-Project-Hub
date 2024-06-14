#include "pros/misc.h"
#include "main.h"
#include "Auton_Drive/Odometry.hpp"
#include "Auton_Drive/util.hpp"
const double DISTANCE_TO_FLYWHEEL_SPEED = 0.01; //PLACEHOLDERS (Will be rigorously tested)
odomVector goalPose = red ? redGoalPose : blueGoalPose;
void switchGoalPose() {
    if(goalPose.x == redGoalPose.x && goalPose.y == redGoalPose.y){
        goalPose = blueGoalPose;
    } else{
        goalPose = redGoalPose;
    }
}
void updateFlywheel(){
    double distance =std::sqrt(pow(get_robot_position().x - goalPose.x, 2) + pow(get_robot_position().y - goalPose.y, 2));
    /*flywheel = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);*/
    flywheel.move_velocity(distance * DISTANCE_TO_FLYWHEEL_SPEED);
}