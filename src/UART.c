/*
 * uart.c
 *
 *  Created on: 26.06.2020
 *      Author: Piotr
 */

#include "UART.h"




volatile uint8_t ascii_line;


// definiujemy w ko�cu nasz bufor UART_RxBuf
volatile char UART_RxBuf[UART_RX_BUF_SIZE];
// definiujemy indeksy okre�laj�ce ilo�� danych w buforze
volatile uint8_t UART_RxHead; // indeks oznaczaj�cy �g�ow� w�a�
volatile uint8_t UART_RxTail; // indeks oznaczaj�cy �ogon w�a�



// definiujemy w ko�cu nasz bufor UART_RxBuf
volatile char UART_TxBuf[UART_TX_BUF_SIZE];
// definiujemy indeksy okre�laj�ce ilo�� danych w buforze
volatile uint8_t UART_TxHead; // indeks oznaczaj�cy �g�ow� w�a�
volatile uint8_t UART_TxTail; // indeks oznaczaj�cy �ogon w�a�


// wska�nik do funkcji callback dla zdarzenia UART_RX_STR_EVENT()
static void (*uart_rx_str_event_callback)(char * pBuf);


// funkcja do rejestracji funkcji zwrotnej w zdarzeniu UART_RX_STR_EVENT()
void register_uart_str_rx_event_callback(void (*callback)(char * pBuf)) {
	uart_rx_str_event_callback = callback;
}


//static volatile struct {
//	uint16_t	tri, twi, tct;
//	uint16_t	rri, rwi, rct;
//	uint8_t		tbuf[UART_TXB];
//	uint8_t		rbuf[UART_RXB];
//} Fifo1;

void UART_init(uint32_t BaudRate)
{

	//*******************************************************************
	//Ustawienie zegar�w mikroprocesora
	//*******************************************************************
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	//pod�aczenie zegara do USART1
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;		//pod�aczenie zegara do portu A

	//*******************************************************************
	//Ustawienie pin�w mikroprocesora
	//*******************************************************************
	gpio_pin_cfg(GPIOA, PA9,  gpio_mode_alternate_PP_2MHz);	// konfiguracja pinu PA1 jako alternatywnego wyj�cia push-pull
	gpio_pin_cfg(GPIOA, PA10, gpio_mode_input_floating);	// konfiguracja pinu PA1 jako wyj�cia p�ywaj�cego


	//*******************************************************************
	//Ustawienie komunikacji USART
	//*******************************************************************

	USART1->BRR = F_PCLK2/BaudRate;		// ustawienie pr�dko�ci transisji
	USART1->CR1 |= USART_CR1_RE;		// w��czenie Rx USART
	USART1->CR1 |= USART_CR1_TE;		// w��czenie Tx USART
//	USART1->CR3 |= USART_CR3_DMAR;		// w�aczzenie DMA dla Rx
	USART1->CR1 |= USART_CR1_RXNEIE;	// w��czenie przerwa� od Rx
	USART1->CR1 |= USART_CR1_TXEIE;		// w��czenie przerwa� od Tx
	//USART1->CR1 |= USART_CR1_TCIE;		// w��cz sygnalizacj� zako�czenia transmisji
	USART1->CR1 |= USART_CR1_UE;		// w��czenie USART


	//*******************************************************************
	//Konfiguracja DMA
	//*******************************************************************
//	DMA1_Channel5->CMAR		= (uint32_t) &received_data;	// podanie adresu tablicy w kt�rej zapisywane s� wyniki
//	DMA1_Channel5->CPAR		= (uint32_t) &USART1->DR;	// Podanie adresu rejestru z kt�ego ma by� wykonywany transfer
//	DMA1_Channel5->CNDTR	= 1;						// ilo�� transfer�w -> 1
//	DMA1_Channel5->CCR     |= DMA_CCR5_MSIZE_0;			// Rozmiar obszaru pami�ci transferowanych danych -> 16 bit�w
//	DMA1_Channel5->CCR     |= DMA_CCR5_PSIZE_0;			// Rozmiar peryferiala transferowanych danych -> 16 bit�w
//	DMA1_Channel5->CCR 	   |= DMA_CCR5_CIRC;			// Circular mode ena
//	DMA1_Channel5->CCR 	   |= DMA_CCR5_EN;				// Channel enable

	//*******************************************************************
	//W�aczenie wektora przerwa� dla USART1
	//*******************************************************************
	NVIC_EnableIRQ( USART1_IRQn);	// RM -> 205 page -> table "Vector table for other STM32F10xxx device" -> position 37

	/* Clear Tx/Rx fifo */
//	Fifo1.tri = 0; Fifo1.twi = 0; Fifo1.tct = 0;
//	Fifo1.rri = 0; Fifo1.rwi = 0; Fifo1.rct = 0;

}

