/*
 * DS3231.c
 *
 *  Created on: 29.08.2020
 *      Author: Piotr
 */


#include "DS3231.h"


#define DS32311_ADDRES 0xD0


uint16_t century;




const char days[7][4]= {
		"pon", "wto", "sro", "czw", "pia", "sob", "nie"
};



//---------------------------------------------------------------------------------
void DS3231_init (void)
{
	uint8_t ctrl = 0;
	datetime.dst = _letni;
	I2C_WRITE(DS32311_ADDRES, 0x0E, sizeof(ctrl), &ctrl);
}

//---------------------------------------------------------------------------------
void DS3231_set_rtc_time (TDATETIME *dt, uint8_t hh, uint8_t mm, uint8_t ss)
{

	dt->pcf_buf[0] = dec2bcd(ss);
	dt->pcf_buf[1] = dec2bcd(mm);
	dt->pcf_buf[2] = dec2bcd(hh);

	I2C_WRITE(DS32311_ADDRES, 0x00, 3, dt->pcf_buf);

}

//---------------------------------------------------------------------------------
void DS3231_set_rtc_date (TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD)
{
	century = YY / 100;
	dt->pcf_buf[3] = 1 + calculate_weak_day(DD, MM, YY);
	dt->pcf_buf[4] = dec2bcd(DD);
	dt->pcf_buf[5] = dec2bcd(MM);
	dt->pcf_buf[6] = dec2bcd(YY % 100);

	I2C_WRITE(DS32311_ADDRES, 0x03, 4, &dt->pcf_buf[3]);

}

//---------------------------------------------------------------------------------
void DS3231_set_rtc_datetime(TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss )
{
	DS3231_set_rtc_date(dt, YY, MM, DD);

	summer_winter_time_correction(&dt->dst, YY, MM, DD, hh);

	DS3231_set_rtc_time(dt, hh, mm, ss);
}

//---------------------------------------------------------------------------------
void DS3231_get_temp (TTEMP *temp)
{
	int8_t buf[1];
	uint8_t fr[4] = {0, 2, 5, 7};
	int16_t avearage_temp_value = 0;
	static uint8_t i = 1;
	uint8_t k;

	I2C_READ(DS32311_ADDRES, 0x11, 2, &temp->pcf_buf);

	uint8_t *wsk = temp->pcf_buf;

	temp->cel = *wsk;
	temp->fract = *(wsk + 1)>>6;


	if(i <= No_of_samles)
	{
			if(temp->cel >= 0) 	temp->smaples_of_temp[i-1] = 10 * temp->cel + fr[temp->fract];
			else 				temp->smaples_of_temp[i-1] = 10 * temp->cel + fr[3 - temp->fract];
	}

	else
	{
		for (k = 1; k < i - 1; k++)
		{
			temp->smaples_of_temp[k-1] = temp->smaples_of_temp[k];
		}

		if(temp->cel >= 0) 	temp->smaples_of_temp[--k] = 10 * temp->cel + fr[temp->fract];
		else 				temp->smaples_of_temp[--k] = 10 * temp->cel + fr[3 - temp->fract];
	}

	for (k = 0; k < i - 1; k++)
	{
		avearage_temp_value += temp->smaples_of_temp[k];
	}


	temp->avearage_cel = avearage_temp_value / 10 / (i - 1) ;
	temp->avearage_fract = (avearage_temp_value / (i - 1)) % 10;

	if( i <= No_of_samles) i++;

	I2C_READ(DS32311_ADDRES, 0x0e, 1, buf);
	buf[0] |= (1<<5);
	I2C_WRITE(DS32311_ADDRES, 0x0e, 1, buf);


#ifdef DATETIME_AS_STRING
	char *znak = temp->temperature;

	if(*wsk & 0x80) *(znak++) = '-';
	else			*(znak++) = ' ';


	if(temp->avearage_cel < 0) 	itoa((-1 * temp->avearage_cel), znak, 10);
	else						itoa(temp->avearage_cel, znak, 10);

	if((temp->avearage_cel > -10) && (temp->avearage_cel < 10) ) znak++;
	else						znak += 2;

	*(znak++) = ',';

	itoa(temp->avearage_fract,znak,10);

	*(++znak) = 0;

#endif
}

