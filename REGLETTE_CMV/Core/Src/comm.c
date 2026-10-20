/*
 * comm.c
 *
 *  Created on: Mar 24, 2024
 *      Author: madec
 */


#include "main.h"
#include "comm.h"
#include "speed.h"
#include "accelerometer.h"
#include "usart.h"
#include "pwmCommand.h"
#include "tickTimer.h"
#include "pidFilter.h"
#include "mccCommand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//====================================================================
//			SIZE
//====================================================================

#define ID_CHAR_SIZE 4
#define FLOAT_CHAR_SIZE 16
#define INT_CHAR_SIZE 8
#define UINT32_CHAR_SIZE 16
#define UINT16_CHAR_SIZE 8

#define RX_BUFFER_SIZE 	(int)16

//====================================================================
//			RX IDENTIFIERS TABLE
//====================================================================
//ALL UNSIGNED CHAR
#define ID_RX_START			"0x01"
#define ID_RX_STOP			"0x02"
#define ID_RX_PWM_START		"0x03"
#define ID_RX_PWM_STOP		"0x04"
#define ID_RX_DUTY_SET		"0x05"
#define ID_RX_DIR			"0x06"
#define ID_RX_VOLTAGE_SET  	"0x07"
#define ID_RX_SET_MAG		"0x08"
#define ID_RX_SET_COMMAND 	"0x09"

/*
#define ID_RX_SEND_KP		0x08
#define ID_RX_SET_KP		0x09
*/

#define ID_RX_ACK			"0xFF"
#define ID_RX_CALIBRATE 	"0xFE"
#define ID_RX_CALIB_ZEROG 	"0xFA"
#define ID_RX_CALIB_MINUSG 	"0xFB"
#define ID_RX_CALIB_PLUSG 	"0xFC"

#define ID_RX_SEND_KP		"0x0A"
#define ID_RX_SET_KP		"0x0B"
#define ID_RX_SEND_KI		"0x0C"
#define ID_RX_SET_KI		"0x0D"
#define ID_RX_SEND_KD		"0x0E"
#define ID_RX_SET_KD		"0x0F"

//====================================================================
//			TX IDENTIFIERS TABLE
//====================================================================
//ALL UNSIGNED CHAR
//ID size = 4 chars
#define ID_TX_TIME			"0x01"
#define ID_TX_ADC			"0x09"
#define ID_TX_ACC			"0x10"
#define ID_TX_PWM			"0x11"
#define ID_TX_RPM			"0x12"
#define ID_TX_DIR			"0x13"
#define ID_TX_ACK			"0xFF"

volatile int comm_ack = 0;
volatile int comm_start = 0;
volatile int comm_stop = 0;

char tx_data[64];
char *tx_buffer;

char tx_data_array[60000][64]; //max size, 60s @ 1ms
int add_data_index = 0;
int send_data_index = 0;

void comm_Init(void)
{
	/* USART RECEPTION */
	//HAL_USART_Receive_IT(&husart2, rx_data, RX_BUFFER_SIZE);
}

void debug_readRx(void)
{
	//printf("RX_DATA %s \n\r", rx_data);
}

//====================================================================
//			utils
//====================================================================

