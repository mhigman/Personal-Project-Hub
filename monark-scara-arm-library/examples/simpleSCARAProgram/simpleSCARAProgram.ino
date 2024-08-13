#define CAN0_MESSAGE_RAM_SIZE (0)
#define CAN1_MESSAGE_RAM_SIZE (1728)
#include "RMDStepperSCARA.h"
#include "ACANFD_FeatherM4CAN.h"

RMDLAdafruit motorController(&can1);
static const uint8_t DIRECTION1 = 10;
static const uint8_t PULSE1 = 9;

AccelStepper zStepper(AccelStepper::DRIVER, PULSE1, DIRECTION1);

const int arm1Length = 94;
const int arm2Length = 148;

SCARAController robot(true, &motorController, &zStepper, arm1Length, 1, 100, arm2Length, 2);

SCARAController::pointPosition pose;


const uint8_t positionKP = 50;
const uint8_t positionKI = 10;
const uint8_t velocityKP = 100;
const uint8_t velocityKI = 20;
const uint8_t torqueKP = 255;
const uint8_t torqueKI = 63;

CANFDMessage pidConstantsRead;
CANFDMessage pidConstantsTune;


uint32_t errorCodePID1;
uint32_t errorCodePID2;

double x = 30;
double y = 130;



void setup(){

    ACANFD_FeatherM4CAN_Settings settings (1000*1000, DataBitRateFactor::x1);
    settings.mModuleMode = ACANFD_FeatherM4CAN_Settings::NORMAL_FD;
    const uint32_t errorCode = can1.beginFD(settings);


    motorController.enable(1);
    motorController.enable(2);
    delay(1000);
    pidConstantsRead = motorController.createMessage(2, 0x30);
    motorController.canReadWrite(pidConstantsRead);
    delay(1000);
    pidConstantsTune = motorController.createMessage(2, 0x31);
    pidConstantsTune.data[2] = positionKP;
    pidConstantsTune.data[3] = positionKI;
    pidConstantsTune.data[4] = velocityKP-15;
    pidConstantsTune.data[5] = 0;
    pidConstantsTune.data[6] = torqueKP;
    pidConstantsTune.data[7] = torqueKI+60;
    errorCodePID1 = motorController.canReadWrite(pidConstantsTune); 
    delay(1000);
    pidConstantsRead = motorController.createMessage(1, 0x30);
    motorController.canReadWrite(pidConstantsRead);
    delay(1000);
    pidConstantsTune = motorController.createMessage(1, 0x31);
    pidConstantsTune.data[2] = positionKP;
    pidConstantsTune.data[3] = 0;
    pidConstantsTune.data[4] = 160;
    pidConstantsTune.data[5] = 0;
    pidConstantsTune.data[6] = torqueKP;
    pidConstantsTune.data[7] = 255;
    errorCodePID2 = motorController.canReadWrite(pidConstantsTune); 
    delay(1000);
    Serial.begin(115200);

    zStepper.setAcceleration(400);
    zStepper.setMinPulseWidth(20);
    zStepper.setMaxSpeed(2000);

  }

int count = 0;
bool active = true;
void loop(){
  /*
  Serial.println("Position 1");
  Serial.println(arm1Length * sin(motorController.getPositionCircle(40
  /*
    pose = robot.position();
    Serial.println(pose.x);
    Serial.println(pose.y);
    robot.moveHorizontal(pose.x + 40, pose.y - 80);
    robot.moveVertical(4000);
    delay(2000);
    pose = robot.position();
    Serial.println(pose.x);
    Serial.println(pose.y);
    robot.moveHorizontal(pose.x - 40, pose.y + 80);
    robot.moveVertical(-4000);
    */
    if(active){
      robot.moveHorizontal(x, y);
      robot.moveVertical(-200);
      robot.moveHorizontal(-x, y);
      robot.moveVertical(0);
      active = false;
    }

    /*
    unsigned long start = micros();
    noInterrupts();
    while(((micros() - start) < ((1500000/200) * angleMetric))){
			delay(2);
		}
    interrupts();
    if(count%2){
      x+=40;
      y+=40;
    } else{
      x-=40;
      y-=40;
    }

    count++;
    */
}