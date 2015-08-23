// 16MHz + 9600 Baud
// Possible bit rates:
// http://bit.ly/1ND0wxf
#define F_CPU 16000000
#define BAUD 9600
#define BAUDRATE (((F_CPU)/(BAUD*16UL))-1)
