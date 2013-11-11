/*
 * pwm.cpp
 *
 * Created: 4/13/2013 ☢ 19:51:14 ☢
 *  Author: Chris
 */ 

#include <avr/interrupt.h>
#include "pwm.h"

void setup_timers()
{
	char sreg = SREG; //10 bit register writes shouldn't be interrupted
	cli();
		
	//timers 1 and 3 are 16 bit counters they are set to phase correct 10bit pwm mode to match accuracy of timer 4
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
	TCCR1B = (1<<CS10);
	TCCR3A = (1<<COM3A1)|(1<<WGM31)|(1<<WGM30);
	TCCR3B = (1<<CS30);
	TCCR4A = (1<<COM4A1)|(1<<PWM4A);
	TCCR4B = (1<<CS40);
	TCCR4D = (1<<WGM40);

	TC4H = 3;	
	OCR4C = 0xFF;
	
	SREG = sreg;
}

void setpwm( char duty, char channel )
{
	char sreg = SREG; //10 bit register writes shouldn't be interrupted
	cli();
	
	int doodie = (1023 / SCALE) * duty;		//scale the input value to full range for 10 bit pwm
	
	//prevent over/underflow on duty cycle 
	if (doodie > 1023) 
	{
		doodie = 1023;		
	}
	
	if (doodie < 0)
	{
		doodie = 0;	
	}		
	
	// the pwm channels have been numbered from top to bottom for the vehicle control board
	// probably should have labeled them in the board silk screen.... oh well

	switch (channel)
	{
		case 0:						
			TC4H = (doodie >> 8);		//timer 4 requires a write to an shared upper byte register
			OCR4A = doodie & 0x00FF;	//
			break;
		case 1: 
			OCR3A = doodie;
			break;
		case 2:
			OCR1B = doodie;
			break;
		case 3:
			OCR1A = doodie;
			break;
		default:			
			OCR1A = doodie;
			OCR1B = doodie;
			OCR3A = doodie;
			TC4H = (doodie >> 8);
			OCR4A = doodie & 0xFF;
	}
	
	SREG = sreg; 
}
