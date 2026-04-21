/*
 * Hurtig-Bil.c
 *
 * Created: 21/03/2026 18.27.28
 * Author : Micha
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD - 1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "usart.h"
#include "adc.h"
#include "pwm.h"
#include "timer2.h"

char buffer[32];
char cmd[4];



int main(void)
{
    /*  */
	USART_Init(MYUBRR);
	pwm_Init();
	ADC_Init();
	timer2_init();
	sei();

	uint8_t speed = 0;
	uint8_t lock = 0;


	while (1) 
	{
		uint16_t value = USART_Receive;

		if (value >=0){ // Wait for the first speed command
			char c = (char)value;

			switch (lock)
			{
			case 0:	// confirms it is an actual value and not noice
				if (c == 's'){
					speed = 0;
					lock = 1;
				}
				break;

			case 1: // reading value and convert ASCII to numerical value.
				if (c >= '0' && c <= '9') {
					speed = speed * 10 + (c - '0');
				}
				else if (c == '\n'){
					pwm_set_speed(speed);
					snprintf(buffer, sizeof(buffer), "Motor started at speed %u\r\n", speed);
					USART_Print(buffer);
					lock = 0; // go pack to waiting for the next speed command
				}
			}
		}


		if (tick200ms) {
            tick200ms = 0;

            uint16_t x = ADC_Read(0);
            uint16_t y = ADC_Read(1);
            uint16_t z = ADC_Read(2);

            snprintf(buffer, sizeof(buffer), "X=%u Y=%u Z=%u\r\n", x, y, z);
            USART_Print(buffer);
        }
    }
}

