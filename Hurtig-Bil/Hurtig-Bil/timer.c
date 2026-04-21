#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer2.h"

volatile uint8_t tick200ms = 0;
static uint8_t overflow_count = 0;

ISR(TIMER2_OVF_vect) {
    if (++overflow_count >= 12) {
        overflow_count = 0;
        tick200ms = 1;
    }
}

void timer2_init(void) {
    TCCR2 = (1 << CS22) | (1 << CS21) | (1 << CS20); // prescaler 1024
    TIMSK |= (1 << TOIE2); // enable overflow interrupt
}