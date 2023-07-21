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
	static constexpr uint16_t Index_BatteryStatus = 0x6000;
	static constexpr uint16_t Index_ChargerStatus = 0x6001;
	static constexpr uint16_t Index_Temperature = 0x6010;
	static constexpr uint16_t Index_BatteryInformation = 0x6020;
	static constexpr uint8_t Subindex_BatteryType = 0x01;
	static constexpr uint8_t Subindex_Capacity = 0x02;
	static constexpr uint8_t Subindex_MaxChargeCurrent = 0x03;
	static constexpr uint8_t Subindex_NumberOfCells = 0x04;
	static constexpr uint16_t Index_BatterySerial = 0x6030;
	static constexpr uint8_t Subindex_BatterySerial1 = 0x01;
	static constexpr uint8_t Subindex_BatterySerial2 = 0x02;
	static constexpr uint16_t Index_CumulativeCharge = 0x6050;
	static constexpr uint16_t Index_CurrentExpendedSinceLastCharge = 0x6051;
	static constexpr uint16_t Index_CurrentReturnedSinceLastCharge = 0x6051;
	static constexpr uint16_t Index_BatteryVoltage = 0x6060;
	static constexpr uint16_t Index_ChargeCurrentRequested = 0x6070;
	static constexpr uint16_t Index_BatterySoC = 0x6081;


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
		sdo(0x31, Index_DeviceType);
		sdo(0x31, 0x1008); // Mfr device name
		sdo(0x31, Index_HardwareVersion);
		sdo(0x31, Index_SoftwareVersion);
		sdo(0x31, Index_Manufacturer, Subindex_VendorId); // Vendor ID
		sdo(0x31, Index_Manufacturer, Subindex_ProductCode); // Product code
		sdo(0x31, Index_Manufacturer, 0x03); // Revision no
		sdo(0x31, Index_Manufacturer, 0x04); // Serial no
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
		}
		battery->metadata_received = true;
	}


	void on_nmt(uint16_t node, uint8_t data)
	{
		battery->last_message = Timer::now();
		if (!battery->metadata_received)
			poll_metadata();
	}
	

	void on_pdo(uint16_t cob, uint8_t* data)
	{
		uint8_t node = ((cob & 0xff) - 0x90) | 0x10;
		uint16_t pdo = cob - node;
		if (pdo == 0x290)
			on_tpdo6(data);
		else if (pdo == 0x190)
			on_tpdo5(data);
		else if (pdo == 0x480)
			on_tpdo4(data);
		else if (pdo == 0x380)
			on_tpdo3(data);
		else if (pdo == 0x280)
			on_tpdo2(data);
		else if (pdo == 0x180)
			on_tpdo1(data);
	}


	void on_tpdo1(uint8_t* data)
	{
		battery->number_of_batteries = lsb_uint8_to_uint8(data+0);
		battery->virtual_state_of_charge = lsb_uint8_to_uint8(data+1);
		battery->virtual_current_capacity = lsb_uint16_to_uint16(data+2);
		battery->virtual_remaining_run_time =  lsb_uint16_to_uint16(data+4);
		battery->virtual_remaining_charge_time =  lsb_uint16_to_uint16(data+6);
		battery->timestamp_tpdo1 = Timer::now();
	}


	void on_tpdo2(uint8_t* data)
	{
		battery->virtual_voltage = lsb_uint16_to_float(data+0, 1000);
		battery->virtual_current = lsb_int16_to_float(data+2, 10);
		battery->virtual_discharge_current_limit = lsb_uint16_to_float(data+4, 10);
		battery->virtual_charge_cutoff_current_limit = lsb_uint16_to_float(data+6, 10);
		battery->virtual_full_charge_flag = lsb_uint8_to_uint8(data+7);
		battery->timestamp_tpdo2 = Timer::now();
	}


	void on_tpdo3(uint8_t* data)
	{
		battery->virtual_battery_temperature = lsb_int16_to_float(data+0, 8);
		battery->virtual_discharge_cutoff_voltage = lsb_uint16_to_float(data+2, 1000);
		battery->virtual_charge_current_limit = lsb_uint16_to_float(data+4, 10);
		battery->virtual_maximum_charge_voltage = lsb_uint16_to_float(data+6, 1000);
		battery->timestamp_tpdo3 = Timer::now();
	}
	

	void on_tpdo4(uint8_t* data)
	{
		battery->virtual_state_of_health = lsb_uint8_to_uint8(data+0);
		battery->number_of_faulted_batteries = lsb_uint8_to_uint8(data+1);
		battery->number_of_active_batteries = lsb_uint8_to_uint8(data+2);
		battery->virtual_operation_mode = lsb_uint8_to_uint8(data+3);
		battery->charge_faults = lsb_uint16_to_uint16(data+4);
		battery->discharge_faults = lsb_uint16_to_uint16(data+6);
		battery->timestamp_tpdo4 = Timer::now();
	}


	void on_tpdo5(uint8_t* data)
	{
		battery->virtual_regen_current_limit = lsb_uint16_to_float(data+0, 10);
		battery->virtual_minimum_cell_voltage = lsb_uint16_to_float(data+2, 1000);
		battery->virtual_maximum_cell_voltage = lsb_uint16_to_float(data+4, 1000);
		battery->cell_balance_status = lsb_uint16_to_uint16(data+6);
		battery->timestamp_tpdo5 = Timer::now();
	}
	

	void on_tpdo6(uint8_t* data)
	{
		battery->virtual_average_voltage = lsb_uint16_to_float(data+0, 1000);
		battery->virtual_average_state_of_charge = lsb_uint8_to_uint8(data+2);
		battery->virtual_average_temperature = lsb_int16_to_int16(data+3);
		battery->heater_status = lsb_uint16_to_uint16(data+5);
		battery->master_node_id = lsb_uint8_to_uint8(data+7);
		battery->timestamp_tpdo6 = Timer::now();
	}


	InventusBattery* battery = { 0 };
};

#endif /* INC_COMMS_NECCANOPEN_H_ */
