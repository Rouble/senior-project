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
#include "print.h"
#include "usb_mouse_debug.h"

#define dpad 2

void pinsetup();					//configures I/O pins
void unused_modules();				//turn off unused modules

static int potatoes[] = {0x706f, 0x7461, 0x746f, 0x6573}; //the sync key is potatoes. because potatoes.

int main(void)
{					   
	char remote_input[63] = {0};
		
	CLKPR = 0x80;	//prepare clock prescaler for change
	CLKPR = 1;		//divide clock by 2
	
	XCVR XC = XCVR(10, potatoes, RX_EN); //initialize the transceiver
	
	unused_modules();	//disable stuff that isnt being used

	usb_init();
	while(!usb_configured());
	_delay_ms(1000);


    while(1)
    {
		if (XC.rx(remote_input))
		{
			usb_gamepad_update(remote_input);
		}
    }
}


void unused_modules()
{
	PRR0 = ((1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRADC)); //disable 2wire interface/ timer 0/ timer 1 / ADC
	PRR1 = ((1<<PRTIM4)|(1<<PRTIM3)|(1<<PRUSART1));			//disable timer 4 / timer 3/ and usart1
	
	MCUCR |= (1<<JTD);		//must be written twice in 4 clocks to disable jtag interface on port F
	MCUCR |= (1<<JTD);
}
