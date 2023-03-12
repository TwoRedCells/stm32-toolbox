/**
 * \file       devices/Watchdog.h
 * \class      Watchdog
 * \brief      Implements independent hardware watchdog abstraction.
 */

#ifndef INC_UTILITY_WATCHDOG_H_
#define INC_UTILITY_WATCHDOG_H_

#include "hal/hal.h"
#include "diagnostics/Fault.h"


class Watchdog
{
public:

    /**
     * Configures the watchdog.
     * @param expected_flags Value that is expected when healthy.
     */
    void setup(uint32_t expected_flags)
    {
        expected = expected_flags;
    }


    /**
     * Starts the watchdog.
     */
    void start(void)
    {
        hiwdg.Instance = IWDG;
        hiwdg.Init.Prescaler = IWDG_PRESCALER_128; // About 16 seconds.
        hiwdg.Init.Reload = 4095;
        if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
            fault.alert(Fault::Watchdog);
    }


    /**
     * Resets the watchdog timer.
     */
    void feed(uint32_t flag)
    {
        flags |= flag;
    }


    /**
     * Refreshes the watchdog only if all tasks have checked in.
     */
    void taunt(void)
    {
        if (flags == expected)
            HAL_IWDG_Refresh(&hiwdg);
        flags = 0;
    }

private:
    IWDG_HandleTypeDef hiwdg;
    volatile uint32_t flags = 0;
    uint32_t expected = 0;
};


#endif /* INC_UTILITY_WATCHDOG_H_ */
