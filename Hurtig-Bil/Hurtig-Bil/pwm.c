/*
 * pwm.c
 *
 * Created: 30/03/2026 21.53.13
 *  Author: Micha
 */ 

#include <avr/io.h>
#include "pwm.h"

void pwm_Init(void)
{
	// Set PD7 (OC2) as output
	DDRD |= (1<< PD7);
	
	// Set fast PWM mode (Mode 3)
	TCCR2 |= (1<< WGM21) | (1<< WGM20);
	
	// non-inverting
	TCCR2 |= (1<< COM21);
	
	// Prescaler = 64 -> PWM frequency = 976 (CS21 for a prescaler of 8)
	TCCR2 |= (1<< CS22);
	
	// init motor
	OCR2 = 0;	
}

void pwm_set_speed(uint8_t duty)
{
	// duty in [0; 255]
	OCR2 = duty;
}