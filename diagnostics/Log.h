///	@file       diagnostics/Log.h
///	@class      Log
///	@brief      A flexible serial logging subsystem.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE

#ifndef LOG_H
#define LOG_H

#include "comms/Serial.h"

#define DEC 10
#define HEX 16

enum LogLevels {
    LOGLEVEL_DEBUG      = 0,
    LOGLEVEL_INFO       = 1,
    LOGLEVEL_WARNING    = 2,
    LOGLEVEL_ERROR      = 3,
    LOGLEVEL_FATAL      = 4
};


/**
 * Handles logging information, primarily for debugging.
 */
class Log
{

public:
    Serial *serial;
    const char* level_names[5] = { "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL" };


    /**
     * Performs one-time initialization of the logging subsystem.
     */
    Log(Serial *port)
    {
   	serial = port;
        minimum_level = LOGLEVEL_INFO;
#ifdef USING_FREERTOS
		const osMutexAttr_t mutex_attr = {
			"SerialMutex",
			osMutexPrioInherit,
			NULL, 0
		};
		mutex = osMutexNew(&mutex_attr);
#endif
    }

    /**
     * Sets the threshold below which messages will not be output.
     * @param level The threshold level.
     */
    void set_loglevel(LogLevels level)
    {
        minimum_level = level;
    }

    /**
     * Logs a message.
     * @param level The logging level of this message.
     * @param message The text of the message.
     */
    template<typename... Args>
    void log(LogLevels level, const char* format, Args... args)
    {
        if (level < minimum_level)
            return;

        get_mutex();
        serial->printf("# %s: ", level_names[level]);
        serial->printf(format, args...);
        serial->printf("\r\n");
        last_level = level;
        release_mutex();
    }


    void get_mutex(void)
    {
#ifdef USING_FREERTOS
	if (osThreadGetId() != nullptr)
		osMutexAcquire(mutex, osWaitForever);
#endif
    }

    void release_mutex(void)
    {
#ifdef USING_FREERTOS
	if (osThreadGetId() != nullptr)
		osMutexRelease(mutex);
#endif
    }

private:
    LogLevels last_level;
    LogLevels minimum_level;

#ifdef USING_FREERTOS
    osMutexId_t mutex;
#endif

};
#endif
