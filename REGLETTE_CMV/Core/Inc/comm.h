/*
 * comm.h
 *
 *  Created on: Mar 24, 2024
 *      Author: Madec
 */


#ifndef INC_COMM_H_
#define INC_COMM_H_

#include "main.h"

extern volatile int comm_ack;
extern volatile int comm_start;
extern volatile int comm_stop;

void comm_Init(void);

void debug_readRx(void);

void comm_SendData(uint32_t msvalue);

void comm_SliceInput(const char * input, char * identifier, char *value);

void comm_ComputeInput(char * identifier, char * value);



#endif /* INC_COMM_H_ */
