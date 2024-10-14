#include "Odometry.hpp"
#include "main.h"
#include "math.h"
#include "pros/llemu.hpp"
#include "util.hpp"
#include <cmath>
#include <string>


// Utility functions to parse angle heading into an angle change input for the turning PID controller.
//This funciton get the distance to the 0 heading
double angleDistanceToZero(double angle){
    if(angle < 0){
        angle += 360;
    }
    return angle < 180 ? angle : angle - 360;
}

// Using the previous funciton, I can get a delta angle like so:
double getAngleDistance(double angle1, double angle2){
    if(fabs(angle1 - angle2) < fabs(angleDistanceToZero(angle1) - angleDistanceToZero(angle2)))
        return angle1 - angle2;
    return angleDistanceToZero(angle1) - angleDistanceToZero(angle2);
}

//The current anglular target for the PID controller (needs to be global)
double angularTarget = 0;


// PID Turning Controller
void turn(double target, bool waitUntilSettled){
    // Initialize variables & Define Constants
    double kP = 1.65;
    double kI = 0;
    double kD = 1.77;
    angularTarget = target;
    double error = 0;
    double prevError = 0;
    double threashold = 0;
    double derivative = 0;
    double integral = 0;
    double cycleTime = 0;
    double output = 0;
    double prevOutput = 0;
    double position = imuSensor.get_heading();
    //std::cout << position <<std::endl;

    // While we are far away from the target angle or haven't been close to target for long...
    while((fabs(getAngleDistance(target, position)) > 0.5) || (cycleTime < 250)){
        // Get and parse our current sensor data
        position = imuSensor.get_heading();
        error = getAngleDistance(target, position);
        // Calculate an approximate of the derivative of the error.
        derivative = error - prevError;
        // Approximate the integral of the error, up to a certain threashold.
        if(kI != 0){
            integral += error;
            if(integral > threashold){
                integral = 0;
            }
        }
        prevError = error;
        //pros::lcd::set_text(3, std::to_string(error));

        //Ouput data for convience.
        pros::lcd::set_text(2, std::to_string(output));
        pros::lcd::set_text(3, std::to_string(imuSensor.get_heading()));

        // Get the output
        output = (kP * error) + (kI * integral) + (kD * derivative);
        // If we are somewhat close to the target, speed up to give the controller a final push.
        if(fabs(error)<1.5 && fabs(error) > 0.5){
            output *= 5;
        }
        // If we are close to target and are moving slow
        if((fabs(error) < 0.5) && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 20){
            //Stop
            output = 0;
            //pros::lcd::set_text(3, std::to_string(error));
            pros::lcd::set_text(4, std::to_string(target)); 
            //std::cout << "Cycled" << std::endl;
            //See if we stay in range
            cycleTime += 10;
        } else { 
            //Reset if we oscillate.
            cycleTime = 0;
        }
        //if(cycleTime > 750 && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            //break;
        //}
        // Move the motors, clamping at 90% speed to minimize jerk with sacrificing much speed.
        topLeft.move_velocity(3 * clamp(-180, 180, /*clamp(prevOutput - 5, prevOutput + 5, output)*/output));
        bottomLeft.move_velocity( 3 * clamp(-180, 180, /*clamp(prevOutput - 5, prevOutput + 5, output)*/output));
        topRight.move_velocity(3 * clamp(-180, 180, /*clamp(-(prevOutput - 5), -(prevOutput + 5), -output)*/-output));
        bottomRight.move_velocity(3 * clamp(-180, 180, /*(clamp(-(prevOutput - 5), -(prevOutput + 5), -output)*/-output));
        prevOutput = output;
        pros::delay(10);
    }

    //Then, we can settle if the controller is settled.
    pros::lcd::set_text(1, "settled");
    std::cout << imuSensor.get_heading()<< std::endl;
    topLeft.move_velocity(0);
    bottomLeft.move_velocity(0);
    topRight.move_velocity(0);
    bottomRight.move_velocity(0);
    if(waitUntilSettled){
        while(fabs((topLeft.get_actual_velocity() + bottomLeft.get_actual_velocity() + topRight.get_actual_velocity() + bottomRight.get_actual_velocity())/4) > 2){
            pros::delay(20);
        }
    }
}

