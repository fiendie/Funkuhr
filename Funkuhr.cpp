/**
 * Funkuhr.cpp - Library for interacting with DCF77 radio clock modules.
 * 
 * Based on the Arduino DCF77 decoder v0.2 by Mathias Dalheimer (md@gonium.net).
 * Adapted by Andreas Tacke (at@mail.fiendie.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Funkuhr.h"


#define DCF77PIN 2                     // Input pin for the DCF receiver
#define BLINKPIN 13                    // LED indicator output

#define DCF_split_millis 140           // Number of milliseconds before we assume a logic 1
#define DCF_sync_millis 1200           // No signal at second 59

/* *
 * Definitions for the timer interrupt 2 handler
 * The Arduino runs at 16 Mhz, we use a prescaler of 64 -> We need to 
 * initialize the counter with 6. This way, we have 1000 interrupts per second.
 * We use tick_counter to count the interrupts.
 */
#define INIT_TIMER_COUNT 6
#define RESET_TIMER2 TCNT2 = INIT_TIMER_COUNT
int tick_counter = 0;


// DCF time format struct 
struct DCF77Buffer 
{
	unsigned long long prefix	:21;
	unsigned long long Min		:7;	// minutes
	unsigned long long P1		:1;	// parity minutes
	unsigned long long Hour		:6;	// hours
	unsigned long long P2		:1;	// parity hours
	unsigned long long Day		:6;	// day
	unsigned long long Weekday	:3;	// day of week
	unsigned long long Month	:5;	// month
	unsigned long long Year		:8;	// year (5 -> 2005)
	unsigned long long P3		:1;	// parity
};


// Parity struct 
struct 
{
	unsigned char parity_flag	:1;
	unsigned char parity_min	:1;
	unsigned char parity_hour	:1;
	unsigned char parity_date	:1;
} flags;


// Clock variables 
volatile unsigned char DCFSignalState = 0;  
unsigned char previousSignalState;
int previousFlankTime;
int bufferPosition;
unsigned long long dcf_rx_buffer;

// Time variables
volatile unsigned char ss;
volatile unsigned char mm;
volatile unsigned char hh;
volatile unsigned char day;
volatile unsigned char mon;
volatile unsigned char year;

unsigned char previousSecond;


// Interrupt handler for INT0. Called when the signal on Pin 2 changes. 
void int0handler() 
{		
	// Inverted because the signal is fed through a transistor 
	DCFSignalState = !digitalRead(DCF77PIN);
}


// Initialize the variables and configure the interrupt behaviour.
void Funkuhr::init() 
{
	previousSignalState = 0;
	previousFlankTime = 0;
	bufferPosition = 0;
	dcf_rx_buffer = 0;
	ss = mm = hh =day = mon = year = 0;
	
	pinMode(DCF77PIN, INPUT);

	/**
	 * Timer2 Settings: Timer Prescaler /64,
	 */
	
	// Turn on CS22 bit 
	TCCR2B |= (1<<CS22);					
		
	// Turn off CS21 and CS20 bits   
	TCCR2B &= ~((1<<CS21) | (1<<CS20));	

	// Turn off WGM21 and WGM20 bits 
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20)); 
	
	// Turn off WGM22
	TCCR2B &= ~(1<<WGM22); 

	// Use internal clock
	ASSR |= (0<<AS2);
	
	// Timer2 Overflow Interrupt Enable  
	TIMSK2 |= (1<<TOIE2) | (0<<OCIE2A);
	RESET_TIMER2;
	
	attachInterrupt(0, int0handler, CHANGE);
}

// Constructor
Funkuhr::Funkuhr() 
{
	
}


/**
 * Evaluates the information stored in the buffer. This is where the DCF77
 * signal is decoded and the internal clock is updated.
 */
