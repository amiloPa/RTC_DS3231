/*
 * uart.c
 *
 *  Created on: 26.06.2020
 *      Author: Piotr
 */

#include "UART.h"




volatile uint8_t ascii_line;


// definiujemy w koñcu nasz bufor UART_RxBuf
volatile char UART_RxBuf[UART_RX_BUF_SIZE];
// definiujemy indeksy okreœlaj¹ce iloœæ danych w buforze
volatile uint8_t UART_RxHead; // indeks oznaczaj¹cy „g³owê wê¿a”
volatile uint8_t UART_RxTail; // indeks oznaczaj¹cy „ogon wê¿a”



// definiujemy w koñcu nasz bufor UART_RxBuf
volatile char UART_TxBuf[UART_TX_BUF_SIZE];
// definiujemy indeksy okreœlaj¹ce iloœæ danych w buforze
volatile uint8_t UART_TxHead; // indeks oznaczaj¹cy „g³owê wê¿a”
volatile uint8_t UART_TxTail; // indeks oznaczaj¹cy „ogon wê¿a”


// wskaŸnik do funkcji callback dla zdarzenia UART_RX_STR_EVENT()
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
	//Ustawienie zegarów mikroprocesora
	//*******************************************************************
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	//pod³aczenie zegara do USART1
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;		//pod³aczenie zegara do portu A

	//*******************************************************************
	//Ustawienie pinów mikroprocesora
	//*******************************************************************
	gpio_pin_cfg(GPIOA, PA9,  gpio_mode_alternate_PP_2MHz);	// konfiguracja pinu PA1 jako alternatywnego wyjœcia push-pull
	gpio_pin_cfg(GPIOA, PA10, gpio_mode_input_floating);	// konfiguracja pinu PA1 jako wyjœcia p³ywaj¹cego


	//*******************************************************************
	//Ustawienie komunikacji USART
	//*******************************************************************

	USART1->BRR = F_PCLK2/BaudRate;		// ustawienie prêdkoœci transisji
	USART1->CR1 |= USART_CR1_RE;		// w³¹czenie Rx USART
	USART1->CR1 |= USART_CR1_TE;		// w³¹czenie Tx USART
//	USART1->CR3 |= USART_CR3_DMAR;		// w³aczzenie DMA dla Rx
	USART1->CR1 |= USART_CR1_RXNEIE;	// w³¹czenie przerwañ od Rx
	USART1->CR1 |= USART_CR1_TXEIE;		// w³¹czenie przerwañ od Tx
	//USART1->CR1 |= USART_CR1_TCIE;		// w³¹cz sygnalizacjê zakoñczenia transmisji
	USART1->CR1 |= USART_CR1_UE;		// w³¹czenie USART


	//*******************************************************************
	//Konfiguracja DMA
	//*******************************************************************
//	DMA1_Channel5->CMAR		= (uint32_t) &received_data;	// podanie adresu tablicy w której zapisywane s¹ wyniki
//	DMA1_Channel5->CPAR		= (uint32_t) &USART1->DR;	// Podanie adresu rejestru z któego ma byæ wykonywany transfer
//	DMA1_Channel5->CNDTR	= 1;						// iloœæ transferów -> 1
//	DMA1_Channel5->CCR     |= DMA_CCR5_MSIZE_0;			// Rozmiar obszaru pamiêci transferowanych danych -> 16 bitów
//	DMA1_Channel5->CCR     |= DMA_CCR5_PSIZE_0;			// Rozmiar peryferiala transferowanych danych -> 16 bitów
//	DMA1_Channel5->CCR 	   |= DMA_CCR5_CIRC;			// Circular mode ena
//	DMA1_Channel5->CCR 	   |= DMA_CCR5_EN;				// Channel enable

	//*******************************************************************
	//W³aczenie wektora przerwañ dla USART1
	//*******************************************************************
	NVIC_EnableIRQ( USART1_IRQn);	// RM -> 205 page -> table "Vector table for other STM32F10xxx device" -> position 37

	/* Clear Tx/Rx fifo */
//	Fifo1.tri = 0; Fifo1.twi = 0; Fifo1.tct = 0;
//	Fifo1.rri = 0; Fifo1.rwi = 0; Fifo1.rct = 0;

}

