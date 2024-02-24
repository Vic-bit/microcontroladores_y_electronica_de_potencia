/*
 * UART.c
 * Biblioteca de funciones básicas de UART y manejo de interrupción por recepción de dato.
 * Created: 03/05/2019 02:13:03 a.m.
 *  Author: MyEP
 */ 
#include "main.h"
#include "UART.h"
FILE uart_io = FDEV_SETUP_STREAM(mi_putchar, mi_getchar, _FDEV_SETUP_RW);	//Inicializa un tipo stream de E/S con las funciones de salida y entrada

void mi_UART_Init( uint32_t brate, uint8_t IntRX, uint8_t IntTX)
{
	stdout = stdin = &uart_io;
	UBRR0 = F_CPU/16/brate-1;
	UCSR0A = 0;							//Velocidad normal
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	//8bits de dato
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);		//Habilitamos la transmisión y recepción
	if (IntRX)
	UCSR0B|= (1<<RXCIE0);
	if (IntTX)
	UCSR0B|= (1<<TXCIE0);
}

int mi_putchar(char c, FILE *stream) {
	while(!(UCSR0A & (1<<UDRE0)) );		//Se asegura que hay lugar para poner el valor comprobando el valor de UDRE0
	UDR0 = c;
	return 0;
}

int mi_getchar(FILE *stream) {
	while ( !(UCSR0A & (1<<RXC0)) );	//Mientras no haya dato disponible (RCX0) se queda y cuando lo haya sale y lo regresa
	return UDR0;						//Recpción completa
}

ISR(USART_RX_vect)	//Interrupción por recepción de datos
{
	char Dato;
	Dato=mi_getchar(&uart_io);
	switch(Dato)
	{
		case ':':
			flagcom=1;
			indcom=0;
		break;
		case 8:		//backspace
			if(indcom>0) indcom--;
		break;
		case 13:	//retorno de carro
			if (flagcom)
				{
				comando[indcom]=0;
				InterpretaComando();					
				}
			flagcom=0;
		break;
		default:
			if (indcom<comSize)
				{
				comando[indcom++]=Dato;			
				}
		break;		
	}
}


