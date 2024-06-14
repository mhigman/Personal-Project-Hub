#include "Odometry.hpp"
#include "main.h"
#include "math.h"
#include "pros/llemu.hpp"
#include "util.hpp"
#include <cmath>
#include <string>

double angleDistanceToZero(double angle){
    if(angle < 0){
        angle += 360;
    }
    return angle < 180 ? angle : angle - 360;
}
double getAngleDistance(double angle1, double angle2){
    if(fabs(angle1 - angle2) < fabs(angleDistanceToZero(angle1) - angleDistanceToZero(angle2)))
        return angle1 - angle2;
    return angleDistanceToZero(angle1) - angleDistanceToZero(angle2);
}
double angularTarget = 0;

void turn(double target, bool waitUntilSettled){
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
    while((fabs(getAngleDistance(target, position)) > 0.5) || (cycleTime < 250)){
        position = imuSensor.get_heading();
        error = getAngleDistance(target, position);
        derivative = error - prevError;
        if(kI != 0){
            integral += error;
            if(integral > threashold){
                integral = 0;
            }
        }
        prevError = error;
        //pros::lcd::set_text(3, std::to_string(error));
        pros::lcd::set_text(2, std::to_string(output));
        pros::lcd::set_text(3, std::to_string(imuSensor.get_heading()));

       
        output = (kP * error) + (kI * integral) + (kD * derivative);
        if(fabs(error)<1.5 && fabs(error) > 0.5){
            output *= 5;
        }
        if((fabs(error) < 0.5) && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 20){
            output = 0;
            //pros::lcd::set_text(3, std::to_string(error));
            pros::lcd::set_text(4, std::to_string(target)); 
            //std::cout << "Cycled" << std::endl;
            cycleTime += 10;
        } else { 
            cycleTime = 0;
        }
        //if(cycleTime > 750 && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            //break;
        //}
        topLeft.move_velocity(3 * clamp(-180, 180, /*clamp(prevOutput - 5, prevOutput + 5, output)*/output));
        bottomLeft.move_velocity( 3 * clamp(-180, 180, /*clamp(prevOutput - 5, prevOutput + 5, output)*/output));
        topRight.move_velocity(3 * clamp(-180, 180, /*clamp(-(prevOutput - 5), -(prevOutput + 5), -output)*/-output));
        bottomRight.move_velocity(3 * clamp(-180, 180, /*(clamp(-(prevOutput - 5), -(prevOutput + 5), -output)*/-output));
        prevOutput = output;
        pros::delay(10);
    }
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
void move(double target, bool waitUntilSettled){
    double kP = 0.65;
    double kI = 0;
    double kD = 0.5;
    double startingPose = rightEncoder.get_value();
    //std::cout << startingPose << "\n"<<std::endl;
    double error, prevError,derivative, integral, cycleTime, output, prevOutput = 0;
    double threashold = 200;
    double position = 0;
    double direction = fabs(target)/target;
    target *= 41.6697;
    while(fabs(target-position) > 10 || (cycleTime < 500)){
        position = rightEncoder.get_value() - startingPose;
        error = fabs(target-position);
        //std::cout << rightEncoder.get_value() <<std::endl;
        direction = target-position < 0 ? -1 : 1;
        derivative = error - prevError;
        if(kI != 0){
            integral += error;
            if(integral > threashold){
                integral = 0;
            }
        }
        prevError = error;
        
        output = (kP * error) + (kI * integral) + (kD * derivative);
        if ((fabs(error) < 20) && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            cycleTime += 20;
            
            output *= 0.5;
        }
        if(cycleTime > 500){
            break;
        }
        
        topLeft.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) + 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        bottomLeft.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction))  + 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        topRight.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        bottomRight.move_velocity(/*3 * */0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 5 * getAngleDistance(angularTarget, imuSensor.get_heading()));
        prevOutput = clamp(-200, 200, prevOutput + 5);
        pros::delay(10);
    }
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
double angleBetweenPoints(odomVector desiredPoint, odomVector currentPoint){
    double xDistance = desiredPoint.x - currentPoint.x;
    double yDistance = desiredPoint.y - currentPoint.y;
    double turningDeg = std::atan(yDistance/xDistance);
    if(xDistance < 0){
        turningDeg += PI;
    } 
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

double calculateRelativeDistance(odomVector point1, odomVector point2, int preference1, int preference2, double scaleFactor){
    double xDistance = point1.x - point2.x;
    double yDistance = point1.y - point2.y;
    double d = std::sqrt(std::pow(xDistance * scaleFactor, 2) + std::pow(yDistance * scaleFactor, 2));
    if(preference1 <= 0 || preference2 <= 0){
        if(preference1 <= 0 && preference2 <=0){
            if(yDistance - 0.25 <= -xDistance <= yDistance + 0.25 && d < 40){
                return 0;
            }
            if(-(xDistance) >= yDistance){
                return d;
            } else {
                return -d;
            }
        } else if(preference1<= 0){
            if(yDistance - 0.25 <= xDistance <= yDistance + 0.25 && d < 40){
                return 0;
        }
        if(xDistance >= yDistance){
            return -d;
        } else {
            return d;
        }
        }
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
double AngularOutput(double error, double prevError){
    double kP = 1.01;
    double kD = 1.81;
    return (kP * error) + (kD * (error-prevError));
}
// ronak wuz here
void turnToPoint(odomVector desiredPoint, bool waitUntilSettled){
    odomVector currentPoint = get_robot_position();
    std::cout <<angleBetweenPoints(desiredPoint, currentPoint)<<std::endl;
    std::cout <<imuSensor.get_heading()<<std::endl;
    std::cout <<getAngleDistance(angleBetweenPoints(desiredPoint, currentPoint), imuSensor.get_heading())<<std::endl;
    turn(angleBetweenPoints(desiredPoint, currentPoint), waitUntilSettled);

}
void moveToPoint(odomVector desiredPoint, bool waitUntilSettled){
    odomVector currentPoint = get_robot_position();
    //double distance = std::sqrt(pow(desiredPoint.x - currentPoint.x, 2) + pow(desiredPoint.y - currentPoint.y, 2));

    turnToPoint(desiredPoint,true);
    //odomVector target = desiredPoint;
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
    while(fabs(calculateRelativeDistance(position, desiredPoint, xPreference, yPreference, 41.6697)) > 20 || (cycleTime < 250)){
        if(fabs(error) > 200){
            angularTarget = angleBetweenPoints(desiredPoint, get_robot_position());
        }
        position = get_robot_position();
        error = calculateRelativeDistance(position, desiredPoint, xPreference, yPreference, 41.6697);
        
        //std::cout << rightEncoder.get_value() <<std::endl;
        //direction = target-position < 0 ? -1 : 1;
        derivative = error - prevError;
        if(kI != 0){
            integral += error;
            if(integral > threashold){
                integral = 0;
            }
        }
        prevError = error;
        //std::cout << calculateRelativeDistance(desiredPoint, position, xPreference, yPreference, 41.6697) << std::endl;
        output = (kP * error) + (kI * integral) + (kD * derivative);
        if (error < 20 && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            cycleTime += 10;


        }
        /*
        if(cycleTime > 500 && (fabs(topLeft.get_actual_velocity()) + fabs(bottomLeft.get_actual_velocity()) + fabs(topRight.get_actual_velocity()) + fabs(bottomRight.get_actual_velocity()))/4 < 5){
            break;
        }*/
        topLeft.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) + 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        bottomLeft.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction))  + 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        topRight.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        bottomRight.move_velocity(3 * 0.9 * clamp(-200, 200, clamp(-200, prevOutput + 5, output * direction)) - 1.15 * AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO));
        prevO =  getAngleDistance(angularTarget, imuSensor.get_heading());
        //std::cout <<  AngularOutput(getAngleDistance(angularTarget, imuSensor.get_heading()), prevO) << std::endl;
        prevOutput = clamp(-200, 200, prevOutput + 5);
        pros::delay(10);
    }
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
