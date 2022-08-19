/**
 * \file       utility/Pwm.h
 * \class      Pwm
 * \brief      Encapsulation of the STM32 PWM GPIO timer.
 */


#ifndef INC_PWM_H_
#define INC_PWM_H_


class Pwm
{
protected:
	virtual void PWM(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t period, uint16_t pulse)
	{
		HAL_TIM_PWM_Stop(timer, channel);
		TIM_OC_InitTypeDef sConfigOC;
		timer->Init.Period = period;
		HAL_TIM_PWM_Init(timer);
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = pulse;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, channel);
		HAL_TIM_PWM_Start(timer, channel);
	}
};



#endif /* INC_PWM_H_ */
