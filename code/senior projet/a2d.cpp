/* a2d.cpp
 *
 * Created: 3/10/2013 ☢ 19:24:20 ☢
 *  Author: Chris
 */ 
#define F_CPU 8000000UL //define cpu frequency as 8MHz for the _delay_() macro

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "a2d.h"

void ADCon()
{
	ADCSRA = (1<<ADEN); //enables the ADC
}

void ADCoff()
{
	ADCSRA &= 0x7F;		//disable ADC to save power
}

char readval(char channel)
{
	char result = 0;
	
	ADMUX = 0x60 | channel;			//set ADC channel to read
	ADCSRB = 0;
	SMCR = (1<<SM0)|(1<<SE);			//set the sleep mode to adc noise reduction
	ADCSRA |= (1<<ADSC)|(1<<ADIE);		//Start conversion prescaler = 1/2
	asm volatile("sleep"::);			//put the mcu to sleep till woken by adc interrupt
	
	result = ADCH;
	return result;								//return lower ADC result byte
}


// wrappers to clarify what pin is being read
char readleftvert()
{
	return readval(0x00);
}

char readlefthoriz()
{
	return readval(0x01);
}

char readrightvert()
{
	return readval(0x04);
}

char readrighthoriz()
{
	return readval(0x05);
}


ISR(ADC_vect)
{
	SMCR = 0;	
}