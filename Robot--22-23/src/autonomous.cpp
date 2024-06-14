#include "main.h"
#include "Auton_Drive/Odometry.hpp"
#include "Auton_Drive/PID.hpp"
#include "Auton_Drive/util.hpp"
#include "pros/imu.hpp"
#include "pros/llemu.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include <string>
const int NUMBER_OF_PAGES = 6;

int current_page = 0;

bool red = true;
odomVector blueGoalPose = {0,0,0};
odomVector redGoalPose = {0,0,0};
void select() {
    for (int i = 0; i<6;i++){
        pros::lcd::clear_line(i);
    }
        

    pros::lcd::set_text(0, "Page "+std::to_string(current_page+1));

    if(current_page == 0){
        pros::lcd::set_text(2, "Right AWP");
        pros::lcd::set_text(3, "");
        if(red){
            blueGoalPose = {30,30,0};
            redGoalPose = {-22,15,0};
        } else {
            blueGoalPose = {30,30,0};
            redGoalPose = {-22,15,0};
        }
    } else if(current_page == 1){
        pros::lcd::set_text(2, "Left AWP");
        pros::lcd::set_text(3, "");
        if(red){
            blueGoalPose = {30,30,0};
            redGoalPose = {-22,15,0};
        } else {
            blueGoalPose = {30,30,0};
            redGoalPose = {-22,15,0};
        }
    } else if(current_page == 2){
        pros::lcd::set_text(2, "Skills");
        if(red){
            blueGoalPose = {30,30,0};
            redGoalPose = {-22,15,0};
        } else {
            blueGoalPose = {30,30,0};
            redGoalPose = {-22,15,0};
        }
    } else if(current_page == 3){
        pros::lcd::set_text(2, "Left Tall Goal Rush");
    } else if(current_page == 4){
        pros::lcd::set_text(2, "Right Tall Goal Rush");
    }else{
        pros::lcd::set_text(2, "Do Nothing");
    }
}

void pageUp() {
    if(current_page==NUMBER_OF_PAGES-1)
        current_page=0;
    else
        current_page++;
    select();
}
void pageDown() {
    if(current_page==0)
        current_page=NUMBER_OF_PAGES-1;
    else
        current_page--;
    select();
}
void redOrBlue(){
    red = !red;
    if(red){
        pros::lcd::set_text(7, "Red");
    }else{
        pros::lcd::set_text(7, "Blue");
    }
}





void stepLoop(void *){
	while(true){
		step();
		pros::delay(10);
	}
}


void autonomous() {
	//OdometryInstance builder = OdometryInstance();
	//odomTwoEncoderImuController positionTracker = builder.constructController(imuSensor, middleEncoder, rightEncoder);
	//builder.~OdometryInstance();
	std::cout << "Hello" << std::endl;
    pros::Task odometry(stepLoop);
	//Odometry my dudes ^^^^^^^^^^^^^^^^^
	//topLeft.set_brake_mode(MOTOR_BRAKE_COAST);
	//bottomLeft.set_brake_mode(MOTOR_BRAKE_COAST);
	//topRight.set_brake_mode(MOTOR_BRAKE_COAST);
	//bottomRight.set_brake_mode(MOTOR_BRAKE_COAST);
	//pros::delay(12000);s
	topLeft.set_brake_mode(MOTOR_BRAKE_BRAKE);
	bottomLeft.set_brake_mode(MOTOR_BRAKE_BRAKE);
	topRight.set_brake_mode(MOTOR_BRAKE_BRAKE);
	bottomRight.set_brake_mode(MOTOR_BRAKE_BRAKE);
	//std::cout << angleBetweenPoints({30, 30, 0}, get_robot_position()) << std::endl;
	
	moveToPoint({15, 20, 0}, true);
	//pros::lcd::set_text(5, std::to_string(get_robot_position().x));
	//pros::lcd::set_text(6, std::to_string(get_robot_position().y));
	
	moveToPoint({0,30,0}, true);
	//pros::lcd::set_text(5, std::to_string(get_robot_position().x));
	//pros::lcd::set_text(6, std::to_string(get_robot_position().y));
	moveToPoint({0,0,0}, true);
	//pros::lcd::set_text(5, std::to_string(get_robot_position().x));
	//pros::lcd::set_text(6, std::to_string(get_robot_position().y));
	turnToPoint({38, -36, 0}, true);
	/*
	topLeft.move_velocity(100);
	bottomLeft.move_velocity(100);
	topRight.move_velocity(100);
	bottomRight.move_velocity(100);
	pros::delay(2000);
	topLeft.move_velocity(0);
	bottomLeft.move_velocity(0);
	topRight.move_velocity(0);
	bottomRight.move_velocity(0);
	*/
	//turn(100, true);
	//turn(-100, true);
	//std::cout << imuSensor.get_heading() << std::endl;
	/*
	std::cout << get_robot_position().x <<std::endl;
	std::cout << get_robot_position().y <<std::endl;
	std::cout << std::sqrt(std::pow( (get_robot_position().x - 15) * 41.6697, 2) + std::pow((get_robot_position().y - 20) * 41.6697, 2)) << std::endl;
	*/
	//turn(315, true);
	//odometry.suspend();
}

