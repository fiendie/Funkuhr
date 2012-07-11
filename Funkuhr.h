/**
 * Funkuhr.h - Library for interacting with DCF77 radio clock modules.
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

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#ifndef Funkuhr_h
#define Funkuhr_h


struct Dcf77Time {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
};


class Funkuhr {
	public:
  		Funkuhr(uint8_t intNumber = 0, uint8_t dcf77Pin = 2, uint8_t blinkPin = 13, bool invertedSignal = false);
		void init();
		void getTime(Dcf77Time& dt);
		uint8_t synced();
};

#endif
