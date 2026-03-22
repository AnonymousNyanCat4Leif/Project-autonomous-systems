#include "usart.h"

void USART_Init( unsigned int baud )
{
	/* Set TxD (PD1) as output*/
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

void USART_Print(const char *str)
{
	while (*str) USART_Transmit(*str++);
}