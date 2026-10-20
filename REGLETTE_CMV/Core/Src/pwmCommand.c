/*
 * pwmCommand.c
 *
 *  Created on: Mar 20, 2024
 *      Author: Madec
 */

#include "main.h"
#include "pwmCommand.h"
#include "mccCommand.h"
#include "speed.h"
#include "tim.h"
#include "gpio.h"


//====================================================================
//			COMMANDE PWM
//====================================================================
static int PWM_Toggle;
volatile uint32_t PWM_Value;

//duty cycle de commande (2 degrees de precisions) vers valeur pulse
uint32_t pulseCalc(float val)
{
	uint32_t pulseVal = 0;

	if (val < 0.0f)
	{
		//If val inf to 0, pulseVal = 0
		pulseVal = (uint32_t)(0x0);
	}
	else if (val > 100.0f)
	{
		//If val sup to 100, pulseVal = 1000
		pulseVal = (uint32_t)(0x3E8);
	}
	else if ((val >= 0.0f) & (val <= 100.0f))
	{
		/* CCR = duty_cycle*ARR <=> CCR = duty_cycle * 100 * ARR(1000)  /100.0f*/
		pulseVal = (uint32_t)(10*val*(1000)/100.0f); // val*10 to keep 0.x precision
		pulseVal = pulseVal/10; // val/10 to be on point
	}
	return pulseVal;
}

void pwm_Init(void){

	PWM_Toggle = 0;
	PWM_Value = 0;
	TIM11->CCR1 = 10;
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
}

void pwm_SetDuty(float duty) //
{
	//Formula duty : Pulse Value = duty*(2^16 - 1)/100
	//Float = 32bits

	//__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, pulseCalc(duty));
	PWM_Value = pulseCalc(duty);
	TIM11->CCR1 = PWM_Value;
}


void pwm_SetVoltage(float voltage)
{
	// Voltage e [0.0,24.0]
	float duty = 0;

	if (voltage < 0.0f)
	{
		//If val inf to 0, duty = 0
		duty = 0.0f;
	}
	else if (voltage > 24.0f)
	{
		//If val sup to 24, duty = 100
		duty = 24.0f;
	}
	else if ((voltage >= 0.0f) & (voltage <= 24.0f))
	{
		duty = (voltage*100.0f)/(24.0f);
	}
	PWM_Value = pulseCalc(duty);
	TIM11->CCR1 = PWM_Value;
}

void pwm_Enable(void)
{
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
	PWM_Toggle = 1;
}

void pwm_Disable(void)
{
	HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1);
	PWM_Toggle = 0;
}

void pwm_Toggle(void)
{
	/*PWM_Toggle :
	* 0 = Off
	* 1 = On
	*/
	if (PWM_Toggle == 0)
	{
		pwm_Enable();
		PWM_Toggle = 1;
	}
	else
	{
		pwm_Disable();
		PWM_Toggle = 0;
	}
}


