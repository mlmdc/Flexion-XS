/*
 * accelerometer.c
 *
 *  Created on: Apr 9, 2024
 *      Author: amay
 */

#include "adc.h"
#include "dma.h"
#include "main.h"
#include "comm.h"
#include "accelerometer.h"
#include <stdlib.h>

#define N_CONVERSIONS 8
#define USE_12BIT_RESOLUTION 1
#define V_REF 3.3


#if USE_12BIT_RESOLUTION
#define MAX_VALUE 4095
#endif


volatile float acc_LastVoltage;
volatile float acc_LastAccel;

uint32_t acc_rawIntData[N_CONVERSIONS];
float acc_fltData[N_CONVERSIONS];
static uint32_t ADC_DATA_BUFFER;
int conv_index = 0;

float acc_zeroG = 1.6415f;
float acc_minusG = 1.961f;
float acc_plusG = 1.34f;
float acc_VperG = 0.305f;

extern uint32_t test;

void acc_Init(void)
{
	/*
	 * Tconv = (Sampling_Time + 12Bits)/(APB2_CLOCK/4) *
	 */
	/* accelerodata */
	acc_LastVoltage;
	acc_zeroG = 1.63f;
	acc_minusG = 1.93f;
	acc_plusG = 1.32f;
	acc_VperG = 0.305f;

	ADC1->SMPR2 |= (uint32_t)(0x7); //SET SAMPLING TIME TO 480 CYCLES, FCONV = 42.7 kHz, TCONV = 2.342 10^-5 s
	HAL_ADC_Start_DMA (&hadc1, &ADC_DATA_BUFFER, 1);
}

//Prise en compte des critiques de M. Bourgeot
//Permet le redémarrage à la volée de l'ADC et donc des mesures
void acc_StartDMA(void)
{
	HAL_ADC_Start_DMA (&hadc1, &ADC_DATA_BUFFER, 1);
}

//Prise en compte des critiques de M. Bourgeot
//Permet la coupure à la volée de l'ADC et donc des mesures


void acc_StopDMA(void)
{
	HAL_ADC_Stop_DMA (&hadc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   acc_rawIntData[conv_index] = ADC_DATA_BUFFER;
   //ADC1->CR2 |= ADC_CR2_DDS;
   conv_index++;
   if (conv_index >= N_CONVERSIONS)
   {
	   conv_index = 0;
   }

}

void acc_ReadDataBlocking(uint32_t* destination)
{
	for (int it = 0; it < (int)N_CONVERSIONS; it++)
	{
		//start ADC Conversion
		HAL_ADC_Start(&hadc1);
		//fetch value
		HAL_ADC_PollForConversion(&hadc1, 100);
		test = HAL_ADC_GetValue(&hadc1);
		destination[it] = (uint32_t)(0x0000FFFF & HAL_ADC_GetValue(&hadc1));
		HAL_ADC_Stop(&hadc1);
	}
}

/* Converts int 0-4095 to float 0-3.3, representing the voltage */
void acc_int2flt(uint32_t* source, float* destination)
{
	for (int n = 0; n < N_CONVERSIONS; n++)
	{
		destination[n] = ((float)(source[n]*10*V_REF)/((float)(MAX_VALUE*10)));
	}
}

//For best accuracy
// The higher the value of alpha, the more closely the EWMA tracks the original time series
float acc_EWMAverage(float* source, float alpha)
{
	//EWMA Formula :
	//EWMA[x] = alpha*source[x] + (1-alpha)*EWMA[x-1]
	//EWMA[0] & init
	float avgd_value = (float)(alpha*source[0])/10.0f;

	for (int i = 1; i < N_CONVERSIONS; i++)
	{
			avgd_value = (float)(alpha*source[i]+(1-alpha)*avgd_value);
	}
	return avgd_value;
}


//example value
void acc_GetValue(void)
{
	acc_int2flt(acc_rawIntData, acc_fltData);
	acc_LastVoltage = acc_fltData[conv_index];//acc_EWMAverage(acc_fltData, 1.0f);
}

void acc_Voltage2Accel(void)
{
	//Prendre Valeur Voltage
	acc_GetValue();
	//acc_LastAccel = ((acc_zeroG-acc_LastVoltage)/acc_VperG);
	acc_LastAccel = (float)((1.63f-acc_LastVoltage)/0.305f);
}

//====================================================================
//			accelerometer calibration
//====================================================================
/*
void acc_CalibrationZeroG(void)
{

	acc_GetValue();
	acc_zeroG = acc_LastVoltage;

}

void acc_CalibrationMinusG(void)
{

	acc_GetValue();
	acc_minusG = acc_LastVoltage;
}

void acc_CalibrationPlusG(void)
{

	acc_GetValue();
	acc_plusG = acc_LastVoltage;
}

void acc_Calibrate(void)
{

	float zeroG_c = 1.63f;

	zeroG_c = (acc_minusG+acc_plusG)/2;
	acc_zeroG = (acc_zeroG+zeroG_c)/2;
	acc_VperG = ((acc_zeroG-acc_plusG) + (acc_minusG - acc_zeroG))/2;

}
*/
