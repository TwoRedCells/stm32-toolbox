/**
 * \file       globals.h
 * \brief      Globally scoped variables.
 */

#ifndef INC_GLOBALS_HPP_
#define INC_GLOBALS_HPP_

#include <cmsis_os.h>
extern osMessageQueueId_t hmi_q;
extern osMessageQueueId_t work_q;

#include "diagnostics/Log.h"
extern Log l;
#define log_i(...) l.log(LOGLEVEL_INFO, __VA_ARGS__)
#define log_w(...) l.log(LOGLEVEL_WARNING, __VA_ARGS__)
#define log_e(...) l.log(LOGLEVEL_ERROR, __VA_ARGS__)
#define log_d(...) l.log(LOGLEVEL_DEBUG, __VA_ARGS__)
#define log_f(...) l.log(LOGLEVEL_FATAL, __VA_ARGS__)

#include "devices/Lcd.h"
extern Lcd lcd;

#include "devices/Fan.h"
extern Fan fan;
extern bool fan_override;

#include "comms/Serial.hpp"
extern Serial serial;

#include "devices/Encoder.h"
extern Encoder encoder;

#include "diagnostics/Fault.h"
extern Fault fault;

#include "devices/Nec12V35i.h"
extern Nec12V35i batteries[2];

#include "devices/Relay.h"
extern Relay pilot;
extern Relay contactor;
extern Relay discharge;

#include "utility/Watchdog.h"
extern Watchdog watchdog;

#include "utility/Settings.h"
extern Settings settings;


extern TIM_HandleTypeDef htim3;
extern CAN_HandleTypeDef hcan;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;

extern float int_temp, ext_temp;
extern uint32_t uptime;


#endif /* INC_GLOBALS_HPP_ */
