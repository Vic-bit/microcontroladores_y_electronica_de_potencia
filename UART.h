/*
 * UART.h
 *
 * Created: 03/05/2019 02:16:24 a.m.
 *  Author: MyEP
 */ 


#ifndef UART_H_
#define UART_H_

void mi_UART_Init( uint32_t, uint8_t IntRX, uint8_t intTX);
int mi_putchar(char, FILE *stream);
int mi_getchar(FILE *stream);
#define fgetc() mi_getc(&uart_io)		// redefine la primitiva de entrada como  función recibir por UART, las funciones estandar
#define fputc(x) mi_putc(x,&uart_io)	// redefine la primitiva de salida como función transmitir por UART
#endif /* UART_H_ */