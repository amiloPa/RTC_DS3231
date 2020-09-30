#include "I2C.h"

void I2C_Conf(uint8_t SCK_speed)
{
	//Konfiguracja I2C1
	I2C_InitTypeDef I2CInit;
	I2C_StructInit(&I2CInit);
	//Tryb pracy uk�adu
	I2CInit.I2C_Mode = I2C_Mode_I2C;
	//Stosunek trwania stanu wysokiego do niskiego linii SCK
	I2CInit.I2C_DutyCycle = I2C_DutyCycle_2;
	//Adres I2C po stronie mikrokontrolera je�li pracuje jako slave
	I2CInit.I2C_OwnAddress1 = 0x00;
	//Wlaczenie potwierdzenia transmisji
	I2CInit.I2C_Ack = I2C_Ack_Enable;
	 //Cz�stotliwo�c SCK 400 kHz
	I2CInit.I2C_ClockSpeed = 100 * SCK_speed;

	I2C_Init(I2C1, &I2CInit);
	I2C_Cmd(I2C1, ENABLE);
}


//Ustawienie adresu pami�ci
void I2C_ADDRES(uint8_t SLA, uint32_t addr)
{
 //Rozpocz�cie komunikacji
 I2C_GenerateSTART(I2C1, ENABLE);
 //Czekanie a� polecenie zostanie wykonane
 while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
 //W tym kroku wys�any jest identyfikator uk�adu
 I2C_Send7bitAddress(I2C1, SLA, I2C_Direction_Transmitter);
 //Odczekanie na odpowied�
 while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);

 //Wys�anie adresu kom�rki pami�ci do/z jakiej b�d� zapisywane/odczytywane dane.
 I2C_SendData(I2C1, addr);
 while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);

}



//Odczyt z pami�ci
void I2C_READ(uint8_t SLA, uint32_t addr,  int size, void* data)
{
	  //Deklaracja zmiennych
	  int i;
	  uint8_t* buffer = (uint8_t*)data;

	  //Wyb�r adresu urz�dzenia
	  I2C_ADDRES(SLA, addr);

	  //Wys�anie sygna�u start, nast�pnie czekanie na odpowied�
	  I2C_GenerateSTART(I2C1, ENABLE);
	  while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

	  //Odbi�r tylko jednego bajtu, wlacz potwierdzenie
	  I2C_AcknowledgeConfig(I2C1, ENABLE);
	  //Wys�anie adresu uk�adu, ustawienie i2c jako master w trybie received mode
	  I2C_Send7bitAddress(I2C1, SLA, I2C_Direction_Receiver);
	  //Odczekanie na EV6
	  while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);

	  for (i = 0; i < size - 1; i++)
	  {
	   while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	   buffer[i] = I2C_ReceiveData(I2C1);
	  }
	  //Odbi�r pojedy�czego bajtu
	  I2C_AcknowledgeConfig(I2C1, DISABLE);

	  //Wygenerowanie sygna�u stop
	  I2C_GenerateSTOP(I2C1, ENABLE);
	  //Odczekanie na sygna�
	  while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	  //Odczytaj dane z rejestru
	  buffer[i] = I2C_ReceiveData(I2C1);
}


//Zapis do pami�ci
void I2C_WRITE(uint8_t SLA, uint32_t addr, int size, const void* data)
{
  int i;
  const uint8_t* buffer = (uint8_t*)data;

  //Wyb�r adresu urz�dzenia
  I2C_ADDRES(SLA, addr);

  //Przej�cie przez ca�y buffor z danymi
  for (i = 0; i < size; i++)
  {
   I2C_SendData(I2C1, buffer[i]);
   //while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);
   while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
  }
  //Wygenerowanie sygna�u Stop
  I2C_GenerateSTOP(I2C1, ENABLE);
}

