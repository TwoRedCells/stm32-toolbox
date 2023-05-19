/**
 * \file       devices/Zlac8015.h
 * \class      Zlac8015
 * \brief      Encapsulates CANopen communications ZLAC8015 motor controllers.
 */

#ifndef INC_COMMS_ZLAC8015_H_
#define INC_COMMS_ZLAC8015_H_

#include "stm32-toolbox/comms/CanOpen.h"

class Zlac8015 : public CanOpen::ICanOpenCallback
{
public:
	static constexpr uint16_t Index_CommunicationOfflineTime = 0x2000;  // 16 rw
	static constexpr uint16_t Index_InputSignalStatus = 0x2003;  // 16-ro
	static constexpr uint16_t Index_OutputSignalStatus = 0x2004;  // 16-ro
	static constexpr uint16_t Index_ClearPositionFeedback = 0x2005;  // 16-rw
	static constexpr uint16_t Index_ClearCurrentPosition = 0x2006;  // 16-rw
	static constexpr uint16_t Index_LimitPackingMode = 0x2007;  // 16-rw
	static constexpr uint16_t Index_InitialSpeed = 0x2008;  // 16-rw
	static constexpr uint16_t Index_RegisterParameter = 0x2009;  // 16-rw
	static constexpr uint16_t Index_MaximumMotorSpeed = 0x200a;  // 16-rw
	static constexpr uint16_t Index_EncoderWireSetting = 0x200b;  // 16-rw
	static constexpr uint16_t Index_MotorPolePairs = 0x200c;  // 16-rw
	static constexpr uint16_t Index_CanNodeNumber = 0x200d;  // 16-rw
	static constexpr uint16_t Index_CanBaudrate = 0x200e;  // 16-rw
	static constexpr uint16_t Index_ShaftLockMethod = 0x200f;  // 16-rw
	static constexpr uint16_t Index_SaveEepromSynchronously = 0x2010;  // 16-rw
	static constexpr uint16_t Index_OffsetAngle = 0x2011;  // 16-rw
	static constexpr uint16_t Index_OverloadFactor = 0x2012;  // 16-rw
	static constexpr uint16_t Index_MotorTemperatureProtectionThreshold = 0x2013;  // 16-rw
	static constexpr uint16_t Index_RatedCurrent = 0x2014;  // 16-rw
	static constexpr uint16_t Index_MaximumCurrent = 0x2015;  // 16-rw
	static constexpr uint16_t Index_OverloadProtectionTime = 0x2016;  // 16-rw
	static constexpr uint16_t Index_OutOfToleranceAlarmThreshold = 0x2017;  // 16-rw
	static constexpr uint16_t Index_VelocitySmoothingFactor = 0x2018;  // 16-rw
	static constexpr uint16_t Index_CurrentLoopProportionalCoefficient = 0x2019;  // 16-rw
	static constexpr uint16_t Index_CurrentLoopIntegralGain = 0x201a;  // 16-rw
	static constexpr uint16_t Index_FeedForwardOutputSmoothingCoefficient = 0x201b;  // 16-rw
	static constexpr uint16_t Index_TorqueOutputSmoothingFactor = 0x201c;  // 16-rw
	static constexpr uint16_t Index_SpeedProportionalGain = 0x201d;  // 16-rw
	static constexpr uint16_t Index_SpeedIntegralGain = 0x201e;  // 16-rw
	static constexpr uint16_t Index_SpeedFeedForwardGain = 0x201f;  // 16-rw
	static constexpr uint16_t Index_PositionProportionalGain = 0x2020;  // 16-rw
	static constexpr uint16_t Index_PositionFeedForwardGain = 0x2021;  // 16-rw
	static constexpr uint16_t Index_Rs485NodeNumber = 0x2022;  // 16-rw
	static constexpr uint16_t Index_Rs485Baudrate = 0x2023;  // 16-rw
	static constexpr uint16_t Index_SoftwareVersion = 0x2025;  // 16-rw
	static constexpr uint16_t Index_MotorTemperature = 0x2026;  // 16-rw
	static constexpr uint16_t Index_MotorStatusRegister = 0x2027;  // 16-rw
	static constexpr uint16_t Index_HallInputStatus = 0x2028;  // 16-rw
	static constexpr uint16_t Index_BusVoltage = 0x2029;  // 16-rw
	static constexpr uint16_t Index_ProcessingMethods = 0x202f;
	static constexpr uint16_t Subindex_AlarmPwm = 0x01;  // 16-rw
	static constexpr uint16_t Subindex_Overload = 0x02;  // 16-rw
	static constexpr uint16_t Index_Terminals = 0x2030;
	static constexpr uint16_t Subindex_InputTerminalEffectiveLevel = 0x01;  // 16-rw
	static constexpr uint16_t Subindex_InputX0FunctionSelection = 0x02;  // 16-rw
	static constexpr uint16_t Subindex_InputX1FunctionSelection = 0x03;  // 16-rw
	static constexpr uint16_t Subindex_OutputY0FunctionSelection = 0x0c;  // 16-rw
	static constexpr uint16_t Subindex_OutputY1FunctionSelection = 0x0d;  // 16-rw

