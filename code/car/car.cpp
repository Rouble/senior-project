/* senior_projet.cpp
 *
 * Created: 3/10/2013 ☢ 16:43:55 ☢
 *  Author: Chris
 */ 
#define F_CPU 8000000UL //define cpu frequency as 8MHz for the _delay_() macro

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "xcvr.h"
#include "pwm.h"
#include "movement.h"


#define rightstick 4		//byte in input array containing right analog stick value
#define leftstick 6			//byte in input array containing left analog stick value

void pinsetup();					//configures I/O pins
void unused_modules();				//turns off unused modules

static int potatoes[] = {0x706f, 0x7461, 0x746f, 0x6573}; //the sync key is potatoes. because potatoes.
int main(void)
{
	char remote_input[63] = {0};
	char stop = 0;
	
	CLKPR = 0x80;	//prepare clock prescaler for change
	CLKPR = 1;		//divide clock by 2

	unused_modules();	//disable stuff that isnt being used
	setup_timers();
	pinsetup();

	XCVR XC = XCVR(10, potatoes, RX_EN); //initialize the transceiver


	while(1)
	{
		if (XC.rx(remote_input))
		{
			set_right(remote_input[rightstick]);
			set_left(remote_input[leftstick]);
			set_flippers(remote_input[1]);
		}
		else
		{
			if (stop < 20) stop++;
		}
		
		if (stop > 20)
		{
			stahp();
		}
	}
}



void unused_modules()
{
	PRR0 = ((1<<PRTWI)|(1<<PRTIM0)|(1<<PRADC)); //disable 2wire interface / ADC
	PRR1 = ((1<<PRUSART1)/*|(1>>PRUSB)*/);			//disable USB and usart1 

	MCUCR |= (1<<JTD);		//must be written twice in 4 clocks to disable jtag interface on port F
	MCUCR |= (1<<JTD);
}


void pinsetup()
{
	DDRB = (1<<DDB4)|(1<<DDB5)|(1<<DDB6);
	DDRC = (1<<DDC6)|(1<<DDC7);
	DDRD = (1<<DDD4)|(1<<DDD6)|(1<<DDD7);
}
