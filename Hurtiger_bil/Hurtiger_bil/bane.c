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
uint16_t oldOdo = 0;
uint16_t lenght = 0;
uint8_t count = 0;
Segment segments[MAX_SEGMENTS];
	
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
		lenght = Bil.Odo - oldOdo;
		if (state == LIGE)
		{
			snprintf(buffer, sizeof(buffer), "l %u", lenght);
			USART_Print(buffer);
			segments[count].type = LIGE;
			count++;
		} else
		{
			snprintf(buffer, sizeof(buffer), "s %u", lenght);
			USART_Print(buffer);
			segments[count].type = SVING;
			count++;
		}
		oldOdo = Bil.Odo;
	}	
}


void bane_reset(void)
{
	bane_index = 0;
	state = LIGE;
}
// Husk Bil.Odo måler afstand målt i pulstællinger
//gemmer segmenter 

	
uint8_t segment_count = 0;
uint8_t last_seg = 0;
uint16_t track_length = 0;

	
void bane_build_segments(void)
{
	segment_count = 0;
		
	for (int i = 0; i < bane_index -1; i++)
	{
		
		if (segments[segment_count].type == LIGE)
		{
			segments[segment_count].start = bane[i];
			segments[segment_count].end   = bane[i+1]-6;
			segments[segment_count].speed = 255; 
		} else {
			segments[segment_count].start = bane[i]-6;
			segments[segment_count].end   = bane[i+1];
			segments[segment_count].speed = 60;
		}
		
		segment_count++;
	}
	if (segment_count > 0)
	{
	track_length = segments[segment_count - 1].end;
	snprintf(buffer, sizeof(buffer), "calculated track length = %u\r\n", track_length);
	USART_Print(buffer);
	}
}
//her finder den hvor bilen befinder sig på banen gennem segmenter
	
int find_segment(uint16_t odo)
{
	uint16_t position = odo % track_length + bane[0];
	
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

void bane_update_learning(void)
{
	segment_count = 0;
	
	for (int i = 0; i < bane_index -1; i++)
	{
		
		if (segments[segment_count].type == LIGE)
		{
			segments[segment_count].speed += 10;
		} else {
			segments[segment_count].speed += 3;
		}

		segment_count++;
	}
}