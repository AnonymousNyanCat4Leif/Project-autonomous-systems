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
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Speedometer.h"
#include "usart.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"

void Init_ports( );    // Declaration of a function to be implemented later

char buffer[52];


///////////////////////////////////  m a i n ()  funktion ///////////////////////////////
int main(void)
{
    /*  */
	int speedbar = 0;  // Til_hastighedsvisning p�_lysdioder
	
	Init_ports();
	Init_speedometer();                // Ops�tning_af Timer1 capture_og Overflow
	//Det_er rart_at kunne_skrive tekst_og data p�_PC sk�rmen_ - derfor_den_ops�tning
	USART_Init(MYUBRR);
	pwm_Init();
	ADC_Init();
	timer2_init();
	sei();

	uint8_t speed = 0;
	uint8_t lock = 0;


	while (1) 
	{		
		int value = USART_Receive();
//
		if (value >=0){ // Wait for the first speed 
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
				else if (c == '\r' || c == '\n'){
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

            snprintf(buffer, sizeof(buffer), "X=%u Y=%u Z=%u Value=%u\r\n", x, y, z, speed);
            USART_Print(buffer);
        }

		if(Ur.LapFlag)  // N�r_LapFlag=1 betyder_det at der_har v�ret_en ICP_puls
		{   
			Beregn_hastighed_og_acc( ANTAL_FELTER);      // ANTAL_FELTER = 3 => 1 Omdr.
			// Bem�rk_at de_f�lgende 2 statements skriver_tekst p�_PC_sk�rmen 
			// hvis_der er_installeret den_rette USB/UART driver og_et_terminalprogram
			//  %3d => udskrift af integer og  %8s udskrift af string (tal er feltbredde)
			snprintf(buffer, sizeof(buffer), "ODO=%3d  Hast= %8s  Acc=%8s\r\n", Bil.Odo, floatstr(Bil.Hastighed), floatstr(Bil.Acceleration));
			USART_Print(buffer);
			// Disse_linjer laver_en "Speedbar_" til_visning_af_hastighed (Tryk_p�_S10)
		}
    }

	
}
//======================= I n i t _ p o r t s ( ) ===========================
// Note - the function takes no parameters and return nothing, hence void
void Init_ports( )
{	DDRD  &= ~(1<<PD6);	    // setup PORTD, bit6 and bit2 as input with ....
	PORTD |= (1<<PD6);   // AKTIV�R PULL-UP
	//PORTD = PORTD | 0b01000100; // UPS! internal pull-up enabled .. not needed
	DDRC  = 0x00;	// setup PORTC as input with ....
	PORTC = 0xFF;	// internal pull-up enabled
}