//---------------------------------------------------------------------------------
void DS3231_get_rtc_datetime(TDATETIME *dt)
{

	//https://www.youtube.com/watch?v=2qJDq2d0Qe0&ab_channel=Atnel-mirekk36
	I2C_READ(DS32311_ADDRES, 0x00, 7, dt->pcf_buf);

	int8_t i;
	uint8_t *wsk = dt->pcf_buf;
#ifdef DATETIME_AS_STRING
	char *znak = dt->time;
#endif
	//for(i = 0; i < 7; i++) dt->bytes[i] = bcd2dec(dt->pcf_buf[i]);
	for (i = 2; i > -1; i--)
	{

#ifdef DATETIME_AS_STRING
		*(znak++) = ( (*(wsk + i) & (2 == i ? 0x3F : 0x7F )) >> 4) + '0';//starsza czêœc bajtu zamaskowana
		*(znak++) = ( *(wsk + i) & 0x0F) + '0';//m³odsza czêœc bajtu
		*(znak++) = i?':':0;

#endif
		*((uint8_t *)dt + 2 - i) = bcd2dec( * (wsk + i));

	}

	//---------------------------------------------------------------------------
	dt->DD = bcd2dec(*(wsk + 4) & 0x3F);
	dt->MM = bcd2dec(*(wsk + 5) & 0x1F);
	//dt->YY = bcd2dec(*(wsk + 6));
	dt->YY = century * 100 + bcd2dec(*(wsk + 6));
	dt->weakday = bcd2dec(wsk[3] & 0x07);

//przygotowanie stringa do wyœwietlenia daty
#ifdef DATETIME_AS_STRING

	znak = dt->date;
	itoa(dt->YY, znak, 10);
	znak += 4;
	*(znak++) = DATA_SEPARATOR;

	*(znak++) = ((wsk[5] & 0x1F) >> 4) + '0';
	*(znak++) = (wsk[5] & 0x0F)        + '0';
	*(znak++) = DATA_SEPARATOR;

	*(znak++) = ((wsk[4] & 0x3F) >> 4) + '0';
	*(znak++) =  (wsk[4] & 0x0F)       + '0';
	*(znak++) = 0;
#endif


#ifdef DATETIME_AS_STRING

	znak = dt->YY_MM;
	itoa(dt->YY, znak, 10);
	znak += 4;
	*(znak++) = FILE_NAME_SEPARATOR;

	*(znak++) = ((wsk[5] & 0x1F) >> 4) + '0';
	*(znak++) = (wsk[5] & 0x0F)        + '0';
	*(znak++) = *".";
	*(znak++) = *"c";
	*(znak++) = *"s";
	*(znak++) = *"v";
	*(znak++) = 0;
#endif



	uint8_t godziny  = dt->hh;
	if (summer_winter_time_correction( &dt->dst, dt->YY, dt->MM, dt->DD, godziny))
	{
		//datetime.force_ntp = 25;
		if (_letni == dt->dst)
		{
			godziny = (godziny + 1) % 24;
		}
		else godziny = (godziny - 1) % 24;

		dt->hh = godziny;
		DS3231_set_rtc_time(dt, dt->hh, dt->mm, dt->ss + 1);
	}
}

//---------------------------------------------------------------------------------
//wskaŸnik do funkcji callback dla zdarzenia RTC_EVENT()
static void (*rtc_callback)(TDATETIME *dt);

//---------------------------------------------------------------------------------
//funkcja do rejestracji funkcji zwrotnej w zdarzeniu RTC_EVENT()
void register_rtc_event_callback (void (*callback)(TDATETIME *dt))
{
	rtc_callback = callback;
}

//---------------------------------------------------------------------------------
//wskaŸnik do funkcji callback dla zdarzenia RTC_EVENT i pokazaniu temperatury modu³u RTC()
static void (*rtc_temperature_callback)(TTEMP *temp);

//---------------------------------------------------------------------------------
//funkcja do rejestracji funkcji zwrotnej w zdarzeniu RTC_EVENT()
void register_temperature_rtc_event_callback (void (*callback)(TTEMP *temp))
{
	rtc_temperature_callback = callback;
}

//---------------------------------------------------------------------------------
void DS3231_EVENT ( void )
{
	if(flag)
	{
		DS3231_get_rtc_datetime(&datetime);
		DS3231_get_temp(&temperature);

		if(rtc_callback) rtc_callback(&datetime);
		if(rtc_temperature_callback) rtc_temperature_callback(&temperature);



		flag = 0;
	}
}


//---------------------------------------------------------------------------------
uint8_t calculate_weak_day ( uint8_t DD, uint8_t MM, uint16_t YY)
{
	int Y, C, M, N, D;
	M = 1 + ( 9 + MM) % 12;
	Y = YY - (M > 10);
	C = Y / 100;
	D = Y % 100;
	N = ((13 * M - 1)/5 +D + D/4 + 6*C + DD + 5) % 7;
	return (7 + N) % 7;
}

//---------------------------------------------------------------------------------
//konwersja liczby dziesiêtnej na BCD
uint8_t dec2bcd(uint8_t dec)
{
	return ((dec / 10) << 4) | (dec %10);
}

//---------------------------------------------------------------------------------
//konwersja liczby BCD na dziesiêtn¹
uint8_t bcd2dec(uint8_t bcd)
{
	return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}





//--------------------------------------------------------------------------------------------------------
//************************ automatyczna korekta czasu letniego i zimowego ***********************
//	zmiana czasu letniego na zimowy - zmiana z godziny 3:00 na 2:00 w ostatni¹ niedzielê paŸdzienrika
//	zmiana czasu zimowego na letni - zmiana z godizny 2:00 na 3:00 w ostatni¹ niedzielê marca

uint8_t summer_winter_time_correction (uint8_t * czas_lz, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh)
{
	uint8_t tmp_czlz = *czas_lz;
	if(MM > 3 && MM < 10) *czas_lz = _letni;
	else
		if(MM < 3 || MM > 10 ) *czas_lz = _zimowy;
		else
			if( 3 == MM || 10 == MM)
			{
				uint8_t dof = 0;
				uint8_t i = 32;
				i = 32;
				do
				{
					i--;
					dof = calculate_weak_day(i, MM, YY);
				} while (dof != 6);

				if( DD > i) *czas_lz = (3 == MM) ? _letni : _zimowy;
				else
				if( DD < i) *czas_lz = (3 == MM) ? _zimowy : _letni;
				else
				if( DD == i && hh >= ((3 == MM)? 2 : 3) ) * czas_lz = ( 3 == MM) ? _letni : _zimowy;
				else
				if( DD == i && hh < ((_zimowy != *czas_lz)? 3 : 2) ) *czas_lz = ( 3 == MM) ? _zimowy : _letni;
			}
	if (tmp_czlz != *czas_lz) return 1;	// nast¹pi³a zmiana czasu

	return 0;							// nie wyst¹pi³¹ zmiana czasu
}

