/**
 * \file       tasks/temperature_task.h
 * \brief      Retreives temperature data from the sensors.
 */

#ifndef INC_TASKS_TEMPERATURE_TASK_H_
#define INC_TASKS_TEMPERATURE_TASK_H_

#include "devices/DS18x20.h"
#include "devices/Tmp101.h"
#include "diagnostics/Log.h"
#include "diagnostics/Fault.h"


void temperature_task(void *argument)
{
	log_d("Temperature task started.");
	DS18x20 ds18(TEMP_EXT_DATA_GPIO_Port, TEMP_EXT_DATA_Pin);

	Tmp101 tmp101;
	if (!tmp101.setup(&hi2c2))
	{
		log_e("I2C setup for TMP101 failed.");
		fault.alert(Fault::InternalTemperatureCommunications);
	}

	for(;;)
	{
        watchdog.feed(WATCHDOG_THERMAL_TASK_FLAG);
        watchdog.taunt();  // Reset watchdog here because this task is run least frequently.

        // Read internal temperature.
        if (!tmp101.read(int_temp))
            fault.alert(Fault::InternalTemperatureCommunications);
        else
            fault.reset(Fault::InternalTemperatureCommunications);

        // Read external temperature.
        if(!ds18.get_temperature(ext_temp))
            fault.alert(Fault::ExternalTemperatureCommunications);
        else
            fault.reset(Fault::ExternalTemperatureCommunications);

        // Fault if outside operating temps.
        fault.update(Fault::SystemOverTemperature, int_temp > MAXIMUM_OPERATING_TEMPERATURE);
        fault.update(Fault::SystemUnderTemperature, ext_temp < MINIMUM_OPERATING_TEMPERATURE);
        osDelay(5000);
	}
}



#endif /* INC_TASKS_TEMPERATURE_TASK_H_ */
