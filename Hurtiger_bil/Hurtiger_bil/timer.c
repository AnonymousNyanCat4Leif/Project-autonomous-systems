#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

volatile uint8_t tick5ms = 0;
static uint8_t overflow_count = 0;

ISR(TIMER0_OVF_vect) {
    if (++overflow_count >= 5) {
        overflow_count = 0;
        tick5ms = 1;
    }
}

void timer2_init(void) {
    TCCR0 = (1 << CS01) |  (1 << CS00); // prescaler 64
    TIMSK |= (1 << TOIE0); // enable overflow interrupt
}