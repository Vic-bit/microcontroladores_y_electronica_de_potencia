/*
 * SD.c
 *
 * Created: 28/07/2021 12:07:11 p.m.
 *  Author: Víctor
 */ 

#include "main.h"
#include "SD.h"

//=====================================SD MEMORY CARD===========================================================
unsigned char SD_comando(char dt0, char dt1, char dt2, char dt3, char dt4, char dt5)
{
	unsigned char result;
	long int contador=0;
	SPI_master_Enviar(dt0);	//Inicio
	SPI_master_Enviar(dt1);	//Argumento
	SPI_master_Enviar(dt2);
	SPI_master_Enviar(dt3);
	SPI_master_Enviar(dt4);
	SPI_master_Enviar(dt5);	//Suma de comprobación
	do // Esperando una respuesta en formato R1 (hoja de datos página 109), si es un 1 el MSB no hay s
	{
		result=SPI_master_Recibir();
		contador++;
	} while (((result&0x80)!=0x00)&&contador<0xFFFF); //time out o MSB!=0 
	return result;
}
//----------------------------------------------------------------------------------
unsigned char SD_Init(void)
{
	unsigned char aux;
	long int contador=0;
	for (int i=0; i<10; i++)		//Cuando encienda le debemos dar 74 pulsos de clock con un pin, le damos 80 para asegurarnos por SPI, con 10 veces 11111111. Hoja de datos página 91
	SPI_master_Enviar(0xFF);	//Pasamos algún tipo de orden, hacemos la suma de comprobación. Enviemos el comando CMD0
									//Aquí vemos que este comando es para recargar el mapa, y vemos que debería devolvernos GO_IDLE_STATE, es decir, como resultado solo debería establecerse el bit cero.
									//Resets the SD Memory Card
	aux=SD_comando(0x40, 0x00, 0x00, 0x00, 0x00, 0x95);
	if (aux!=0x01)	//Indica con un 1 cuando está incializada
	{
		return 1;
	}
	SPI_master_Enviar(0xFF);	//Enviemos el byte FF al bus para sacar toda la basura del registro de desplazamiento de la tarjeta.
	do
	{
		aux=SD_comando(0x41, 0x00, 0x00, 0x00, 0x00, 0x95);
		SPI_master_Enviar(0xFF);
		contador++;
	} while ((aux!=0x00)&&contador<0xFFFF);	//Cuando aux sea 0 sale
	if (contador>=0xFFFF)
	{
		return 2;
	}
	return 0;
}
//----------------------------------------------------------------------------------
unsigned char SD_Leer_Bloque (char *buffer, unsigned char dt1, unsigned char dt2, unsigned char dt3, unsigned char dt4)
{
	unsigned char result;
	long int contador=0;;
	result=SD_comando(0x51,dt1,dt2,dt3,dt4,0x95);	//Según el datasheet. CMD17 - pag 50 y 96
	if(result!=0x00)	//
	{
		return 3;
	}
	SPI_master_Enviar(0xFF);	//Luego también transfiera 0xFF para esperar el tiempo y al mismo tiempo borrar el registro SPI de la tarjeta
	do
	{
		result=SPI_master_Recibir();
		contador++;
	} while ((result!=0xFE)&&contador<0xFFFF);
	if (contador>=0xFFFF)
	{
		return 5;
	}
	for (contador=0;contador<512;contador++)
	{
		buffer[contador]=SPI_master_Recibir();	 // obtener los bytes y los escribimos en el buffer
	}
	SPI_master_Recibir(); // Obtener la suma de comprobación
	SPI_master_Recibir();
	return 0;
}
//----------------------------------------------------------------------------------
unsigned char SD_Escribir_Bloque (char *bf, unsigned char dt1, unsigned char dt2, unsigned char dt3, unsigned char dt4)
{
	unsigned char result;
	long int contador;
	result=SD_comando(0x58,dt1,dt2,dt3,dt4,0x95);	//Según el datasheet. CMD24 - pag 51 97 y 98
	if(result!=0x00)
	{
		return 3;
	}
	SPI_master_Enviar(0xFF);	//dummy byte
	SPI_master_Enviar(0xFE);	//Comienzo del buffer, El comienzo del búfer es una especie de etiqueta, se incluye en el paquete de transferencia
	//de datos, que consta de la etiqueta de inicio y los datos en sí.Es esta etiqueta (0b11111110) la que debería ser para CMD 24, CMD17 y CMD18
	for (contador=0;contador<512;contador++)	//Llenamos el buffer
	{
		SPI_master_Enviar(bf[contador]);
	}
	SPI_master_Enviar(0xFF);
	SPI_master_Enviar(0xFF);	//sumas de comprobación?
	result=SPI_master_Recibir();
	if ((result&0x05)!=0x05)		//Enmascaramos, según indica la página 111 - 101=0x05 data reject error
	{
		return 4;
	}
	contador=0;
	do
	{
		result=SPI_master_Recibir();
		contador++;
	} while ((result!=0xFF)&&contador<0xFFFF);
	if (contador>=0xFFFF)
	{
		return 5;
	}
	return 0;
}