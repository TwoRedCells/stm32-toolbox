/**
 * \file       tasks/battery_task.h
 * \brief      Retrieves status information from batteries.
 */

#ifndef INC_THREADS_BATTERY_H_
#define INC_THREADS_BATTERY_H_

#include "globals.hpp"
#include "comms/NecCanOpen.h"
#include "utility/Timer.h"
#include "diagnostics/Fault.h"


void evalutate_battery_status(void);


void battery_task(void *argument)
{
	log_d("Battery task started.");
	NecCanOpen can(batteries);
	can.setup(settings.CanBitrate);

	while (!can.is_alive)
	{
		can.nmt(NecCanOpen::NMT_ResetCommunications, 0x000);
		can.nmt(NecCanOpen::NMT_Operational, 0x000);
		can.loop();
		fault.alert(Fault::BatteryCommunications);
		log_e("Unable to reach battery over CAN.");
		osDelay(250);
	    watchdog.feed(WATCHDOG_BATTERY_TASK_FLAG);
	}
	fault.reset(Fault::BatteryCommunications);

	for(;;)
	{
	    watchdog.feed(WATCHDOG_BATTERY_TASK_FLAG);

		for (int b=1; b <=2; b++)
		{
		    // Request updates.
		    uint16_t cob = b + 0x600;
			can.request_sdo(cob, NecCanOpen::Index_Temperature, NecCanOpen::Subindex_TemperatureCell);
			can.request_sdo(cob, NecCanOpen::Index_Temperature, NecCanOpen::Subindex_TemperatureFET);
			can.loop();
			osDelay(100);
			can.request_sdo(cob, NecCanOpen::Index_RelativeStateOfCharge);
			can.request_sdo(cob, NecCanOpen::Index_AbsoluteStateOfCharge);
			can.loop();
			osDelay(100);
			can.request_sdo(cob, NecCanOpen::Index_BatteryStatus);
			can.request_sdo(cob, NecCanOpen::Index_CycleCount);
			can.loop();
			osDelay(100);
			can.request_sdo(cob, NecCanOpen::Index_Current);
			can.request_sdo(cob, NecCanOpen::Index_AverageCurrent);
			can.request_sdo(cob, NecCanOpen::Index_RawAverageCurrent);
			can.loop();
			osDelay(100);
			can.request_sdo(cob, NecCanOpen::Index_RunToEmpty);
			can.request_sdo(cob, NecCanOpen::Index_AverageTimeToEmpty);
			can.request_sdo(cob, NecCanOpen::Index_AverageTimeToFull);
			can.loop();
			osDelay(100);
			can.request_sdo(cob, NecCanOpen::Index_RemainingCapacity);
			can.request_sdo(cob, NecCanOpen::Index_FullChargeCapacity);
			can.loop();
			osDelay(100);
		}

		can.sync();
		can.loop();
		evalutate_battery_status();

        // Check whether we've lost comms with a battery.
        bool lost_comms = uptime - batteries[0].last_message > BATTERY_COMMS_LOST_TIMEOUT || uptime - batteries[1].last_message > BATTERY_COMMS_LOST_TIMEOUT;
        fault.update(Fault::BatteryCommunications, lost_comms);

		osDelay(200);
	}
}


/**
 * Returns true if either battery's status has specified bit set.
 * @param bit The bit tp check.
 * @return True if it is set.
 */
bool either(uint16_t bit)
{
    return batteries[0].status & bit || batteries[1].status & bit;
}


/**
 * Evaluates the batteries' status and raises faults accordingly.
 */
void evalutate_battery_status(void)
{
    fault.update(Fault::BatteryOvercharged, either(NecCanOpen::Status_OverChargedAlarm));
    fault.update(Fault::BatteryOvertemperature, either(NecCanOpen::Status_OverTempAlarm));
    fault.update(Fault::BatteryTerminateCharge, either(NecCanOpen::Status_TerminateChargeAlarm));
}

#endif /* INC_THREADS_BATTERY_H_ */
