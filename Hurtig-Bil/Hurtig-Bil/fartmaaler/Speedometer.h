/* Speedometer.h
 * Created: 19-03-2024 13:15:02
 * Author: jeppe
 */ 
#ifndef SPEEDOMETER_H_
#define SPEEDOMETER_H_ 1

#define F_CPU 8000000UL 

void Init_speedometer( );
void Beregn_hastighed_og_acc( int Tn);

#define MAXur 8         // Dette_tal SKAL_være 4,8,16,32
#define ANTAL_FELTER 3  // Bør_være lig_med antallet_af felter_på_hjulet
                        // Og_det skal_være_mindre end MAXur
struct Stopur_t
{ 	uint16_t  MSW;          // Most Significant Word (of the 32 bits)
	uint32_t  Laps[MAXur];  // MAXur- 32 bits_mellemtider
	int       Linx;         // Laps Index
	char      LapFlag;      // New Lap Flag -en ny_mellemtid er_klar
} Ur;

struct Bil_t
{   long  Periodetid;       // Tiden_imellem 2 pulser (en_hjulomdrejning)
    float OldHastighed;     // Den forrige_hastighedsmåling
	float Hastighed;        // Den aktuelle_hastighedsmåling
	float Acceleration;     // Hastighedsændring_= Acc/Deceleration
	int   Hast;             // Hastighed som_integer
	int   Acc;              // Accelleration_som_integer
	int   Odo;              // Afstandsmåler_til Speedometer
} Bil;

#endif /* SPEEDOMETER_H_ */