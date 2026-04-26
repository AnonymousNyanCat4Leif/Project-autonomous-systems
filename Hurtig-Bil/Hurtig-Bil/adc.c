/*
 * adc.c
 *
 * Created: 22/03/2026 16.53.10
 *  Author: Micha
 */ 

#include "adc.h"

void ADC_Init(void)
{
	ADMUX = 0;		// AVCC reference
	ADCSRA = (1 << ADEN) |		// Enables the ADC
			(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);		// Sets prescaler to 128 -> ADC at 128 kHz 
}

uint16_t ADC_Read(uint16_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);	// Select channel
	ADCSRA |= (1 << ADSC);						// Start conversion
	while (ADCSRA & (1 << ADSC));				
	return ADCW;								// Return 10-bit result
}