//PID Movement Controller
void move(double target, bool waitUntilSettled){
    // Define constants and variables
    double kP = 0.65;
    double kI = 0;
    double kD = 0.5;
    double startingPose = rightEncoder.get_value();
    //std::cout << startingPose << "\n"<<std::endl;
    double error, prevError,derivative, integral, cycleTime, output, prevOutput = 0;
    double threashold = 200;
    double position = 0;

    // Get the direction of the motion and convert to encoder degrees.
    double direction = fabs(target)/target;
    target *= 41.6697;
    // While we are far from the target position or we have stayed close to the target position for a long time.
    while(fabs(target-position) > 10 || (cycleTime < 500)){
        // Get relative postion and error
        position = rightEncoder.get_value() - startingPose;
        error = fabs(target-position);
        //std::cout << rightEncoder.get_value() <<std::endl;
        // Get the direction multiplier for this iteration (allows robot to go backwards if overshot)
        direction = target-position < 0 ? -1 : 1;

        // Approximate derivatives and integrals
        derivative = error - prevError;
        if(kI != 0){
            integral += error;
            if(integral > threashold){
                integral = 0;
            }
        }
        // Update prevError
        prevError = error;

        //Calculate output
        output = (kP * error) + (kI * integral) + (kD * derivative);

        // If we're very close to the target and are approaching the target slowly
        if ((fabs(error) < 20) && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            // Cycle cycle time and reduce output
            cycleTime += 20;
            
            output *= 0.5;
        }
        if(cycleTime > 500){
            break;
        }
        

        // Move motors with 90% output (This also has a slew rate as well as an angular controller builtin)
        topLeft.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) + 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        bottomLeft.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction))  + 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        topRight.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        bottomRight.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        prevOutput = clamp(-200, 200, prevOutput + 5);
        pros::delay(10);
    }

    // Finally, we can settle
    std::cout << "movement settled" <<std::endl;
    topLeft.move_velocity(0);
    bottomLeft.move_velocity(0);
    topRight.move_velocity(0);
    bottomRight.move_velocity(0);
    if(waitUntilSettled){
        while(fabs((topLeft.get_actual_velocity() + bottomLeft.get_actual_velocity() + topRight.get_actual_velocity() + bottomRight.get_actual_velocity())/4) > 2){
            pros::delay(20);
        }
    }
}

// Here is a simple utility function to compute the angle between two points formatting like a heading
double angleBetweenPoints(odomVector desiredPoint, odomVector currentPoint){

    // Get the change in x and y for both of these points
    double xDistance = desiredPoint.x - currentPoint.x;
    double yDistance = desiredPoint.y - currentPoint.y;

    // Use arctan and check for a second domain 
    // (yes, I didn't check for when x = 0, but I was controlling the input to this function, and it never came up)
    double turningDeg = std::atan(yDistance/xDistance);
    if(xDistance < 0){
        turningDeg += PI;
    } 

    // Convert to degrees and format like a heading
    turningDeg *= 180/PI;
    if(turningDeg > 360){
        
        turningDeg -= 360;
    }
    turningDeg = getAngleDistance(90, turningDeg);
    if(turningDeg < 0){
        turningDeg += 360;
    }


    return turningDeg;
}


