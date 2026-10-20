/*
 * pwmCommand.h
 *
 *  Created on: Mar 20, 2024
 *      Author: Madec
 */

#ifndef INC_PWMCOMMAND_H_
#define INC_PWMCOMMAND_H_

#include "main.h"

//====================================================================
//			COMMANDE PWM
//====================================================================

extern volatile uint32_t PWM_Value;

uint32_t pulseCalc(float val);

void pwm_Init(void);

void pwm_SetDuty(float duty);

void pwm_SetVoltage(float voltage);

void pwm_Enable(void);

void pwm_Disable(void);

void pwm_Toggle(void);


#endif /* INC_PWMCOMMAND_H_ */
