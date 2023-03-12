/**
 * \file       tasks/fan_task.h
 * \brief      Controls fan speed.
 */

#ifndef INC_TASKS_FAN_TASK_H_
#define INC_TASKS_FAN_TASK_H_


#include "devices/Fan.h"
#include "diagnostics/Fault.h"

void fan_task(void *argument)
{
    log_d("Fan task started.");
    fan.setup(&htim3);
    fan.set_target_speed(3000);
    osDelay(1000);

    for(;;)
    {
        watchdog.feed(WATCHDOG_FAN_TASK_FLAG);

        osDelay(50);
         // Raise fault if there is no tachometer feedback.
        fault.update(Fault::FanTachmometer, fan.get_measured_speed() == 0);

        // Raise fault if there too much difference between measured speed and target speed.
        fault.update(Fault::FanSpeedError, fan.get_error() > 0.1f);

        bool temp_fault = (int)int_temp == 0 || (int)ext_temp == 0;

        fan.set(!temp_fault);

        // Set speed based on measured temperatures.
        if (!fan_override && !temp_fault)
        {
            float delta = int_temp - ext_temp;
            float speed = delta * settings.FanRpmPerDegree;
            if (speed < settings.MinFanSpeed)
                fan.set_target_speed(0);
            else if (speed > settings.MaxFanSpeed)
                fan.set_target_speed(settings.MaxFanSpeed);
            else
                fan.set_target_speed(speed);
        }

        fan.loop();
        HAL_GPIO_EXTI_Callback(FAN_TACH_Pin); // Trigger ISR occasionally, even if fan isn't turning, so it can recover.
   }
}

#endif /* INC_TASKS_FAN_TASK_H_ */
