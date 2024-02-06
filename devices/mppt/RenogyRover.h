/*
 * RenogyRover.h
 *
 *  Created on: Jan 26, 2024
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_DEVICES_MPPT_RENOGYROVER_H_
#define LIB_STM32_TOOLBOX_DEVICES_MPPT_RENOGYROVER_H_

#include <stdint.h>
#include <functional>
#include "comms/Serial.h"
#include "utility/PrintLite.h"
#include "utility/Crc.h"


class RenogyRover
{
public:
	RenogyRover(Serial* mppt)
	{
		this->mppt = mppt;
	}

	struct Parameter
	{
		uint16_t Register;  // Address of register.
		uint8_t Length;		// Length of register value in 16-bit words.
	};

	static const constexpr Parameter RegisterRatedVoltage { 0x000a, 1 };
	static const constexpr Parameter RegisterRatedChargingCurrent { 0x000a, 1 };
	static const constexpr Parameter RegisterRatedDischargingCurrent { 0x000b, 1 };
	static const constexpr Parameter RegisterProductType { 0x000b, 1 };
	static const constexpr Parameter RegisterModel { 0x000c, 8 };
	static const constexpr Parameter RegisterFirmwareVersion { 0x0014, 2 };
	static const constexpr Parameter RegisterHardwareVersion { 0x0016, 2 };
	static const constexpr Parameter RegisterSerialNumber { 0x0018, 2 };
	static const constexpr Parameter RegisterDeviceAddress { 0x001a, 1 };
	static const constexpr Parameter RegisterSoC { 0x0100, 1 };
	static const constexpr Parameter RegisterBatteryVoltage { 0x0101, 1 };
	static const constexpr Parameter RegisterChargingCurrent { 0x0102, 1 };
	static const constexpr Parameter RegisterControllerTemperature { 0x0103, 1 };
	static const constexpr Parameter RegisterBatteryTemperature = { 0x0103, 1 };
	static const constexpr Parameter RegisterLoadVoltage { 0x0104, 1 };
	static const constexpr Parameter RegisterLoadCurrent { 0x0105, 1 };
	static const constexpr Parameter RegisterLoadPower { 0x0106, 1 };
	static const constexpr Parameter RegisterPhotovoltaicVoltage { 0x0107, 1 };
	static const constexpr Parameter RegisterPhotovoltaicCurrent { 0x0108, 1 };
	static const constexpr Parameter RegisterPhotovoltaicPower = { 0x0109, 1 };
	static const constexpr Parameter RegisterMinimumBatteryVoltageToday { 0x010b, 1 };
	static const constexpr Parameter RegisterMaximumBatteryVoltageToday { 0x010c, 1 };
	static const constexpr Parameter RegisterMaximumChargingCurrentToday { 0x010d, 1 };
	static const constexpr Parameter RegisterMaximumDischargingCurrentToday { 0x010e, 1 };
	static const constexpr Parameter RegisterMaximumChargingPowerToday { 0x010f, 1 };
	static const constexpr Parameter RegisterMaximumDischargingPowerToday { 0x0110, 1 };
	static const constexpr Parameter RegisterChargingToday { 0x0111, 1 };
	static const constexpr Parameter RegisterDischargingToday { 0x0112, 1 };
	static const constexpr Parameter RegisterPowerGenerationToday { 0x0113, 1 };
	static const constexpr Parameter RegisterPowerConsumptionToday { 0x0114, 1 };
	static const constexpr Parameter RegisterOperatingDays { 0x0115, 1 };
	static const constexpr Parameter RegisterNumberOfBatteryOverDischarges { 0x0116, 1 };
	static const constexpr Parameter RegisterNumberOfBatteryFullCharges { 0x0117, 1 };
	static const constexpr Parameter RegisterCumulativeCharging { 0x0118, 2 };
	static const constexpr Parameter RegisterCumulativeDischarging { 0x0111a, 2 };
	static const constexpr Parameter RegisterCumulativePowerGeneration { 0x011c, 2 };
	static const constexpr Parameter RegisterCumulativePowerConsumption { 0x011e, 2 };
	static const constexpr Parameter RegisterFaults { 0x0121, 2 };

	struct RenogyRoverState
	{
		int16_t ControllerTemperature;
		int16_t BatteryTemperature;

		uint16_t HardwareVersion;
		uint16_t DeviceAddress;
		uint16_t SoC;
		uint16_t LoadPower;
		uint16_t PhotovoltaicPower;
		uint16_t MaximumChargingPowerToday;
		uint16_t MaximumDischargingPowerToday;
		uint16_t ChargingToday;
		uint16_t DischargingToday;
		uint16_t PowerGenerationToday;
		uint16_t PowerConsumptionToday;
		uint16_t OperatingDays;
		uint16_t NumberOfBatteryOverDischarges;
		uint16_t NumberOfBatteryFullCharges;

		uint32_t CumulativeCharging;
		uint32_t CumulativeDischarging;
		uint32_t CumulativePowerGeneration;
		uint32_t CumulativePowerConsumption;
		uint32_t Faults;

		float RatedVoltage;
		float RatedChargingCurrent;
		float RatedDischargingCurrent;
		float BatteryVoltage;
		float ChargingCurrent;
		float LoadVoltage;
		float LoadCurrent;
		float PhotovoltaicVoltage;
		float PhotovoltaicCurrent;
		float MinimumBatteryVoltageToday;
		float MaximumBatteryVoltageToday;
		float MaximumChargingCurrentToday;
		float MaximumDischargingCurrentToday;

		enum ProductTypes { Inverter, MPPT } ProductType;
		char Model[17];
		char FirmwareVersion[10];
		char SerialNumber[10];
		bool IsOnline;  // Whether or not this record's data should be used.
	};

	RenogyRoverState get_state(void)
	{
		return state;
	}

	bool poll(void)
	{
		uint8_t buffer[20];
		uint8_t errors = 0;

		// These two share a register.
		if (query_parameter(RegisterRatedVoltage, buffer))
		{
			uint16_t value = (uint16_t) *(uint16_t*) buffer;
			state.RatedVoltage = value >> 8;
			state.RatedChargingCurrent = value & 0xff;
		}
		else
			errors++;

		if (query_parameter(RegisterRatedDischargingCurrent, buffer))
		{
			uint16_t value = (uint16_t) *(uint16_t*) buffer;
			state.RatedDischargingCurrent = value >> 8;
			state.ProductType = (RenogyRoverState::ProductTypes)(value & 0xff);
		}
		else
			errors++;

		if (query_parameter(RegisterModel, buffer))
		{
			memcpy(state.Model, buffer, 16);
			state.Model[16] = 0; // NUL terminator.
		}
		else
			errors++;

		if (query_parameter(RegisterFirmwareVersion, buffer))
			PrintLite::vsprintf(state.FirmwareVersion, "%d.%d.%d", buffer[1], buffer[2], buffer[3]);
		else
			errors++;

		if (query_parameter(RegisterHardwareVersion, buffer))
			state.HardwareVersion = (uint16_t) *(uint16_t*) buffer;
		else
			errors++;

		if (query_parameter(RegisterSerialNumber, buffer))
			PrintLite::vsprintf(state.SerialNumber, "%d.%d.%d", buffer[0], buffer[1], *(uint16_t*)buffer+2);
		else
			errors++;

		if (query_parameter(RegisterDeviceAddress, buffer))
			state.DeviceAddress = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterSoC, buffer))
			state.SoC = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterBatteryVoltage, buffer))
			state.BatteryVoltage = ptr_to_float(buffer) / 10;
		else
			errors++;

		if (query_parameter(RegisterChargingCurrent, buffer))
			state.ChargingCurrent = ptr_to_float(buffer) / 100;

		if (query_parameter(RegisterControllerTemperature, buffer))
		{
			state.ControllerTemperature = buffer[0];
			state.BatteryTemperature = buffer[1];
		}
		else
			errors++;

		if (query_parameter(RegisterLoadVoltage, buffer))
			state.LoadVoltage = ptr_to_float(buffer) / 10;
		else
			errors++;

		if (query_parameter(RegisterLoadCurrent, buffer))
			state.LoadCurrent = ptr_to_float(buffer) / 100;
		else
			errors++;

		if (query_parameter(RegisterLoadPower, buffer))
			state.LoadPower = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterPhotovoltaicVoltage, buffer))
			state.PhotovoltaicVoltage = ptr_to_float(buffer) / 10;
		else
			errors++;

		if (query_parameter(RegisterPhotovoltaicCurrent, buffer))
			state.PhotovoltaicCurrent = ptr_to_float(buffer) / 100;
		else
			errors++;

		if (query_parameter(RegisterPhotovoltaicPower, buffer))
			state.PhotovoltaicPower = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterMinimumBatteryVoltageToday, buffer))
			state.MinimumBatteryVoltageToday = ptr_to_float(buffer) / 10;
		else
			errors++;

		if (query_parameter(RegisterMaximumChargingCurrentToday, buffer))
			state.MaximumChargingCurrentToday = ptr_to_float(buffer) / 100;
		else
			errors++;

		if (query_parameter(RegisterMaximumDischargingCurrentToday, buffer))
			state.MaximumDischargingCurrentToday = ptr_to_float(buffer) / 100;
		else
			errors++;

		if (query_parameter(RegisterMaximumChargingPowerToday, buffer))
			state.MaximumChargingPowerToday = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterMaximumDischargingPowerToday, buffer))
			state.MaximumDischargingPowerToday = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterChargingToday, buffer))
			state.ChargingToday = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterDischargingToday, buffer))
			state.DischargingToday = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterPowerGenerationToday, buffer))
			state.PowerGenerationToday = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterPowerConsumptionToday, buffer))
			state.PowerConsumptionToday = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterOperatingDays, buffer))
			state.OperatingDays = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterNumberOfBatteryOverDischarges, buffer))
			state.NumberOfBatteryOverDischarges = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterNumberOfBatteryFullCharges, buffer))
			state.NumberOfBatteryFullCharges = ptr_to_16(buffer);
		else
			errors++;

		if (query_parameter(RegisterCumulativeCharging, buffer))
			state.CumulativeCharging = ptr_to_32(buffer);
		else
			errors++;

		if (query_parameter(RegisterCumulativeDischarging, buffer))
			state.CumulativeDischarging = ptr_to_32(buffer);
		else
			errors++;

		if (query_parameter(RegisterCumulativePowerGeneration, buffer))
			state.CumulativePowerGeneration = ptr_to_32(buffer);
		else
			errors++;

		if (query_parameter(RegisterCumulativePowerConsumption, buffer))
			state.CumulativePowerConsumption =  ptr_to_32(buffer);
		else
			errors++;

		if (query_parameter(RegisterFaults, buffer))
			state.Faults = ptr_to_32(buffer);
		else
			errors++;

		state.IsOnline = errors == 0;
		return errors == 0;
	}

private:
	void* parse_mppt_response(uint16_t length)
	{
		static uint8_t buf[20];
		uint8_t* p = buf;

#ifdef DEBUG_MPPT
		const char* error = "Malformed response from MPPT";
#endif

		// The address of the MPPT is always 1.
		if ((*p++ = mppt->read()) != 0x01)
		{
			return nullptr;
		}

		// 0x03 is the response to a request to read a register.
		if ((*p++ = mppt->read()) != 0x03)
		{
			return nullptr;
		}

		// The byte is the length.
		uint8_t size = *p++ = mppt->read();

		for (int i=0; i < size; i++)
		{
			uint8_t b = mppt->read();
			*p++ = b;
		}

		uint16_t crc16a = mppt->read() | mppt->read() << 8;
		uint16_t crc16b = Crc::crc16_modbus(buf, 3+size);
		if (crc16a != crc16b)
			return nullptr;

		return buf + 3;
	}

	bool query_parameter(RenogyRover::Parameter parameter, uint8_t* buffer)
	{
		// Structure of request packet.
		uint8_t data[8] = {
				0x01,  	// Query device #1
				0x03,	// Read register
				(uint8_t) (parameter.Register >> 8),
				(uint8_t) (parameter.Register & 0xff),
				(uint8_t) (parameter.Length >> 8),
				(uint8_t) (parameter.Length & 0xff),
				0x00, // CRC MSB
				0x00, // CRC LSB
		};

		// Calculate the CRC and inject it into the request.
		uint16_t crc = Crc::crc16_modbus(data, 6);
		data[6] = crc & 0xff;
		data[7] = crc >> 8;

		// Write the request.
		mppt->write(data, 8);
		osDelay(100);

		// Parse the response.
		if (mppt->available() != parameter.Length*2U + 5U)
		{
			mppt->flush_read();
			return false;
		}

		void* response = parse_mppt_response(parameter.Length);
		if (response != nullptr)
			memcpy(buffer, response, parameter.Length*2);
		return response != nullptr;
	}

	uint16_t ptr_to_16(uint8_t* b)
	{
		return b[1] | b[0] << 8;
	}

	float ptr_to_float(uint8_t* b)
	{
		return (float)(b[1] | b[0] << 8);
	}

	uint32_t ptr_to_32(uint8_t* b)
	{
		return b[3] | b[2] << 8 | b[1] << 16 | b[0] << 24;
	}

	Serial* mppt;
	RenogyRoverState state;
};

#endif /* LIB_STM32_TOOLBOX_DEVICES_MPPT_RENOGYROVER_H_ */
