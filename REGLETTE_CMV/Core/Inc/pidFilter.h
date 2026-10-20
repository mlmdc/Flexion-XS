/*
 * filter.h
 *
 *  Created on: Apr 7, 2024
 *      Author: Madec
 */

#ifndef INC_PIDFILTER_H_
#define INC_PIDFILTER_H_

#include "main.h"
//====================================================================
//			PID
//====================================================================

extern volatile float Kp;
extern volatile float Kd;
extern volatile float Ki;
//extern volatile uint32_t te_ms;
extern volatile float Oe_k[3];
extern volatile float Uc_k[2];
extern volatile float command;

extern volatile float debug_tension;

void pidFilter_Init(void);

void pidFilter_SetKp(float new_Kp);

void pidFilter_SetKi(float new_Ki);

void pidFilter_SetKd(float new_Kd);

void pidFilter_SetCommand(float new_command);

void pidFilter_SetFrequency(float freq);

void pidFilter_SetCommandVoltage(float new_voltage);

void pidFilter_Compute(void);

#endif /* INC_PIDFILTER_H_ */
