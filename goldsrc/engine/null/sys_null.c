/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2018 Headcrab Garage

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/// @file
/// @brief null system driver to aid porting efforts

#include "quakedef.h"
#include "errno.h"

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}

/*
void Sys_DebugLog(char *file, char *fmt, ...)
{
}
*/

void Sys_Error (const char *error, ...)
{
	va_list         argptr;

	printf ("Sys_Error: ");
	va_start (argptr,error);
	vprintf (error,argptr);
	va_end (argptr);
	printf ("\n");

	exit (1);
}

void Sys_Printf (const char *fmt, ...)
{
	va_list         argptr;
	
	va_start (argptr,fmt);
	vprintf (fmt,argptr);
	va_end (argptr);
}

void Sys_Quit ()
{
	exit (0);
}

double Sys_FloatTime ()
{
	static double t;
	
	t += 0.1;
	
	return t;
}

char *Sys_ConsoleInput ()
{
	return NULL;
}

void Sys_Sleep ()
{
}

void Sys_SendKeyEvents ()
{
}

void Sys_HighFPPrecision ()
{
}

void Sys_LowFPPrecision ()
{
}