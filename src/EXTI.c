/*
 * EXTI.c
 *
 *  Created on: 11.09.2020
 *      Author: Piotr
 */

#include "EXTI.h"

void EXTI_Conf(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);


	GPIO_InitTypeDef GPIOInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);


	 //Definicja pinu PB5 do zewnêtrznego przerwania EXTI

	GPIOInit.GPIO_Pin = GPIO_Pin_5;
	GPIOInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIOInit.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIOInit);


	//Connect EXTI Lines to Button Pins
	AFIO->EXTICR[1] = AFIO_EXTICR2_EXTI5_PB;


	/* PB5 is connected to EXTI_Line5 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line5;
    //select interrupt mode
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    //generate interrupt on rising edge
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	//enable EXTI line
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    //send values to registers
    EXTI_Init(&EXTI_InitStruct);

    //configure NVIC
    //select NVIC channel to configure
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    //set priority to lowest
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
    //set subpriority to lowest
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
    //enable IRQ channel
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    //update NVIC registers
    NVIC_Init(&NVIC_InitStruct);
}


void EXTI9_5_IRQHandler(void)
{
	static volatile uint8_t counter = 0;

    //Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
    	counter++;
    	flag = 1;
    }
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line5);

    if(counter == No_of_samles)
    {
    	counter = 0;
    }
}

