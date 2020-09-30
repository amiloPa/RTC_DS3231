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



//    //enable AFIO clock
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
//    EXTI_InitTypeDef EXTI_InitStructure;
//    //NVIC structure to set up NVIC controller
//    NVIC_InitTypeDef NVIC_InitStructure;
//    //GPIO structure used to initialize Button pins
//    //Connect EXTI Lines to Button Pins
//    GPIO_EXTILineConfig(BWAKEUPPORTSOURCE, BWAKEUPPINSOURCE);
//    GPIO_EXTILineConfig(BTAMPERPORTSOURCE, BTAMPERPINSOURCE);
//    GPIO_EXTILineConfig(BUSER1PORTSOURCE, BUSER1PINSOURCE);
//    GPIO_EXTILineConfig(BUSER2PORTSOURCE, BUSER2PINSOURCE);
//    //select EXTI line0
//    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//    //select interrupt mode
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    //generate interrupt on rising edge
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//    //enable EXTI line
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    //send values to registers
//    EXTI_Init(&EXTI_InitStructure);
//    //select EXTI line13
//    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
//    EXTI_Init(&EXTI_InitStructure);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
//    EXTI_Init(&EXTI_InitStructure);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
//    EXTI_Init(&EXTI_InitStructure);
    //disable AFIO clock
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  DISABLE);
//    //configure NVIC
//    //select NVIC channel to configure
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//    //set priority to lowest
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
//    //set subpriority to lowest
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
//    //enable IRQ channel
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    //update NVIC registers
//    NVIC_Init(&NVIC_InitStructure);
//    //select NVIC channel to configure
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
}


