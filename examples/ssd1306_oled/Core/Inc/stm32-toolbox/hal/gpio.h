/*
 * gpio.h
 *
 *  Created on: Oct 5, 2021
 *      Author: Yvan
 */

#ifndef INC_HAL_GPIO_H_
#define INC_HAL_GPIO_H_

#include "globals.hpp"
#include "utility/Timer.h"

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(FAULT_LED_GPIO_Port, FAULT_LED_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, READY_LED_Pin|CHG_EN_Pin|CHG_PILOT_EN_Pin|LOAD_EN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin
			|LCD_D3_Pin|LCD_EN_Pin|LCD_RW_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : ENCODER_B_Pin ENCODER_A_Pin */
	GPIO_InitStruct.Pin = ENCODER_B_Pin|ENCODER_A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : FAULT_LED_Pin */
	GPIO_InitStruct.Pin = FAULT_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(FAULT_LED_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : READY_LED_Pin  */
	GPIO_InitStruct.Pin = READY_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : CHG_EN_Pin */
	GPIO_InitStruct.Pin = CHG_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LOAD_EN_Pin */
	GPIO_InitStruct.Pin = LOAD_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : SELECT_BTN_Pin */
	GPIO_InitStruct.Pin = SELECT_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SELECT_BTN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : CHG_RLY_IN_Pin */
	GPIO_InitStruct.Pin = CHG_RLY_IN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(CHG_RLY_IN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : CHG_PILOT_EN_Pin */
	GPIO_InitStruct.Pin = CHG_PILOT_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CHG_PILOT_EN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins :  CHG_PILOT_MON_Pin */
	GPIO_InitStruct.Pin = CHG_PILOT_MON_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : TEMP_EXT_DATA_Pin */
	GPIO_InitStruct.Pin = TEMP_EXT_DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : FAN_TACH_Pin */
	GPIO_InitStruct.Pin = FAN_TACH_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(FAN_TACH_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins :  LCD_D0_Pin LCD_D1_Pin LCD_D2_Pin
                           LCD_D3_Pin LCD_EN_Pin LCD_RW_Pin LCD_RS_Pin */
	GPIO_InitStruct.Pin = LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin
			|LCD_D3_Pin|LCD_EN_Pin|LCD_RW_Pin|LCD_RS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : VIN_Pin */
	GPIO_InitStruct.Pin = VIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(VIN_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/**
 * Callback invoked by hardware on interrupt. Redirects the call to the end-user.
 * @param pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
	// Interpret encoder states.
	if (pin == ENCODER_A_Pin || pin == ENCODER_B_Pin || pin == SELECT_BTN_Pin)
		encoder.interrupt(pin);

	if (pin == FAN_TACH_Pin)
	    fan.interrupt();
}


#endif /* INC_HAL_GPIO_H_ */
