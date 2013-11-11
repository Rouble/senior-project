/* xcvr.h
 *
 * Created: 3/11/2013 ☢ 11:58:56 ☢
 *  Author: Chris
 */ 

#ifndef XCVR_H_
#define XCVR_H_

#define F_CPU 8000000UL //define cpu frequency as 8MHz for the _delay_() macro

#include <avr/io.h>
#include <util/delay.h>

#define SS_LO		(PORTB &= ~(1<<PORTB0))
#define SS_HI		(PORTB |= (1<<PORTB0))
#define FIFO 50
#define STAT 48
#define TX_EN 0x0100	//sets the tx enable bit for reg 7
#define RX_EN 0x0080	//sets the rx enable bit for reg 7

class XCVR 
{
	public:
	XCVR( char channel, int sync_word[4], int direction ); //sets up the xcvr for use
	void write( char reg, char data );	//writes one byte to specified register 
	void write( char reg, int data );	//writes two bytes to the specified register
	char read( char reg );	//reads a single byte from the xcvr
	int read2( char reg );	//reads 2 bytes from xcvr
	char check();			//checks the pkt flag to see if anything was recieved
	void tx( char packet[], char len ); //sends an array of data
	char rx( char packet[]);	// times out after 100mS; returns 0 on timeout, -1 on success
	
	private:
	char _channel;	//keep channel in private memory for future use
	
	void startSPI();		//starts the SPI module
	char SPI(char data);	//sends/receives data over SPI

	
};

#endif /* XCVR_H_ */