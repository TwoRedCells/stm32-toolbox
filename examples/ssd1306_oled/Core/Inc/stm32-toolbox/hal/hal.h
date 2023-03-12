/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void Error_Handler(void);

#define ENCODER_B_Pin GPIO_PIN_13
#define ENCODER_B_GPIO_Port GPIOC
#define ENCODER_B_EXTI_IRQn EXTI15_10_IRQn
#define ENCODER_A_Pin GPIO_PIN_14
#define ENCODER_A_GPIO_Port GPIOC
#define ENCODER_A_EXTI_IRQn EXTI15_10_IRQn
#define FAULT_LED_Pin GPIO_PIN_15
#define FAULT_LED_GPIO_Port GPIOC
#define READY_LED_Pin GPIO_PIN_0
#define READY_LED_GPIO_Port GPIOA
#define SELECT_BTN_Pin GPIO_PIN_1
#define SELECT_BTN_GPIO_Port GPIOA
#define CHG_RLY_IN_Pin GPIO_PIN_2
#define CHG_RLY_IN_GPIO_Port GPIOA
#define CHG_EN_Pin GPIO_PIN_3
#define CHG_EN_GPIO_Port GPIOA
#define CHG_PILOT_EN_Pin GPIO_PIN_4
#define CHG_PILOT_EN_GPIO_Port GPIOA
#define TEMP_EXT_DATA_Pin GPIO_PIN_5
#define TEMP_EXT_DATA_GPIO_Port GPIOA
#define CHG_PILOT_MON_Pin GPIO_PIN_6
#define CHG_PILOT_MON_GPIO_Port GPIOA
#define LOAD_EN_Pin GPIO_PIN_7
#define LOAD_EN_GPIO_Port GPIOA
#define FAN_PWM_Pin GPIO_PIN_0
#define FAN_PWM_GPIO_Port GPIOB
#define FAN_TACH_Pin GPIO_PIN_1
#define FAN_TACH_GPIO_Port GPIOB
#define FAN_TACH_EXTI_IRQn EXTI1_IRQn
#define SCL2_Pin GPIO_PIN_10
#define SCL2_GPIO_Port GPIOB
#define SDA2_Pin GPIO_PIN_11
#define SDA2_GPIO_Port GPIOB
#define LCD_D0_Pin GPIO_PIN_12
#define LCD_D0_GPIO_Port GPIOB
#define LCD_D1_Pin GPIO_PIN_13
#define LCD_D1_GPIO_Port GPIOB
#define LCD_D2_Pin GPIO_PIN_14
#define LCD_D2_GPIO_Port GPIOB
#define LCD_D3_Pin GPIO_PIN_15
#define LCD_D3_GPIO_Port GPIOB
#define TX_Pin GPIO_PIN_9
#define TX_GPIO_Port GPIOA
#define RX_Pin GPIO_PIN_10
#define RX_GPIO_Port GPIOA
#define CAN_RX_Pin GPIO_PIN_11
#define CAN_RX_GPIO_Port GPIOA
#define CAN_TX_Pin GPIO_PIN_12
#define CAN_TX_GPIO_Port GPIOA
#define VIN_Pin GPIO_PIN_15
#define VIN_GPIO_Port GPIOA
#define LCD_EN_Pin GPIO_PIN_5
#define LCD_EN_GPIO_Port GPIOB
#define LCD_RW_Pin GPIO_PIN_8
#define LCD_RW_GPIO_Port GPIOB
#define LCD_RS_Pin GPIO_PIN_9
#define LCD_RS_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
