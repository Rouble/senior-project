/* a2d.h
 *
 * Created: 3/10/2013 ☢ 19:23:59 ☢
 *  Author: Chris
 */ 


#ifndef a2d_H_
#define a2d_H_

void ADCon();
void ADCoff();
char readleftvert();
char readlefthoriz();
char readrightvert();
char readrighthoriz();

char readval(char channel);
char analogRead(uint8_t pin);


#endif /* a2d_H_ */