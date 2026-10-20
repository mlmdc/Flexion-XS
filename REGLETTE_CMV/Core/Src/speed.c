/*
 * speed.c
 *
 *  Created on: Mar 20, 2024
 *      Author: Madec
 */


#include "speed.h"
#include "main.h"
#include "tim.h"
#include "gpio.h"

volatile float output_speed;

#define N_MEASURES 8
#define M_PI (float)3.141592653589793
#define TE_ms 10 //
#define COUNT_PER_ROUND 64
#define MAX_CNT_PER_REV ((COUNT_PER_ROUND)*4 - 1)
#define MAX_COUNT 128
#define HALF_MAX_COUNT (MAX_COUNT>>1)

volatile int32_t Nindex;
int32_t pos_count;
int32_t angle_position[2];
float speed;

void encoder_Init(void)
{
	/* VALUES INIT */
	pos_count = 0;
	Nindex = 0;
	speed = 0;
	angle_position[0] = 0;
	angle_position[1] = 0;

	/* ENCODER INIT */
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);
}


uint16_t encoder_FetchPosition()
{
	int TIMCNT = TIM4->CNT;
	return (uint16_t)TIMCNT;
}

void encoder_UpdatePosition()
{
	angle_position[1] = angle_position[0];
	angle_position[0] = pos_count;
}

void encoder_GetSpeed()
{
	float delta_pos = 0; //init

	if (__HAL_TIM_DIRECTION_STATUS(&htim4)==1)
		{
			//Nindex--;
			// TEST
			pos_count -= (4000-encoder_FetchPosition());
		}
	else
		{
			//Nindex++;
			// TEST
			pos_count += encoder_FetchPosition();
		}
		__HAL_TIM_SetCounter(&htim4, 0);
		/* ENCODER RESTART */
		HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
		HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);

	encoder_UpdatePosition(); //fetch the position

	delta_pos = (float)angle_position[0] - (float)angle_position[1];


	/* SPEED TREATMENT */
	// 1 tour = 64
	// Nbre de Tours pendant Te: DELTA_pos/64
	// Nbre de Tours pendant 1s (Te en ms) : (DELTA_pos/64)*(1000/Te)
	// Nbre de Tours par minute : : (DELTA_pos/64)*((60*1000)/Te)

	speed = delta_pos/(float)256;
	speed = speed*(1000/TE_ms);
	output_speed = 0.1f*60.0f*speed;//*(5.0f/12.0f); //(19/129) = 3800/25800 == vitesse reelle a vide / vitesse constatée
}


void encoder_CallbackIndex()
{
	if (__HAL_TIM_DIRECTION_STATUS(&htim4)==1)
	{
		Nindex--;
		// TEST
		pos_count -= (4000-encoder_FetchPosition());
	}
	else
	{
		Nindex++;
		// TEST
		pos_count += encoder_FetchPosition();
	}
	__HAL_TIM_SetCounter(&htim4, 0);
	/* ENCODER RESTART */
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);

}

//====================================================================
//			EXTI PIN 7
//====================================================================

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_7) {
	  encoder_CallbackIndex();
  }
}





