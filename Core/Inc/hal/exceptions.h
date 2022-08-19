/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
 ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

 /**
   * @brief This function handles Non maskable interrupt.
   */
 void NMI_Handler(void)
 {
   while (1)
   {
   }
 }

 /**
   * @brief This function handles Hard fault interrupt.
   */
 void HardFault_Handler(void)
 {
   while (1)
   {
   }
 }

 /**
   * @brief This function handles Memory management fault.
   */
 void MemManage_Handler(void)
 {
   while (1)
   {
   }
 }

 /**
   * @brief This function handles Prefetch fault, memory access fault.
   */
 void BusFault_Handler(void)
 {
   while (1)
   {
   }
 }

 /**
   * @brief This function handles Undefined instruction or illegal state.
   */
 void UsageFault_Handler(void)
 {
   while (1)
   {
   }
 }

 /**
   * @brief This function handles Debug monitor.
   */
 void DebugMon_Handler(void)
 {

 }


#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
