#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stddef.h>
#include <string.h>			// Para grandes cadenas 2.0	
#include <stdio.h>			// para las funciones estándar de E/S	
#include <stdlib.h>			// para printf	
#include "UART.h"			
#include "SPI.h"
#include "SD.h"
#include "Timer.h"

#define PASO 1.8
#define PI 3.141592653589793238462
#define multiplicador 2

uint8_t indcom;
uint8_t flagcom;
#define comSize 20
char comando[comSize];

char LeerBuffer(char *buffer2);
void InterpretaComando(void);
void delay(int us);
void calcular_angulos(float zenit, float azimut);
void inicio(void);
void homing(void);

#endif 