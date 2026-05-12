/*
 * bane.c
 *
 * Created: 27-04-2026 14:21:00
 *  Author: bobby
 */ 

#include <stdio.h>
#include "bane.h"
#include "Speedometer.h"
#include "pwm.h"
#include "usart.h"

#define HYSTERESIS 20
#define TURN 30
#define STRAIGHT 338
#define MAX_SEGMENTS 50
#define ENTER_COUNT 4
#define EXIT_COUNT  10


char buffer[64]; 

extern struct Bil_t Bil;

uint16_t bane[MAX_SEGMENTS];
uint8_t bane_index = 0;
	
static state_t state = LIGE;
	
state_t swing_detect(uint16_t accel_x)
{
	static state_t state = LIGE;

	if (state == LIGE)
	{
		if (accel_x < STRAIGHT-TURN || accel_x > STRAIGHT+TURN)
		{
			state = SVING;
		}	
	}
	else // SVING
	{
		if (accel_x > (STRAIGHT-TURN)+HYSTERESIS && accel_x < (STRAIGHT+TURN)-HYSTERESIS)
		{
			state = LIGE;
		}
	}
	return state;
}

void bane_opmaaling(state_t state)
{
	if (bane_index < MAX_SEGMENTS)
	{
		bane[bane_index++] = Bil.Odo;
		if (state == LIGE)
		{
			USART_Transmit('l');
		} else
		{
			USART_Transmit('s');
		}
	}	
}


void bane_reset(void)
{
	bane_index = 0;
	state = LIGE;
}
// Husk Bil.Odo måler afstand målt i pulstællinger
//gemmer segmenter 

typedef struct {
	uint16_t start;
	uint16_t end;
	uint8_t speed;
} Segment;
	
#define MAX_SEG 25
	
Segment segments[MAX_SEG];
uint8_t segment_count = 0;
uint8_t last_seg = 0;
uint16_t track_length = 0;

	
void bane_build_segments(void)
{
	segment_count = 0;
		
	for (int i = 0; i < bane_index -1; i += 2)
	{
		segments[segment_count].start = bane[i];
		segments[segment_count].end   = bane[i+1];
		if (segment_count % 2 == 1)
		{
			segments[segment_count].speed = 100; 
		} else {
			segments[segment_count].speed = 60;
		}
		
		segment_count++;
	}
	if (segment_count > 0)
	{
	track_length = segments[segment_count - 1].end;
	}
}
//her finder den hvor bilen befinder sig på banen gennem segmenter
	
int find_segment(uint16_t odo)
{
	uint16_t position = odo % track_length;
	
	for (int i = 0; i < segment_count; i ++)
	{
		if (position >= segments[i].start && position < segments[i].end)
		{
			return i;
		}
	}
	return -1;
}
	
void bane_run(void)
{
	int seg = find_segment(Bil.Odo);
		
	if (seg >= 0){
		pwm_set_speed(segments[seg].speed);
			
		//printer en fart når segmenterne ændre sig
		if (seg != last_seg){
			last_seg = seg;
				
			snprintf(buffer, sizeof(buffer), "segment %d, speed %u\r\n", seg, segments[seg].speed);
			USART_Print(buffer);
		}
	}
}
//void bane_update_learning(void)
//{
	//int seg = find_segment(bil.odo);
		//
	//if (seg < 0) return;
		//
	//if (bil.acceleration < -5){
		//segments[seg].speed -= 2;
	//}
	//else if (bil.acceleration > 0){
		//segments[seg].speed += 1;
	//}
	//
////her kommer der nogle begrænsninger 
	//if (segments[seg].speed > 100) segments[seg].speed = 100;
	//if (segments[seg].speed < 40)  segments[seg].speed = 40;	
//}
	
	