// This function calculuates the distance between two points, determining the sign accordingly
// As distance is always positive, I needed to tell the robot when to move backwards.
// This fucntion does this in a multi step process:
//      Creating a coordinate system on the desired point (point2), preference1 and preference2 decides which "quadrant" the robot is in
//      I then draw a line (y=x if robot is in quadrant IV or II, y=-x otherwise)
//      Anything on the robot's side of the line is positive error, the other side is negative error, and some tolerance within the line is 0.
// Making the line 0 was an ad hoc solution, but the angular PID controller made sure it was never and issue.
double calculateRelativeDistance(odomVector point1, odomVector point2, int preference1, int preference2, double scaleFactor){
    // Calculate distance
    double xDistance = point1.x - point2.x;
    double yDistance = point1.y - point2.y;
    double d = std::sqrt(std::pow(xDistance * scaleFactor, 2) + std::pow(yDistance * scaleFactor, 2));
    //If in quadrants IV or III...
    if(preference1 <= 0 || preference2 <= 0){
        //If in quandrant III...
        if(preference1 <= 0 && preference2 <=0){
            //Check if really close to point or close to line y=-x to return 0 distance
            if(yDistance - 0.25 <= -xDistance <= yDistance + 0.25 && d < 40){
                return 0;
            }
            //Deterine sign with y=-x as the delimiter
            if(-(xDistance) >= yDistance){
                return d;
            } else {
                return -d;
            }
        //If in quadrant IV...
        } else if(preference1<= 0){
            //Check if we are close to center or line
            if(yDistance - 0.25 <= xDistance <= yDistance + 0.25 && d < 40){
                return 0;
            }

            //Determine sign with y=-x 
            if(xDistance >= yDistance){
                return -d;
            } else {
                return d;
            }
        }
            // Ensuring that all cases are covered (this is when we are in quadrant II)
            if(yDistance - 0.25 <= xDistance <= yDistance + 0.25 && d < 40){
                    return 0;
            }
            if(xDistance >= yDistance){
                return d;
            } else {
                return -d;
            }
        //return (-((point1.x - point2.x) * fabs(point1.x - point2.x))) * scaleFactor * scaleFactor + (-((point1.y - point2.y) * fabs(point1.y - point2.y))) * scaleFactor * scaleFactor;


        //return (-((point1.x - point2.x) * fabs(point1.x - point2.x))) * scaleFactor * scaleFactor + (((point1.y - point2.y) * fabs(point1.y - point2.y)))* scaleFactor * scaleFactor;
    } 
        //return (((point1.x - point2.x) * fabs(point1.x - point2.x))) * scaleFactor * scaleFactor + (-((point1.y - point2.y) * fabs(point1.y - point2.y)))* scaleFactor * scaleFactor;
    // If we are in quadrant I, calculate distance again
    else{
        //xDistance = point2.x - point1.x;
        //yDistance = point2.y - point1.y;
        if(yDistance - 0.25 <= xDistance <= yDistance + 0.25 && d < 40){
                return 0;
        }
        if(xDistance >= yDistance){
            return d;
        } else {
            return -d;
        }


       //return (((point1.x - point2.x) * fabs(point1.x - point2.x))) * scaleFactor * scaleFactor + (((point1.y - point2.y) * fabs(point1.y - point2.y)))* scaleFactor * scaleFactor;
    }
}
/*
double checkForNegativeRoot(double number){
    if(number < 0){ 
        return -(std::sqrt(fabs(number)));
    }
    return std::sqrt(number);
}
*/

// Use a simple PD controller for keeping the drivetrain straight
double AngularOutput(double error, double prevError){
    double kP = 1.01;
    double kD = 1.81;
    return (kP * error) + (kD * (error-prevError));
}
// ronak wuz here
// ^^ Friend was here 
// Simple wrapper function that turns towards a given point.
void turnToPoint(odomVector desiredPoint, bool waitUntilSettled){
    odomVector currentPoint = get_robot_position();
    std::cout <<angleBetweenPoints(desiredPoint, currentPoint)<<std::endl;
    std::cout <<imuSensor.get_heading()<<std::endl;
    std::cout <<getAngleDistance(angleBetweenPoints(desiredPoint, currentPoint), imuSensor.get_heading())<<std::endl;
    turn(angleBetweenPoints(desiredPoint, currentPoint), waitUntilSettled);

}


