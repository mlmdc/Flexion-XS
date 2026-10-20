/*
 * accelerometer.h
 *
 *  Created on: Apr 9, 2024
 *      Author: amay
 */

#ifndef INC_ACCELEROMETER_H_
#define INC_ACCELEROMETER_H_

#include "main.h"

extern volatile float acc_LastVoltage;
extern volatile float acc_LastAccel;

void acc_Init(void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

void acc_ReadDataBlocking(uint32_t* destination);

void acc_int2flt(uint32_t* source, float* destination);

void acc_StartDMA(void);

//Prise en compte des critiques de M. Bourgeot
//Permet la coupure à la volée de l'ADC et donc des mesures


void acc_StopDMA(void);

//For best accuracy
// The higher the value of alpha, the more closely the EWMA tracks the original time series
float acc_EWMAverage(float* source, float alpha);

void acc_Voltage2Accel(void);

//example value
void acc_GetValue();
 /*
void acc_CalibrationZeroG(void);

void acc_CalibrationMinusG(void);

void acc_CalibrationPlusG(void);

void acc_Calibrate(void);
*/

#endif /* INC_ACCELEROMETER_H_ */
