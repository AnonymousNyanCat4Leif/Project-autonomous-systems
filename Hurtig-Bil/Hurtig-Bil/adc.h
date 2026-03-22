/*
 * adc.h
 *
 * Created: 22/03/2026 17.08.33
 *  Author: Micha
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void ADC_Init(void);
uint16_t ADC_Read(uint16_t channel);

#endif /* ADC_H_ */