	static constexpr uint16_t Fault_None = 0x0000;
	static constexpr uint16_t Fault_Overload = 0x0008;
	static constexpr uint16_t Fault_Overvoltage = 0xff01;
	static constexpr uint16_t Fault_Undervoltage = 0xff02;
	static constexpr uint16_t Fault_Overcurrent = 0xff04;
	static constexpr uint16_t Fault_CurrentOutOfTolerance = 0x0010;
	static constexpr uint16_t Fault_EncoderOutOfTolerance = 0x0020;
	static constexpr uint16_t Fault_SpeedOutOfTolerance = 0x0040;
	static constexpr uint16_t Fault_ReferenceVoltage = 0x0080;
	static constexpr uint16_t Fault_Eeprom = 0xff10;
	static constexpr uint16_t Fault_Hall = 0x0200;


	typedef struct
	{
		int16_t target_linear_velocity;
		int16_t actual_linear_velocity;
		int16_t target_angular_velocity;
		int16_t actual_angular_velocity;
		int32_t angular_position;
		int32_t linear_position;
		float temperature;
		uint16_t target_torque;
		uint16_t actual_torque;
		uint16_t status;
		uint8_t brake : 1;
	} MotorData;

	enum OperatingModes { Undefined=0, Position=1, Velocity=3, Torque=6 };
	enum ControlWords { Step0=0x0000, Stop=0x0002, Step1=0x0006, Step2=0x0007, Step3=0x000f, Clear=0x0080 };
	enum VelocityUnits { Rpm, Mms };

	/**
	 * Instantiates the battery CANopen class.
	 * @param	can Pointer to CanBus instance.
	 * @param	id Device ID.
	 * @param	wheel_circumference The circumference in millimetres.
	 */
	Zlac8015(CanOpen* can, uint8_t id, float wheel_circumference, bool reverse=false)
	{
		this->can = can;
		this->id = id;
		this->wheel_circumference = wheel_circumference;
		this->reverse = reverse;
	}


	/**
	 * Initializes communications with the motor controller and resets the states.
	 * @param	acceleration_time The time it takes to accelerate from 0 to the commanded speed in milliseconds.
	 * @param	deceleration_time The time it takes to accelerate from 0 to the commanded speed in milliseconds.
	 */
	void setup(uint32_t acceleration_time=250, uint32_t deceleration_time=250)
	{
		this->acc_time = acceleration_time;
		this->dec_time = deceleration_time;
		can->nmt(CanOpen::NMT_ResetCommunications, id);
		can->nmt(CanOpen::NMT_Operational, id);
		osDelay(2000);
		reset();
	}


	/**
	 * Resets the motor controller state.
	 * @param	acceleration_time The time it takes to accelerate from 0 to the commanded speed in milliseconds.
	 * @param	deceleration_time The time it takes to accelerate from 0 to the commanded speed in milliseconds.
	 */
	void reset(uint32_t acceleration_time=250, uint32_t deceleration_time=250)
	{
		control_word(Step0);
		this->acceleration_time(acc_time);
		this->deceleration_time(dec_time);
		maximum_velocity(500);
		control_word(Step1);
		control_word(Step2);
		control_word(Step3);
	}


	/**
	 * Sets the control word.
	 * @param value The control word.
	 */
	void control_word(uint16_t value)
	{
		can->sdo(id, CanOpen::Index_ControlWord, 0x00, value, sizeof(uint16_t));

	}


	/**
	 * Commands the velocity of the motor.
	 * @param value The target
	 */
	void velocity(int32_t value, VelocityUnits units)
	{
		if (reverse) value = -value;
		if (units == Mms)
		{
			state.target_linear_velocity = value;
			state.target_angular_velocity = (value * 60) / wheel_circumference;
		}
		else
		{
			state.target_angular_velocity = value;
			state.target_linear_velocity = (value * wheel_circumference) / 60;

		}
		can->sdo(id, CanOpen::Index_TargetSpeed, 0x00, state.target_angular_velocity, sizeof(uint32_t));
	}


