#include "main.h"
#include "Auton_Drive/Odometry.hpp"
#include "Auton_Drive/PID.hpp"
#include "Auton_Drive/util.hpp"
#include "pros/imu.hpp"
#include "pros/llemu.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include <string>
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
 /*
void on_center_button() {
	//pros::lcd::set_text(2, std::to_string(imuSensor.get_heading()));
	/*
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}

}
/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	/*
	imuSensor.reset();
	while(imuSensor.is_calibrating()){
		pros::delay(20);
	}
	std::cout << "IMU Calibrated" << std::endl;
	*/
	pros::lcd::initialize();
	pros::lcd::register_btn1_cb(redOrBlue);
	pros::lcd::register_btn0_cb(pageDown);
	pros::lcd::register_btn2_cb(pageUp);
	topLeft.set_brake_mode(MOTOR_BRAKE_BRAKE);
	bottomLeft.set_brake_mode(MOTOR_BRAKE_BRAKE);
	topRight.set_brake_mode(MOTOR_BRAKE_BRAKE);
	bottomRight.set_brake_mode(MOTOR_BRAKE_BRAKE);


}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}


/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */





void setMotors(int left_motor_move, int right_motor_move) {
    //check if param values will cause error, set params to max if true.
    if (left_motor_move > 127) {
        left_motor_move = 127;
    } else if (left_motor_move < -127) {
        left_motor_move = -127;
    }
    if (right_motor_move > 127){
        right_motor_move = 127;
    }else if (right_motor_move < -127){
        right_motor_move = -127;
    }
    //move
    topLeft = left_motor_move;
    bottomLeft = left_motor_move;
    topRight = right_motor_move;
    bottomRight = right_motor_move;
}


void arcadeDrive(){
    int power = master.get_analog(ANALOG_LEFT_Y);
    int turn = master.get_analog(ANALOG_RIGHT_X);
    if(abs(power) < 10) {
        power = 0;
    }
    if (abs(turn) < 10) {
        turn = 0;
    }
    int left = ((power + turn) * abs(power + turn))/127;
    int right = ((power - turn) * abs(power - turn))/127;
    setMotors(left, right);
}

void opcontrol() {
	pros::Task odometry(stepLoop);
	while(true){
		arcadeDrive();
		updateFlywheel();
		updateIndexer();
		updateIntake();
		updateRoller();
		pros::delay(10);
	}
	
}
