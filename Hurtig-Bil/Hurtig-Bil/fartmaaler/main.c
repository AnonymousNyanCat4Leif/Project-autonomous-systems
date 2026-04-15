/*************************** HastighedsMaaler.c *************************************/ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Speedometer.h"
#include "UART_driver.h"     // Driver til_UART - forklares_senere

void Init_ports( );    // Declaration of a function to be implemented later

///////////////////////////////////  m a i n ()  funktion ///////////////////////////////
int main(void)
{   int speedbar = 0;  // Til_hastighedsvisning på_lysdioder
	
	Init_ports();
	Init_speedometer();                // Opsætning_af Timer1 capture_og Overflow
	//Det_er rart_at kunne_skrive tekst_og data på_PC skærmen_ - derfor_den_opsætning
	Init_Serial_Port_and_FIFO( 9600);  //9600, 14400, 19200, 38400, 57600, 115200 standards
	sei();	                           // Enable Global Interrupt
	writestr( "Hej verden", CR+LF);
    while (1) 
    {
		if(Ur.LapFlag)  // Når_LapFlag=1 betyder_det at der_har været_en ICP_puls
		{   
			Beregn_hastighed_og_acc( ANTAL_FELTER);      // ANTAL_FELTER = 3 => 1 Omdr.
			// Bemærk_at de_følgende 2 statements skriver_tekst på_PC_skærmen 
			// hvis_der er_installeret den_rette USB/UART driver og_et_terminalprogram
			//  %3d => udskrift af integer og  %8s udskrift af string (tal er feltbredde)
			sprintf( str, "ODO=%3d  Hast= %8s  Acc=%8s",
			               Bil.Odo, floatstr(Bil.Hastighed), floatstr(Bil.Acceleration));
			writestr( str, CR+LF);
			// Disse_linjer laver_en "Speedbar_" til_visning_af_hastighed (Tryk_på_S10)
			speedbar=1;
			for (int i=0; i<Bil.Hast; i++ ) speedbar = (speedbar<<1)+1;
			PORTB = ~speedbar;
			if(Ur.LapFlag)
			{
				Beregn_hastighed_og_acc( ANTAL_FELTER);
				// Tænd dioderne direkte baseret på afstanden
				PORTB = ~speedbar;
			}
		}
    }

	
}
//======================= I n i t _ p o r t s ( ) ===========================
// Note - the function takes no parameters and return nothing, hence void
void Init_ports( )
{	DDRD  &= ~(1<<PD6);	    // setup PORTD, bit6 and bit2 as input with ....
	PORTD |= (1<<PD6);   // AKTIVÉR PULL-UP
	//PORTD = PORTD | 0b01000100; // UPS! internal pull-up enabled .. not needed
	DDRC  = 0x00;	// setup PORTC as input with ....
	PORTC = 0xFF;	// internal pull-up enabled
	DDRB  = 0xFF;	// setup PORTB as output ....
	PORTB = 0xFF;	// and turn LEDs off
	DDRA  = 0;      // setup PORTA as input
	PORTA = 0;      // Internal pull-up not needed
}


