/* senior_projet.cpp
 *
 * Created: 3/10/2013 ☢ 16:43:55 ☢
 *  Author: Chris
 */ 
#define F_CPU 8000000UL //define cpu frequency as 8MHz for the _delay_() macro
		
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "a2d.h"
#include "xcvr.h"
#include "usb_mouse_debug.h"
#include "print.h"

#define _left	(PIND & (1<<PIND6))		//pin status checks for readability
#define _right	(PIND & (1<<PIND5))
#define _up		(PIND & (1<<PIND3))
#define _down	(PIND & (1<<PIND4))
#define _tri	(PINC & (1<<PINC7))
#define _ex		(PINF & (1<<PINF7))
#define _squ	(PINF & (1<<PINF6))
#define _cir	(PINE & (1<<PINE2))
#define _L1		(PIND & (1<<PIND2))
#define _L2		(PIND & (1<<PIND1))
#define _L3		(PINB & (1<<PINB7))
#define _R1		(PINC & (1<<PINC6))
#define _R2		(PINB & (1<<PINB6))
#define _R3		(PIND & (1<<PIND0))
#define _start	(PINB & (1<<PINB5))
#define _select	(PINB & (1<<PINB4))

#define packlen 0			//offsets for input array readability
#define buttons1 1
#define buttons2 2
#define dpad 2
#define rightVanalog 3
#define rightHanalog 4
#define leftVanalog 5
#define leftHanalog 6


void pinsetup();					//configures I/O pins
void update_inputs(char input[]);	//populates the remote_input array with all input data from remote
void unused_modules();				//turn off unused modules
void start_usb();


static int potatoes[] = {0x706f, 0x7461, 0x746f, 0x6573}; //the sync key is potatoes. because potatoes.

int xcheck = 0;


int main(void)
{					   
	char remote_input[7] = {0};
	//←		→	↓	↑	△	×	□	○	L1	L2	L3	R1	R2	R3	select	start
	//PD6	PD5	PD4	PD3	PC7	PF7	PF6	PE2	PD2	PD1	PB7	PC6	PB6	PD0	PB4		PB5

	CLKPR = 0x80;	//prepare clock prescaler for change
	CLKPR = 1;		//divide clock by 2
		
	XCVR XC = XCVR(10, potatoes, TX_EN);

	pinsetup();			//setup pins 
	ADCon();			//start the ADC module
	unused_modules();	//turn extra stuff off
 	start_usb();		//check for usb connection / enable usb

	
	while(1)
    {
		update_inputs(remote_input);	//update values for buttons/analogs

		if (usb_configured())			//usb? dont send wireless
		{
			usb_gamepad_update(remote_input); //if connected to a computer send gamepad data over usb instead of transmitter
		}        

		XC.tx(remote_input, 7);		

		_delay_ms(50);
    }
}


void pinsetup()
{	
	//disable digital input functionality of adc pins 5,4,1,0 (reduces power consumption)
	DIDR0 =	0x33;	
	
	//enable internal pullups for 
	PORTB |= (1<<PORTB4)|(1<<PORTB5)|(1<<PORTB6)|(1<<PORTB7);
	PORTC |= (1<<PORTC6)|(1<<PORTC7);
	PORTD = 0x7F;
	PORTE |= (1<<PORTE2);
	PORTF |= (1<<PORTF6)|(1<<PORTF7);
			
	DDRE |= (1<<DDE6);		//charge mode pin = output
}


void update_inputs(char input[])								
{											
	input[packlen] = 6;		//packet length

	input[buttons1] = 0;	//clear button data
	input[dpad] = 0;
	
	//if button pressed set bit in array
	if (!_tri) input[buttons1] |= 0x01;
	if (!_cir) input[buttons1] |= 0x02;
	if (!_ex) input[buttons1] |= 0x04;
	if (!_squ) input[buttons1] |= 0x08;
	if (!_L2) input[buttons1] |= 0x10;
	if (!_R2) input[buttons1] |= 0x20;
	if (!_L1) input[buttons1] |= 0x40;
	if (!_R1) input[buttons1] |= 0x80;
	
	if (!_select) input[buttons2] |= 0x10;
	if (!_start) input[buttons2] |= 0x20;
	if (!_L3) input[buttons2] |= 0x40;
	if (!_R3) input[buttons2] |= 0x80;
	if (!_up) input[dpad] |= 0x01;
	if (!_right) input[dpad] |= 0x02;
	if (!_down) input[dpad] |= 0x04;
	if (!_left) input[dpad] |= 0x08;

	switch (input[dpad] & 0x0F)	//USB HID directional pad position data is weird
	{															//cardinal direction
		case 1: input[dpad] = (input[dpad] & 0xF0) | 0; break;	//N
		case 2: input[dpad] = (input[dpad] & 0xF0) | 2; break;	//E
		case 3: input[dpad] = (input[dpad] & 0xF0) | 1; break;	//NE
		case 4: input[dpad] = (input[dpad] & 0xF0) | 4; break;	//S
		case 6: input[dpad] = (input[dpad] & 0xF0) | 3; break;	//SE
		case 8: input[dpad] = (input[dpad] & 0xF0) | 6; break;	//W
		case 9: input[dpad] = (input[dpad] & 0xF0) | 7; break;	//NW
		case 12: input[dpad] = (input[dpad] & 0xF0) | 5; break;	//SW
		default: input[dpad] = (input[dpad] & 0xF0) | 15; break; //no direction
	}
// 	if (!usb_configured())		//this is to correct problems with wiring
// 	{
// 		input[rightVanalog] = readrightvert();
// 		input[leftHanalog] = ~readrighthoriz();
// 		input[leftVanalog] = readleftvert();				//read analog stick values
// 		input[rightHanalog] = readlefthoriz();
// 	}
// 	else						//this is correct for PC 
	{
		input[rightVanalog] = readrightvert();
		input[rightHanalog] = ~readrighthoriz();
		input[leftVanalog] = readleftvert();				//read analog stick values
		input[leftHanalog] = ~readlefthoriz();
	}	
}


void unused_modules()
{
	PRR0 = ((1<<PRTWI)|(1<<PRTIM0)|(1<<PRTIM1));		//disable 2wire interface/ timer 0/ timer 1
	PRR1 = ((1<<PRTIM4)|(1<<PRTIM3)|(1<<PRUSART1));		//disable timer 4 / timer 3/ and usart1

	MCUCR |= (1<<JTD);		//must be written twice in 4 clocks to disable jtag interface on port F
	MCUCR |= (1<<JTD);
}


void start_usb()
{
	USBCON = (1<<OTGPADE)|(1<<VBUSTE);	//enable the vbus pad/interrupt to check for usb connection
	
	_delay_ms(50);
	
	sei();
}
