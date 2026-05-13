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
#include "maalstreg.h"
#include "bane.h"

void Init_ports( );    // Declaration of a function to be implemented later


extern volatile uint16_t lap_count;
char buffer[256];


///////////////////////////////////  m a i n ()  funktion ///////////////////////////////
int main(void)
{
	
	Init_ports();
	Init_speedometer();                // Ops�tning_af Timer1 capture_og Overflow
	//Det_er rart_at kunne_skrive tekst_og data p�_PC sk�rmen_ - derfor_den_ops�tning
	USART_Init(MYUBRR);
	pwm_Init();
	ADC_Init();
	timer2_init();
	INT0_init();
	sei();

	uint8_t speed = 0;
	uint8_t lock = 0;
	uint8_t last_lap_count = 0;
	uint8_t last_lap_count2 = 7;
	uint16_t debug_counter = 0;
	uint16_t top_accel_x = 0; //hweuhw
	state_t current_state = SVING;
	uint16_t distance = 0;


	while (1) 
	{		
		int value = USART_Receive();
//
		if (value >=0){ // Wait for the first speed 
			char c = (char)value;

			switch (lock)
			{
			case 0:	// confirms it is an actual value and not noise
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
		

		if(Ur.LapFlag)  // N�r_LapFlag=1 betyder_det at der_har v�ret_en ICP_puls
		{   
			Beregn_hastighed_og_acc(ANTAL_FELTER);      // ANTAL_FELTER = 3 => 1 Omdr.
			// Bem�rk_at de_f�lgende 2 statements skriver_tekst p�_PC_sk�rmen 
			// hvis_der er_installeret den_rette USB/UART driver og_et_terminalprogram
			//  %3d => udskrift af integer og  %8s udskrift af string (tal er feltbredde)
		}

		if (lap_count != last_lap_count)
		{
			last_lap_count = lap_count;
			uint16_t lap_lenght = Bil.Odo - distance;
			distance = Bil.Odo;
			snprintf(buffer, sizeof(buffer), "\r                                                                                           lap = %u length = %3d", lap_count, lap_lenght);
			USART_Print(buffer);
		}
		
		switch(lap_count)
		{		
		case 0:
			if(tick5ms)
			{
				tick5ms = 0;
				
				uint16_t accel_x = Accelerometer(2);		// we run the Accelerometer here to fill the moving average filter
				
				if (++debug_counter >= 1000)
				{
					debug_counter = 0;
					
					snprintf(buffer, sizeof(buffer), "\rSpeed=%u ODO=%3d TopX=%u Hast=%s acc=%s     ", speed, Bil.Odo, top_accel_x, floatstr(Bil.Hastighed), floatstr(Bil.Acceleration));
					USART_Print(buffer);
					
					//pwm_set_speed(83);
				}
				
			}
			break;
		
		case 1:
			if(tick5ms)
			{
				tick5ms = 0;
				uint16_t accel_x = Accelerometer(2);
			
				state_t state = swing_detect(accel_x);
				if (state != current_state)
				{
					current_state = state;
					bane_opmaaling(state);
				}
			
				if (top_accel_x < accel_x)
				{
					top_accel_x = accel_x;
				}
				
				if (++debug_counter >= 20)
				{
					debug_counter = 0;
					
					snprintf(buffer, sizeof(buffer), "\rX=%u Speed=%u ODO=%3d TopX=%u Hast=%s acc=%s     ", accel_x, speed, Bil.Odo, top_accel_x, floatstr(Bil.Hastighed), floatstr(Bil.Acceleration));
					USART_Print(buffer);
				}
			}
			break;
				
		case 2:
			bane_opmaaling(SVING);
			
			lap_count++;
			break;
			
		case 3:
			bane_build_segments();
			
			snprintf(buffer, sizeof(buffer), "\r                                                                                                                               Segmenter bygget!");
			USART_Print(buffer);
			
			lap_count++;
			break;
			
		case 4:
			break;
			
		case 5:
			break;
				
		case 6:
			break;
		
			
		default:
			bane_run();
			
			if (lap_count != last_lap_count2)
			{
				bane_update_learning();
				last_lap_count2 = lap_count;
			}
			break;	
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


