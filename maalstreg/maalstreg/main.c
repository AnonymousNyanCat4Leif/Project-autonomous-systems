/*
 * maalstreg.c
 *
 * Created: 22-04-2026 08:34:07
 * Author : Pierre
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define MAX_LAPS 50

volatile uint32_t ms_counter = 0;               // tæller ms mellem passager
volatile uint32_t last_lap_time = 0;            // seneste omgangstid
volatile uint32_t lap_times[MAX_LAPS];          // historik
volatile uint16_t lap_count = 0;                // antal omgange registreret


// -------------------------
// Timer1: 1 ms tidsbase
// -------------------------
void Timer1_init(void)
{
	TCCR1B |= (1 << WGM12);               // CTC mode
	OCR1A = 249;                          // 1 ms @ 16 MHz, prescaler 64
	TCCR1B |= (1 << CS11) | (1 << CS10);  // prescaler 64
	TIMSK |= (1 << OCIE1A);               // enable compare interrupt
}

ISR(TIMER1_COMPA_vect)
{
	ms_counter++;     // tæller millisekunder
}


// -------------------------
// INT0: registrerer målstrengen
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
	last_lap_time = ms_counter;   // gem seneste omgangstid
	ms_counter = 0;               // start ny måling

	// Gem i historik (kan slås fra)
	if (lap_count < MAX_LAPS)
	{
		lap_times[lap_count] = last_lap_time;
		lap_count++;
	}
}


// -------------------------
// MAIN
// -------------------------
int main(void)
{
	Timer1_init();
	INT0_init();
	sei();   // global interrupt enable

	while (1)
	{
		// Her kan du bruge:
		// last_lap_time  -> seneste omgangstid i ms
		// lap_count      -> antal omgange
		// lap_times[]    -> historik

		// Du kan fx sende data ud på UART, vise på display,
		// eller bruge det i din state machine.
	}
}
