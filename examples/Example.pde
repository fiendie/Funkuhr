/**
 * Example sketch for the Funkuhr Arduino library
 */
#include "Funkuhr.h"

Funkuhr dcf;
struct Dcf77Time dt = { 0 };

uint8_t curSec;


void dumpTime(void)
{	
	Serial.println("DCF77 Time");

	// Print date
	Serial.print(" ");

	if(dt.day < 10)
		Serial.print("0");
	Serial.print(dt.day, DEC);
	Serial.print(".");
	
	if(dt.month < 10)
		Serial.print("0");
	Serial.print(dt.month, DEC);
	Serial.print(".");
	
	if(dt.year == 0)
	{
		Serial.print("000")
	}
	else
	{
		Serial.print("20")
	}	

	Serial.print(dt.year, DEC);
	
	if(dcf.synced()) 
	{
		Serial.println(" ");
		Serial.print(" ");
	}
	else 
	{
		Serial.println(" ");
		Serial.print("~");
	}
	
	// Print Time 
	if (dt.hour < 10) 
		Serial.print("0");
	
	Serial.print(dt.hour, DEC);
	Serial.print(":");
	
	if (dt.min < 10) 
		Serial.print("0");
	
	Serial.print(dt.min, DEC);
	Serial.print(":");
	
	if (dt.sec < 10) 
		Serial.print("0");

	Serial.println(dt.sec, DEC);
	
	Serial.println(" ");
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
