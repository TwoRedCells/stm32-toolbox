/**
 * \file       devices/Encoder.h
 * \class      Encoder
 * \brief      Encapsulates a rotary encoder/pushbutton knob.
 */

#ifndef INC_DEVICES_ENCODER_H_
#define INC_DEVICES_ENCODER_H_

#include <stdint.h>
#include "hal/hal.h"


class Encoder
{
public:
    /**
     * Gets the current counter value since last reset.
     * @return The current value.
     */
    uint32_t get_value(void)
    {
        return value;
    }


    /**
     * Gets the direction of the last turn.
     * @return +1 if clockwise; -1 if counter-clockwise.
     */
    uint8_t get_direction(void)
    {
        return direction;
    }


    /**
     * Resets the internal counter to zero.
     */
    void reset(void)
    {
        value = 0;
    }


    /**
     * Invoked by hardware interupt when the encoder has changed position.
     * @note Should not be called directly by user code.
     * @param pin The pin that caused the interrupt.
     */
    void interrupt(uint16_t pin)
    {
        bool a = HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin);
        bool b = HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin);

        if (pin == ENCODER_A_Pin && b)
            decrement();
        else if (pin == ENCODER_B_Pin && a)
            increment();
    }


    /**
     * Gets whether the button is pressed.
     * @param wait True to have the method block return until the button is released; otherwise false.
     * @return True if pressed; otherwise false.
     */
    bool is_pressed(bool wait=false)
    {
        bool pressed = !HAL_GPIO_ReadPin(SELECT_BTN_GPIO_Port, SELECT_BTN_Pin);
        while (pressed && wait && !HAL_GPIO_ReadPin(SELECT_BTN_GPIO_Port, SELECT_BTN_Pin))
            osDelay(50);

        return pressed;
    }

private:
	void increment(void)
	{
		value += 1;
		direction = +1;
	}

	void decrement(void)
	{
		value -= 1;
		direction = -1;
	}

	int32_t value = 0;
	int8_t direction = 0;
};


extern Encoder encoder;

#endif /* INC_DEVICES_ENCODER_H_ */
