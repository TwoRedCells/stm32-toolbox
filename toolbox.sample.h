///	@file       toolbox.h
///	@brief      Compile-time constants used to configure the behaviour of stm32-toolbox.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef INC_TOOLBOX_H_
#define INC_TOOLBOX_H_

#include <stdint.h>
#include "main.h"

#define HIGH (GPIO_PIN_SET)
#define LOW (GPIO_PIN_RESET)

// FreeRTOS
#define FREERTOS

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
#define ENABLE_NEOPIXEL_BUILTIN_PATTERNS (1)	// Whether or not to include build-in patterns.
#define ENABLE_NEOPIXEL_DEMO_PATTERN (1)		// Whether or not to include the demo pattern.
#define ENABLE_NEOPIXEL_BINARYFILE (1)			// Whether or not to include binary file support.
#define NEOPIXEL_MAX_PIXELS (35)				// The maximum number of pixels.
#define NEOPIXEL_BUFFER_SIZE (NEOPIXEL_MAX_PIXELS + (NEOPIXEL_MAX_PIXELS+2) * 0x20)  // Allocate uint32_t[NEOPIXEL_BUFFER_SIZE].

// Flash memory
#define INTERNAL_FLASH_LAYOUT_SECTORS (1)   	// For MCUs that layout FLASH into sectors.
#define INTERNAL_FLASH_LAYOUT_PAGES (0)     	// For MCUs that layout FLASH into pages.
#define INTERNAL_FLASH_ALLOW_OVERWRITING (1)	// Can add a new copy of existing file. Old one will be unlinked.
#define INTERNAL_FLASH_DIRECTORY_SIZE (16)  	// The maximum number of directory entries (files) that the filesystem will support.
#define INTERNAL_FLASH_FILENAME_LENGTH (80)     // The maximum length of a filename, including the terminating NUL.
#define EXTERNAL_FLASH_FILENAME_LENGTH (80)     // The maximum length of a filename, including the terminating NUL.

// HTTPd
#define HTTP_SERVER_MAX_HANDLERS (3)

// One-time programmable
#define OTP_BASE_ADDRESS 0x1fff7800  // The memory address of the first block of OTP memory. See your MCU reference manual.
#define OTP_BLOCK_MAC 0  // Where to permanently store a MAC address.
#define OTP_BLOCK_SERIAL 1 // Where to store the serial number.

// StringBuilder
#define ENABLE_STRINGBUILDER_MALLOC (0)
#define STRINGBUILDER_BLOCK_SIZE (0x40)  // StringBuilder will allocate memory in these increments.

// Ethernet, DNS, DHCP, etc. using Wiznet W5500 modile.
#define ENABLE_W5500 (1)

// Displays
#define ENABLE_ILI9488_DMA (0)  // Stopped working.

// Serial.
#define SERIAL_USE_DMA_TX (0)  // Whether to use DMA for transmitting.

// Used by `Revision`, possibly others. Set to 0 if compiler says functions don't exist.
#define ENABLE_ADC_CALIBRATION (0)

// Used by `OledSsd1306`
#define OLED_SSD1306_WIDTH (128)
#define OLED_SSD1306_HEIGHT (64)

// Graphics
#define ICANVAS_MAX_WIDGETS (100)

// Generics
#define GENERICS_ALLOW_NEW (1)   // Whether the generics are allowed to use dynamic memory allocations.

// Whether to allow classes to call malloc for dynamic memory allocation.
// Chip
#define MCU_STM32L4

#ifdef MCU_STM32F4
#include "main.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal.h"
#endif


#ifdef MCU_STM32L4
#include "main.h"
#include "stm32l4xx_hal_conf.h"
#include "stm32l4xx_hal.h"
#endif
#endif

