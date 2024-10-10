//Example of Adafruit CAN actuator comms

#define CAN0_MESSAGE_RAM_SIZE (0)
#define CAN1_MESSAGE_RAM_SIZE (1728)
#include "RMDLAdafruit.h"
#include "ACANFD_FeatherM4CAN.h"
RMDLAdafruit motorController(&can1);
CANFDMessage pidConstantsRead;
CANFDMessage pidConstantsTune;

const uint8_t positionKP = 50;
const uint8_t positionKI = 10;
const uint8_t velocityKP = 100;
const uint8_t velocityKI = 20;
const uint8_t torqueKP = 255;
const uint8_t torqueKI = 63;

uint32_t errorCodePID1;
uint32_t errorCodePID2;
uint32_t errorCode;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ACANFD_FeatherM4CAN_Settings settings (1000*1000, DataBitRateFactor::x1);
  settings.mModuleMode = ACANFD_FeatherM4CAN_Settings::NORMAL_FD;
  errorCode = can1.beginFD(settings);
  Serial.println("ERROR CODE--------");
  Serial.println(errorCode);
  Serial.println("------------------");
  motorController.enable(2);
  motorController.enable(1);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("ERROR CODES--------");
  Serial.println(errorCode);
  Serial.println(errorCodePID1);
  Serial.println(errorCodePID2);
  Serial.println("-------------------");
  
  Serial.println(motorController.setPosition(1, 2000, 200, false));
  Serial.println(motorController.setPosition(2, 2000, 200, false));
  delay(1000);
  /*
  pidConstantsRead = motorController.createMessage(2, 0x30);
  motorController.canReadWrite(pidConstantsRead);
  pidConstantsTune = motorController.createMessage(2, 0x31);
  pidConstantsTune.data[2] = positionKP;
  pidConstantsTune.data[3] = positionKI;
  pidConstantsTune.data[4] = pidConstantsRead.data[4];
  pidConstantsTune.data[5] = pidConstantsRead.data[5];
  pidConstantsTune.data[6] = pidConstantsRead.data[6];
  pidConstantsTune.data[7] = pidConstantsRead.data[7];
  errorCodePID1 = motorController.canReadWrite(pidConstantsTune); 
  pidConstantsRead = motorController.createMessage(1, 0x30);
  motorController.canReadWrite(pidConstantsRead);
  pidConstantsTune = motorController.createMessage(1, 0x31);
  pidConstantsTune.data[2] = positionKP;
  pidConstantsTune.data[3] = positionKI;
  pidConstantsTune.data[4] = pidConstantsRead.data[4];
  pidConstantsTune.data[5] = pidConstantsRead.data[5];
  pidConstantsTune.data[6] = pidConstantsRead.data[6];
  pidConstantsTune.data[7] = pidConstantsRead.data[7];
  errorCodePID2 = motorController.canReadWrite(pidConstantsTune); 
  */
  delay(1000);
  Serial.println("===PID Constants2===");
  pidConstantsRead = motorController.createMessage(2, 0x30);
  Serial.println(motorController.canReadWrite(pidConstantsRead) == 0 ? "Successful" : "Error");
  Serial.print("kP position");
  Serial.println(pidConstantsRead.data[2]);
  Serial.print("kI position");
  Serial.println(pidConstantsRead.data[3]);
  Serial.print("kP velocity");
  Serial.println(pidConstantsRead.data[4]);
  Serial.print("kI velocity");
  Serial.println(pidConstantsRead.data[5]);
  Serial.print("kP torque");
  Serial.println(pidConstantsRead.data[5]);
  Serial.print("kI torque");
  Serial.println(pidConstantsRead.data[7]);
  Serial.println("===================");
  delay(1000);
  Serial.println("===PID Constants1===");
  pidConstantsRead = motorController.createMessage(1, 0x30);
  Serial.println(motorController.canReadWrite(pidConstantsRead) == 0 ? "Successful" : "Error");
  Serial.print("kP position");
  Serial.println(pidConstantsRead.data[2]);
  Serial.print("kI position");
  Serial.println(pidConstantsRead.data[3]);
  Serial.print("kP velocity");
  Serial.println(pidConstantsRead.data[4]);
  Serial.print("kI velocity");
  Serial.println(pidConstantsRead.data[5]);
  Serial.print("kP torque");
  Serial.println(pidConstantsRead.data[5]);
  Serial.print("kI torque");
  Serial.println(pidConstantsRead.data[7]);
  Serial.println("===================");
  delay(1000);
  
  Serial.println(motorController.setPosition(1, 0, 200, true));
  Serial.println(motorController.setPosition(2, 0, 200, true));
  delay(3000);
  
}
