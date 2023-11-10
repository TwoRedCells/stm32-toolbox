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
	uint8_t node_id = 0;
	uint8_t change_node_id = 0;  // If set to non-zero, there is an outstanding request to change this node to the id set by this field.

	// TPDO1
	uint8_t number_of_batteries = 0;
	uint8_t virtual_state_of_charge = 0;
	uint16_t virtual_current_capacity = 0;
	uint16_t virtual_remaining_run_time = 0;
	uint16_t virtual_remaining_charge_time = 0;

	// TPDO2
	float virtual_voltage = 0; // in mV
	float virtual_current = 0; //  in 100mA
	float virtual_discharge_current_limit = 0; // in 100mA
	float virtual_charge_cutoff_current_limit = 0; // in 100mA
	uint8_t virtual_full_charge_flag = 0; // 0 all not charged; 1 all charged.

	// TPDO3
	float virtual_battery_temperature = 0; // in 0.125C
	float virtual_discharge_cutoff_voltage = 0; // mV
	float virtual_charge_current_limit = 0; // in 100mA
	float virtual_maximum_charge_voltage = 0; // mV

	// TPDO4
	uint8_t virtual_state_of_health = 0; // %
	uint8_t number_of_faulted_batteries = 0;
	uint8_t number_of_active_batteries = 0;
	uint8_t virtual_operation_mode = 0;
	uint16_t virtual_charge_faults = 0;
	uint16_t virtual_discharge_faults = 0;

	// TPDO5
	float virtual_regen_current_limit = 0; // 100 mA
	float virtual_minimum_cell_voltage = 0; // mV
	float virtual_maximum_cell_voltage = 0; // mV
	uint16_t cell_balance_status = 0; // See datasheet.

	// TPDO6
	// These include faulted batteries.
	float virtual_all_voltage = 0; // mV
	uint8_t virtual_all_state_of_charge = 0;
	float virtual_all_temperature = 0; // 0.125C
	uint16_t heater_status = 0;
	uint8_t master_node_id = 0x31;

	// Other
	uint32_t last_message = 0;
	bool metadata_received = 0; // CiA SDO response received.
	uint32_t timestamp_tpdo1 = 0;
	uint32_t timestamp_tpdo2 = 0;
	uint32_t timestamp_tpdo3 = 0;
	uint32_t timestamp_tpdo4 = 0;
	uint32_t timestamp_tpdo5 = 0;
	uint32_t timestamp_tpdo6 = 0;
	uint8_t configure_node_id_response = 0xff;
	uint8_t store_configuration_response = 0xff;

	// Detail CiA 418
	uint8_t state_of_charge = 0;
	float charge_current_requested = 0;
	float battery_voltage = 0;
	float charge_returned_during_last_charge = 0;
	float charge_expended_during_last_charge = 0;
	uint32_t cumulative_charge = 0;
	char serial_number[9] = {0};
	uint16_t number_of_cells = 0;
	uint16_t max_charge_current = 0;
	uint16_t battery_capacity = 0;
	uint8_t battery_type = 0;
	float temperature = 0;
	uint8_t charger_status = 0;
	uint8_t battery_status = 0;

	// Proprietary
	uint8_t state_of_health = 0;
	uint16_t operational_mode = 0;
	uint16_t charge_fault = 0;
	uint16_t discharge_fault = 0;
	float current = 0;
	float regen_current_limit = 0;
	float charge_current_limit = 0;
	float discharge_current_limit = 0;
	float minimum_cell_temperature = 0;
	float maximum_cell_temperature = 0;
	float minimum_cell_voltage = 0;
	float maximum_cell_voltage = 0;


	/**
	 * Returns the description of the battery chemistry.
	 * @param battery_type The battery type value reported by the battery.
	 * @returns A string containing the battery chemistry.
	 */
	static const char* get_chemistry(uint8_t battery_type)
	{
		uint8_t type = battery_type >> 4;
		assert(type >= 0 && type < 14);
		const char* long_strings[] = { "Unknown", "Lead acid", "Nickel cadmium", "Nickel zinc", "Nickel iron", "Silver oxide", "Nickel hydrogen", "Nickel metal hydride", "Zinc/Alkaline/Manganese dioxide", "Lithium-ion", "Nickel Cobalt Manganese", "Metal air", "Lithium/Iron sulfide", "Sodium beta" };
		return long_strings[type];
	}


	/**
	 * Gets the string associated with the specified mode number.
	 * @param mode The mode number.
	 */
	static const char* get_mode_string(uint8_t mode)
	{
		assert(mode < 9);
		const char* mode_strings[] = { "NONE", "BALANCING",	"SHIP",	"PRE-DISCHARGE", "STANDBY",	"DISCHARGE", "CHARGE", "FAULT",	"PRE-CHARGE" };
		return mode_strings[mode];
	}
};

#endif /* INC_DEVICES_INVENTUSBATTERY_H_ */
