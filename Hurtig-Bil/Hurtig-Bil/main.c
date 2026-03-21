/*
 * Hurtig-Bil.c
 *
 * Created: 21/03/2026 18.27.28
 * Author : Micha
 */ 

#define F_CPU 1000000UL
#define BAUD 4800
#define MYUBRR (F_CPU/16/BAUD - 1)

#include <avr/io.h>
#include <util/delay.h>

void USART_Init( unsigned int baud )
{
	/* Set TSD (PD1) as output*/
	DDRD |= (1 << PD1);
	/* Set baud rate */
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	/* Enable transmitter */
	UCSRB = (1<<TXEN);
	/* Set frame format: 8data, 1stop bit (|(1<<USBS) for 2stop bit)*/
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
	;
	/* Put data into buffer, sends the data */
	UDR = data;
}

void USART_Print(const char *str){
	while (*str) USART_Transmit(*str++);
}

int main(void)
{
    /*  */
	USART_Init(MYUBRR);
	
	USART_Print("Hello from 1MHz ATmega32A!\r\n");	
    while (1) {
		USART_Print("Tick\r\n");
		_delay_ms(1000);
    }
}

