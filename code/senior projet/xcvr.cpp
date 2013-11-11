/* xcvr.cpp
 *
 * Created: 3/11/2013 ☢ 11:59:30 ☢
 *  Author: Chris
 */ 


#include "xcvr.h"
#ifndef _car_
#include "print.h"
#endif


XCVR::XCVR(char channel, int sync_word[4], int direction)
{
	startSPI();				//config spi 

	_channel = channel;		//store channel
		
		//reset timings longer than those on page 5 of CYRF8935 datasheet
#ifdef _receiver_
	_delay_ms(250);		//mcu might be starting early, added large delay
	DDRB |= (1<<DDB7);
	PORTB &= ~(1<<PORTB7);
	_delay_us(40);
	DDRB &= ~(1<<DDB7);
	_delay_ms(4);
#else						//vehicle and remote use different pins to reset the xcvr
	_delay_ms(250);			
	DDRD |= (1<<DDD7);		//set pin to output
	PORTD &= ~(1<<PORTD7);	//pull xcvr reset low
	_delay_us(40);
	DDRD &= ~(1<<DDD7);		//tristate the pin
	_delay_ms(4);
#endif

	//all values are recommended values unless otherwise noted
	//registers with no comment are not described in data sheet
	write(27,(int) 0x4200);		//reg27 is sleep current
	write( 0,(int) 0x6FE1);
	write( 2,(int) 0x5517);
	write( 4,(int) 0x9CC9);
	write( 5,(int) 0x6647);
	write( 7,(int) ((_channel & 0x7F) | direction)); //sets frequency and data direction	
	write( 8,(int) 0x6C90);
	write( 9,(int) 0x1920);		//tx power level
	write(11,(int) 0x0008);		//RSSI enabled
	write(13,(int) 0x4880);	
	write(22,(int) 0x00FF);
	write(23,(int) 0x8007);
	write(24,(int) 0x0067);
	write(26,(int) 0x19E0);
	write(33,(int) 0x63F0);
	write(34,(int) 0x3000);
	write(35,(int) 0x0381);		//auto ACK reties =3 , bit 14 puts chip in sleep mode, bit 12 shuts off oscilator in sleep mode
	write(36, sync_word[0]);	//sync word - also described as a MAC address in the datasheet
	write(37, sync_word[1]);	
	write(38, sync_word[2]);
	write(39, sync_word[3]);
	write(40,(int) 0x2047);		//sets empty/full thresholds on FIFO and number acceptable error bits in sync word
	write(41,(int) 0xF800);		//CRC on, Scramble off, 1st byte packet length, auto ACK on
	write(42,(int) 0xFDFF);

}

char XCVR::check()
{
	return (char)(read2(STAT) & 0x0040);
}

void XCVR::tx(char packet[], char len)
{
	write(FIFO, len);
	
	for (char i = 0; i < len; i++) 
	{
		write(FIFO, packet[i]);			//write all data
	}
	
	write( 7,(int) ((_channel & 0x7F) | TX_EN) ); //TODO: if tx/rx doesnt work uncomment this and try again
}

char XCVR::rx(char packet[])
{
	int timeout = 0;
	char len = 0;
	write( 7,(int) ((_channel & 0x7F) | RX_EN) ); //TODO: if tx/rx doesnt work uncomment this and try again
	

	while ((timeout< 20000) && (!check()))	//count to 20k, time out if no data
	{
		timeout++;
	}

	if (check())	//data?
	{
		len = read(52) & 0x3F;				//read data length
		for (int i = 0; i < len; i++)		//read all data into buffer
		{
			packet[i] = read(FIFO);
		}
		return len;							//return non zero number for success
	}	
	else
	{
		return 0;							//failure (timeout)
	}
}


void XCVR::write( char reg , char data )
{
	SS_LO;
	SPI(reg);		//set register
	SPI(data);		//write data
	SS_HI;
}

void XCVR::write( char reg , int data )
{
	char d1 = (data >> 8) & 0xFF;	//break int into two bytes to write to spi register
	char d2 = data & 0xFF;
	
	SS_LO;
	SPI(reg);		//set register
	SPI(d1);		//write MSB
	SPI(d2);		//write LSB
	SS_HI;
}


char XCVR::read( char reg )
{
	char temp = 0;
	SS_LO;
	SPI(reg | 0x80);	//set register/direction
	temp = SPI(0);		//get spi data
	SS_HI;
	return temp;		//return spi data
}

int XCVR::read2( char reg )
{
	int temp = 0;
	SS_LO;
	SPI(reg | 0x80);		//set register/direction
    temp = SPI(0);			//read MSB
	temp <<= 8;				//shift over for lsb
	temp |= SPI(0);			//read LSB
	SS_HI;
	return temp;			//return 2 bytes of data
}

void XCVR::startSPI()
{
	DDRB |= (1<<DDB2)|(1<<DDB1)|(1<<DDB0);	//set mosi to output, sck to output, ss to output respectively
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPHA);	//enable spi, spi master mode, sample on falling sck edge
	SPSR = (1<<SPI2X);						//set spi to max speed (4Mbit/s)
}

char XCVR::SPI(char data)
{
	SPDR = data;				//write data to spi bus
	while(!(SPSR & (1<<SPIF))); //wait for transmission to finish
	return SPDR;				//spdr now has received data in it. return it
}