//***********************************************************************************************
  __attribute__((interrupt)) void USART1_IRQHandler (void)
{



	  if(USART1->SR & USART_SR_RXNE)		//rejestr USART1->SR zawiera flagi przerwañ uk³adu peryferyjnego. Sprawdzamy w nim co wyo³a³o przerwanie.
	  {
		  register uint8_t tmp_head;
		  register char data;

		  data = USART1->DR; //pobieramy natychmiast bajt danych z bufora sprzêtowego

		  // obliczamy nowy indeks „g³owy wê¿a”
		  tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;

		  // sprawdzamy, czy w¹¿ nie zacznie zjadaæ w³asnego ogona
		  if ( tmp_head == UART_RxTail )
		  {
			  // tutaj mo¿emy w jakiœ wygodny dla nas sposób obs³u¿yæ  b³¹d spowodowany
			  // prób¹ nadpisania danych w buforze, mog³oby dojœæ do sytuacji gdzie
			  // nasz w¹¿ zacz¹³by zjadaæ w³asny ogon
			  // jednym z najbardziej oczywistych rozwi¹zañ jest np natychmiastowe
			  // wyzerowanie zmiennej ascii_line lub sterowanie sprzêtow¹ lini¹
			  // zajêtoœci bufora
			  UART_RxHead = UART_RxTail;
		  } else
		  {
			  switch( data )
			  {
				  case 0:					// ignorujemy bajt = 0
				  case 10: break;			// ignorujemy znak LF
				  case 13: ascii_line++;	// sygnalizujemy obecnoœæ kolejnej linii w buforze
				  default : UART_RxHead = tmp_head; UART_RxBuf[tmp_head] = data;

				  uart_putc(UART_RxBuf[tmp_head]);
			  }
		  }
		  USART1->SR |= ~USART_SR_RXNE;	// kasujemy flagê przerwania w rejestrze ADC1->SR (- rc_w0 (bit mo¿na czytaæ (read) i kasowaæ (clear) poprzez wpisanie (write) zera (0))
	  }

	  if(USART1->SR & USART_SR_TXE)		//rejestr USART1->SR zawiera flagi przerwañ uk³adu peryferyjnego. Sprawdzamy w nim co wyo³a³o przerwanie.
	  {
		  // sprawdzamy czy indeksy s¹ ró¿ne
		  if ( UART_TxHead != UART_TxTail )
		  {
			  // obliczamy i zapamiêtujemy nowy indeks ogona wê¿a (mo¿e siê zrównaæ z g³ow¹)
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
			// zerujemy flagê przerwania wystêpuj¹cego gdy bufor pusty
//PLA			UCSRB &= ~(1<<UDRIE);
			  USART1->CR1 &= ~USART_CR1_TXEIE;
		  }
	  }
/*
	if(USART1->SR & USART_SR_RXNE)		//rejestr USART1->SR zawiera flagi przerwañ uk³adu peryferyjnego. Sprawdzamy w nim co wyo³a³o przerwanie.
	{
		USART1->SR |= ~USART_SR_RXNE;	// kasujemy flagê przerwania w rejestrze ADC1->SR (- rc_w0 (bit mo¿na czytaæ (read) i kasowaæ (clear) poprzez wpisanie (write) zera (0))
		uint16_t temp;
		temp = USART1->DR; 				// odczytaj odebrane dane
//		USART1->DR = *received_data + 1; 			//zwiêksz o 1 i wyœlij
		USART1->DR = temp + 1; 			//zwiêksz o 1 i wyœlij
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
  // Zdarzenie do odbioru danych ³añcucha tekstowego z bufora cyklicznego
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
  // definiujemy funkcjê dodaj¹c¹ jeden bajt do bufora cyklicznego
  void uart_putc( char data )
  {
  	uint8_t tmp_head;
 // 	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
  		tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;
 // 	}
            // pêtla oczekuje je¿eli brak miejsca w buforze cyklicznym na kolejne znaki
      while ( tmp_head == UART_TxTail ){}

      UART_TxBuf[tmp_head] = data;
      UART_TxHead = tmp_head;

      // inicjalizujemy przerwanie wystêpuj¹ce, gdy bufor jest pusty, dziêki
      // czemu w dalszej czêœci wysy³aniem danych zajmie siê ju¿ procedura
      // obs³ugi przerwania
//PLA      UCSRB |= (1<<UDRIE);
      USART1->CR1 |= USART_CR1_TXEIE;
  }

  //***********************************************************************************************
  void uart_puts(char *s)		// wysy³a ³añcuch z pamiêci RAM na UART
  {
    register char c;
    while ((c = *s++)) uart_putc(c);			// dopóki nie napotkasz 0 wysy³aj znak
  }

  //***********************************************************************************************
  void uart_putint(int value, int radix)	// wysy³a na port szeregowy tekst
  {
  	char string[17];			// bufor na wynik funkcji itoa
  	itoa(value, string, radix);		// konwersja value na ASCII
  	uart_puts(string);			// wyœlij string na port szeregowy
  }

  //***********************************************************************************************
  // definiujemy funkcjê pobieraj¹c¹ jeden bajt z bufora cyklicznego
  int uart_getc(void)
  {
  	int data = -1;
      // sprawdzamy czy indeksy s¹ równe
      if ( UART_RxHead == UART_RxTail ) return data;
//      ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
          // obliczamy i zapamiêtujemy nowy indeks „ogona wê¿a” (mo¿e siê zrównaæ z g³ow¹)
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