// A function to move to a point from any point.
void moveToPoint(odomVector desiredPoint, bool waitUntilSettled){
    odomVector currentPoint = get_robot_position();
    //double distance = std::sqrt(pow(desiredPoint.x - currentPoint.x, 2) + pow(desiredPoint.y - currentPoint.y, 2));
    // First, turn towards the point.
    turnToPoint(desiredPoint,true);
    //odomVector target = desiredPoint;
    //Then, we will move. First, we intialize variables and define constants
    double kP = 0.55;
    double kI = 0;
    double kD = 0.178;
    double xPreference = currentPoint.x - desiredPoint.x;
    double yPreference = currentPoint.y - desiredPoint.y;
    double startingPose = rightEncoder.get_value();
    //std::cout << startingPose << "\n"<<std::endl;
    double error, prevError,derivative, integral, cycleTime, output, prevOutput = 0;
    double threashold = 200;
    odomVector position = currentPoint;
    double direction = -1;
    double prevO = 0;
    pros::lcd::set_text(0, std::to_string(xPreference));
    pros::lcd::set_text(1, std::to_string(yPreference));

    // While we aren't close to the point or haven't stayed close to the point for a while...
    while(fabs(calculateRelativeDistance(position, desiredPoint, xPreference, yPreference, 41.6697)) > 20 || (cycleTime < 250)){
        // Get the angle between points to use angular PID
        if(fabs(error) > 200){
            angularTarget = angleBetweenPoints(desiredPoint, get_robot_position());
        }
        // Get our position and error
        position = get_robot_position();
        error = calculateRelativeDistance(position, desiredPoint, xPreference, yPreference, 41.6697);
        
        //std::cout << rightEncoder.get_value() <<std::endl;
        //direction = target-position < 0 ? -1 : 1;

        // Approximate Derivatives and integrals
        derivative = error - prevError;
        if(kI != 0){
            integral += error;
            if(integral > threashold){
                integral = 0;
            }
        }
        prevError = error;
        //std::cout << calculateRelativeDistance(desiredPoint, position, xPreference, yPreference, 41.6697) << std::endl;

        // Calculate output
        output = (kP * error) + (kI * integral) + (kD * derivative);
        //If we are close to the point and moving slowly, cycle the time
        if (error < 20 && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            cycleTime += 10;


        }
        /*
        if(cycleTime > 500 && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            break;
        }*/

       // Move the motors with slew rate as well as accounting for angular PID.
        topLeft.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) + 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        bottomLeft.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction))  + 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        topRight.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        bottomRight.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        prevO =  getAngleDistance(angularTarget, imuSensor.get_heading());
        //std::cout <<  AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO) << std::endl;
        prevOutput = clamp(-200, 200, prevOutput + 5);
        pros::delay(10);
    }

    // Finally, settle.
    std::cout << "movement settled" <<std::endl;
    std::cout << calculateRelativeDistance(desiredPoint, position, xPreference, yPreference, 41.6697) << std::endl;
    topLeft.move_velocity(0);
    bottomLeft.move_velocity(0);
    topRight.move_velocity(0);
    bottomRight.move_velocity(0);
    if(waitUntilSettled){
        while(fabs((topLeft.get_actual_velocity() + bottomLeft.get_actual_velocity() + topRight.get_actual_velocity() + bottomRight.get_actual_velocity())/4) > 2){
            pros::delay(20);
        }
    }
}
        /*_ooo--.
     @@@=@MMM\.`,_.',-
   _.\X/"/"   \  33,
  ===A   |     \ P""B
    /@,_ (  __,/""\.M\
    |; \"/\"_,/ / .'.A
    \,\._><-__./    "V
     \F _       a_3R"---,.
      _>"#           _   )
     (  /           .@J  /
     ) /           /    )
     ( |           \    /,
     | \            `,._,/ ___
      "=\,          ]@7,.n| P @\
          7-______.  \____.,   .)
          /  /     \ \      \WWW/
          |  |     |  |      ""'
   ___   /   \.   ,/  \._
/"   """      \ (       """"\
|(       ___.-'  "--.       \)
 ""-`""""            `--(__)*/
