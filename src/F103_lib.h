#ifndef F103_LIB_H
#define F103_LIB_H


#include <stm32f10x.h>

#define SRAM_BB_REGION_START	0x20000000
#define SRAM_BB_REGION_END		0x200fffff
#define SRAM_BB_ALIAS			0x22000000

#define PERIPH_BB_REGION_START	0x40000000
#define PERIPH_BB_REGION_END	0x400fffff
#define PERIPH_BB_ALIAS			0x42000000

#define SRAM_ADR_COND(adres) 	((uint32_t)&adres >= SRAM_BB_REGION_START && (uint32_t)&adres <= SRAM_BB_REGION_END)
#define PERIPH_ADR_COND(adres)	((uint32_t)&adres >= PERIPH_BB_REGION_START && (uint32_t)&adres <= PERIPH_BB_REGION_END)
#define BB_SRAM2(adres, bit)		(SRAM_BB_ALIAS + ((uint32_t)&adres - SRAM_BB_REGION_START)*32u + (uint32_t)(bit*4u))
#define BB_PERIPH(adres, bit)	(PERIPH_BB_ALIAS + ((uint32_t)&adres - PERIPH_BB_REGION_START)*32u +(uint32_t)__builtin_ctz(bit)*4u)

/*bit - bit mask, not bit position! */
#define BB(adres, bit)			*(__IO uint32_t *)(SRAM_ADR_COND(adres) ? BB_SRAM2(adres, bit) : (PERIPH_ADR_COND(adres) ? BB_PERIPH(adres,bit) :0))
#define BB_SRAM(adres, bit)		*(__IO uint32_t *)BB_SRAM2(adres, bit)


#define EXTI_PR_Reset_value ((uint32_t)0xFFF00000)
#define TIM_SR_Reset_value ((uint32_t)0xE000)

typedef enum	//GpioMode_t
{

	/* Push - Pull */
	gpio_mode_output_PP_2MHz = 2,
	gpio_mode_output_PP_10MHz = 1,
	gpio_mode_output_PP_50MHz = 3,

	/* Open-Drain */
	gpio_mode_output_OD_2MHz = 6,
	gpio_mode_output_OD_10MHz = 5,
	gpio_mode_output_OD_50MHz = 7,

	/* Push-Pull */
	gpio_mode_alternate_PP_2MHz = 10,
	gpio_mode_alternate_PP_10MHz = 9,
	gpio_mode_alternate_PP_50MHz = 11,

	/* Open-Drain */
	gpio_mode_alternate_OD_2MHz = 14,
	gpio_mode_alternate_OD_10MHz = 13,
	gpio_mode_alternate_OD_50MHz = 15,

	/* Analog input (ADC) */
	gpio_mode_input_analog = 0,
	/* Floating digital input */
	gpio_mode_input_floating = 4,
	/* Digital input with pull-up/down (depending on the ODR reg.) -> GPIOx->ODR = Pxy; // pull-up to VCC*/
	gpio_mode_input_pull = 8

/*
 *                              Table 20. Port bit configuration table
 * ______________________________________________________________________________________________
 * |					                  |	     |	    |		                    |		     |
 * | Configuration mode                   | CNF1 | CNF0 |            MODE           |     ODR    |
 * |					                  |	     |	    |				            |   register |
 * |______________________________________|______|______|___________________________|____________|
 * |                    |                 |      |      |                           |            |
 * |                    | push-pull       |      |   0  |                           | 0 or 1     |
 * | General purpose    |_________________|   0  |______|                           |____________|
 * |      output        |                 |      |      |                           |            |
 * |                    | Open-drain      |      |   1  | 00 - reserved             | 0 or 1     |
 * |____________________|_________________|______|______| 10 - max out speed  2 MHz |____________|
 * |                    |                 |      |      | 01 - max out speed 10 MHz |            |
 * |                    | push-pull       |      |   0  | 11 - max out speed 50 MHz | Don't care |
 * | Alternate Function |_________________|   1  |______|                           |____________|
 * |      output        |                 |      |      |                           |            |
 * |                    | Open-drain      |      |   1  |                           | Don't care |
 * |____________________|_________________|______|______|___________________________|____________|
 * |                    |                 |      |      |                           |            |
 * |                    | Analog          |      |   0  |                           | Don't care |
 * |                    |_________________|   0  |______|                           |____________|
 * |                    |                 |      |      |                           |            |
 * |                    | Input floating  |      |   1  |                           | Don't care |
 * |                    |_________________|______|______|              00           |____________|
 * |       Input        |                 |      |      |                           |            |
 * |                    | Input pull-down |      |      |                           |      0     |
 * |                    |_________________|   1  |   0  |                           |____________|
 * |                    |                 |      |      |                           |            |
 * |                    | Input pull-up   |      |      |                           |      1     |
 * |____________________|_________________|______|______|___________________________|____________|
 */
} GpioMode_t;

typedef enum	//GpioPin_t
{
	PA0 	= 0x00000001,
	PA1 	= 0x00000002,
	PA2 	= 0x00000004,
	PA3 	= 0x00000008,
	PA4 	= 0x00000010,
	PA5 	= 0x00000020,
	PA6 	= 0x00000040,
	PA7 	= 0x00000080,
	PA8 	= 0x00000100,
	PA9 	= 0x00000200,
	PA10 	= 0x00000400,
	PA11 	= 0x00000800,
	PA12 	= 0x00001000,
	PA13	= 0x00002000,
	PA14	= 0x00004000,
	PA15	= 0x00008000,
	PB0 	= 0x00000001,
	PB1 	= 0x00000002,
	PB2 	= 0x00000004,
	PB3 	= 0x00000008,
	PB4 	= 0x00000010,
	PB5 	= 0x00000020,
	PB6 	= 0x00000040,
	PB7 	= 0x00000080,
	PB8 	= 0x00000100,
	PB9 	= 0x00000200,
	PB10 	= 0x00000400,
	PB11 	= 0x00000800,
	PB12 	= 0x00001000,
	PB13 	= 0x00002000,
	PB14 	= 0x00004000,
	PB15 	= 0x00008000,
	PC13 	= 0x00002000,
	PC14 	= 0x00004000,
	PC15 	= 0x00008000,


} GpioPin_t;

//enum {SRAM_BB_REGION_START = 0x20000000};
//enum {SRAM_BB_REGION_END = 0x200fffff};

void gpio_pin_cfg(GPIO_TypeDef * const port, GpioPin_t pin, GpioMode_t mode);


#endif
