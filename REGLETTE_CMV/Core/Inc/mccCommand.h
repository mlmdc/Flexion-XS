/*
 * mccCommand.h
 *
 *  Created on: Mar 25, 2024
 *      Author: Madec
 */


#ifndef INC_MCCCOMMAND_H_
#define INC_MCCCOMMAND_H_

#include "main.h"

void debugled_SetOn(void);

void debugled_SetOff(void);

void debugled_Toggle(void);

void mcc_SetCW(void);

void mcc_SetCCW(void);

void mcc_SetMagnet(int state);

void mcc_ToggleDir(void);

#endif /* INC_MCCCOMMAND_H_ */
