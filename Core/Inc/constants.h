/**
 * \file       constants.h
 * \brief      Constants used by the software.
 */

#ifndef INC_CONSTANTS_H_
#define INC_CONSTANTS_H_

#include <stdint.h>

#define DEBUG  // verbose logging
#define FIRMWARE_VERSION "1.00.033"  // Update last 3 digits every build.
#define HARDWARE_VERSION "024697_A"

// Enable/disable features.
#define ENABLE_SETTINGS 		(1)  // Settings that can be changed at runtime.
#define ENABLE_PERSISTENCE 		(1)  // Save settings to EEPROM.


#define HIGH (GPIO_PIN_SET)
#define LOW (GPIO_PIN_RESET)

// Defaults.
#define DEFAULT_CAN_BITRATE (125000)
#define DEFAULT_CHARGE_DURATION (30)
#define DEFAULT_WAIT_FOR_CHARGE (10)

// Fan things.
#define FAN_MAX_ERROR (0.17)
#define FAN_MIN_SPEED (500)  // The fan shall not operate below this speed.
#define FAN_MAX_SPEED (9000)  // The fan shall not operate above this speed.
#define FAN_RPM_PER_DEGREE (800)  // The fan will go this much faster for every degree C difference between internal and external temperatures.
#define FAN_SPEED_SEEK_DILIGENCE (8)  // How actively the dog should chase its tail (lower is more aggressive).

// Test task things.
#define BATTERY_FULL_THRESHOLD  (95)  // Do not allow a test to start if SOC is above this.
#define BATTERY_DISCHARGE_THRESHOLD (50)  // Stop discharging when this lower limit is reached.
//#define BATTERY_DISCHARGE_THRESHOLD (75)  // Stop discharging when this lower limit is reached.
#define WAIT_FOR_USER_TO_READ_INTERVAL (1000)  // How to long show messages during test for user to read.
#define MINIMUM_CHARGE_CURRENT (+90)  // Charge current below which a test should be considered failed.

// Battery task things.
#define BATTERY_COMMS_LOST_TIMEOUT (5)  // After this many seconds we declare comms with battery broken.

// Watchdog.
#define WATCHDOG_BATTERY_TASK_FLAG  (0x01)
#define WATCHDOG_FAN_TASK_FLAG  (0x02)
#define WATCHDOG_THERMAL_TASK_FLAG  (0x04)
#define WATCHDOG_HMI_TASK_FLAG  (0x08)
#define WATCHDOG_TEST_TASK_FLAG  (0x10)

// Temperature task things.
#define MINIMUM_OPERATING_TEMPERATURE (5.0)
#define MAXIMUM_OPERATING_TEMPERATURE (60.0)

// Inter-task messages.
typedef uint16_t message;
const message MSG_NONE                       = 0x00;
const message MSG_TEST_START                 = 0x01;
const message MSG_TEST_ACTIVE_FAULT          = 0x02;
const message MSG_TEST_BATTERY_FULL          = 0x03;
const message MSG_TEST_BATTERY_EMPTY         = 0x04;
const message MSG_TEST_CONTACTOR_ENERGIZED   = 0x05;
const message MSG_TEST_CONTACTOR_FEEDBACK    = 0x06;
const message MSG_TEST_PILOT_ENERGIZED       = 0x07;
const message MSG_TEST_PILOT_FEEDBACK        = 0x08;
const message MSG_TEST_CONTACTOR_DEENERGIZED = 0x09;
const message MSG_TEST_PILOT_DEENERGIZED     = 0x0a;
const message MSG_TEST_PROGRESS              = 0x0b;
const message MSG_TEST_CHARGING              = 0x0c;
const message MSG_TEST_WAITING_FOR_CHARGER   = 0x0d;
const message MSG_TEST_INSUFFICIENT_CURRENT  = 0x0e;
const message MSG_TEST_SUCCESSFUL            = 0x0f;

// String table.
#define STR_OK						"OK"
#define STR_FAILED					"FAILED"
#define STR_SPACE                   " "
#define STR_RIGHT_ARROW             "\x7e"
#define STR_LEFT_ARROW              "\x7f"
#define STR_ONE_DECIMAL_FLOAT		"%.1f"
#define STR_TWO_DECIMAL_FLOAT       "%.2f"
#define STR_TITLE					"SPARKY"
#define STR_BACK_MENU               "\x7f back"
#define STR_STARTING_CHARGE_TEST	"Starting charge test"
#define STR_THERE_IS_A_FAULT		"There is a fault."
#define STR_FAULT_EQUALS			"FAULT = %x%x"
#define STR_BATTERY_SOC_TOO_HIGH	"Battery SOC too high"
#define STR_BATTERY_IS_EMPTY_OR		"Battery is empty or"
#define STR_NOT_COMMUNICATING		"not communicating."
#define STR_ALLOW_TO_DISCHARGE		"Allow to discharge."
#define STR_TO_RUN_TEST_SOC			"to run test (%u%%)."
#define STR_UNABLE_TO_CONTINUE      "Unable to continue."
#define STR_PLEASE_WAIT             "Please wait..."
#define STR_FAN_TIMER               "Unable to initialize fan timer."
#define STR_ENERGIZING_PILOT		"Energizing pilot"
#define STR_RELAY_AND_CHECKING		"relay and checking"
#define STR_THE_CONTACTS			"the contacts."
#define STR_ENERGIZING_CONTACTOR	"Energizing contactor"
#define STR_AND_CHECKING_THE		"and checking the"
#define STR_RELAY_CONTACTS			"relay contacts."
#define STR_PILOT_CONTACTS			"Pilot contacts"
#define STR_CONTACTOR_CONTACTS		"Contactor contacts"
#define STR_FAILED_TO_RESPOND		"failed to respond."
#define STR_WAITING_FOR_CHARGER		"Waiting for charger"
#define STR_TO_RAMP_UP_CURRENT		"to ramp up current."
#define STR_CHARGE_CURRENT_DID		"Charge current did"
#define STR_NOT_REACH_EXPECTED		"not reach expected"
#define STR_LEVEL_TEST_FAILED		"level. Test failed."
#define STR_RELAY_CONTACTS			"relay contacts."
#define STR_AND_CHECKING_THE		"and checking the"
#define STR_RELEASING_CONTACTOR		"Releasing contactor"
#define STR_RELEASING_PILOT			"Releasing pilot"
#define STR_RELAY_AND_CHECKING		"relay and checking"
#define STR_ALLOWING_CHARGER		"Allowing charger"
#define STR_TO_CHARGE_BATTERY		"to charge battery."
#define STR_TEST_COMPLETED			"Test completed."
#define STR_SUCCESS					"SUCCESS."
#define STR_CHARGE_COLON			"Charge:"
#define STR_STATUS_COLON			"Status:"
#define STR_CYCLES_COLON			"Cycles:"
#define STR_REMAIN_COLON			"Remain:"
#define STR_CHARGE_PILOT			"Charge pilot"
#define STR_CONTACTOR				"Contactor"
#define STR_DISCHARGE				"Discharge"
#define STR_C						"C"
#define STR_O						"O"
#define STR_HEX_HEX					"%x%x"
#define STR_SYSTEM_STATUS			"System Status"
#define STR_DIAGNOSTICS				"Diagnostics"
#define STR_START_TEST				"Start Test"
#define STR_SETTINGS				"Settings"
#define STR_CHARGE_TIME				"Charge Time"

#endif /* INC_CONSTANTS_H_ */
