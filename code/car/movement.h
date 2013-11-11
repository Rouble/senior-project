/*
 * movement.h
 *
 * Created: 4/18/2013 ☢ 12:23:58 ☢
 *  Author: Chris
 */ 

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <avr/io.h>
#include "pwm.h"

//these defines are used to create a value range for which the vehicle doesnt move
#define deadzone_pct	0.1
#define upper_deadzone	(char)(stopval + (stopval * deadzone_pct))
#define lower_deadzone	(char)(stopval - (stopval * deadzone_pct))
#define stopval 128			//neutral value

//used to set flipper pin states
#define FFlipFwd (PORTD |= (1<<PORTD4))
#define FFlipRev (PORTD |= (1<<PORTD6))
#define FFlipStp (PORTD &= ~((1<<PORTD4)|(1<<PORTD6)))
#define RFlipFwd (PORTD |= (1<<PORTD7))
#define RFlipRev (PORTB |= (1<<PORTB4))
#define RFlipStp (PORTD &= ~(1<<PORTD7)); (PORTB &= ~(1<<PORTB4))

void stahp();						//stops all movement
void set_right(char val);			//set right track speed/direction		//TODO: determine min/max values and deadzone
void set_left(char val);			//set left track speed/direction
void set_flippers(char buttons);	//sets flipper movement using trigger buttons on remote


#endif /* INCFILE1_H_ */