///	@file       devices/InventusBattery.h
///	@class      InventusBattery
///	@brief      Represents an Inventusattery
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef INC_DEVICES_INVENTUSBATTERY_H_
#define INC_DEVICES_INVENTUSBATTERY_H_

#include <stdint.h>

struct InventusBattery
{
public:
	uint8_t node_id;
	uint8_t change_node_id;  // If set to non-zero, there is an outstanding request to change this node to the id set by this field.

	// TPDO1
	uint8_t number_of_batteries;
	uint8_t virtual_state_of_charge;
	uint16_t virtual_current_capacity;
	uint16_t virtual_remaining_run_time;
	uint16_t virtual_remaining_charge_time;

	// TPDO2
	float virtual_voltage; // in mV
	float virtual_current; //  in 100mA
	float virtual_discharge_current_limit; // in 100mA
	float virtual_charge_cutoff_current_limit; // in 100mA
	uint8_t virtual_full_charge_flag; // 0 all not charged; 1 all charged.

	// TPDO3
	float virtual_battery_temperature; // in 0.125C
	float virtual_discharge_cutoff_voltage; // mV
	float virtual_charge_current_limit; // in 100mA
	float virtual_maximum_charge_voltage; // mV

	// TPDO4
	uint8_t virtual_state_of_health; // %
	uint8_t number_of_faulted_batteries;
	uint8_t number_of_active_batteries;
	uint8_t virtual_operation_mode;
	uint16_t virtual_charge_faults;
	uint16_t virtual_discharge_faults;

	// TPDO5
	float virtual_regen_current_limit; // 100 mA
	float virtual_minimum_cell_voltage; // mV
	float virtual_maximum_cell_voltage; // mV
	uint16_t cell_balance_status; // See datasheet.

	// TPDO6
	// These include faulted batteries.
	float virtual_all_voltage; // mV
	uint8_t virtual_all_state_of_charge;
	int8_t virtual_all_temperature; // 0.125C
	uint16_t heater_status;
	uint8_t master_node_id;

	// Other
	uint32_t last_message;
	bool metadata_received; // CiA SDO response received.
	uint32_t timestamp_tpdo1 = 0;
	uint32_t timestamp_tpdo2 = 0;
	uint32_t timestamp_tpdo3 = 0;
	uint32_t timestamp_tpdo4 = 0;
	uint32_t timestamp_tpdo5 = 0;
	uint32_t timestamp_tpdo6 = 0;

	// Detail CiA 418
	uint8_t state_of_charge;
	float charge_current_requested;
	float battery_voltage;
	float charge_returned_during_last_charge;
	float charge_expended_during_last_charge;
	uint32_t cumulative_charge;
	char serial_number[9];
	uint16_t number_of_cells;
	uint16_t max_charge_current;
	uint16_t battery_capacity;
	uint8_t battery_type;
	float temperature;
	uint8_t charger_status;
	uint8_t battery_status;

	// Proprietary
	uint8_t state_of_health;
	uint16_t operational_mode;
	uint16_t charge_fault;
	uint16_t discharge_fault;
	float current;
	float regen_current_limit;
	float charge_current_limit;
	float discharge_current_limit;
	float minimum_cell_temperature;
	float maximum_cell_temperature;
	float minimum_cell_voltage;
	float maximum_cell_voltage;
};


#endif /* INC_DEVICES_INVENTUSBATTERY_H_ */
