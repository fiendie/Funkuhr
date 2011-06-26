/**
 * Example sketch for the Funkuhr Arduino library
 */
#include "Funkuhr.h"

Funkuhr dcf;
Dcf77Time dt;

unsigned char curSec;


void dumpTime(void)
{	
	Serial.println("DCF77 Time");
	Serial.print("  ");

	if (dt.day < 10) 
		Serial.print(" ");	

	// Print date
	if(dt.day < 10)
		Serial.print("0");
	Serial.print(dt.day, DEC);
	Serial.print(".");
	
	if(dt.month < 10)
		Serial.print("0");
	Serial.print(dt.month, DEC);
	Serial.print(".");
	
	if(year == 0)
	{
		Serial.print("000")
	}
	else
	{
		Serial.print("20")
	}	

	Serial.print(dt.year, DEC);
	Serial.print(" ");
	
	// Print Time with flashing separator		
	if (dt.hour < 10) 
		Serial.print("0");
	
	Serial.print(dt.hour, DEC);

	if ((dt.sec % 2) == 0) 
		Serial.print(":");
	} 
	else 
	{
		Serial.print(" ");
	}
	
	if (dt.min < 10) 
		Serial.print("0");
	
	Serial.print(dt.min, DEC);
	
	if ((dt.sec % 2) == 0) 
	{
		Serial.print(":");
	}
	else 
	{
		Serial.print(" ");
	}
	
	if (dt.sec < 10) 
		Serial.print("0");

	Serial.println(dt.sec, DEC);
}


void setup(void) 
{
	Serial.begin(9600);
	lcd.begin(20, 4);
	dcf.init();
}


void loop(void) 
{
	dcf.getTime(dt);
	
	if(dt.sec != curSec)
	{
		dumpTime();
	}
	
	curSec = dt.sec;
}
