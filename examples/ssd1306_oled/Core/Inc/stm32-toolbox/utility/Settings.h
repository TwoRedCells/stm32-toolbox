/**
 * \file       utility/Settings.h
 * \class      Settings
 * \brief      Stores and persists configuration data.
 */

#ifndef INC_UTILITY_SETTINGS_H_
#define INC_UTILITY_SETTINGS_H_

#include <stdint.h>

#if ENABLE_PERSISTENCE
	#include "devices/FlashMemory.h"
#endif

class Settings
{
public:
    // Time let battery charge once handshake is complete.
    uint16_t ChargeTime = DEFAULT_CHARGE_DURATION;

    // Time to wait for charger to start charger after pilot relay closes.
    uint16_t WaitForCharger = DEFAULT_WAIT_FOR_CHARGE;

    // CAN bitrate.
    uint32_t CanBitrate = DEFAULT_CAN_BITRATE;

    // Don't test above this SOC.
    uint16_t MaxSocForTest = BATTERY_FULL_THRESHOLD;

    // Discharge to this SOC.
    uint16_t DischargeTarget = BATTERY_DISCHARGE_THRESHOLD;

    // Fan speed.
    uint16_t FanRpmPerDegree = FAN_RPM_PER_DEGREE;
    uint16_t MaxFanSpeed = FAN_MAX_SPEED;
    uint16_t MinFanSpeed = FAN_MIN_SPEED;


#if ENABLE_PERSISTENCE
    /**
     * Saves the settings to persistent memory.
     */
    void save(void)
    {
    	FlashMemory flash;
    	//flash.write((uint32_t*)this, sizeof(Settings));
    	flash.write((uint32_t*)&ChargeTime, 1);
    }


    /**
     * Loads the settings from persistent memory.
     */
    void load(void)
    {
    	FlashMemory flash;
    	//flash.read((uint32_t*)this, sizeof(Settings));
    	flash.read((uint32_t*)&ChargeTime, 1);
    }
#endif
};


#endif /* INC_UTILITY_SETTINGS_H_ */
