#include "maalstreg.h"

#define MAX_LAPS 50

volatile uint16_t lap_count = 0;                // antal omgange registreret


// -------------------------
// INT0: registrerer m�lstrengen
// -------------------------
void INT0_init(void)
{
	DDRD &= ~(1 << PD2);      // PD2 som input
	PORTD &= ~(1 << PD2);     // ingen pull-up (74HC14 driver signalet)

	MCUCR |=  (1 << ISC01);   // falling edge
	MCUCR &= ~(1 << ISC00);

	GICR |= (1 << INT0);      // enable INT0
}

ISR(INT0_vect)
{
	// Gem i historik (kan sl�s fra)
	if (lap_count < MAX_LAPS)
	{
		lap_count++;
	} else if (lap_count >= MAX_LAPS)
	{
		lap_count = 0;
	}
}
