/*
Microcontroladores y Electrónica de Potencia

Trabajo integrador: Helióstato doméstico

Alumno:  Víctor David Silva
Legajo:  10988
 */ 

#include "main.h"

char buffer[512]={};	
volatile int conteo=0;
volatile int indice_buffer=0;
char comando_trama[10]={};
volatile int indice_trama=0;
volatile int tiempo=0;
volatile int tiempo_aux=0;
volatile float zenit=0;
volatile float azimut=0;
volatile int activar_pulsos=0;
volatile int positivo=1;	
volatile float beta_z=0;
volatile float beta_a=0;
volatile float beta_z_real=0;
volatile float beta_a_real=0;
volatile float beta_z_aux=0;
volatile float beta_a_aux=0;
volatile int offset=0x00;
volatile int tramas_leidas=0;
enum tEstado{E_desactivado, E_activado, E_homing, E_funcionando};
volatile int activado=0;
volatile int FC1=0;
volatile int FC2=0;
volatile int finhoming=0;
volatile int flagestado;
volatile int homeactivado=0;
volatile int apagado=1;
volatile int activar_motor=0;
volatile int norte=1;
volatile float alpha_z=0;
volatile float alpha_a=0;

//======================================LeeBuffer================================================
char LeerBuffer(char *buffer2)
{
	char Dato2;
	int aux;
	Dato2=buffer2[indice_buffer];
	switch(Dato2)
	{
		case '0':									//Si es un número lo agrega al comando_trama
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '1':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '2':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '3':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '4':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '5':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '6':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '7':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '8':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case '9':
			comando_trama[indice_trama] = Dato2;
			indice_trama++;
			break;
		case 'Z':									//Tiempo
			comando_trama[indice_trama] = '\0';
			indice_trama = 0;
			aux= atoi(&comando_trama[0]);
			tiempo=aux;
			break;			
		case 'A':									//Zenit
			comando_trama[indice_trama] = '\0';
			indice_trama = 0;
			aux= atoi(&comando_trama[0]);
			zenit=aux;
			if (zenit==0)
			{
				positivo=1;
			}
			break;
		case '-':									//negativo
			positivo=0;
			break;
		case '\r':									//Azimut
			comando_trama[indice_trama] = '\0';
			indice_trama = 0;
			aux= atoi(&comando_trama[0]);
			azimut=aux;	
			break;
		case '\n':									
			return '0';
			break;
		default:
			break;
	}
}
//==================================Interrupcion TIMER=====================================
ISR(TIMER1_COMPA_vect)		
{	
	if(finhoming==1)
	{
		conteo+=1;	
		char salto_linea='1';
		if(tramas_leidas==0)
		{	
			unsigned char result;
			int flag=1;
			while(flag){
				printf("Leyendo memoria... \n");
				buffer[0]='\0';
				indice_buffer=0;
				PORTB&=~(1<<SS);
				while ((buffer[0] == '\0') || (buffer[500] == 152))		//Leo la tarjeta micro SD
				{	
					result=SD_Init();
					SD_Leer_Bloque(buffer, 0x00, 0x27, offset, 0x00);					
				}
				PORTB|=(1<<SS);
				printf("Datos en sector que comienza con la direccion 0x 00 27 0%d 00 :\n", offset);
				printf("%s\n", buffer);	
				if(apagado==1){							//Si es la primera vez que se enciende
					for(int i=0;i<strlen(buffer);i++)
					{
						salto_linea=LeerBuffer(buffer);
						indice_buffer+=1;
						if(tiempo==tiempo_aux){			//Lee hasta encontrar el tiempo deseado
							tiempo=tiempo_aux;
							tramas_leidas=(int) ((indice_buffer/16)*multiplicador);
							flag=0;
							apagado=0;
							offset=0x00;
							break;
						} else if(indice_buffer==511) {	//Siguiente sector
							offset=offset+0x02;
						}
					}	
				}
				if (apagado==0)
				{
					flag=0;
				}
			}	
		}

		if (conteo==multiplicador)	//Cada cuanto hace la lectura de la trama
		{
			PORTD^=(1<<PORTD6);		
			conteo=0;
			salto_linea='1';
			int flag=1;
		
			while(flag)
			{
				salto_linea=LeerBuffer(buffer);	
				indice_buffer+=1;
				if (salto_linea=='0')
				{
					flag=0;
				}
			}
			activar_pulsos=1;		//Se lee el main
			printf("\n");
		}
		tramas_leidas=tramas_leidas+1;
	}
}
//----------------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect){
	activar_motor+=1;		//tiempo estado alto y bajo
}
//======================================delay================================================
void delay(int us)
{
	int t;
	for(t=0;t<us;t++)
	_delay_us(1);
}
//======================================calcular_angulos====================================
void calcular_angulos(float zenit, float azimut){
	float theta;
	zenit*=PI/180;
	azimut*=PI/180;
	if (norte==1)
	{	
		theta=(zenit-alpha_z)/2;
		beta_z=alpha_z+theta;
		if ((azimut>=180) && (2*PI-azimut>=alpha_a))
		{
			theta=PI-(azimut+alpha_a)/2;
			beta_a=alpha_a+theta;
		}
		else if((azimut>180) && (2*PI-azimut<alpha_a))
		{
			theta=-PI+(azimut+alpha_a)/2;
			beta_a=alpha_a-theta;
		}
		else if((azimut<180) && (azimut<alpha_a))
		{
			theta=(alpha_a-azimut)/2;
			beta_a=alpha_a-theta;
		}
		else if((azimut<=180) && (azimut>=alpha_a))
		{
			theta=(azimut-alpha_a)/2;
			beta_a=alpha_a+theta;	
		}		
	}else{
		theta=(PI-zenit-alpha_z)/2;
		beta_z=alpha_z+theta;			
		if ((azimut>180) && (alpha_a<=0))
		{
			theta=(-PI+azimut-alpha_a)/2;
			beta_a=alpha_a+theta;
		}
		else if((azimut<180) && (alpha_a<0))
		{
			theta=(PI+azimut-alpha_a)/2;
			beta_a=PI-alpha_a-theta;
		}
		else if((azimut>180) && (alpha_a>0))
		{
			theta=(3*PI-azimut-alpha_a)/2;
			beta_a=PI-alpha_a-theta;
		}
		else if((azimut<180) && (alpha_a>=0))
		{
			theta=(PI-azimut-alpha_a)/2;
			beta_a=alpha_a+theta;
		}	
	}
	
	zenit*=180/PI;
	azimut*=180/PI;
	beta_z*=180/PI;
	beta_a*=180/PI;
}
//======================================inicio====================================
void inicio(void){
	printf("Iniciando... \n\n");
	printf("Configure los agulos deseados donde quiere realizar la reflexion (:Z_ _->alpha_z, :A_ _ _->alpha_a) \n\n");
	printf("Quiere reflejar hacia el N o hacia el S? (:N1->N , :NS->S) \n\n");
	printf("El tiempo inicial sera el dado por :T_ _ _ _\n\n");	
	printf("Para continuar realize primero la maniobra de homing (:H)\n\n");
}
//======================================InterpretaComando====================================
void InterpretaComando(void)
{
	int aux;
	switch(comando[0])		
	{
		case 'A':			//Ángulo alpha azimut deseado
			if(comando[1])						
			{
				aux = atoi(&comando[1]);		
				alpha_a=aux;
				printf("alpha_a:%f\r\n\n",alpha_a);
				alpha_a*=PI/180;
			}
			break;	
		case 'Z':			//Ángulo alpha zenit deseado
			if(comando[1])
			{
				aux = atoi(&comando[1]);
				alpha_z=aux;
				printf("alpha_z:%f\r\n\n",alpha_z);
				alpha_z*=PI/180;
			}
			break;
		case 'T':			//Tiempo de comienzo
			if(comando[1])
			{
				aux = atoi(&comando[1]);
				tiempo_aux=aux;
				printf("tiempo aux:%d\r\n\n",tiempo_aux);
			}
			break;
		case 'C':			//Comenzar
			if(comando[1])
			{
				aux = atoi(&comando[1]);
				if(aux==1 || aux==0){
				
					activado=aux;
					printf("comenzar:%d\r\n\n",activado);
					if (aux==0) flagestado=1;
				}
			}
			break;	
		case 'N':			//Norte / Sur
			if(comando[1])
			{
				aux = atoi(&comando[1]);
				norte=aux;
				printf("norte:%d\n\n",norte);
			}
			break;
		case 'H':			//Homing
			homeactivado=1;
			flagestado=2;
			break;
		default:
			break;
	}
}
//======================================homing====================================
void homing(void)
{
	printf("Realizando maniobra de homing...\n\n");
	TCCR2B=(1<<CS20)|(1<<CS21)|(1<<CS22);
	while (FC1==0)					//Fin de carrera 1
	{	
		if (activar_motor==1)		
		{
			PORTD&=~(1<<PORTD2);	//Gira en un sentido
			PORTD|=(1<<PORTD5);
		}
		if (activar_motor==2)
		{
			PORTD&=~(1<<PORTD5);
			activar_motor=0;
		}
	}
	TCCR2B&=~((1<<CS20)|(1<<CS21)|(1<<CS22));
	delay(100);
	TCCR2B=(1<<CS20)|(1<<CS21)|(1<<CS22);
	while (FC1==1)
	{
		if (activar_motor==1)
		{
			PORTD|=(1<<PORTD2);		//Gira en el otro sentido
			PORTD|=(1<<PORTD5);
		}
		if (activar_motor==2)
		{
			PORTD&=~(1<<PORTD5);
			activar_motor=0;
		}
	}
	TCCR2B&=~((1<<CS20)|(1<<CS21)|(1<<CS22));
	delay(100);
	TCCR2B=(1<<CS20)|(1<<CS21)|(1<<CS22);
	while (FC2==0)					////Fin de carrera 2
	{	
		if (activar_motor==1)
		{
			PORTD&=~(1<<PORTD3);	//Gira en un sentido
			PORTD|=(1<<PORTD4);
		}
		if (activar_motor==2)
		{
			PORTD&=~(1<<PORTD4);
			activar_motor=0;
		}	
	}
	TCCR2B&=~((1<<CS20)|(1<<CS21)|(1<<CS22));
	delay(100);
	TCCR2B=(1<<CS20)|(1<<CS21)|(1<<CS22);
	while (FC2==1)
	{
		if (activar_motor==1)
		{
			PORTD|=(1<<PORTD3);		//Gira en el otro sentido
			PORTD|=(1<<PORTD4);
		}
		if (activar_motor==2)
		{
			PORTD&=~(1<<PORTD4);
			activar_motor=0;
		}
	}
	TCCR2B&=~((1<<CS20)|(1<<CS21)|(1<<CS22));
	beta_a_aux=-90;
	beta_z_aux=0;
	flagestado=0;
	finhoming=1;
}
//----------------------------------------------------------------------------------
ISR (PCINT0_vect)		
{
	delay(10000);
	if(PINB & (1<<PINB1)){
		FC1=1;
		} else{
		FC1=0;
	}
	if(PINB & (1<<PINB2)){
		FC2=1;
		} else{
		FC2=0;
	}
	PCIFR|=(1<<PCIF0);
}
//======================================MAIN================================================
int main(void)
{
	enum tEstado estado;	
	
	DDRB = (1<<DDB1)|(1<<DDB2);
	DDRD = (1<<DDD6)|(1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5);
			
	mi_UART_Init(57600,1,0);
	
	Timer1_Init();
	Timer2_Init();
	
	SPI_init();
		
	PCICR|=(1<<PCIE0);
	PCIFR|=(1<<PCIF0);
	PCMSK0|=(1<<PCINT1)|(1<<PCINT2);	
		  		
	sei();

	int cont=0;
 	int mov_motor1;
 	int mov_motor2;
	printf("---------------Heliostato domestico---------------\n\n");
	printf("Se encuentra desactivado\n");
	printf("(Pulse :C1 para activar y :C0 para desactivar) \n\n");
	estado=E_desactivado;	
	
	while (1) 
    {	
		switch(estado)
		{
			case E_desactivado:
				if(activado==1) estado=E_activado;
				break;
			case E_activado:
				if (cont==0)
				{
					inicio();
					cont++;
				}
				if(homeactivado==1) estado=E_homing;
				break;
			case E_homing:
				homing();
				if(finhoming==1) estado=E_funcionando;
				break;
			case E_funcionando:
				 TCCR1B=(1<<CS12);			//Activo timer1
				 if (activar_pulsos==1 )
 				 {	
					if (positivo==0)
					{
						zenit*=-1;
					}
					printf("tiempo: %d\n", tiempo);
					printf("zenit: %f\n", zenit);
					printf("azimut: %f\n", azimut);
					calcular_angulos(zenit, azimut);
					printf("beta zenit: %f\n", beta_z);
					printf("beta azimut: %f\n", beta_a);
					printf("beta zenit aux: %f\n", beta_z_aux);
					if(((beta_a - beta_a_aux)>100) && (beta_a_aux>0)){			//Para el cambio de 0 a 360 y que no pase en la primera del día
						beta_a_aux+=180;
					}
					printf("beta azimut aux: %f\n", beta_a_aux);					
					if (positivo==1)											//Movimiento motores
					{
						mov_motor1=(int) (fabs((beta_z) - (beta_z_aux))/PASO);
						
						mov_motor2=(int) (fabs((beta_a) - (beta_a_aux))/PASO);
					}else{
						mov_motor1=0;
						mov_motor2=0;
					}
					printf("mov_motor1 1: %d\n", mov_motor1);
					printf("mov_motor2 2: %d\n", mov_motor2);	
 					if(beta_z>=beta_z_aux)										//Dirección de giro
 					{
 						PORTD|=(1<<PORTD2);
						beta_z_aux+=mov_motor1*PASO;
 					}else{
 						PORTD&=~(1<<PORTD2);
						beta_z_aux-=mov_motor1*PASO; 
 					}
 					if(beta_a>=beta_a_aux )
 					{
 						PORTD|=(1<<PORTD3);
						beta_a_aux+=mov_motor2*PASO;
 					}else{
 						PORTD&=~(1<<PORTD3);
						beta_a_aux-=mov_motor2*PASO;
 					}	 
					TCCR2B=(1<<CS20)|(1<<CS21)|(1<<CS22);
 					while(mov_motor1>0)											//Pulsos de a cada motor
 					{ 
						if (activar_motor==1)
						{
 							PORTD|=(1<<PORTD5);
						}
						if (activar_motor==2)
						{
							PORTD&=~(1<<PORTD5);
							mov_motor1=mov_motor1-1;
							activar_motor=0;
						}
								 				 
 					}
					TCCR2B&=~((1<<CS20)|(1<<CS21)|(1<<CS22));
					delay(10000);
					TCCR2B=(1<<CS20)|(1<<CS21)|(1<<CS22);
					while(mov_motor2>0)
 					{
						if (activar_motor==1)
						{
							PORTD|=(1<<PORTD4);
						}
						if (activar_motor==2)
						{
							PORTD&=~(1<<PORTD4);
 							mov_motor2=mov_motor2-1;
							activar_motor=0;							
						}
 					}
					TCCR2B&=~((1<<CS20)|(1<<CS21)|(1<<CS22));
 					activar_pulsos=0;
				}	
				if (tramas_leidas==32*multiplicador)						//Una vez leído todo el buffer
				{
					tramas_leidas=0;
					indice_buffer=0;
					offset=offset+0x02;
					if (tiempo==2345)
					{
						finhoming=0;
						flagestado=2;
					}
				}		
				if (flagestado==2) estado=E_homing;
				if (flagestado==1) estado=E_desactivado;
				break;
			default:
				break;
		}
    }
}