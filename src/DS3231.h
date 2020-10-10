/*
 * DS3231.h
 *
 *  Created on: 27.09.2020
 *      Author: Piotr
 */

#ifndef DS3231_H_
#define DS3231_H_

#include "stm32f10x.h"
#include "I2C.h"
#include "EXTI.h"

#define DATETIME_AS_STRING 1
#define DATA_SEPARATOR '-'
#define FILE_NAME_SEPARATOR '_'


typedef enum { pon =1, wto = 2, sro = 3, czw = 4, pia = 5, sob = 6, nied = 7 } TDAYS;
enum { _zimowy = 1, _letni = 2};

void DS3231_init (void);

const char days[7][4];


typedef struct
{
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	uint16_t YY;
	uint8_t MM;
	uint8_t DD;
	//uint8_t weakday;
	TDAYS weakday;
	uint8_t dst;

#if DATETIME_AS_STRING
	char time[9];
	char date [11];
	char YY_MM [12];
#endif
	uint8_t pcf_buf[5];
}TDATETIME;


typedef struct
{
	int8_t cel;
	uint8_t fract;

	int8_t avearage_cel;
	uint8_t avearage_fract;
	int16_t smaples_of_temp[No_of_samles];

#if DATETIME_AS_STRING
	char temperature[5];
#endif

	uint8_t pcf_buf[2];

} TTEMP;


TDATETIME datetime;
TTEMP temperature;


void DS3231_set_rtc_datetime(TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss );
void register_rtc_event_callback (void (*callback)(TDATETIME *dt));
void register_temperature_rtc_event_callback (void (*callback)(TTEMP *temp));
uint8_t summer_winter_time_correction (uint8_t * czas_lz, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh);
uint8_t calculate_weak_day ( uint8_t DD, uint8_t MM, uint16_t YY);
void DS3231_get_rtc_datetime(TDATETIME *dt);

void DS3231_EVENT ( void );

uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);

#endif /* DS3231_H_ */
