/*
Library for controlling a SCARA bot with RMD pancake motors and a vertical stepper motor
Originally designed for use in 2D fabrication
Will have muiltiple tools at the end of the arm
*/
#ifndef RMDStepperSCARA
#define RMDStepperSCARA
#include <AccelStepper.h>
#include <RMDLAdafruit.h>
#include <math.h>


/// @brief Utility function that, given a target angle and a current angle, chooses the optimal spin direction. 
/// Assumes breakpoint is 18000 centidegrees
/// @param angleTarget The target angle.
/// @param angleCurrent The current angle.
/// @param originalJoint Not currently used, may be used later
/// @return true if the optimal direction is counter-clockwise, false if the optimal direction is clockwise.
/// @todo Generalize this utility function
bool spinDriection(double angleTarget, double angleCurrent, bool originalJoint=false);
/// @brief Utility function that determines if solution is valid.
/// @note Assumes that PI rads +/- 0.5 rads is the "no-go" zone
/// @param angle1 First angle
/// @param x x position
/// @param y y position
/// @return true if valid, false otherwise
/// @todo Generalize this utility function
bool isValidSolution(double angle1, double x, double y);


class SCARAController{
	public:
		
		/// @brief This struct defines a posverticalJoinedSteppersition of the SCARA bot from its horizontial position and orientation.
		/// @param y position of the robot.
		/// @param x position of the robot.
		/// @param heading orientation of the robot in the horizontal.
		struct pointPosition{
			/// @brief X Positon
			double x;
			/// @brief Y Position
			double y;
			/// @brief Orientation
			double heading;
		};

		/// @brief Configures a controller class for the SCARA robot.
		/// @param config Defines whether the SCARA robot has a second arm. It is true if the robot does have a second arm, and false if it doesn't.
		/// @param pancakeController A pointer to an RMDL adafruit controller that can control the pancake motors via CAN. Fork if you need to use different protocols.
		/// @param zStepper A pointer to an AccelStepper object that is able to control the vertical stepper motor via a speed profile.
		/// @param armLength The length of the first arm.
		/// @param pancakeID The ID of the pancake motor for use in the RMDX controller.
		/// @param maxPancakeSpeed The max speed to run the pancake motors, in dps. Defaults to 1rev/s.
		/// @param armLength2 The length of the second arm. Defaults to 0. Not used if config is false.
		/// @param pancakeID2 The ID of the 2nd pancake motor for use in the RMDX controller. Defaults to 0. Not used if config is flase.
		SCARAController(bool config, RMDLAdafruit *pancakeController, AccelStepper *zStepper, double armLength, byte pancakeID, uint16_t maxPancakeSpeed=360, double armLength2=0, uint8_t pancakeID2=0);

		/// @brief Configures a controller class for the SCARA robot.
		/// @param config Defines whether the SCARA robot has a second arm. It is true if the robot does have a second arm, and false if it doesn't.
		/// @param pancakeController A pointer to an RMDL adafruit controller that can control the pancake motors via CAN. Fork if you need to use different protocols.
		/// @param zStepper A pointer to an AccelStepper object that is able to control the vertical stepper motor via a speed profile.
		/// @param armLength The length of the first arm.
		/// @param pancakeID The ID of the pancake motor for use in the RMDX controller.
		/// @param maxPancakeSpeed The max speed to run the pancake motors, in dps. Defaults to 1rev/s.
		/// @param armLength2 The length of the second arm. Defaults to 0. Not used if config is false.
		/// @param pancakeID2 The ID of the 2nd pancake motor for use in the RMDX controller. Defaults to 0. Not used if config is flase.
		/// SCARAController(bool config, RMDLAdafruit *pancakeController, AccelStepper *zStepper, double armLength, byte pancakeID, uint16_t maxPancakeSpeed=360, double armLength2=0, uint8_t pancakeID2=0);

		/// @brief Calculates the horizontal position and orientation of the SCARA bot.
		/// @return Returns a pointPosition struct defining the horizontal position and orientation of the SCARA bot.
		pointPosition position();

		/// @brief Moves the pancake motors to put the SCARA bot into a particular position. 
		/// If the bot only has one arm, it will point twoards the coordinate specified.
		/// @note If the bot has two arms and the coordinate specified isn't reachable, erratic behavoir may happen.
		/// @note This method assumes the first joint in the two arm configuration is defined as a heading, not from the x axis.
		/// @param x The desired x position, measured in the same units as the arm lengths.
		/// @param y The desired y position, measured in the same units as the arm lengths.
		/// @return true if it moved to the target coordinate within 3 seconds, false otherwise.
		/// @todo Consider both solutions to the 2 jointed inverse kinematics and pick the optimal one. Also allow user override.
		/// @todo Calcutate wait time depending on speed.
		bool moveHorizontal(double  x, double y);

		/// @brief Steps the stepper motor to move the robot vertically.
		/// @param z The number of steps the stepper motor should take. Positive is clockwise, negative is counter-clockwise.
		/// @return true if moved. false if ran out of time.
		bool moveVertical(int z);

		/// @brief Sets a new max speed for the vertical stepper motor.
		/// @param maxSpeed The new max speed for the stepper motor.
		void setStepperMaxSpeed(int maxSpeed);

		/// @brief Sets a new target acceleration rate for the vertical stepper motor.
		/// @param acceleration The new target acceleration for the stepper motor.
		void setStepperAcceleration(int acceleration);

		/// @brief Sets a new max speed for the pancake motors.
		/// @param maxPancakeSpeed The new max speed for the pancake motors
		void setMaxPancakeSpeed(uint16_t maxPancakeSpeed);
	private:

		/// @brief Defines whether the SCARA robot has a second arm. It is true if the robot does have a second arm, and false if it doesn't.
		bool config_;

		/// @brief A pointer to an RMDX controller that can control the pancake motors via CAN. Fork if you need to use different protocols.
		RMDLAdafruit *pancakeController_;

		/// @brief A pointer to an AccelStepper object that is able to control the vertical stepper motor via a speed profile.
		AccelStepper *zStepper_;

		/// @brief The length of the first arm.
		double armLength_;

		/// @brief The length of the second arm. Not used if config_ is false.
		double armLength2_;

		/// @brief The ID of the pancake motor for use in the RMDX controller.
		byte pancakeID_;

		/// @brief The ID of the 2nd pancake motor for use in the RMDX controller. Not used if config_ is false.
		byte pancakeID2_;

		/// @brief The max speed to run the pancake motors, in dps.
		uint16_t maxPancakeSpeed_;

};
#endif