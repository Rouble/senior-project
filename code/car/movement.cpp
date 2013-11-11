/*
 * movement.cpp
 *
 * Created: 4/18/2013 ☢ 12:26:53 ☢
 *  Author: Chris
 */ 

#include "movement.h"

void stahp()
{
	setpwm(0, 6);	//stahp the pwm
	FFlipStp;		//stahp the flippers
	RFlipStp;
}

void set_right(char val)
{
	if (val < lower_deadzone)		//set pwm if input is greater than the deadzone value
	{
		setpwm(stopval - val, 0);
		setpwm(0, 1);
	}
	else if (val > upper_deadzone)	//set pwm if less than deadzone value
	{
		setpwm(0, 0);
		setpwm((val - stopval), 1);
	}
	else							//prevent twitching
	{
		setpwm(0, 0);
		setpwm(0, 1);
	}
}

void set_left(char val)
{
	if (val < lower_deadzone)
	{
		setpwm(stopval - val, 2);
		setpwm(0, 3);
	}
	else if (val > upper_deadzone)
	{
		setpwm(0, 2);
		setpwm((val - stopval), 3);
	}
	else
	{
		setpwm(0, 2);
		setpwm(0, 3);
	}
}

void set_flippers(char buttons)
{
	//stop the flippers before changes are made
	FFlipStp;
	RFlipStp;
	
	if (buttons & 0x80) FFlipFwd;
	if (buttons & 0x40) RFlipFwd;
	if (buttons & 0x20) FFlipRev;
	if (buttons & 0x10) RFlipRev;

}