void finalizeBuffer(void) 
{
	if (bufferPosition == 59) 
	{
		struct DCF77Buffer *rx_buffer;
		rx_buffer = (struct DCF77Buffer *)(unsigned long long)&dcf_rx_buffer;

		if (flags.parity_min == rx_buffer->P1 && 
			 flags.parity_hour == rx_buffer->P2 && 
			 flags.parity_date == rx_buffer->P3) 
		{ 
			// Convert the received bits from BCD to decimal
			mm = rx_buffer->Min-((rx_buffer->Min/16)*6);
			hh = rx_buffer->Hour-((rx_buffer->Hour/16)*6);
			day = rx_buffer->Day-((rx_buffer->Day/16)*6); 
			mon = rx_buffer->Month-((rx_buffer->Month/16)*6);
			year = 2000 + rx_buffer->Year-((rx_buffer->Year/16)*6);
		}
	} 

	ss = 0;
	bufferPosition = 0;
	dcf_rx_buffer=0;
}


/* *
 * Append a signal to the dcf_rx_buffer. Argument can be 1 or 0. An internal
 * counter shifts the writing position within the buffer. If position > 59,
 * a new minute begins -> time to call finalizeBuffer(). 
 */
void appendSignal(unsigned char signal) 
{
	dcf_rx_buffer = dcf_rx_buffer | ((unsigned long long) signal << bufferPosition);

	// Update the parity bits. First: Reset when minute, hour or date starts.
	if (bufferPosition ==  21 || bufferPosition ==  29 || bufferPosition ==  36) 
	{
		flags.parity_flag = 0;
	}
	
	// Save the parity when the corresponding segment ends
	if (bufferPosition ==  28) { flags.parity_min  = flags.parity_flag; };
	if (bufferPosition ==  35) { flags.parity_hour = flags.parity_flag; };
	if (bufferPosition ==  58) { flags.parity_date = flags.parity_flag; };

	// When we received a 1, toggle the parity flag
	if (signal == 1) 
	{
		flags.parity_flag = flags.parity_flag ^ 1;
	}
	
	bufferPosition++;
	
	if (bufferPosition > 59) 
		finalizeBuffer();
}


// Evaluates the signal as it is received. 
void scanSignal(void)
{ 
	if (DCFSignalState == 1) 
	{
		int thisFlankTime = millis();

		if (thisFlankTime - previousFlankTime > DCF_sync_millis) 
		{
			finalizeBuffer();
		}

		else if (thisFlankTime - previousFlankTime < 300) 
		{
			bufferPosition--;

			if (bufferPosition < 0)
				bufferPosition = 0;
		}

		if (thisFlankTime - previousFlankTime > 300)	
			previousFlankTime = thisFlankTime;
	} 

	else 
	{
		int difference = millis() - previousFlankTime;

		if (difference < DCF_split_millis) 
		{
			appendSignal(0);
		}
		 
		else 
		{
			appendSignal(1);
		}
	}
}


// The interrupt routine for counting seconds - increment hh:mm:ss.
ISR(TIMER2_OVF_vect) {
	RESET_TIMER2;
	tick_counter += 1;

	if (tick_counter == 1000) 
	{
		ss++;

		if (ss==60) 
		{
			ss=0;
			mm++;

			if (mm==60) 
			{
				mm=0;
				hh++;

				if (hh==24)
				{
					hh=0;
				} 

			}
		}

		tick_counter = 0;
	}
};
	

void Funkuhr::getTime(Dcf77Time& dt) 
{
	if (ss != previousSecond) {	

		dt.sec = ss;
		
		if(dt.min != mm)
			dt.min = mm;
			
		if(dt.hour != hh)
			dt.hour = hh;
		
		if(dt.day != day)	
			dt.day = day;
		
		if(dt.month != mon) 	
			dt.month = mon;
			
		if(dt.year != year)
			dt.year = year;
		
		previousSecond = ss;
	}
	
	if (DCFSignalState != previousSignalState) {
		scanSignal();

		if (DCFSignalState) {
			digitalWrite(BLINKPIN, HIGH);
		} 
		else {
			digitalWrite(BLINKPIN, LOW);
		}
		
		previousSignalState = DCFSignalState;
	}
}
