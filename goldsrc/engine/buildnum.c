/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"

//const char *date = "Oct 9 2016";
//const char *time = "13:22:52";
static char *date = __DATE__;
//const char *curtime = __TIME__;

static const char *mon[12] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char mond[12] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// returns days since Sep 09 2018 (first build)
int build_number()
{
	int m = 0;
	int d = 0;
	int y = 0;
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
		b += 1;

	//b -= 34995; // Oct 24 1996
	b -= 42985; // Sep 9 2018

	return b;
};