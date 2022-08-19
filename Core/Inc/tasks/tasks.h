/**
 * \file       tasks/tasks.h
 * \brief      Manages the application tasks.
 */

#ifndef INC_THREADS_TASKS_H_
#define INC_THREADS_TASKS_H_

#include <FreeRTOS.h>
#include <task.h>
#include <tasks/thermal_task.h>
#include "tasks/test_task.h"
#include "tasks/battery_task.h"
#include "tasks/hmi_task.h"
#include "tasks/fan_task.h"

// Supposed to enforce stack overflow checking, but doesn't seem to work.
#define OS_STKSCHECK		     (1)
#define OS_STKINIT			     (1)
#define OS_RUNPRIV			     (1)

// Enable/disable tasks for debugging.
#define ENABLE_TEST_TASK         (0)
#define ENABLE_HMI_TASK          (1)
#define ENABLE_THERMAL_TASK      (0)
#define ENABLE_BATTERY_TASK      (0)
#define ENABLE_FAN_TASK          (0)

/**
 * Invoked when a task cannot be allocated.
 * @param task_name The name of the task.
 */
void startTaskFault(const char *task_name)
{
	log_f("Unable to start '%s' task.", task_name);
	fault.alert(Fault::TaskAllocation);
}


/**
 * Invoked when a queue cannot be allocated.
 * @param queue_name The name of the queue.
 */
void startQueueFault(const char *queue_name)
{
	log_f("Unable to start '%s' queue.", queue_name);
	fault.alert(Fault::QueueAllocation);
}


/**
 * Starts all run-time tasks.
 */
void startTasks(void)
{
    // Configure and start watchdog.
    watchdog.setup(WATCHDOG_THERMAL_TASK_FLAG | WATCHDOG_HMI_TASK_FLAG | WATCHDOG_TEST_TASK_FLAG | WATCHDOG_FAN_TASK_FLAG | WATCHDOG_BATTERY_TASK_FLAG);
    watchdog.start();

	// Create queues.
    hmi_q = osMessageQueueNew(3, sizeof(message), NULL);
    if (hmi_q == NULL)
        startQueueFault("hmi_q");

    work_q = osMessageQueueNew(3, sizeof(message), NULL);
    if (work_q == NULL)
        startQueueFault("work_q");

#if ENABLE_TEST_TASK
	const osThreadAttr_t test_attributes = {
			.name = "test",
			.stack_size = 128 * 4,
			.priority = (osPriority_t) osPriorityNormal,
	};
	if (osThreadNew(test_task, NULL, &test_attributes) == NULL)
		startTaskFault(test_attributes.name);
#endif

#if ENABLE_HMI_TASK
	const osThreadAttr_t hmi_attributes = {
			.name = "hmi",
			.stack_size = 300 * 4,
			.priority = (osPriority_t) osPriorityNormal,
	};
	if (osThreadNew(hmi_task, NULL, &hmi_attributes) == NULL)
		startTaskFault(hmi_attributes.name);
#endif

#if ENABLE_THERMAL_TASK
	const osThreadAttr_t temperature_attributes = {
			.name = "thermal",
			.stack_size = 96 * 4,
			.priority = (osPriority_t) osPriorityBelowNormal,
	};
	if (osThreadNew(temperature_task, NULL, &temperature_attributes) == NULL)
		startTaskFault(temperature_attributes.name);
#endif

#if ENABLE_BATTERY_TASK
    const osThreadAttr_t battery_attributes = {
            .name = "battery",
            .stack_size = 256 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };
    if (osThreadNew(battery_task, NULL, &battery_attributes) == NULL)
        startTaskFault(battery_attributes.name);
#endif

#if ENABLE_FAN_TASK
    const osThreadAttr_t fan_attributes = {
            .name = "fan",
            .stack_size = 64 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };
    if (osThreadNew(fan_task, NULL, &fan_attributes) == NULL)
        startTaskFault(fan_attributes.name);
#endif
}
#endif /* INC_THREADS_TASKS_H_ */
