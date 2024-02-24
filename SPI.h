#ifndef SPI_H_
#define SPI_H_

#define SCK PINB5	
#define MISO PINB4
#define MOSI PINB3
#define SS PINB0

void SPI_init();
void SPI_master_Enviar(uint8_t data);
uint8_t SPI_master_Recibir();

#endif /* SPI_H_ */