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

#include "comms/CanOpen.h"
#include "devices/InventusBattery.h"
#include "utility/Timer.h"

class Inventus : public CanOpen, CanOpen::ICanOpenCallback
{
public:
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
	Inventus(InventusBattery* battery, CAN_HandleTypeDef* port) : CanOpen(port)
	{
		set_callback(this);
		this->battery = battery;
	}

	/**
	 * Sends SDO requests for CiA objects e.g. manufacturer, serial number, etc.
	 */
	void poll_metadata(void)
	{
		sdo(0x31, 0x1000); // Device type
		sdo(0x31, 0x1008); // Mfr device name
		sdo(0x31, 0x1009); // HW version
		sdo(0x31, 0x100a); // SW version
		sdo(0x31, 0x1018, 0x01); // Vendor ID
		sdo(0x31, 0x1018, 0x02); // Product code
		sdo(0x31, 0x1018, 0x03); // Revision no
		sdo(0x31, 0x1018, 0x04); // Serial no
	}


private:
	void on_sdo(uint16_t address, uint16_t index, uint8_t subindex, uint8_t* data)
	{
		uint8_t node = address & 0xff;
		if (index == 0x1000)
			battery->device_type = lsb_uint32_to_uint32(data);
		else if (index == 0x1008)
			bytes_to_string(data, battery->manufacturer_device_name, 4);
		else if (index == 0x1009)
			bytes_to_string(data, battery->manufacturer_hardware_version, 4);
		else if (index == 0x100a)
			battery->manufacturer_software_version = lsb_uint32_to_uint32(data);
		else if (index == 0x1018)
		{
			if (subindex == 0x01)
				battery->vendor_id = lsb_uint32_to_uint32(data);
			else if (subindex == 0x02)
				bytes_to_string(data, battery->product_code, 4);
			else if (subindex == 0x03)
				bytes_to_string(data, battery->revision_number, 4);
			else if (subindex == 0x04)
				bytes_to_string(data, battery->serial_number, 4);
			battery->metadata_received = true;
		}
	}

	void on_nmt(uint16_t node, uint8_t data)
	{
		battery->last_message = Timer::now();
		//if (!battery->metadata_received)
			poll_metadata();
	}

	void on_pdo(uint16_t cob, uint8_t* data)
	{
		uint8_t node = ((cob & 0xff) - 0x90) | 0x10;
		uint16_t pdo = cob - node;
		if ((pdo & 0x290) == 0x290)
			on_tpdo6(data);
		else if ((pdo & 0x190) == 0x190)
			on_tpdo5(data);
		else if ((pdo & 0x480) == 0x480)
			on_tpdo4(data);
		else if ((pdo & 0x380) == 0x380)
			on_tpdo3(data);
		else if ((pdo & 0x280) == 0x280)
			on_tpdo2(data);
		else if ((pdo & 0x180) == 0x180)
			on_tpdo1(data);
	}

	void on_tpdo1(uint8_t* data)
	{
		battery->number_of_batteries = lsb_uint8_to_uint8(data+0);
		battery->virtual_state_of_charge = lsb_uint8_to_uint8(data+1);
		battery->virtual_current_capacity = lsb_uint16_to_uint16(data+2);
		battery->virtual_remaining_run_time =  lsb_uint16_to_uint16(data+4);
		battery->virtual_remaining_charge_time =  lsb_uint16_to_uint16(data+6);
	}

	void on_tpdo2(uint8_t* data)
	{
		battery->virtual_voltage = lsb_uint16_to_float(data+0, 1000);
		battery->virtual_current = lsb_int16_to_float(data+2, 10);
		battery->virtual_discharge_current_limit = lsb_uint16_to_float(data+4, 10);
		battery->virtual_charge_cutoff_current_limit = lsb_uint16_to_float(data+6, 10);
		battery->virtual_full_charge_flag = lsb_uint8_to_uint8(data+7);
	}

	void on_tpdo3(uint8_t* data)
	{
		battery->virtual_battery_temperature = lsb_int16_to_float(data+0, 8);
		battery->virtual_discharge_cutoff_voltage = lsb_uint16_to_float(data+2, 1000);
		battery->virtual_charge_current_limit = lsb_uint16_to_float(data+4, 10);
		battery->virtual_maximum_charge_voltage = lsb_uint16_to_float(data+6, 1000);
	}

	void on_tpdo4(uint8_t* data)
	{
		battery->virtual_state_of_health = lsb_uint8_to_uint8(data+0);
		battery->number_of_faulted_batteries = lsb_uint8_to_uint8(data+1);
		battery->number_of_active_batteries = lsb_uint8_to_uint8(data+2);
		battery->virtual_operation_mode = lsb_uint8_to_uint8(data+3);
	}

	void on_tpdo5(uint8_t* data)
	{
		battery->virtual_regen_current_limit = lsb_uint16_to_float(data+0, 10);
		battery->virtual_minimum_cell_voltage = lsb_uint16_to_float(data+2, 1000);
		battery->virtual_maximum_cell_voltage = lsb_uint16_to_float(data+4, 1000);
		battery->cell_balance_status = lsb_uint16_to_uint16(data+6);
	}

	void on_tpdo6(uint8_t* data)
	{
		battery->virtual_average_voltage = lsb_uint16_to_float(data+0, 1000);
		battery->virtual_average_state_of_charge = lsb_uint8_to_uint8(data+2);
		battery->virtual_average_temperature = lsb_int16_to_int16(data+3);
		battery->heater_status = lsb_uint16_to_uint16(data+5);
		battery->master_node_id = lsb_uint8_to_uint8(data+7);
	}

private:
	InventusBattery* battery;
};

#endif /* INC_COMMS_NECCANOPEN_H_ */