//***********************************************************************************************
  __attribute__((interrupt)) void USART1_IRQHandler (void)
{



	  if(USART1->SR & USART_SR_RXNE)		//rejestr USART1->SR zawiera flagi przerwa� uk�adu peryferyjnego. Sprawdzamy w nim co wyo�a�o przerwanie.
	  {
		  register uint8_t tmp_head;
		  register char data;

		  data = USART1->DR; //pobieramy natychmiast bajt danych z bufora sprz�towego

		  // obliczamy nowy indeks �g�owy w�a�
		  tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;

		  // sprawdzamy, czy w�� nie zacznie zjada� w�asnego ogona
		  if ( tmp_head == UART_RxTail )
		  {
			  // tutaj mo�emy w jaki� wygodny dla nas spos�b obs�u�y�  b��d spowodowany
			  // pr�b� nadpisania danych w buforze, mog�oby doj�� do sytuacji gdzie
			  // nasz w�� zacz��by zjada� w�asny ogon
			  // jednym z najbardziej oczywistych rozwi�za� jest np natychmiastowe
			  // wyzerowanie zmiennej ascii_line lub sterowanie sprz�tow� lini�
			  // zaj�to�ci bufora
			  UART_RxHead = UART_RxTail;
		  } else
		  {
			  switch( data )
			  {
				  case 0:					// ignorujemy bajt = 0
				  case 10: break;			// ignorujemy znak LF
				  case 13: ascii_line++;	// sygnalizujemy obecno�� kolejnej linii w buforze
				  default : UART_RxHead = tmp_head; UART_RxBuf[tmp_head] = data;

				  uart_putc(UART_RxBuf[tmp_head]);
			  }
		  }
		  USART1->SR |= ~USART_SR_RXNE;	// kasujemy flag� przerwania w rejestrze ADC1->SR (- rc_w0 (bit mo�na czyta� (read) i kasowa� (clear) poprzez wpisanie (write) zera (0))
	  }

	  if(USART1->SR & USART_SR_TXE)		//rejestr USART1->SR zawiera flagi przerwa� uk�adu peryferyjnego. Sprawdzamy w nim co wyo�a�o przerwanie.
	  {
		  // sprawdzamy czy indeksy s� r�ne
		  if ( UART_TxHead != UART_TxTail )
		  {
			  // obliczamy i zapami�tujemy nowy indeks ogona w�a (mo�e si� zr�wna� z g�ow�)
			  UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK;
			  // zwracamy bajt pobrany z bufora  jako rezultat funkcji
			  #ifdef UART_DE_PORT
			  UART_DE_NADAWANIE;
			  #endif
			  USART1->DR = UART_TxBuf[UART_TxTail];
			  while(!(USART1->SR & USART_SR_TC));
			  USART1->SR &= !USART_SR_TC;
		  }
		  else
		  {
			// zerujemy flag� przerwania wyst�puj�cego gdy bufor pusty
//PLA			UCSRB &= ~(1<<UDRIE);
			  USART1->CR1 &= ~USART_CR1_TXEIE;
		  }
	  }
/*
	if(USART1->SR & USART_SR_RXNE)		//rejestr USART1->SR zawiera flagi przerwa� uk�adu peryferyjnego. Sprawdzamy w nim co wyo�a�o przerwanie.
	{
		USART1->SR |= ~USART_SR_RXNE;	// kasujemy flag� przerwania w rejestrze ADC1->SR (- rc_w0 (bit mo�na czyta� (read) i kasowa� (clear) poprzez wpisanie (write) zera (0))
		uint16_t temp;
		temp = USART1->DR; 				// odczytaj odebrane dane
//		USART1->DR = *received_data + 1; 			//zwi�ksz o 1 i wy�lij
		USART1->DR = temp + 1; 			//zwi�ksz o 1 i wy�lij
	}
	*/


//		uint32_t sr = USART1->SR;	/* Interrupt flags */
//		uint8_t d;
//		int i;
//
//
//		if (sr & USART_SR_RXNE) {	/* RXNE is set: Rx ready */
//			d = USART1->DR;	/* Get received byte */
//			i = Fifo1.rct;
//			if (i < UART_RXB) {	/* Store it into the rx fifo if not full */
//				Fifo1.rct = ++i;
//				i = Fifo1.rwi;
//				Fifo1.rbuf[i] = d;
//				Fifo1.rwi = ++i % UART_RXB;
//			}
//			uart_putc(d);
//		}
//
//		if (sr & USART_SR_TXE) {	/* TXE is set: Tx ready */
//			i = Fifo1.tct;
//			if (i--) {	/* There is any data in the tx fifo */
//				Fifo1.tct = (uint16_t)i;
//				i = Fifo1.tri;
//				USART1->DR = Fifo1.tbuf[i];
//				Fifo1.tri = ++i % UART_TXB;
//			} else {	/* No data in the tx fifo */
//				USART1->CR1 &= ~USART_CR1_TXEIE;		/* Clear TXEIE - Disable TXE irq */
//			}
//		}

}
//***********************************************************************************************
  // Zdarzenie do odbioru danych �a�cucha tekstowego z bufora cyklicznego
  void UART_RX_STR_EVENT(char * rbuf)
  {
  	if( ascii_line ) {
  		if( uart_rx_str_event_callback ) {
  			uart_get_str( rbuf );
  			(*uart_rx_str_event_callback)( rbuf );
  		} else {
  			UART_RxHead = UART_RxTail;
  		}
  	}
  }

  //***********************************************************************************************
  // definiujemy funkcj� dodaj�c� jeden bajt do bufora cyklicznego
  void uart_putc( char data )
  {
  	uint8_t tmp_head;
 // 	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
  		tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;
 // 	}
            // p�tla oczekuje je�eli brak miejsca w buforze cyklicznym na kolejne znaki
      while ( tmp_head == UART_TxTail ){}

      UART_TxBuf[tmp_head] = data;
      UART_TxHead = tmp_head;

      // inicjalizujemy przerwanie wyst�puj�ce, gdy bufor jest pusty, dzi�ki
      // czemu w dalszej cz�ci wysy�aniem danych zajmie si� ju� procedura
      // obs�ugi przerwania
//PLA      UCSRB |= (1<<UDRIE);
      USART1->CR1 |= USART_CR1_TXEIE;
  }

  //***********************************************************************************************
  void uart_puts(char *s)		// wysy�a �a�cuch z pami�ci RAM na UART
  {
    register char c;
    while ((c = *s++)) uart_putc(c);			// dop�ki nie napotkasz 0 wysy�aj znak
  }

  //***********************************************************************************************
  void uart_putint(int value, int radix)	// wysy�a na port szeregowy tekst
  {
  	char string[17];			// bufor na wynik funkcji itoa
  	itoa(value, string, radix);		// konwersja value na ASCII
  	uart_puts(string);			// wy�lij string na port szeregowy
  }

  //***********************************************************************************************
  // definiujemy funkcj� pobieraj�c� jeden bajt z bufora cyklicznego
  int uart_getc(void)
  {
  	int data = -1;
      // sprawdzamy czy indeksy s� r�wne
      if ( UART_RxHead == UART_RxTail ) return data;
//      ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
          // obliczamy i zapami�tujemy nowy indeks �ogona w�a� (mo�e si� zr�wna� z g�ow�)
          UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;
          // zwracamy bajt pobrany z bufora  jako rezultat funkcji
          data = UART_RxBuf[UART_RxTail];
//      }
      return data;
  }

  //***********************************************************************************************
  char * uart_get_str(char * buf)
  {
	  int c;
	  char * wsk = buf;
	  if( ascii_line )
	  {
		while( (c = uart_getc()) )
		{
			if( 13 == c || c < 0) break;
  			*buf++ = c;
  		}
  		*buf=0;
  		ascii_line--;
	  }
	  return wsk;
  }

  //***********************************************************************************************
//  uint8_t uart_getc (void)
//  {
//  	uint8_t d;
//  	int i;
//
//  	/* Wait while rx fifo is empty */
//  	while (!Fifo1.rct) ;
//
//  	i = Fifo1.rri;			/* Get a byte from rx fifo */
//  	d = Fifo1.rbuf[i];
//  	Fifo1.rri = ++i % UART_RXB;
//  	__disable_irq();
//  	Fifo1.rct--;
//  	__enable_irq();
//
//  	return d;
//  }
//
//
//
//  void uart_putc (uint8_t d)
//  {
//  	int i;
//
//  	/* Wait for tx fifo is not full */
//  	while (Fifo1.tct >= UART_TXB) ;
//
//  	i = Fifo1.twi;		/* Put a byte into Tx fifo */
//  	Fifo1.tbuf[i] = d;
//  	Fifo1.twi = ++i % UART_TXB;
//  	__disable_irq();
//  	Fifo1.tct++;
//  	USART1->CR1 |= USART_CR1_TXEIE;	/* Set TXEIE - Enable TXE irq */
//  	__enable_irq();
//  }
//
//  int uart_test (void)
//  {
//  	return Fifo1.rct;
//  }
