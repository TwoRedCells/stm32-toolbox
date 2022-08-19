/**
 * \file       tasks/battery_task.h
 * \brief      Performs testing of charger.
 */

#ifndef INC_THREADS_READY_H_
#define INC_THREADS_READY_H_

#include "diagnostics/Fault.h"
#include "diagnostics/Log.h"
#include "devices/Led.h"
#include "constants.h"
#define q_hmi_msg(x)    osMessageQueuePut(hmi_q, &x, 0, 0)

void run_test(void);


void test_task(void *argument)
{
	osStatus_t status;
	message msg;
	Led ready;
	ready.setup(READY_LED_GPIO_Port, READY_LED_Pin);

	log_d("Work task started.");

	for(;;)
	{
		watchdog.feed(WATCHDOG_TEST_TASK_FLAG);

		osDelay(100);
		// Wait for signal from HMI to start.
		status = osMessageQueueGet(work_q, &msg, NULL, 0U);
		if (status == osOK)
		{
			if (msg == MSG_TEST_START)
			{
				ready.off();  // Turn off green light.
				discharge.off();  // Stop discharging.
				run_test();
				ready.on();  // Restore green light.
			}
		}

		// Discharge when not in use.
		float v1 = batteries[0].relative_state_of_charge;
		float v2 = batteries[1].relative_state_of_charge;
		float min = v1 < v2 ? v1 : v2;
		discharge.set(min > settings.DischargeTarget);

		// Ready light
		uint32_t faults = fault.get();
		ready.set(faults == 0);
	}
}

void run_test(void)
{
	// Check for faults before we begin.
	uint32_t faults = fault.get();
	if (faults != Fault::None)
	{
		q_hmi_msg(MSG_TEST_ACTIVE_FAULT);
		return;
	}

	// Check whether the battery is too full to charge.
	if (batteries[0].relative_state_of_charge > settings.MaxSocForTest || batteries[1].relative_state_of_charge > settings.MaxSocForTest)
	{
		q_hmi_msg(MSG_TEST_BATTERY_FULL);
		return;
	}

	// Check if battery is reporting empty (or not reporting at all).
	if (batteries[0].relative_state_of_charge == 0 || batteries[1].relative_state_of_charge == 0)
	{
		q_hmi_msg(MSG_TEST_BATTERY_EMPTY);
		return;
	}

	// Close contactor and check.
	q_hmi_msg(MSG_TEST_CONTACTOR_ENERGIZED);
	osDelay(WAIT_FOR_USER_TO_READ_INTERVAL);
    watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
	if (!contactor.on())
	{
		contactor.off();
		fault.alert(Fault::ContactorFeedback);
		q_hmi_msg(MSG_TEST_CONTACTOR_FEEDBACK);
		return;
	}
	osDelay(WAIT_FOR_USER_TO_READ_INTERVAL);
    watchdog.feed(WATCHDOG_TEST_TASK_FLAG);

	// Close pilot relay and check.
	q_hmi_msg(MSG_TEST_PILOT_ENERGIZED);
	osDelay(WAIT_FOR_USER_TO_READ_INTERVAL);
    watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
	if (!pilot.on())
	{
		pilot.off();
		contactor.off();
		fault.alert(Fault::ChargePilotFeedback);
		q_hmi_msg(MSG_TEST_PILOT_FEEDBACK);
		return;
	}
    watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
	osDelay(WAIT_FOR_USER_TO_READ_INTERVAL);

	// Wait for charger to do stuff.
	q_hmi_msg(MSG_TEST_WAITING_FOR_CHARGER);
	for (int i=4; i < 8; i++)  // Three ticks on the graph, so...
	{
        watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
		q_hmi_msg(MSG_TEST_PROGRESS);
		if (batteries[0].current >= MINIMUM_CHARGE_CURRENT || batteries[1].current >= MINIMUM_CHARGE_CURRENT)
			break;
		osDelay(settings.WaitForCharger / 3);  // Divide this by three.
	}

	// Check that we are charging.
	if (batteries[0].current < MINIMUM_CHARGE_CURRENT || batteries[1].current < MINIMUM_CHARGE_CURRENT)
	{
		q_hmi_msg(MSG_TEST_INSUFFICIENT_CURRENT);
		pilot.off();
		contactor.off();
		return;
	}

	// Give it some time to charge.
	q_hmi_msg(MSG_TEST_CHARGING);
	for (int i=8; i < 17; i++)  // Eight ticks on the graph, so...
	{
        watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
		q_hmi_msg(MSG_TEST_PROGRESS);
		osDelay(settings.ChargeTime / 8);  // Divide total wait time by eight.
	}

	q_hmi_msg(MSG_TEST_CONTACTOR_DEENERGIZED);
	if (contactor.off())
	{
		contactor.off();
		fault.alert(Fault::ContactorFeedback);
		q_hmi_msg(MSG_TEST_CONTACTOR_FEEDBACK);
		return;
	}
    watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
	osDelay(WAIT_FOR_USER_TO_READ_INTERVAL);

	q_hmi_msg(MSG_TEST_PILOT_DEENERGIZED);
	if (pilot.off())
	{
		fault.alert(Fault::ChargePilotFeedback);
		q_hmi_msg(MSG_TEST_PILOT_FEEDBACK);
		return;
	}
    watchdog.feed(WATCHDOG_TEST_TASK_FLAG);
	osDelay(WAIT_FOR_USER_TO_READ_INTERVAL);

	q_hmi_msg(MSG_TEST_SUCCESSFUL);
}


#endif /* INC_THREADS_READY_H_ */
