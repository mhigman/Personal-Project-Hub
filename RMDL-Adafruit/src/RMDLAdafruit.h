/// \file RMDX.h
/// \brief A simple driver for the RMDX brushless motor from MyActuator
///
/// \details This code is meant to work as a simple example for communicating with
///          the RMDX brushless motors. Note that only single-motor commands are implemented.

#ifndef MYACTUATOR_RMDLAdafruit
#define MYACTUATOR_RMDLAdafruit

#include <ACANFD_FeatherM4CAN-from-cpp.h>

#ifndef MY_ACTUATOR_COMMANDS
#define MY_ACTUATOR_COMMANDS
namespace MyActuator::commands
{
    byte const READ_PID = 0x30;
    byte const WRITE_PID_RAM = 0x31;
    byte const WRITE_PID_ROM = 0x32;
    byte const READ_ACCEL = 0x42;
    byte const WRITE_ACCEL = 0x43;
    byte const READ_POS = 0x60;
    byte const READ_RAW_POS = 0x61;
    byte const READ_HOME_POS = 0x62;
    byte const WRITE_ENCODER_ZERO = 0x63;
    byte const WRITE_ENCODER_CURRENT_POS_AS_ZERO = 0x64;
    byte const READ_MULTITURN_ANGLE = 0x92;
    byte const READ_CIRCLE_ANGLE = 0x94;
    byte const READ_MOTOR_STATUS1 = 0x9A;
    byte const READ_MOTOR_STATUS2 = 0x9C;
    byte const READ_MOTOR_STATUS3 = 0x9D;
    byte const SHUTDOWN = 0x80;
    byte const STOP = 0x81;
    byte const TORQUE_COMMAND = 0xA1;
    byte const SPEED_COMMAND = 0xA2;
    byte const ABS_POS_COMMAND = 0xA6;
    byte const OPERATING_MODE = 0x70;
    byte const POWER_ACQUISITION = 0x71;
    byte const RESET = 0x19;
    byte const BRAKE_RELEASE = 0x77;
    byte const BRAKE_LOCK = 0x78;
    byte const MOTOR_DISABLE = 0x80;
    byte const MOTOR_ENABLE = 0x88;
    byte const RUNTIME_READ = 0xB1;
    byte const SOFTWARE_VERSION = 0xB2;
    byte const COMM_INTERRUPT_SAFETY = 0xB3;
    byte const BAUD_RATE = 0xB4;
}
#endif

class RMDLAdafruit{
    public:


        RMDLAdafruit(ACANFD_FeatherM4CAN *canDriver);

        CANFDMessage createMessage(byte const& motorId, byte const& command, long const& data=0);
        /// \brief Reset a given motor
        /// \param[in] motorId Motor id
        void reset(byte const& motorId);

        void enable(byte const& motorId);
        void disable(byte const& motorId);

        /// \brief Get the motor acceleration command
        /// \param[in] motorId Motor id
        /// \return Acceleration command, in dps/s^2 ; 0 on failure.
        int32_t getAccelerationCommand(byte const& motorId);

        /// \brief Get the motor position (multiturn)
        /// \param[in] motorId Motor id
        /// \return Position,  command, in dps/s ; 0 on failure.
        long int getPosition(byte const& motorId);

        /// \brief Get the motor position (Circle)
        /// \param[in] motorId Motor id
        /// \return Position,  command, in dps/s ; 0 on failure.
        int32_t getPositionCircle(byte const& motorId);

        /// \brief Get the motor speed (1dps/LSB)
        /// \param[in] motorID Motor id
        /// \return Speed in dps ; 0 on failure
        //int32_t getSpeed(byte const& motorId)
        int32_t getSpeed(byte const& motorId);

        /// \brief Close-loop speed contro: set motor target speed
        /// \param[in] motorId Motor id
        /// \param[in] targetSpeed Target speed, 0.01dps/LSB
        /// \return Current speed, 0 on failure.
        int setSpeed(byte const& motorId, int32_t const& targetSpeed);

        /// \brief Torque control: set motor torque (i.e. quadrature current) target
        /// \param[in] motorId Motor id
        /// \param[in] targetTorque Target torque, 0.01A/LSB
        /// \return Current torque (current), 0.01A/LSB ; 0 on failure.
        int setTorque(byte const& motorId, int32_t const& targetTorque);

        /// \brief Position control: set motor position target, given a speed limit and spin direction
        /// \param[in] motorId Motor id
        /// \param[in] targetPostion Target position, ranging from 0-359.99 degrees; 0.01 degree/LSB
        /// \param[in] speedLimit maximum speed for closed loop control; 1dps/LSB
        /// \param[in] spin Direction for spin; true for CCW and false for CW
        /// \return Current position in degrees; 0 on failure
        int setPosition(byte const& motorId, uint16_t const& targetPostion, uint16_t const& speedLimit, bool const& spin);
        /// \brief Send and recieve a data frame from the motor.
        ///
        /// \param[in, out] message The message to send, modified in place.
        /// \param waitForReply Whether or not to wait for a reply
        /// \return 0 on succes ; -1 if write failed ; -2 if write succeeded but read failed.
        int canReadWrite(CANFDMessage& message, bool const& waitForReply = true);
    private:
        ACANFD_FeatherM4CAN *canDriver_;
};

#endif
