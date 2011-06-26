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

#include <WProgram.h>


#ifndef Funkuhr_h
#define Funkuhr_h

struct Dcf77Time
{
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned char year;
};


class Funkuhr
{
	public:
		void getTime(Dcf77Time& dt);
	private:
		void init();
};

#endif