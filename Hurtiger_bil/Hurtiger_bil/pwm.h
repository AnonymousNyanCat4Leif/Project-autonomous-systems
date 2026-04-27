/*
 * pwm.h
 *
 * Created: 30/03/2026 21.51.27
 *  Author: Micha
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

void pwm_Init(void);
void pwm_set_speed(uint8_t duty);


#endif /* PWM_H_ */