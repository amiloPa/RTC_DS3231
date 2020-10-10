/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/



#include "stm32f10x.h"
#include <stdio.h>
#include "DS3231.h"
#include "UART.h"
#include "EXTI.h"


ErrorStatus HSEStartUpStatus;

volatile u8 flag = 0;
u16 i = 0;

void RCC_Conf(void);
void GPIO_Conf(void);
void NVIC_Conf(void);
void SysTick_Conf(void);
void show_data_and_time(TDATETIME *dt);
void show_temperature_rtc(TTEMP *temp);


//void delay_ms(int ms)
//{
//	int i, tms;
//	tms = 9000*ms;
//	for(i=0;i<tms;i++);
//}


int main(void)
{

	RCC_Conf();
	register_rtc_event_callback(show_data_and_time);
	register_temperature_rtc_event_callback(show_temperature_rtc);
	GPIO_Conf();
	I2C_Conf(100);
	EXTI_Conf();

	UART_init(115200);
	SysTick_Conf();

	//*******************************************************************
	//Ustawienie zegarów PORTUC mikroprocesora
	//*******************************************************************
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	//pod³aczenie zegara do portu C

	//*******************************************************************
	//Ustawienie pinów mikroprocesora
	//*******************************************************************
	gpio_pin_cfg(GPIOC, PC13,  gpio_mode_output_PP_2MHz);	// ustawienie pinu PC13 jako wyjœciowego typu Push-Pull o czêstotliwoœci granicznej narastania zbocza wynosz¹cej 2MHz

	uart_puts("\n\rTest \n\r");
	DS3231_init();

	//DS3231_set_rtc_datetime(&datetime, 2015, 3, 29, 1, 59, 55 );
	DS3231_set_rtc_datetime(&datetime, 2015, 10, 25, 2, 59, 55 );

	for(;;)
	{
		DS3231_EVENT();
	}
}



void show_data_and_time(TDATETIME *dt)
{
#ifdef DATETIME_AS_STRING

	char znak[2];

	itoa(dt->weakday, znak, 10);

	uart_puts(dt->date);
	uart_puts("\t");

	uart_puts(znak);
	uart_puts("\t");

	uart_puts(&days[dt->weakday - 2][4]);
	uart_puts("\t");

	uart_puts(dt->time);
	uart_puts("\t");

#endif

}

void show_temperature_rtc(TTEMP *temp)
{
#ifdef DATETIME_AS_STRING

	uart_puts(temp->temperature);
	uart_puts("\t");
	uart_puts("\n\r");
#endif

}

void SysTick_Conf (void)
{

#define SysTick_Frequency 9000000 // 9MHz

	SysTick_Config(F_PCLK2/8/1000);
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;

}

void RCC_Conf(void)
{
  // Reset ustawien RCC
  RCC_DeInit();

  // Wlacz HSE
  RCC_HSEConfig(RCC_HSE_ON);

  // Czekaj az HSE bedzie gotowy
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
	  /*
	   * wprowadzenie opoznien jest (waitstate) dla wy¿szych czêstotliwoœci taktowania
	   * jest spowodowane tym, ¿e maksymalna czêstotliwoœæ z jak¹ przeprowadzana
	   * jest komunikacja z pamiecia Flash moze wynosic 24 MHz
	   */
	  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	  // zwloka dla pamieci Flash
	  FLASH_SetLatency(FLASH_Latency_2);

	  // HCLK = SYSCLK
	  RCC_HCLKConfig(RCC_SYSCLK_Div1);

	  // PCLK2 = HCLK
	  RCC_PCLK2Config(RCC_HCLK_Div1);

	  // PCLK1 = HCLK/2
	  RCC_PCLK1Config(RCC_HCLK_Div2);

	  // PLLCLK = 8MHz * 9 = 72 MHz
	  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	  // Wlacz PLL
	  RCC_PLLCmd(ENABLE);

	  // Czekaj az PLL poprawnie sie uruchomi
	  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	  // PLL bedzie zrodlem sygnalu zegarowego
	  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	  // Czekaj az PLL bedzie sygnalem zegarowym systemu
	  while(RCC_GetSYSCLKSource() != 0x08);

	  //W³¹czenie systemu nadzoru sygna³u taktuj¹cego
	  //RCC_ClockSecuritySystemCmd(ENABLE);
  }

//  Wlacz taktowanie GPIOB
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}


void NVIC_Conf(void)
{
  // Tablica wektorow przerwan
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
}


void GPIO_Conf(void)
{
	GPIO_InitTypeDef GPIOInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_StructInit(&GPIOInit);

	//Definicja pinów dla I2C GPIOB_PIN6 - SCL, GPIOB_PIN7 - SDA
	GPIOInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIOInit.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIOInit.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB, &GPIOInit);


	 //Definicja pinu PB5 do zewnêtrznego przerwania EXTI

	GPIOInit.GPIO_Pin = GPIO_Pin_5;
	GPIOInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIOInit.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIOInit);

}



__attribute__((interrupt)) void SysTick_Handler(void)
{
//	BB(GPIOC->ODR, PC13) ^= 1;

}



