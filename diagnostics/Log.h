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
        #ifdef DEBUG
    	serial = port;
        minimum_level = LOGLEVEL_INFO;
#ifdef FREERTOS_CONFIG_H
		const osMutexAttr_t mutex_attr = {
			"SerialMutex",
			osMutexPrioInherit,
			NULL, 0
		};
		mutex = osMutexNew(&mutex_attr);
#endif

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
        #ifdef DEBUG
        if (level < minimum_level)
            return;

        get_mutex();
        serial->printf("# %s: ", level_names[level]);
        serial->printf(format, args...);
        serial->printf("\r\n");
        last_level = level;
        release_mutex();
        #endif
    }


//    /**
//     * Outputs supplementary information to the last call to log().
//     * @param message A string to output.
//     */
//    template<typename... Args>
//    void then(const char* format, Args... args)
//    {
//        #ifdef DEBUG
//        if (last_level < minimum_level)
//            return;
//
//        get_mutex();
//        serial->printf(format, args...);
//        serial->print(" ");
//        release_mutex();
//        #endif
//    }
//
//    /**
//     * Outputs supplementary information to the last call to log().
//     * @param message Numeric data to output.
//     * @param base The base of the value for how it should be represented (defaults to hex).
//     */
//    void then(uint32_t message, uint8_t base=HEX)
//    {
//        #ifdef DEBUG
//        if (last_level < minimum_level)
//            return;
//
//        get_mutex();
//        serial->print(message, base);
//        serial->print(" ");
//        release_mutex();
//        #endif
//    }
//
//    /**
//     * Outputs supplementary information to the last call to log().
//     * @param message A string to output.
//     */
//    void finally(const char* message)
//    {
//        #ifdef DEBUG
//        then(message);
//        done();
//        #endif
//    }
//
//    /**
//     * Outputs supplementary information to the last call to log().
//     * @param message Numeric data to output.
//     * @param base The base of the value for how it should be represented (defaults to hex).
//     */
//    void finally(uint32_t message, uint8_t base=HEX)
//    {
//        #ifdef DEBUG
//        then(message, base);
//        done();
//        #endif
//    }
//
//    /**
//     * Prevents future next() calls from publishing without a prior log() call.
//     */
//    void done(void)
//    {
//        last_level = LOGLEVEL_DEBUG;
//    }


    void get_mutex(void)
    {
#ifdef USING_FREERTOS
	if (osThreadGetId() != nullptr)
		osMutexAcquire(mutex, osWaitForever);
#endif
    }

    void release_mutex(void)
    {
#ifdef FREERTOS_CONFIG_H
	if (osThreadGetId() != nullptr)
		osMutexRelease(mutex);
#endif
    }

private:
    LogLevels last_level;
    LogLevels minimum_level;

#ifdef FREERTOS_CONFIG_H
    osMutexId_t mutex;
#endif

};
#endif
