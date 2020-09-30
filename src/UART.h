/*
 * uart.h
 *
 *  Created on: 26.06.2020
 *      Author: Piotr
 */

#ifndef UART_UART_H_
#define UART_UART_H_

#include "stm32f10x.h"
#include "F103_lib.h"
//#include "../CLOCK/clock.h"
#include <stdlib.h>

#define UART_RXB	128		/* Size of Rx buffer */
#define UART_TXB	128		/* Size of Tx buffer */
#define F_PCLK2  72000000

//int uart_test (void);
//void uart_putc (uint8_t d);
//uint8_t uart_getc (void);

void UART_init(uint32_t BaudRate);

//#define UART_BAUDRATE 115200
#define UART_BAUD 115200		// tu definiujemy interesuj�c� nas pr�dko��
//#define __UBRR ((F_CPU+UART_BAUD*8UL)/(16UL*UART_BAUD)-1)  // obliczamy UBRR dla U2X=0

// definicje na potrzeby RS485
//#define UART_DE_PORT PORTD
#define UART_DE_DIR DDRD
#define UART_DE_BIT (1<<PD2)

#define UART_DE_ODBIERANIE  UART_DE_PORT &= ~UART_DE_BIT
#define UART_DE_NADAWANIE  UART_DE_PORT |= UART_DE_BIT


#define UART_RX_BUF_SIZE 32 // definiujemy bufor o rozmiarze 32 bajt�w
// definiujemy mask� dla naszego bufora
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)

#define UART_TX_BUF_SIZE 2 // definiujemy bufor o rozmiarze 16 bajt�w
// definiujemy mask� dla naszego bufora
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)


extern volatile uint8_t ascii_line;


// deklaracje funkcji publicznych

//void USART_Init( uint16_t baud );

int uart_getc(void);
void uart_putc( char data );
void uart_puts(char *s);
void uart_putint(int value, int radix);

char * uart_get_str(char * buf);

void UART_RX_STR_EVENT(char * rbuf);
void register_uart_str_rx_event_callback(void (*callback)(char * pBuf));







#endif /* UART_UART_H_ */