	/**
	 * Sets the acceleration. time.
	 * @param value The acceleration time.
	 */
	void acceleration_time(uint32_t value)
	{
		can->sdo(id, CanOpen::Index_AccelerationTime, 0x00, value, sizeof(uint32_t));
	}


	/**
	 * Sets the deceleration. time.
	 * @param value The acceleration time.
	 */
	void deceleration_time(uint32_t value)
	{
		can->sdo(id, CanOpen::Index_DecelerationTime, 0x00, value, sizeof(uint32_t));
	}


	/**
	 * Sets the maximum allowed velocity.
	 * @param value The velocity.
	 */
	void maximum_velocity(uint32_t value)
	{
		can->sdo(id, CanOpen::Index_MaximumSpeed, 0x00, value, sizeof(uint32_t));
	}


	/**
	 * Refreshes state information.
	 */
	void refresh(void)
	{
		can->sdo(id, CanOpen::Index_ActualPosition, 0x00);
		can->sdo(id, CanOpen::Index_ActualSpeed, 0x00);
		can->sdo(id, CanOpen::Index_LastFaultCode, 0x00);
		can->sdo(id, Index_MotorTemperature, 0x00);
		can->sdo(id, CanOpen::Index_RealtimeTargetTorque, 0x00);
		can->sdo(id, CanOpen::Index_ActualTorque, 0x00);
		can->sdo(id, Index_BusVoltage, 0x00);
	}


	/**
	 * Sets the operating mode.
	 * @param mode The operating mode.
	 */
	void operating_mode(OperatingModes mode)
	{
		can->sdo(id, CanOpen::Index_OperatingMode, 0x00, (uint8_t)mode, sizeof(uint8_t));
	}


	/**
	 * Clears faults.
	 */
	void clear_faults(void)
	{
		control_word(Clear);
		reset();
	}


	/**
	 * Stops
	 */
	void quick_stop(void)
	{
		control_word(Stop);
	}


	/**
	 * Stops
	 */
	void status(void)
	{
		can->sdo(id, CanOpen::Index_StatusWord, 0x00);
	}


	/**
	 * Called when an sdo message is received
	 * @param address The address ID of the receiver.
	 * @param index The SDO index.
	 * @param subindex The SDO subindex.
	 * @param data The data (1-4 bytes).
	 */
	void on_sdo(uint16_t address, uint16_t index, uint8_t subindex, uint8_t* data) override
	{
		if (address != id)
			return;

		switch (index)
		{
		case CanOpen::Index_ActualSpeed:
			state.actual_angular_velocity = CanOpen::lsb_int16_to_float(data, 1);
			state.actual_linear_velocity = (state.actual_angular_velocity * wheel_circumference) / 60.0;
			break;
		case CanOpen::Index_ActualPosition:
			state.angular_position = CanOpen::lsb_int32_to_int32(data);
			state.linear_position = state.angular_position / counts * wheel_circumference;
			break;
		case CanOpen::Index_LastFaultCode:
			last_fault = CanOpen::lsb_uint16_to_uint16(data);
			break;
		case Index_MotorTemperature:
			state.temperature = CanOpen::lsb_int16_to_float(data, 10);
			break;
		case CanOpen::Index_RealtimeTargetTorque:
			state.target_torque = CanOpen::lsb_uint16_to_uint16(data);
			break;
		case CanOpen::Index_ActualTorque:
			state.actual_torque = CanOpen::lsb_uint16_to_uint16(data) * 100;
			break;
		case Index_BusVoltage:
//			state.bus_voltage = CanOpen::data_to_float(data, 100);
			break;
		case CanOpen::Index_StatusWord:
			state.status = CanOpen::lsb_uint16_to_uint16(data);
			break;
		}
	}


	/**
	 * Gets the motor's state.
	 * @returns The motor's state.
	 */
	MotorData get_state(void)
	{
		return state;
	}


	/**
	 * Gets the motor's last fault
	 * @returns The motor's last fault.
	 */
	uint16_t get_last_fault(void)
	{
		return last_fault;
	}

private:
	uint8_t id;
	MotorData state = {0};
	CanOpen* can;
	float wheel_circumference;
	static constexpr uint32_t counts = 4096;
	bool reverse;
	uint32_t acc_time, dec_time;
	uint16_t last_fault;
};

#endif /* INC_COMMS_ZLAC8015_H_ */
