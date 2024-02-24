#include "main.h"
#include "SPI.h"

void SPI_init(void)
{
    DDRB|= (1<<SS)|(1<<MOSI)|(1<<SCK);	//MISO como entrada
    SPCR= (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);	// Habilitar SPI, como master, Mbps=16.000.000/128
}

void SPI_master_Enviar(uint8_t data)
{
	SPDR = data;	//Cargar dato en el registro
	while(!(SPSR & (1 << SPIF))); //Esperar a que se complete
}

uint8_t SPI_master_Recibir()
{
	SPDR = 0xFF;	// transmit dummy byte para que el esclavo nos transmita	
	while(!(SPSR & (1 << SPIF))); //Esperar a que se complete
	return SPDR;	//Devuelve el contenido de SPDR
}