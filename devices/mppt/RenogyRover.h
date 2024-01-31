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

	float RatedVoltage;
	float RatedChargingCurrent;
	float RatedDischargingCurrent;
	enum ProductTypes { Inverter, MPPT } ProductType;
	char Model[17];
	char FirmwareVersion[10];
	uint16_t HardwareVersion;
	char SerialNumber[10];
	uint16_t DeviceAddress;
	uint16_t SoC;
	float BatteryVoltage;
	float ChargingCurrent;
	int16_t ControllerTemperature;
	int16_t BatteryTemperature;
	float LoadVoltage;
	float LoadCurrent;
	uint16_t LoadPower;
	float PhotovoltaicVoltage;
	float PhotovoltaicCurrent;
	uint16_t PhotovoltaicPower;
	float MinimumBatteryVoltageToday;
	float MaximumBatteryVoltageToday;
	float MaximumChargingCurrentToday;
	float MaximumDischargingCurrentToday;
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

	void poll(void)
	{
		uint8_t buffer[20];

		// These two share a register.
		if (query_parameter(RegisterRatedVoltage, buffer))
		{
			uint16_t value = (uint16_t) *(uint16_t*) buffer;
			RatedVoltage = value >> 8;
			RatedChargingCurrent = value & 0xff;
		}

		if (query_parameter(RegisterRatedDischargingCurrent, buffer))
		{
			uint16_t value = (uint16_t) *(uint16_t*) buffer;
			RatedDischargingCurrent = value >> 8;
			ProductType = (ProductTypes)(value & 0xff);
		}

		if (query_parameter(RegisterModel, buffer))
		{
			memcpy(Model, buffer, 16);
			Model[16] = 0; // NUL terminator.
		}

		if (query_parameter(RegisterFirmwareVersion, buffer))
			PrintLite::vsprintf(FirmwareVersion, "%d.%d.%d", buffer[1], buffer[2], buffer[3]);

		if (query_parameter(RegisterHardwareVersion, buffer))
			HardwareVersion = (uint16_t) *(uint16_t*) buffer;

		if (query_parameter(RegisterSerialNumber, buffer))
			PrintLite::vsprintf(FirmwareVersion, "%d.%d.%d", buffer[0], buffer[1], *(uint16_t*)buffer+2);

		if (query_parameter(RegisterDeviceAddress, buffer))
			DeviceAddress = ptr_to_16(buffer);

		if (query_parameter(RegisterSoC, buffer))
			SoC = ptr_to_16(buffer);

		if (query_parameter(RegisterBatteryVoltage, buffer))
			BatteryVoltage = ptr_to_float(buffer) / 10;

		if (query_parameter(RegisterChargingCurrent, buffer))
			ChargingCurrent = ptr_to_float(buffer) / 100;

		if (query_parameter(RegisterControllerTemperature, buffer))
		{
			ControllerTemperature = buffer[0];
			BatteryTemperature = buffer[1];
		}

		if (query_parameter(RegisterLoadVoltage, buffer))
			LoadVoltage = ptr_to_float(buffer) / 10;

		if (query_parameter(RegisterLoadCurrent, buffer))
			LoadCurrent = ptr_to_float(buffer) / 100;

		if (query_parameter(RegisterLoadPower, buffer))
			LoadPower = ptr_to_16(buffer);

		if (query_parameter(RegisterPhotovoltaicVoltage, buffer))
			PhotovoltaicVoltage = ptr_to_float(buffer) / 10;

		if (query_parameter(RegisterPhotovoltaicCurrent, buffer))
			PhotovoltaicCurrent = ptr_to_float(buffer) / 100;

		if (query_parameter(RegisterPhotovoltaicPower, buffer))
			PhotovoltaicPower = ptr_to_16(buffer);

		if (query_parameter(RegisterMinimumBatteryVoltageToday, buffer))
			MinimumBatteryVoltageToday = ptr_to_float(buffer) / 10;

		if (query_parameter(RegisterMaximumChargingCurrentToday, buffer))
			MaximumChargingCurrentToday = ptr_to_float(buffer) / 100;

		if (query_parameter(RegisterMaximumDischargingCurrentToday, buffer))
			MaximumDischargingCurrentToday = ptr_to_float(buffer) / 100;

		if (query_parameter(RegisterMaximumChargingPowerToday, buffer))
			MaximumChargingPowerToday = ptr_to_16(buffer);

		if (query_parameter(RegisterMaximumDischargingPowerToday, buffer))
			MaximumDischargingPowerToday = ptr_to_16(buffer);

		if (query_parameter(RegisterChargingToday, buffer))
			ChargingToday = ptr_to_16(buffer);

		if (query_parameter(RegisterDischargingToday, buffer))
			DischargingToday = ptr_to_16(buffer);

		if (query_parameter(RegisterPowerGenerationToday, buffer))
			PowerGenerationToday = ptr_to_16(buffer);

		if (query_parameter(RegisterPowerConsumptionToday, buffer))
			PowerConsumptionToday = ptr_to_16(buffer);

		if (query_parameter(RegisterOperatingDays, buffer))
			OperatingDays = ptr_to_16(buffer);

		if (query_parameter(RegisterNumberOfBatteryOverDischarges, buffer))
			NumberOfBatteryOverDischarges = ptr_to_16(buffer);

		if (query_parameter(RegisterNumberOfBatteryFullCharges, buffer))
			NumberOfBatteryFullCharges = ptr_to_16(buffer);

		if (query_parameter(RegisterCumulativeCharging, buffer))
			CumulativeCharging = ptr_to_32(buffer);

		if (query_parameter(RegisterCumulativeDischarging, buffer))
			CumulativeDischarging = ptr_to_32(buffer);

		if (query_parameter(RegisterCumulativePowerGeneration, buffer))
			CumulativePowerGeneration = ptr_to_32(buffer);

		if (query_parameter(RegisterCumulativePowerConsumption, buffer))
			CumulativePowerConsumption =  ptr_to_32(buffer);

		if (query_parameter(RegisterFaults, buffer))
			Faults = ptr_to_32(buffer);
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
#ifdef DEBUG_MPPT
			G.log.log(LOGLEVEL_ERROR, error);
#endif
			return nullptr;
		}

		// 0x03 is the response to a request to read a register.
		if ((*p++ = mppt->read()) != 0x03)
		{
#ifdef DEBUG_MPPT
			G.log.log(LOGLEVEL_ERROR, error);
#endif
			return nullptr;
		}

		// The byte is the length.
		uint8_t size = *p++ = mppt->read();
#ifdef DEBUG_MPPT
		G.log.log(LOGLEVEL_DEBUG, "Received %d bytes.", size);
#endif

		for (int i=0; i < size; i++)
		{
			uint8_t b = mppt->read();
			*p++ = b;
#ifdef DEBUG_MPPT
			G.log.log(LOGLEVEL_DEBUG, "%y", b);
#endif
		}

		uint16_t crc16a = mppt->read() | mppt->read() << 8;
		uint16_t crc16b = Crc::crc16_modbus(buf, 3+size);
#ifdef DEBUG_MPPT
		G.log.log(LOGLEVEL_DEBUG, "CRC16: %x %x", crc16a, crc16b);
#endif
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
};

#endif /* LIB_STM32_TOOLBOX_DEVICES_MPPT_RENOGYROVER_H_ */
