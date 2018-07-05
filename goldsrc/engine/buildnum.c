/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"

// char *date = "Oct 9 2016";
// char *time = "13:22:52";
char *date = __DATE__;
const char *curtime = __TIME__;

const char *mon[12] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
char mond[12] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// returns days since Oct 9 2016
int build_number()
{
	int m = 0;
	int d = 0;
	int y = 0;
	int hr, min;
	static int b = 0;

	if(b != 0)
		return b;

	for(m = 0; m < 11; m++)
	{
		if(_strnicmp(&date[0], mon[m], 3) == 0)
			break;
		d += mond[m];
	}

	d += atoi(&date[4]) - 1;

	y = atoi(&date[7]) - 1900;

	b = d + (int)((y - 1) * 365.25);

	if(((y % 4) == 0) && m > 1)
	{
		b += 1;
	}

	// TODO
	b -= 34995; // Oct 24 1996
	//b -= 35741; // Nov 9 1998
	//b -= 42285; // Oct 9 2016
	//b -= 42300; // Oct 24 2016

	hr = (curtime[0] - '0') * 10 + (curtime[1] - '0');
	min = (curtime[3] - '0') * 10 + (curtime[4] - '0');
	//	sec = (curtime[6] - '0') * 10 + (curtime[7] - '0');

	b *= 60 * 24;
	b += hr * 60 + min;

	return b;
};