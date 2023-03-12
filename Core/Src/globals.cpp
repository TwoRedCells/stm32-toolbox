/**
 * \file       globals.cpp
 * \brief      Instantiates globally-scoped variables.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#include "globals.hpp"
#include "constants.h"


Lcd lcd;
Fan fan;
bool fan_override;
Log l;
Serial serial;
Encoder encoder;
Fault fault(FAULT_LED_GPIO_Port, FAULT_LED_Pin);
Nec12V35i batteries[2];
float int_temp, ext_temp;
uint32_t uptime;
Watchdog watchdog;
Settings settings;

// Resource handles.
CAN_HandleTypeDef hcan;
I2C_HandleTypeDef hi2c2;
UART_HandleTypeDef huart1;
Relay pilot;
Relay contactor;
Relay discharge;

// OS handles.
osMessageQueueId_t hmi_q;
osMessageQueueId_t work_q;
