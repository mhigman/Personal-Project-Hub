//Example of Adafruit CAN actuator comms

#define CAN0_MESSAGE_RAM_SIZE (0)
#define CAN1_MESSAGE_RAM_SIZE (1728)
#include "RMDLAdafruit.h"
#include "ACANFD_FeatherM4CAN.h"
RMDLAdafruit motorController(&can1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ACANFD_FeatherM4CAN_Settings settings (1000*1000, DataBitRateFactor::x1);
  settings.mModuleMode = ACANFD_FeatherM4CAN_Settings::NORMAL_FD;
  const uint32_t errorCode = can1.beginFD(settings);
  Serial.println("ERROR CODE--------");
  Serial.println(errorCode);
  Serial.println("------------------");
  motorController.enable(1);
  motorController.enable(2);
  motorController.reset(1);
  motorController.reset(2);
  delay(4000);
}

void loop() {
  delay(1000);
}
