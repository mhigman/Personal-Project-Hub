#include "RMDStepperSCARA.h"

#define PI 3.1415926535897932384626433832795

bool spinDriection(double angleTarget, double angleCurrent, bool originalJoint)
{
	Serial.println("Test");
	bool angleCurrentOver180 = angleCurrent >= 18000;
	bool angleTargetOver180 = angleTarget >= 18000;
	Serial.print("Current angle: ");
	Serial.println(angleCurrent);
	Serial.print("Target angle: ");
	Serial.println(angleTarget);
	Serial.println(angleCurrentOver180);
	Serial.println(angleTargetOver180);
	if(angleCurrentOver180 && angleTargetOver180){
		Serial.println("Left semicircle");
		//if(originalJoint) return !(angleCurrent < angleTarget);
		return angleCurrent > angleTarget;
	}
	else if(!angleCurrentOver180 && !angleTargetOver180){
		Serial.println("Right semicircle");
		//if(originalJoint) return !(angleTarget < angleCurrent);
		return angleTarget > angleCurrent;
	}
	else if(angleCurrentOver180 && (!angleTargetOver180)){
		Serial.println("L to R semicircle");
		//if(originalJoint) return true;
		return false;
	}
	else if((!angleCurrentOver180) && angleTargetOver180){
		Serial.println("R to L semicircle");
		//if(originalJoint) return false;
		return true;
	} else return false;
}

bool isValidSolution(double angle1, double x, double y){
	double cumulativeAngle = -(atan2(y, x)) + PI/2;
	cumulativeAngle += cumulativeAngle < 0 ? 2*PI : 0;
	angle1 += angle1 < 0 ? 2*PI : 0;
	return !(fabs(cumulativeAngle - PI) < 0.5 || fabs(angle1 - PI) < 0.5);
}

SCARAController::SCARAController(bool config, 
								 RMDLAdafruit *pancakeController, 
								 AccelStepper *zStepper, 
								 double armLength, 
								 byte pancakeID, 
								 uint16_t maxPancakeSpeed, 
								 double armLength2, 
								 byte pancakeID2)
								 :	config_(config),
									pancakeController_(pancakeController),
									zStepper_(zStepper),
									armLength_(armLength),
									pancakeID_(pancakeID),
									maxPancakeSpeed_(maxPancakeSpeed),
									armLength2_(armLength2),
									pancakeID2_(pancakeID2)
{
}	



/// @todo Fix this thing
SCARAController::pointPosition SCARAController::position()
{
	// @todo Does this even work? Need to test
	if(config_){
		return {double(armLength_ * (sin(double(pancakeController_->getPositionCircle(pancakeID_)) * (PI/18000))) + 
	            armLength2_ * sin(double((pancakeController_->getPositionCircle(pancakeID_) + pancakeController_->getPositionCircle(pancakeID2_))) * (PI/18000))),
	            double(armLength_ * cos(double(pancakeController_->getPositionCircle(pancakeID_)) * (PI/18000)) + 
	            armLength2_ * cos(double((pancakeController_->getPositionCircle(pancakeID_) + pancakeController_->getPositionCircle(pancakeID2_))) * (PI/18000))),
	            (pancakeController_->getPositionCircle(pancakeID_) + pancakeController_->getPositionCircle(pancakeID2_))};
	} else{
		return {armLength_ * sin(pancakeController_->getPositionCircle(pancakeID_) * (PI/18000)), 
	            armLength_ * cos(pancakeController_->getPositionCircle(pancakeID_) * (PI/18000)), 
	            pancakeController_->getPositionCircle(pancakeID_)};
	}
}

bool SCARAController::moveHorizontal(double x, double y)
{
	if(!config_)
	{
		double angleTarget = int(atan2(x, y) * (18000/PI)) % 36000;
		pancakeController_->setPosition(pancakeID_, angleTarget, maxPancakeSpeed_, spinDriection(angleTarget, pancakeController_->getPositionCircle(pancakeID_), true));
		
		/// @todo Calcutate wait time depending on speed.
		unsigned long start = micros();
		delay(2000);
		return fabs(angleTarget - pancakeController_->getPositionCircle(pancakeID_)) < 25;
	} else {
		double angleTarget2 = acos((pow(x, 2) + pow(y, 2) - pow(armLength_, 2) - pow(armLength2_, 2)) / (2 * armLength_ * armLength2_));
		double angleTarget1 =-(atan2(y, x) - atan2((armLength2_ * sin(angleTarget2)), (armLength_ + armLength2_ * cos(angleTarget2)))) + PI/2;
		/*if(!isValidSolution(angleTarget1, x, y)){
			angleTarget2 = -angleTarget2;
			angleTarget1 =-(atan2(y, x) - atan2((armLength2_ * sin(angleTarget2)), (armLength_ + armLength2_ * cos(angleTarget2)))) + PI/2;
			if(!isValidSolution(angleTarget1, x, y)){
				angleTarget2 = 0;
				angleTarget1 = 0;
			}
		}*/
		angleTarget2 = -angleTarget2 + (2*PI);
		angleTarget1 = int(angleTarget1 * (18000/PI)) % 36000;
		angleTarget2 = int(angleTarget2 * (18000/PI)) % 36000;
    	angleTarget1 += angleTarget1 < 0 ? 36000 : 0;
    	angleTarget2 += angleTarget2 < 0 ? 36000 : 0;
		Serial.println(angleTarget1);
		Serial.println(angleTarget2);
		Serial.println(pancakeController_->getPositionCircle(pancakeID_));
		Serial.println(pancakeController_->getPositionCircle(pancakeID2_));
		delay(1000);
		Serial.println("motor1");
    	bool direction1 = !spinDriection(angleTarget1, pancakeController_->getPositionCircle(pancakeID_), true);
		Serial.println("motor2");
    	bool direction2 = spinDriection(angleTarget2, pancakeController_->getPositionCircle(pancakeID2_), false);
		delay(1000);
		pancakeController_->setPosition(pancakeID_, int(angleTarget1), maxPancakeSpeed_, direction1);
		pancakeController_->setPosition(pancakeID2_, int(angleTarget2), maxPancakeSpeed_, direction2);
    	delay(2000);
		return (fabs(angleTarget1 - pancakeController_->getPositionCircle(pancakeID_)) < 25) && (fabs(angleTarget2 - pancakeController_->getPositionCircle(pancakeID2_)));
	}
}


bool SCARAController::moveVertical(int z)
{
	zStepper_->moveTo(z);
	zStepper_->runToPosition();
	return true;
}

void SCARAController::setStepperMaxSpeed(int maxSpeed)
{
	zStepper_->setMaxSpeed(maxSpeed);
}

void SCARAController::setStepperAcceleration(int acceleration)
{
	zStepper_->setAcceleration(acceleration);
}

void SCARAController::setMaxPancakeSpeed(uint16_t maxPancakeSpeed)
{
	maxPancakeSpeed_ = maxPancakeSpeed;
}