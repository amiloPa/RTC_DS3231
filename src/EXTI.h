/*
 * EXTI.h
 *
 *  Created on: 11.09.2020
 *      Author: Piotr
 */

#ifndef EXTI_H_
#define EXTI_H_

#include "stm32f10x.h"

#define  No_of_samles 10
volatile uint8_t flag;

	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

void EXTI_Conf(void);

#endif /* EXTI_H_ */
