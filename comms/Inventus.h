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
#include "devices/batteries/InventusBattery.h"
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
	static constexpr uint16_t Index_CurrentReturnedSinceLastCharge = 0x6052;
	static constexpr uint16_t Index_BatteryVoltage = 0x6060;
	static constexpr uint16_t Index_ChargeCurrentRequested = 0x6070;
	static constexpr uint16_t Index_BatterySoC = 0x6081;


	/**
	 * Instantiates the battery CANopen class.
	 * @param batteries A pointer to the array of batteries. Enough instances
	 *        of the battery class should be allocated based on the battery
	 *        IDs that will be responding on the bus.
	 */
	Inventus(CAN_HandleTypeDef* port) : CanOpen(port, roles::Master, true)
	{
		set_callback(this);
		master_battery = &batteries[0];
		master_battery->master_node_id = master_node_id;
		for (uint8_t i=0; i<16; i++)
		{
			batteries[i] = {0};
			batteries[i].node_id = 0x31 + i;
		}
	}


	InventusBattery* get_batteries(void)
	{
		return batteries;
	}

	InventusBattery* master_battery;
	static constexpr uint8_t master_node_id = 0x31;
	static constexpr uint8_t maximum_parallel_batteries = 15;

private:
	void on_sdo(uint16_t cob, uint16_t index, uint8_t subindex, uint8_t* data)
	{
		uint8_t node = cob_to_node(cob);
		assert(node >= 0x31 && node <= 0x3f);
		InventusBattery* battery = &batteries[node - master_node_id];

		switch (index)
		{
		case Index_BatteryStatus:
			break;
		case Index_ChargerStatus:
			break;
		case Index_Temperature:
			battery->temperature = lsb_int16_to_float(data) * 0.125f;
			break;
		case Index_BatteryInformation:
			switch (subindex)
			{
			case Subindex_BatteryType:
				break;
			case Subindex_Capacity:
				break;
			case Subindex_MaxChargeCurrent:
				break;
			case Subindex_NumberOfCells:
				break;
			}
			break;
		case Index_BatterySerial:
			switch (subindex)
			{
			case Subindex_BatterySerial1:
				break;
			case Subindex_BatterySerial2:
				break;
			}
			break;
		case Index_CumulativeCharge:
			break;
		case Index_CurrentExpendedSinceLastCharge:
			break;
		case Index_CurrentReturnedSinceLastCharge:
			break;
		case Index_BatteryVoltage:
			break;
		case Index_ChargeCurrentRequested:
			break;
		case Index_BatterySoC:
			battery->state_of_charge = lsb_uint8_to_uint8(data);
			break;

		}
		battery->metadata_received = true;
	}


	void on_nmt(uint8_t data)
	{
	}
	

	void on_heartbeat(uint8_t node)
	{
		assert(node >= 0x31 && node <= 0x3f);
		InventusBattery* battery = &batteries[node - master_node_id];
		battery->last_message = Timer::now();
	}


	void on_pdo(uint16_t cob, uint8_t* data)
	{
		uint8_t node = cob_to_node(cob);
		uint16_t pdo = cob - node;
		assert(node >= 0x31 && node <= 0x3f);
		InventusBattery* battery = &batteries[node - master_node_id];
		if (pdo == 0x290)
			on_tpdo6(battery, data);
		else if (pdo == 0x190)
			on_tpdo5(battery, data);
		else if (pdo == 0x480)
			on_tpdo4(battery, data);
		else if (pdo == 0x380)
			on_tpdo3(battery, data);
		else if (pdo == 0x280)
			on_tpdo2(battery, data);
		else if (pdo == 0x180)
			on_tpdo1(battery, data);
	}


	void on_tpdo1(InventusBattery* battery, uint8_t* data)
	{
		battery->number_of_batteries = lsb_uint8_to_uint8(data+0);
		battery->virtual_state_of_charge = lsb_uint8_to_uint8(data+1);
		battery->virtual_current_capacity = lsb_uint16_to_uint16(data+2);
		battery->virtual_remaining_run_time =  lsb_uint16_to_uint16(data+4);
		battery->virtual_remaining_charge_time =  lsb_uint16_to_uint16(data+6);
		battery->timestamp_tpdo1 = Timer::now();
	}


	void on_tpdo2(InventusBattery* battery, uint8_t* data)
	{
		battery->virtual_voltage = lsb_uint16_to_float(data+0, 1000);
		battery->virtual_current = lsb_int16_to_float(data+2, 10);
		battery->virtual_discharge_current_limit = lsb_uint16_to_float(data+4, 10);
		battery->virtual_charge_cutoff_current_limit = lsb_uint16_to_float(data+6, 10);
		battery->virtual_full_charge_flag = lsb_uint8_to_uint8(data+7);
		battery->timestamp_tpdo2 = Timer::now();
	}


	void on_tpdo3(InventusBattery* battery, uint8_t* data)
	{
		battery->virtual_battery_temperature = lsb_int16_to_float(data+0, 8);
		battery->virtual_discharge_cutoff_voltage = lsb_uint16_to_float(data+2, 1000);
		battery->virtual_charge_current_limit = lsb_uint16_to_float(data+4, 10);
		battery->virtual_maximum_charge_voltage = lsb_uint16_to_float(data+6, 1000);
		battery->timestamp_tpdo3 = Timer::now();
	}
	

	void on_tpdo4(InventusBattery* battery, uint8_t* data)
	{
		battery->virtual_state_of_health = lsb_uint8_to_uint8(data+0);
		battery->number_of_faulted_batteries = lsb_uint8_to_uint8(data+1);
		battery->number_of_active_batteries = lsb_uint8_to_uint8(data+2);
		battery->virtual_operation_mode = lsb_uint8_to_uint8(data+3);
		battery->charge_faults = lsb_uint16_to_uint16(data+4);
		battery->discharge_faults = lsb_uint16_to_uint16(data+6);
		battery->timestamp_tpdo4 = Timer::now();
	}


	void on_tpdo5(InventusBattery* battery, uint8_t* data)
	{
		battery->virtual_regen_current_limit = lsb_uint16_to_float(data+0, 10);
		battery->virtual_minimum_cell_voltage = lsb_uint16_to_float(data+2, 1000);
		battery->virtual_maximum_cell_voltage = lsb_uint16_to_float(data+4, 1000);
		battery->cell_balance_status = lsb_uint16_to_uint16(data+6);
		battery->timestamp_tpdo5 = Timer::now();
	}
	

	void on_tpdo6(InventusBattery* battery, uint8_t* data)
	{
		// Unlike TPDO2, these include faulted batteries.
		battery->virtual_all_voltage = lsb_uint16_to_float(data+0, 1000);
		battery->virtual_all_state_of_charge = lsb_uint8_to_uint8(data+2);
		battery->virtual_all_temperature = lsb_int16_to_int16(data+3);
		battery->heater_status = lsb_uint16_to_uint16(data+5);
		battery->master_node_id = lsb_uint8_to_uint8(data+7);
		for (uint8_t i=0; i < maximum_parallel_batteries; i++)
			batteries[i].master_node_id = battery->master_node_id;
		battery->timestamp_tpdo6 = Timer::now();
	}

	InventusBattery batteries[maximum_parallel_batteries];  // 0x31 to 0x3a
};

#endif /* INC_COMMS_NECCANOPEN_H_ */
