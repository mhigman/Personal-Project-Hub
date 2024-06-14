#include "main.h"
#include "math.h"
#include "util.hpp"

odomVector totalVector = {0,0,0};
odomVector prevVector = {0,0,0};
odomVector localOffset = {0,0,0};
odomVector globalOffset = {0,0,0};
const double offsetM = 5.5;
const double offsetR = 1.575;
double r, m, prevL, prevR, prevM, deltaR, deltaM, totalR, totalM = 0;
double totalHeading,deltaHeading, averageHeading = 0;
double prevHeading = 0;
double polarRadius, polarAngle = 0;
odomVector get_robot_position(){
    return totalVector;
}
/*
double round_value_at_index(double val, double index){
    index = pow(10, index);
    val *= index;
    val = round(val);
    val /= index;
    return val;
}
*/
odomVector stepMath(){
    double heading = imuSensor.get_heading() == INFINITY ? 0 : angleDistanceToZero(imuSensor.get_heading());
    totalHeading = heading * (PI/180.0);
    deltaHeading = totalHeading - prevHeading;
    //std::cout << "imu heading " << imuSensor.get_heading() << std::endl;
	//std::cout << "heading in rads " << totalHeading << std::endl;
	//std::cout << "prev heading " << prevHeading << std::endl;
	//std::cout << "delta heading " << deltaHeading << std::endl;
    
    if (deltaHeading == 0){
        localOffset = {deltaM, deltaR,  totalHeading};
    }else{
        localOffset = {((deltaM/deltaHeading) + offsetM) * (2 * std::sin(deltaHeading/2)), ((deltaR/deltaHeading) + offsetR) * (2 * std::sin(deltaHeading/2)), totalHeading};
    }
    //std::cout << "local x " << localOffset.x << std::endl;
    //std::cout << "local y " << localOffset.y << std::endl;
    averageHeading = prevHeading + (deltaHeading/2);
    if(!std::isnan(localOffset.y/localOffset.x)){
        averageHeading = prevHeading + (deltaHeading/2);
        //std::cout << averageHeading << std::endl;
        polarRadius = std::sqrt(pow(localOffset.x, 2) + pow(localOffset.y, 2));
        //std::cout << "polar radius " << polarRadius << std::endl;
        
        polarAngle = std::atan(localOffset.y/localOffset.x);
        
        if(localOffset.x < 0){
            polarAngle += PI;
            polarAngle *= 180/PI;
            polarAngle = angleDistanceToZero(polarAngle) * PI/180;
        }
        
        //std::cout << "polar angle " << polarAngle << std::endl;
        
    } else {
        if(localOffset.y < 0){
            polarAngle = (3 * PI)/2;
        } else if (localOffset.y == 0){
            polarAngle = 0;
        } else {
            polarAngle = PI/2;
        }
        polarRadius = localOffset.y;


    }
    polarAngle -= averageHeading;

    //std::cout << "polar radius " << polarRadius << std::endl;
    //std::cout << "polar angle " << polarAngle << std::endl;
    prevHeading = totalHeading;
    globalOffset = {polarRadius * std::cos(polarAngle), polarRadius * std::sin(polarAngle), totalHeading};
    //std::cout << globalOffset.x << std::endl;
    //std::cout << "working..." << std::endl;
    
    return globalOffset;

}
odomVector addVector(odomVector vector1, odomVector vector2){
    return {vector1.x + vector2.x, vector1.y + vector2.y, vector1.heading + vector2.heading};
}
void step(){
    m = middleEncoder.get_value();
    r = rightEncoder.get_value();
    //std::cout << "encoder values " << m << " " << r << std::endl; 
    deltaM = m - prevM;
    deltaR = r - prevR;
    //std::cout << "delta encoder values " << deltaM << " " << deltaR << std::endl; 
    deltaM *= 0.024;
    deltaR *= 0.024;
    //std::cout << "delta encoder values in inches " << deltaM << " " << deltaR << std::endl; 
    prevM = m;
    prevR = r;
    //totalM += deltaM;
    //totalR += deltaR;
    totalVector = addVector(prevVector, stepMath());
    prevVector = totalVector;
    //std::cout << totalVector.x << std::endl;
    //std::cout << totalVector.y << std::endl;
}


void zero_everything(){
    totalVector = {0,0,0};
    prevVector = {0,0,0};
    localOffset = {0,0,0};
    globalOffset = {0,0,0};
    middleEncoder.reset();
    rightEncoder.reset();
    r = 0;
    m = 0;
    prevL = 0;
    prevR = 0;
    prevM = 0;
    deltaR = 0;
    deltaM = 0;
    totalR = 0;
    totalM = 0;
    totalHeading = 0;
    prevHeading = 0;
    deltaHeading = 0;
    averageHeading = 0;
}

