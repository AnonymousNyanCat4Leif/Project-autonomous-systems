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

#define TURN_HIGH 600
#define TURN_LOW 500
#define MAX_SEGMENTS 50

char buffer[64]; 

extern struct Bil_t Bil;

uint16_t bane[MAX_SEGMENTS];
uint8_t bane_index = 0;

typedef enum {
	
	LIGE,
	SVING
	} state_t;
	
	static state_t state = LIGE;
	
	void bane_opmaaling(uint16_t accel_x)
	{
		if (state == LIGE && accel_x > TURN_HIGH){
			state = SVING;
			
			if (bane_index < MAX_SEGMENTS){
			bane[bane_index++] = Bil.Odo;
		}
	}

if (state == SVING && accel_x < TURN_LOW) {
	state = LIGE;
	
	if (bane_index < MAX_SEGMENTS){
		bane[bane_index++] = Bil.Odo; 
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

	
	void bane_build_segments(void)
	{
		segment_count = 0;
		
		for (int i = 0; i < bane_index -1; i += 2){
		segments[segment_count].start = bane[i];
		segments[segment_count].end   = bane[i+1];
		segments[segment_count].speed = 60; //det en start værdi 
		
		segment_count++;
		}
	}
	//her finder den hvor bilen befinder sig på banen gennem segmenter
	
	int find_segment(uint16_t odo)
	{
		for (int i = 0; i < segment_count; i ++){
			if (odo >= segments[i].start && odo < segments[i].end){
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
		int seg = find_segment(Bil.Odo);
		
		if (seg < 0) return;
		
		if (Bil.Acceleration < -5){
			segments[seg].speed -= 2;
		}
		else if (Bil.Acceleration > 0){
			segments[seg].speed += 1;
		}
	
	//her kommer der nogle begrænsninger 
	if (segments[seg].speed > 100) segments[seg].speed = 100;
	if (segments[seg].speed > 40)  segments[seg].speed = 40;	
	}
	
	