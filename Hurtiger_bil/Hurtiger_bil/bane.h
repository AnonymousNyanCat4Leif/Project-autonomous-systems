/*
 * bane.h
 *
 * Created: 27-04-2026 14:21:17
 *  Author: bobby
 */ 


#ifndef BANE_H_
#define BANE_H_

#include <stdint.h>

extern uint8_t speed;
extern uint16_t top_accel_x;
extern uint8_t speed_turns;
extern uint8_t speed_straights;

typedef enum {
	LIGE,
	SVING
} state_t;

typedef struct {
	uint16_t start;
	uint16_t end;
	uint8_t speed;
	state_t type;
} Segment;

typedef struct {
	int segment;
	uint16_t pos;
} seg_pos;

void bane_opmaaling(state_t);
state_t swing_detect(uint16_t accel_x);
void bane_reset(void);

// Nye ting så bilen kan lære banen 

void bane_build_segments(void);
void bane_run(void);
void bane_update_learning(void);


#endif /* BANE_H_ */