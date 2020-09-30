/*
 * I2C.h
 *
 *  Created on: 18.09.2020
 *      Author: Piotr
 */

#ifndef I2C_H_
#define I2C_H_
#include "stm32f10x.h"


void I2C_Conf(uint8_t SCK_speed);
void I2C_ADDRES(uint8_t SLA, uint32_t addr);
void I2C_WRITE(uint8_t SLA, uint32_t addr, int size, const void* data);
void I2C_READ(uint8_t SLA, uint32_t addr,  int size, void* data);

#endif /* I2C_H_ */
