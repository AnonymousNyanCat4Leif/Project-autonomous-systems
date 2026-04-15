/*************************** Speedometer.c ***********************************/ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Speedometer.h"

#define PULS_AFSTAND 1000.0  // Dette_tal skal_justeres til_brug i_bilen( svarer til omkredsen af hjul)

//////////// I n i t _ s p e e d o m e t e r ( ) //////////////////////////////
// Denne_funktion_initialiserer timer1 med "input capture" (altså_et_stopur)
// Hver_gang at ICP (PortD pin 6) har_en op-flanke_vil den aktuelle_tællerværdi
// blive_lagret i ICR1 registeret_og samtidig_kommer et TIMER1_CAPT interrupt.
// ICR1 = Input Capture Register 1.
// Bemærk_at der_også_kommer_et TIMER1_OVF (Timer1 overflow) interrupt_hver
// gang at Timer1 når_max værdien_0xFFFF.
// Ved_hjælp af_dette interrupt kan_en exstra_16bits tæller_"MSW" tælles_op.
// Man kan_med andre_ord_danne en 32 bits tidsmåling_= MSW<<16 + ICR1
//-----------------------------------------------------------------------------;
void Init_speedometer( )
{   TIFR  |= 1<<ICF1;	 // Clear: Input Capture Flag timer 1 (bit 5)
	TIMSK &= ~0x18;      // Set bit 4,3 to zero
	TIMSK |= 1<<TOIE1;   // Timer1 Overflow interrupt Enable (bit 2)
	TIMSK |= 1<<TICIE1;	 // Timer1 Input Capture Interrupt Enable (bit 5)
	TCCR1A = 0x00;
	TCCR1B = 0b10000011; // bit7:6 = noisecancel_edgecap
	// bit2:0 = 000 => ingen clk,
	//          001 => Clk/1             =>16MHz     => T= 0,0625us
	//          010 => Clk/8      16/8   => 2MHz     => T= 0,5us
	//          011 => Clk/64     16/64  => 0,25MHz  => T= 4us
	//          100 => Clk/256    16/256 => 62,5kHz  => T=16us
	//          101 => Clk/1024   16/1024=> 15,6kHz  => T=64us
	Ur.MSW = 0;        // Init_ Most Significant Word
	Ur.LapFlag=0;      // Nulstil_flag => ingen mellemtider klar
	Bil.Odo=0;         // Nulstil_afstandmåler
}

//=T i m e r 1    C a p t u r e   i n t e r r u p t ================================
// Timer 1 Capture interrupt service routine
ISR(TIMER1_CAPT_vect)
{	Ur.Linx          = (Ur.Linx+1)&(MAXur-1);      // Increment index 0,1,2,3 ... MAXur-1
	Ur.Laps[Ur.Linx] = ((uint32_t) Ur.MSW<<16) + ICR1;  // Store a 32bit number
	Ur.LapFlag = 1;  // Ny_mellemtid er_klar
	Bil.Odo++;       // Forøg_afstandsmåler
}
//=T i m e r 1    O v e r f l o w    i n t e r r u p t ================================
// Each time the timer reach 0xFFFF must the MSW be incremented
ISR(TIMER1_OVF_vect)
{	Ur.MSW++;
}

//////////////////// B e r e g n _ h a s t i g h e d _ o g _ a c c /////////////////////
// Tn er_antallet af_samples man skal_bruge_til_beregningerne = pulser per_omdrejning
void Beregn_hastighed_og_acc( int Tn)
{	int xLinx;        // Lagerplads_til en kopi_af Ur.Linx -hvorfor_er_det smart/_vigtigt?
	
	if (!Tn) Tn=1;    // Tn skal_altid være_mindst 1
	Ur.LapFlag = 0;   // Kvitter_for modtagelse_af_mellemtid
	// Beregn_tiden for en hel_omdrejning af_hjulet
	// Bil.Periodetid = Ur.Laps[Ur.Linx]- Ur.Laps[(Ur.Linx-Tn)&(MAXur-1)]; //Hvorfor ikke?
	xLinx = Ur.Linx;   // Husk en kopi_af Ur.Linx
	Bil.Periodetid   = Ur.Laps[xLinx]- Ur.Laps[(xLinx-Tn)&(MAXur-1)];
	//--------------------------------------------------------------------------------
	Bil.OldHastighed = Bil.Hastighed;
	Bil.Hastighed    = PULS_AFSTAND * Tn / Bil.Periodetid;
	Bil.Acceleration = 100000.0 *(Bil.Hastighed - Bil.OldHastighed)/ Bil.Periodetid;
	                                 // 100000.0 er_tilfældigt_valgt	 
	Bil.Hast = Bil.Hastighed;        // Lagre_som int
	Bil.Acc = 10*Bil.Acceleration;   // Lagre_acc (10 er_tilfældigt_valgt)
}

