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
#define BREAKING_DIST 10
#define MAX_ACCEL_X 490


char buffer[64]; 

extern struct Bil_t Bil;

uint16_t bane[MAX_SEGMENTS];
uint8_t bane_index = 0;
uint16_t oldOdo = 0;
uint8_t count = 0;
Segment segments[MAX_SEGMENTS];
	
static state_t state = LIGE;
	
state_t swing_detect(uint16_t accel_x)
{
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
		uint16_t length = Bil.Odo - oldOdo;

		const char *type_str = (state == LIGE) ? "STRAIGHT" : "TURN";
		snprintf(buffer, sizeof(buffer), "[SEG] %s  length=%u pulses\r\n", type_str, length);
		USART_Print(buffer);

		segments[count].type = state;
		count++;
		oldOdo = Bil.Odo;
	}
}

// Husk Bil.Odo måler afstand målt i pulstællinger
//gemmer segmenter 

	
uint8_t segment_count = 0;
uint8_t last_speed = 0;
uint16_t track_length = 0;
uint8_t speed_turns = 60;
uint8_t speed_straights = 120;

	
void bane_build_segments(void)
{
	segment_count = 0;
		
	for (int i = 0; i < bane_index -1; i++)
	{
		
		if (segments[segment_count].type == LIGE)
		{
			segments[segment_count].start = bane[i];
			segments[segment_count].end   = bane[i+1]-6;
			segments[segment_count].speed = speed_straights; 
		} else {
			segments[segment_count].start = bane[i]-6;
			segments[segment_count].end   = bane[i+1];
			segments[segment_count].speed = speed_turns;
		}
		
		segment_count++;
	}
	if (segment_count > 0)
	{
		track_length = segments[segment_count - 1].end - bane[0];
		snprintf(buffer, sizeof(buffer), "[SEG] Track length = %u pulses  Segments = %u\r\n", track_length, segment_count);
		USART_Print(buffer);
	}
}
//her finder den hvor bilen befinder sig på banen gennem segmenter
	
seg_pos find_segment(uint16_t odo)
{
	seg_pos r;
	r.segment = -1;
	
	r.pos = odo % track_length + bane[0];
	
	for (int i = 0; i < segment_count; i ++)
	{
		if (r.pos >= segments[i].start && r.pos < segments[i].end)
		{
			r.segment = i;
			return r;
		}
	}
	return r;
}
	
void bane_run(void)
{
	seg_pos r = find_segment(Bil.Odo);

	if (r.segment < 0)
	return;

	uint16_t dist_left = segments[r.segment].end - r.pos;
	uint8_t new_speed;

	if (segments[r.segment].type == LIGE && dist_left < BREAKING_DIST)
	{
		uint8_t diff = speed_straights - speed_turns;
		new_speed = speed_turns + ((diff * dist_left) / BREAKING_DIST);
	}
	else
	{
		new_speed = segments[r.segment].speed;
	}

	pwm_set_speed(new_speed);

	if (new_speed != last_speed)
	{
		last_speed = new_speed;
		snprintf(buffer, sizeof(buffer), "[RUN] Seg=%d  %s  distLeft=%u  speed=%u\r\n", r.segment, (segments[r.segment].type == LIGE) ? "STRAIGHT" : "TURN", dist_left, new_speed);
		USART_Print(buffer);
	}
}

void bane_update_learning(void)
{
	if (top_accel_x < MAX_ACCEL_X)
	{
		speed_turns += 3;
	}
	speed_straights += 10;
}