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

uint16_t ADC_Read(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);	// Select channel
	ADCSRA |= (1 << ADSC);						// Start conversion
	while (ADCSRA & (1 << ADSC));				
	return ADCW;								// Return 10-bit result
}

uint16_t Accelerometer(uint8_t channel)
{
	static uint16_t rolling[15];
	static uint8_t index_rolling = 0;	
	uint16_t accel =  ADC_Read(channel);
	uint16_t accel_filtered = 0;
	
	rolling[index_rolling] = accel;
	index_rolling = (index_rolling + 1) % 15;
	for (uint8_t i = 0; i < 15; i++)
	{
		accel_filtered += rolling[i];
	}
	return accel_filtered/15;
}