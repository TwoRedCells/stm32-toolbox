/**
 * \file       comms/Inventus.h
 * \class      Inventus
 * \brief      Encapsulates CANopen communications with the Inventus TRX series batteries by extending the CanBus class.
 *
 * \author     Yvan Rodrigues <yvan.r@radskunkworks.com>
 * \copyright  Copyright (c) 2022, Robotic Assistance Devices, All rights reserved.
 */

#ifndef INC_COMMS_NECCANOPEN_H_
#define INC_COMMS_NECCANOPEN_H_

#include "comms/CanBus.h"
#include "devices/Battery.h"
#include "utility/Timer.h"

class Inventus : public CanBus
{
public:
	static constexpr uint8_t NMT_Operational = 0x01;
	static constexpr uint8_t NMT_Stopped = 0x02;
	static constexpr uint8_t NMT_Preoperational = 0x80;
	static constexpr uint8_t NMT_ResetNode = 0x81;
	static constexpr uint8_t NMT_ResetCommunications = 0x82;

	static constexpr uint16_t Index_DeviceChemistry = 0x200a;
	static constexpr uint16_t Index_SerialNumber = 0x2000;
	static constexpr uint16_t Index_DesignCapacity = 0x2005;
	static constexpr uint16_t Index_DesignVoltage = 0x2006;
	static constexpr uint16_t Index_ManufacturerName = 0x2008;
	static constexpr uint16_t Index_DeviceName = 0x2009;
	static constexpr uint16_t Index_BatteryStatus = 0x2104;
	static constexpr uint16_t Index_CycleCount = 0x2106;
	static constexpr uint16_t Index_ModuleVoltage = 0x2200;
	static constexpr uint16_t Index_CellVoltageIndex = 0x2205;
	static constexpr uint16_t Index_Temperature = 0x2209;
	static constexpr uint8_t Subindex_TemperatureCell = 0x01;
	static constexpr uint8_t Subindex_TemperatureFET = 0x02;
	static constexpr uint16_t Index_Current = 0x220a;
	static constexpr uint16_t Index_AverageCurrent = 0x220b;
	static constexpr uint16_t Index_RelativeStateOfCharge = 0x220d;
	static constexpr uint16_t Index_AbsoluteStateOfCharge = 0x220e;  // Doesn't seem to work.
	static constexpr uint16_t Index_RemainingCapacity = 0x220f;
	static constexpr uint16_t Index_FullChargeCapacity = 0x2210;
	static constexpr uint16_t Index_RunToEmpty = 0x2211;
	static constexpr uint16_t Index_AverageTimeToEmpty = 0x2212;
	static constexpr uint16_t Index_AverageTimeToFull = 0x2213;
	static constexpr uint16_t Index_RawAverageCurrent = 0x2215;
	static constexpr uint16_t Index_ChargingCurrentLimit = 0x2216;
	static constexpr uint8_t SubindexChargingCurrentLimit = 0x01;
	static constexpr uint16_t Index_ChargingVoltageLimit = 0x2216;
	static constexpr uint8_t SubindexChargingVoltageLimit = 0x02;

	static constexpr uint16_t Status_OverChargedAlarm = 0x8000;
	static constexpr uint16_t Status_TerminateChargeAlarm = 0x4000;
	static constexpr uint16_t Status_OverTempAlarm = 0x1000;
	static constexpr uint16_t Status_TerminateDischargeAlarm = 0x0800;
	static constexpr uint16_t Status_RemainingCapacityAlarm = 0x0200;
	static constexpr uint16_t Status_RemainingTimeAlarm = 0x0100;
	static constexpr uint16_t Status_Initialized = 0x0080;
	static constexpr uint16_t Status_Discharging = 0x0040;
	static constexpr uint16_t Status_FullyCharged = 0x0020;
	static constexpr uint16_t Status_FullyDischarged = 0x0010;


	/**
	 * Instantiates the battery CANopen class.
	 * @param batteries A pointer to the array of batteries. Enough instances
	 *        of the battery class should be allocated based on the battery
	 *        IDs that will be responding on the bus.
	 */
	Inventus(Battery* batteries, CAN_TypeDef* port) : CanBus(port)
	{
		this->batteries = batteries;
	}


	/**
	 * Sends the CANopen SYNC message.
	 */
	void sync(void)
	{
		uint8_t data[] = { 0xff, 0xff };
		send(0x080, data, 2);
		sync_count++;
	}


	/**
	 * Sends the CANopen NMT message.
	 * @param state The state to transition to.
	 * @param node The node ID that is the recipient of this message, or zero for all.
	 */
	void nmt(uint8_t state, uint8_t node=0)
	{
		uint8_t data[] = { state, node };
		send(0x000, data, 2);
	}


