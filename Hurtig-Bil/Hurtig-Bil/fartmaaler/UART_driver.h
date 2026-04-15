//********************** UART_driver.h *******************************
// Inspiration to - "How to use the ATmega UART with interrupts"
//
#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL     // MUST BE ADJUSTED TO FIT THE KIT
#define U2x 2               // U2x must be 1 or 2
#define BAUD_PRESCALE ((((F_CPU*U2x)/(USART_BAUDRATE * 16UL)))-1)

#define CR 13
#define LF 10
#define ESCAPE 27
#define SPACE  32

char str[100];   // Text string to be used with the sprintf( str, ..

char CRflag;      // CRflag==1 => You pressed the <CR> 
char inbuf[9];    // FIFO in buffer - with room for 8 characters + zero
int  incnt;       // In buffer counter = amount of number stored ...
char command;     // Command part of the input in inbuf - like A
int  indtal;      // Number part of the input in inbuf  - like -12345

// MAX value for can be: MAXinBUF>255 (depends on RAM aviable)
#define MAXoutBUF 150
struct FIFO_out_buffer
{	unsigned int count;      // number of chars in buffer
	unsigned int inx, outx;  // pointers (indexes) for buffer
	char buffer[MAXoutBUF];  // the FIFO buffer
} FIFO_out;

void  Init_Serial_Port_and_FIFO(long);
char  read();
int   readint();
void  write( char);
void  writestr( char[], char);
void  writeCRLF( char);

char *bin( unsigned char);  // Convert a byte to a string of '1' and '0'
char *floatstr(float tal);  // sadly the %f attribute not working, hence..

#endif /* UART_DRIVER_H_ */