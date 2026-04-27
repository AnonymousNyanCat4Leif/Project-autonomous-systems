#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#ifndef MAALSTREG_H_
#define MAALSTREG_H_

void INT0_init(void);
extern volatile uint16_t lap_count;


#endif /* MAALSTREG_H_ */