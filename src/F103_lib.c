#include "F103_lib.h"


 void gpio_pin_cfg(GPIO_TypeDef * const port, GpioPin_t pin, GpioMode_t mode)
{
	pin = __builtin_ctz(pin)*4;

	uint32_t volatile * cr_reg;
	uint32_t cr_val;



	if(pin > 28)
	{
		pin -=32;
		cr_reg = &port->CRH;
	}
	else
	{
		cr_reg = &port->CRL;
	}

	cr_val = *cr_reg;
	cr_val &= ~((uint32_t)0x0f << pin);
	cr_val |= (uint32_t)(mode << pin);
	*cr_reg = cr_val;
}
