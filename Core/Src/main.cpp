/**
 * \file       main.cpp
 * \brief      The main entry point of the application.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#define USE_FULL_ASSERT
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "globals.hpp"
#include "tasks/tasks.h"
#include "hal/hal.h"
#include "hal/gpio.h"
#include "hal/timers.h"
#include "cmsis_os.h"


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    // Initialize hardware.
    HAL_Init();
    SystemClock_Config();
    MX_TIM4_Init();
    MX_GPIO_Init();
    DWT_Delay_Init();

    // Configure logging.
    if (!serial.setup(&huart1))
        fault.alert(Fault::UARTCommunications);

    l.setup(&serial);
    l.set_loglevel(LOGLEVEL_DEBUG);
    log_d("");
    log_i(STR_TITLE);
    l.then(FIRMWARE_VERSION);

    // Configure outputs.
    pilot.setup(CHG_PILOT_EN_GPIO_Port, CHG_PILOT_EN_Pin, CHG_PILOT_MON_GPIO_Port, CHG_PILOT_MON_Pin, false, true);
    contactor.setup(CHG_EN_GPIO_Port, CHG_EN_Pin, CHG_RLY_IN_GPIO_Port, CHG_RLY_IN_Pin);
    discharge.setup(LOAD_EN_GPIO_Port, LOAD_EN_Pin, LOAD_EN_GPIO_Port, LOAD_EN_Pin);

#if ENABLE_PERSISTENCE
    // Load persistent settings.
    settings.load();
#endif

    // Start the tasks.
    osKernelInitialize();
    startTasks();
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    while (1)
    {
    }
}


/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    fault.alert(Fault::HardwareAbstractionLayerError);
    while (1)
    {
    }
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    log_e("ASSERTION FAILED: %s : %l", file, line);
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
