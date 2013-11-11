/*
 * pwm.h
 *
 * Created: 4/13/2013 ☢ 19:51:27 ☢
 *  Author: Chris
 */ 


#ifndef PWM_H_
#define PWM_H_

#define SCALE 100


void setup_timers();	//setup timers

void setpwm(char duty, char channel);	/*sets duty cycle (0-100) of selected pwm channel (0-3) 
										other channel numbers will affect all 4*/

#endif /* PWM_H_ */