void slice_str(const char * str, char * buffer, size_t start, size_t end)
{
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

char * slice_str_r(const char * str, size_t start, size_t end)
{
	char * buffer;
	size_t j = 0;
	for (size_t i = start; i <= end; ++i ) {
	      buffer[j++] = str[i];
	}
	buffer[j] = 0;
	return buffer;
}

uint8_t get_used_bytes(const char *source)
{
	for (int i = 0; i < 64; i++)
	{
		if (source[i] == '\r')
		{
			return i;
		}
	}
	return 0;
}

//====================================================================
//			USART TX
//====================================================================

void comm_addData(uint32_t ms_value)
{
	sprintf(tx_data_array[add_data_index],"%s;%i;%s;%f;%s;%f;%s;%i;%s;%f;\n\r", //Only 1 line for sprintf
				ID_TX_TIME, (int)ms_value,
				ID_TX_ADC, acc_LastVoltage,
				ID_TX_ACC, (float)((1.63f-acc_LastVoltage)/0.305f),
				ID_TX_PWM, (int)PWM_Value,
				ID_TX_RPM, output_speed);
	add_data_index++;
}


void comm_SendData(uint32_t msvalue)
{
	//DATA_FORMAT : tx_data[x] = [ID_TX_TypeOfData]+[TypeOfData_0]+...+[TypeOfData_n]+[ID_TX_TypeOfData...]+...

	sprintf(tx_data,"%s;%i;%s;%.3f;%s;%.3f;%s;%i;%s;%.2f;\n\r", //Only 1 line for sprintf
			ID_TX_TIME, (int)HAL_GetTick(),//msvalue,
			ID_TX_ADC, acc_LastVoltage,
			ID_TX_ACC, acc_LastAccel,
			ID_TX_PWM, (int)PWM_Value,
			ID_TX_RPM, output_speed);

	uint8_t used_len = get_used_bytes(tx_data);

	HAL_UART_Transmit_DMA(&huart2,
			(uint8_t *)slice_str_r(tx_data, 0, used_len+2),
			(used_len+2));
}


//====================================================================
//			USART RX
//====================================================================

void comm_SliceInput(const char * input, char * identifier, char *value)
{
	char tmp_value[RX_BUFFER_SIZE-4];
	char *tmp_slice;
	char *tmp_out_value;
	/* Slice input string from 0 to 3 == the identifier of shape 0xXX */
	slice_str(input, identifier, 0, 3);

	/* Slice input string from 5 to buffer size -1 == the value*/
	slice_str(input, tmp_value, 4, RX_BUFFER_SIZE-1); // rest of the string

	/* cast from char*[RX_BUFFER_SIZE-4], incompatible with strsep, to char * */
	tmp_slice = (char *)tmp_value;
	/* SEParate the string with ; as delimiter, keeps the string before ; */
	tmp_out_value = strsep(&tmp_slice, ";");
	strcpy(value,tmp_out_value);
}

void comm_ComputeInput(char * identifier, char * value)
{
	/*
	 * str 1 == str 2 compare the address
	 * strcmp(str1,str2) compare the content
	 */
	if (strcmp(identifier,ID_RX_ACK) == 0)
	{
		comm_ack = 1;
	}
	else if (strcmp(identifier,ID_RX_START) == 0)
	{
		comm_start = 1;
		tickTimer_Start();
		comm_start = 0;
	}
	else if (strcmp(identifier,ID_RX_STOP) == 0)
		{
			comm_start = 1;
			tickTimer_Stop();
			comm_start = 0;
		}
	else if (strcmp(identifier,ID_RX_CALIBRATE) == 0)
	{
		comm_ack = 0;
		//acc_Calibrate();
	}
	else if (strcmp(identifier,ID_RX_CALIB_ZEROG) == 0)
	{
		comm_ack = 0;
		//acc_CalibrationZeroG();
	}
	else if (strcmp(identifier,ID_RX_CALIB_MINUSG) == 0)
	{
		comm_ack = 0;
		//acc_CalibrationMinusG();
	}
	else if (strcmp(identifier,ID_RX_CALIB_PLUSG) == 0)
	{
		comm_ack = 0;
		//acc_CalibrationPlusG();
	}
	else if (strcmp(identifier,ID_RX_STOP) == 0)
	{
		comm_stop = 1;
		tickTimer_Stop();
		comm_stop = 0;
	}
	else if (strcmp(identifier,ID_RX_PWM_START) == 0)
	{
		pwm_Enable();
	}
	else if (strcmp(identifier,ID_RX_PWM_STOP) == 0)
	{
		pwm_Disable();
	}
	else if (strcmp(identifier,ID_RX_DUTY_SET) == 0)
	{
		/* Data sanitization done in SetVoltage */
		pwm_SetDuty((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_DIR) == 0)
	{
		int tmp_dir = atoi(value);
		/* data sanitization */
		if (tmp_dir < 0)
		{
			tmp_dir = 0;
		}
		else if (tmp_dir > 1)
		{
			tmp_dir = 1;
		}
		/* Change of direction */
		if (tmp_dir == 0)
		{
				mcc_SetCCW();
		}
		else
		{
				mcc_SetCW();
		}
	}
	else if (strcmp(identifier,ID_RX_VOLTAGE_SET) == 0)
	{
		/* Data sanitization done in SetVoltage */
		pwm_SetVoltage((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_KP) == 0)
	{
		/* No data sanitization */
		pidFilter_SetKp((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_KI) == 0)
	{
		/* No data sanitization */
		pidFilter_SetKi((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_KD) == 0)
	{
		/* No data sanitization */
		pidFilter_SetKd((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_MAG) == 0)
		{
			/* No data sanitization */
			mcc_SetMagnet(atoi(value));
		}
	else
	{

	}
}
/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if ((send_data_index < 60000) || (send_data_index < 60000))
	{
		uint8_t used_len = get_used_bytes(tx_data[send_data_index]);

		HAL_UART_Transmit_DMA(&huart2,
						(uint8_t *)slice_str_r(tx_data[send_data_index], 0, used_len+2),
						(used_len+2));
		send_data_index++;
	}
}
*/
