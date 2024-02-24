#include "main.h"
#include "Timer.h"
#include <stdio.h>
#include <avr/io.h>

void Timer1_Init(void){
	OCR1A=62500-1;	//1s
	TCCR1A=(sNO<<COM1A0)|(sNO<<COM1B0)|(0<<WGM11)|(0<<WGM10);	//El modo de generación de onda es el 4, WGM13:0=0100
	TCCR1B=(0<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(0<<CS10); //modo CTC con OCR1A como TOP
	TIFR1 =(1<<OCF1A)|(0<<TOV1)|(0<<ICF1);						//prescaler luego en 256 
	TIMSK1=(1<<OCIE1A)|(0<<TOIE1)|(0<<ICIE1);
}

void Timer2_Init(void){
	OCR2A=250-1;	//0.016s
	TCCR2A=(sNO<<COM2A0)|(sNO<<COM2B0)|(1<<WGM21)|(0<<WGM20);	//CTC que es el modo 2 para este timer
	TCCR2B=(0<<WGM22)|(0<<CS22)|(0<<CS21)|(0<<CS20);			//prescaler luego en 1024 
	TIFR2 =(1<<OCF2A)|(0<<TOV2);
	TIMSK2=(1<<OCIE2A)|(0<<TOIE2);
}

