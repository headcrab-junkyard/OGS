/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2018 Headcrab-Garage

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

#include "quakedef.h"

#ifdef _WIN32
qboolean WinNT;

static double pfreq;
static double curtime = 0.0;
static double lastcurtime = 0.0;
static int lowshift;
#endif

void Sys_InitAuthentication()
{
	// TODO
};

void Sys_ShutdownAuthentication()
{
	// TODO
};

void Sys_InitLauncherInterface()
{
	// TODO
};

void Sys_ShutdownLauncherInterface()
{
	// TODO
};

void Sys_InitMemory()
{
	// TODO
};

void Sys_ShutdownMemory()
{
	// TODO
};

/*
================
Sys_InitFloatTime
================
*/
#ifdef _WIN32
void Sys_InitFloatTime()
{
	int j;

	Sys_FloatTime();

	j = COM_CheckParm("-starttime");

	if (j)
		curtime = (double)(Q_atof(com_argv[j+1]));
	else
		curtime = 0.0;

	lastcurtime = curtime;
};
#endif // _WIN32

/*
================
Sys_Init
================
*/
void Sys_Init()
{
#ifdef _WIN32
	LARGE_INTEGER	PerformanceFreq;
	unsigned int	lowpart, highpart;
	OSVERSIONINFO	vinfo;

	MaskExceptions ();
	Sys_SetFPCW ();

	if (!QueryPerformanceFrequency (&PerformanceFreq))
		Sys_Error ("No hardware timer available");

// get 32 out of the 64 time bits such that we have around
// 1 microsecond resolution
	lowpart = (unsigned int)PerformanceFreq.LowPart;
	highpart = (unsigned int)PerformanceFreq.HighPart;
	lowshift = 0;

	while (highpart || (lowpart > 2000000.0))
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / (double)lowpart;

	Sys_InitFloatTime ();

	vinfo.dwOSVersionInfoSize = sizeof(vinfo);

	if (!GetVersionEx (&vinfo))
		Sys_Error ("Couldn't get OS info");

	if ((vinfo.dwMajorVersion < 4) ||
		(vinfo.dwPlatformId == VER_PLATFORM_WIN32s))
	{
		Sys_Error ("WinQuake requires at least Win95 or NT 4.0");
	}

	if (vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		WinNT = true;
	else
		WinNT = false;
#else // if linux
	#if id386
		Sys_SetFPCW();
	#endif
#endif
};

void Sys_Shutdown()
{
	// TODO
};

void Sys_InitArgv()
{
	// TODO
};

void Sys_ShutdownArgv()
{
	// TODO
};

void Sys_Printf(const char *fmt, ...)
{
#ifdef _WIN32
	va_list		argptr;
	char		text[1024];
	DWORD		dummy;
	
	if (isDedicated)
	{
		va_start (argptr,fmt);
		vsprintf (text, fmt, argptr);
		va_end (argptr);

		//WriteFile(houtput, text, strlen (text), &dummy, NULL); // TODO: IDedicatedExports->Printf
	};
#else // if linux
	va_list		argptr;
	char		text[1024];
	unsigned char		*p;

	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);

	if (strlen(text) > sizeof(text))
		Sys_Error("memory overwrite in Sys_Printf");

    if (nostdout)
        return;

	for (p = (unsigned char *)text; *p; p++)
	{
		*p &= 0x7f;
		if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	};
#endif // _WIN32
};