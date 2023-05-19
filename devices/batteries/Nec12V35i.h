/**
 * \file       devices/Lcd.h
 * \class      Lcd
 * \brief      NEC 12V35i battery data model.
 */

#ifndef INC_DEVICES_NEC12V35I_H_
#define INC_DEVICES_NEC12V35I_H_


class Nec12V35i
{
public:
	float module_voltage = 0;
	int16_t current = 0;
	int16_t average_current = 0;
	float raw_average_current = 0;
	float cell_voltages[4] = { 0, 0, 0, 0 };
	int16_t at_rate = 0;
	int16_t at_rate_timetofull = 0;
	int16_t at_rate_ok = 0;
	int16_t at_rate_timetoempty = 0;
	float cell_temperature = 0;
	float fet_temperature = 0;
	uint16_t remaining_capacity = 0;
	uint16_t full_charge_capacity = 0;
	int16_t run_to_empty = 0;
	int16_t average_time_to_empty = 0;
	int16_t average_time_to_full = 0;
	uint16_t relative_state_of_charge = 0;
	uint16_t absolute_state_of_charge = 0;
	uint16_t status = 0;
	uint16_t cycle_count = 0;
	uint32_t last_message = 0;
};


#endif /* INC_DEVICES_NEC12V35I_H_ */