	/**
	 * Sends an SDO request message.
	 * @param address The address ID of the receiver.
	 * @param index The SDO index.
	 * @param subindex The SDO subindex.
	 */
	void request_sdo(uint16_t address, uint16_t index, uint8_t subindex=0)
	{
		uint8_t data[] = { 0x40, index & 0xff, index >> 8, subindex };
		send(address, data, 4);
	}

private:
	/**
	 * Invoked by the CanBus class when a message has arrived on the bus for master.
	 * @param address The CANopen address.
	 * @param data The data bytes.
	 * @param length The number of data bytes.
	 */
	void on_message(uint16_t address, uint8_t* data, uint8_t length) override
	{
		CanBus::on_message(address, data, length);
		uint8_t battery_index = (address & 0x003) - 1; // one -> zero index.
		Battery *battery = &batteries[battery_index];
		//battery->last_message = uptime;

		if ((address & 0xf00) == 0x100)
		{
			battery->module_voltage = data_to_float(data+0, 1000);
			battery->current = data_to_float(data+2, 1000);
			battery->average_current = data_to_float(data+4, 1000);
		}
		else if ((address & 0xf00) == 0x200)
		{
			battery->cell_voltages[0] = data_to_float(data+0, 1000);
			battery->cell_voltages[1] = data_to_float(data+2, 1000);
			battery->cell_voltages[2] = data_to_float(data+4, 1000);
			battery->cell_voltages[3] = data_to_float(data+6, 1000);
		}
		else if ((address & 0xf00) == 0x300)
		{
			battery->at_rate = data_to_int16(data+0);
			battery->at_rate_timetofull = data_to_int16(data+2);
			battery->at_rate_ok = data_to_int16(data+4);
			battery->at_rate_timetoempty = data_to_int16(data+6);
		}
		else if ((address & 0xf00) == 0x500)
		{
			uint16_t index = data_to_uint16(data+1);
			uint8_t subindex = data[3];

			if (index == Index_Temperature && subindex == Subindex_TemperatureCell)
				battery->cell_temperature = data_to_float(data+4, 10);
			else if (index == Index_Temperature && subindex == Subindex_TemperatureFET)
				battery->fet_temperature = data_to_float(data+4, 10);
			else if (index == Index_RelativeStateOfCharge)
				battery->relative_state_of_charge = data_to_uint16(data+4);
			else if (index == Index_AbsoluteStateOfCharge)
				battery->absolute_state_of_charge = data_to_uint16(data+4);
			else if (index == Index_BatteryStatus)
				battery->status = data_to_uint16(data+4);
			else if (index == Index_Current)
				battery->current = data_to_int16(data+4);
			else if (index == Index_AverageCurrent)
				battery->average_current = data_to_int16(data+4);
			else if (index == Index_CycleCount)
				battery->cycle_count = data_to_uint16(data+4);
			else if (index == Index_RemainingCapacity)
				battery->remaining_capacity = data_to_uint16(data+4);
			else if (index == Index_FullChargeCapacity)
				battery->full_charge_capacity = data_to_uint16(data+4);
			else if (index == Index_RunToEmpty)
				battery->run_to_empty = data_to_int16(data+4);
			else if (index == Index_AverageTimeToEmpty)
				battery->average_time_to_empty = data_to_int16(data+4);
			else if (index == Index_AverageTimeToFull)
				battery->average_time_to_full = data_to_int16(data+4);
			else if (index == Index_RawAverageCurrent)
				battery->raw_average_current = data_to_float(data+4, 1);
		}
	}


	/**
	 * Converts LSB word into uint16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	uint16_t data_to_uint16(uint8_t* data)
	{
		return data[0] | data[1] << 8;
	}


    /**
     * Converts LSB word into int16.
     * @param data Pointer to the first byte of the word.
     * @return The value.
     */
	int16_t data_to_int16(uint8_t* data)
	{
		return data[0] | data[1] << 8;
	}


    /**
     * Converts LSB word with implicit decimal into float.
     * @param data Pointer to the first byte of the word.
     * @param divisor Value to divide the integer value by to translate it to float.
     *        The default is 1 (zero decimal places).
     * @return The value.
     */
	float data_to_float(uint8_t* data, uint16_t divisor=1)
	{
		int16_t u = data_to_int16(data);
		return ((float)u) / (float)divisor;
	}

	Battery* batteries;
	uint32_t sync_count = 0;
};

#endif /* INC_COMMS_NECCANOPEN_H_ */
