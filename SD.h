#ifndef SD_H_
#define SD_H_

unsigned char SD_comando(char dt0, char dt1, char dt2, char dt3, char dt4, char dt5);
unsigned char SD_Init(void);
unsigned char SD_Escribir_Bloque (char *buffer, unsigned char dt1, unsigned char dt2, unsigned char dt3, unsigned char dt4);
unsigned char SD_Leer_Bloque (char *buffer, unsigned char dt1, unsigned char dt2, unsigned char dt3, unsigned char dt4);

#endif /* SD_H_ */