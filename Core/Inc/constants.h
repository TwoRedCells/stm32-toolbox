/**
 * @file      constants.h
 * @author    Yvan Rodrigues <yvan.r@radskunkworks.com>
 * @brief     Constants used by stm32-toolbox.
 * @date      2021-08-15
 * @copyright Copyright (c) 2021 Robotic Assistance Devices
 */

#ifndef INC_CONSTANTS_H_
#define INC_CONSTANTS_H_

#include <stdint.h>

#define HIGH (GPIO_PIN_SET)
#define LOW (GPIO_PIN_RESET)

// CAN
#define CAN_DEFAULT_BITRATE (125000)

// Fan
#define FAN_MAX_ERROR (0.17)
#define FAN_MIN_SPEED (0)  // The fan shall not operate below this speed.
#define FAN_MAX_SPEED (7000)  // The fan shall not operate above this speed.
#define FAN_RPM_PER_DEGREE (800)  // The fan will go this much faster for every degree C difference between internal and external temperatures.
#define FAN_SPEED_SEEK_DILIGENCE (8)  // How actively the dog should chase its tail (lower is more aggressive).

// Battery task things.
#define BATTERY_COMMS_LOST_TIMEOUT (5)  // After this many seconds we declare comms with battery broken.

// Network
#define NETWORK_ENABLE_DHCP (0)
#define NETWORK_ENABLE_RANDOMIZE_MAC (0)
#define NETWORK_DEFAULT_MAC { 0x00, 0x08, 0xdc, 0xff, 0xff, 0xff }
#define NETWORK_DHCP_RETRY_INTERVAL (1000) // milliseconds

// Fault
#define FAULT_ENABLE_LED_SUPPORT (1)

// NeoPixel
#define ENABLE_NEOPIXEL_BUILTIN_PATTERNS (1)
#define ENABLE_NEOPIXEL_DEMO_PATTERN (1)
#define ENABLE_NEOPIXEL_BINARYFILE (1)

// Flash memory
#define FLASH_LAYOUT_SECTORS (1)   	// For MCUs that layout FLASH into sectors.
#define FLASH_LAYOUT_PAGES (0)     	// For MCUs that layout FLASH into pages.
#define FLASH_ALLOW_OVERWRITING (1)	// Can add a new copy of existing file. Old one will be unlinked.
#define FLASH_DIRECTORY_SIZE (16)  	// The maximum number of directory entries (files) that the filesystem will support.

// HTTPd
#define HTTP_SERVER_MAX_HANDLERS (3)
#endif /* INC_CONSTANTS_H_ */

// One-time programmable
#define OTP_BASE_ADDRESS 0x1fff7800  // The memory address of the first block of OTP memory. See your MCU reference manual.
#define OTP_BLOCK_MAC 0  // Where to permanently store a MAC address.
#define OTP_BLOCK_SERIAL 1 // Where to store the serial number.
