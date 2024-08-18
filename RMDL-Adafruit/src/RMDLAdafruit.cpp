#include "RMDLAdafruit.h"

// Utility functions
CANFDMessage RMDLAdafruit::createMessage(byte const& motorId, byte const& command, long const& data)
{
    CANMessage message;

    message.id = 0x140 + motorId;
    message.len = 8;
    message.data[0] = command;
    message.data[4] = data & 0xFF;
    message.data[5] = (data >> 8) & 0xFF;
    message.data[6] = (data >> 16) & 0xFF;
    message.data[7] = (data >> 24) & 0xFF;
    return CANFDMessage(message);
}


int32_t messageToInt32(CANFDMessage const& message)
{
    ///The RMDL returns data in little-endian format. @todo look up a built-in solution?
    return message.data[4] + (message.data[5] << 8) +  (message.data[6] << 16) +  (message.data[7] << 24);
}

RMDLAdafruit::RMDLAdafruit(ACANFD_FeatherM4CAN *canDriver):
    canDriver_(canDriver)
{

}


void RMDLAdafruit::reset(byte const& motorId)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::RESET);
    canReadWrite(message);
}



void RMDLAdafruit::enable(byte const& motorId)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::MOTOR_ENABLE);
    canReadWrite(message);
}

void RMDLAdafruit::disable(byte const& motorId)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::MOTOR_DISABLE);
    canReadWrite(message);
}


int32_t RMDLAdafruit::getAccelerationCommand(byte const& motorId)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::READ_ACCEL);
    if (canReadWrite(message) == 0)
        return messageToInt32(message);
    return 0;
}

int32_t RMDLAdafruit::getPosition(byte const& motorId)
{
    ///@todo lol this is a 32 bit processor!! position is simply too large for its tiny brain
    // just do not use this function.
    CANFDMessage message = createMessage(motorId, MyActuator::commands::READ_MULTITURN_ANGLE);
    if (canReadWrite(message) == 0)
        return message.data[1] + (message.data[2] << 8) +  (message.data[3] << 16) +  (message.data[4] << 24);
        // + (message.data[5] << 32) + (message.data[6] << 40) + (message.data[7] << 48);
    return 0;
}

int32_t RMDLAdafruit::getPositionCircle(byte const& motorId)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::READ_CIRCLE_ANGLE, 0L);
    if (canReadWrite(message, true) == 0)
        return message.data[6] + (message.data[7] << 8);
    return 0;
}

int32_t RMDLAdafruit::getSpeed(byte const& motorId)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::READ_MOTOR_STATUS2, 0L);
    if(canReadWrite(message) == 0)     
        return message.data[4] + (message.data[5] << 8);
    return 0;
}


int RMDLAdafruit::setSpeed(byte const& motorId, int32_t const& targetSpeed)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::SPEED_COMMAND, targetSpeed);
    int result = canReadWrite(message);
    if (result != 0)
        return 0;
    return message.data[4] + (message.data[5] << 8);
}

int RMDLAdafruit::setTorque(byte const& motorId, int32_t const& targetTorque)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::TORQUE_COMMAND, targetTorque);
    int result = canReadWrite(message);
    if (result != 0)
        return 0;
    return message.data[2] + (message.data[3] << 8);
}

int RMDLAdafruit::setPosition(byte const& motorId, uint16_t const& targetPostion, uint16_t const& speedLimit, bool const& spin)
{
    CANFDMessage message = createMessage(motorId, MyActuator::commands::ABS_POS_COMMAND, targetPostion);
    message.data[1] = spin;
    message.data[2] = speedLimit & 0xFF;
    message.data[3] = (speedLimit >> 8) & 0xFF;
    int result = canReadWrite(message);
    if (result != 0)
        return 0;
    return message.data[6] + (message.data[7] << 8);
}


int RMDLAdafruit::canReadWrite(CANFDMessage& message, bool const& waitForReply)
{
    CANFDMessage dump;
    while (canDriver_->availableFD0())
        canDriver_->receiveFD0(dump);

    if (canDriver_->tryToSendReturnStatusFD(message) != 0)
        return -1;
    if (waitForReply)
    {
      unsigned long startTime = micros();
       while (!canDriver_->availableFD0() && micros() - startTime < 100000)
           delay(1);
       if (canDriver_->availableFD0())
          canDriver_->receiveFD0(message);
       else
          return -2;
    }
    return 0;
}
