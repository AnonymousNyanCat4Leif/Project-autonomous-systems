/*
 * adc.c
 *
 * Created: 22/03/2026 16.53.10
 *  Author: Micha
 */ 

#include "adc.h"

uint16_t accel = 0;
uint16_t accel_filtered = 0;
uint16_t size = 15;
uint16_t rolling[15];
uint8_t index_rolling = 0;

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
	accel = ADC_Read(channel);
	rolling[index_rolling] = accel;
	index_rolling = (index_rolling + 1) % size;
	accel_filtered = 0;
	for (uint8_t i = 0; i < size; i++)
	{
		accel_filtered += rolling[i];
	}
	return accel_filtered/size;
}