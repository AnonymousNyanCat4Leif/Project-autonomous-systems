/*
 * Hurtig-Bil.c
 *
 * Created: 21/03/2026 18.27.28
 * Author : Micha
 */ 

#define F_CPU 3676400UL
#define BAUD 115200
#define MYUBRR (F_CPU/16/BAUD - 1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "usart.h"
#include "adc.h"

char buffer[32];

int main(void)
{
    /*  */
	USART_Init(MYUBRR);
	ADC_Init();
	
	
	USART_Print("Hello from 1MHz ATmega32A! Testing analog signal\r\n");	
    	
	while (1) 
	{
		uint16_t x = ADC_Read(0);	// PA0
		uint16_t y = ADC_Read(1);	// PA1
		uint16_t z = ADC_Read(2);	// PA2
		
		// Convert to text
		snprintf(buffer, sizeof(buffer), "X=%u Y=%u Z=%u\r\n", x, y, z);
		USART_Print(buffer);
		
		_delay_ms(200);
    }
}

