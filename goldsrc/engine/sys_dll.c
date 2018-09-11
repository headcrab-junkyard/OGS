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

#ifdef _WIN32
qboolean WinNT;

static double pfreq;
static double curtime;
static double lastcurtime;
static int lowshift;
#endif

int starttime;

qboolean isDedicated;

void MaskExceptions();
void Sys_InitFloatTime();
void Sys_PushFPCW_SetHigh();
void Sys_PopFPCW();

#ifdef _WIN32
volatile int sys_checksum;
#endif

/*
================
Sys_PageIn
================
*/
#ifdef _WIN32
void Sys_PageIn(void *ptr, int size)
{
	byte *x;
	int j, m, n;

	// touch all the memory to make sure it's there. The 16-page skip is to
	// keep Win 95 from thinking we're trying to page ourselves in (we are
	// doing that, of course, but there's no reason we shouldn't)
	x = (byte *)ptr;

	for(n = 0; n < 4; n++)
	{
		for(m = 0; m < (size - 16 * 0x1000); m += 4)
		{
			sys_checksum += *(int *)&x[m];
			sys_checksum += *(int *)&x[m + 16 * 0x1000];
		};
	};
};
#endif

void Sys_InitAuthentication(){
	// TODO
};

void Sys_ShutdownAuthentication(){
	// TODO
};

void Sys_InitLauncherInterface(){
	// TODO
};

void Sys_ShutdownLauncherInterface(){
	// TODO
};

void Sys_InitMemory(){
	// TODO
};

void Sys_ShutdownMemory(){
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

	if(j)
		curtime = (double)(Q_atof(com_argv[j + 1]));
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
	LARGE_INTEGER PerformanceFreq;
	unsigned int lowpart, highpart;
	OSVERSIONINFO vinfo;

// TODO
/*
#ifndef SWDS
	// allocate a named semaphore on the client so the
	// front end can tell if it is alive

	// mutex will fail if semephore allready exists
    qwclsemaphore = CreateMutex(
        NULL,         // Security attributes
        0,            // owner
        "qwcl"); // Semaphore name
	if (!qwclsemaphore)
		Sys_Error ("QWCL is already running on this system");
	CloseHandle (qwclsemaphore);

    qwclsemaphore = CreateSemaphore(
        NULL,         // Security attributes
        0,            // Initial count
        1,            // Maximum count
        "qwcl"); // Semaphore name
#endif
*/

	MaskExceptions();
	Sys_SetFPCW();

	if(!QueryPerformanceFrequency(&PerformanceFreq))
		Sys_Error("No hardware timer available");

	// get 32 out of the 64 time bits such that we have around
	// 1 microsecond resolution
	lowpart = (unsigned int)PerformanceFreq.LowPart;
	highpart = (unsigned int)PerformanceFreq.HighPart;
	lowshift = 0;

	while(highpart || (lowpart > 2000000.0))
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / (double)lowpart;

	Sys_InitFloatTime();

	vinfo.dwOSVersionInfoSize = sizeof(vinfo);

	if(!GetVersionEx(&vinfo))
		Sys_Error("Couldn't get OS info");

	if((vinfo.dwMajorVersion < 4) ||
	   (vinfo.dwPlatformId == VER_PLATFORM_WIN32s))
	{
		Sys_Error("WinQuake requires at least Win95 or NT 4.0");
	}

	if(vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		WinNT = true;
	else
		WinNT = false;
#else // if linux or sun or something else
#if id386
	Sys_SetFPCW();
#endif
#endif
};

void Sys_Shutdown(){
	// TODO
};

void Sys_InitArgv(){
	// TODO
};

void Sys_ShutdownArgv(){
	// TODO
};

void Sys_Printf(const char *fmt, ...)
{
#ifdef _WIN32
	va_list argptr;
	char text[1024];
	DWORD dummy;

//
	if(isDedicated)
	{
		va_start(argptr, fmt);
		vsprintf(text, fmt, argptr);
		va_end(argptr);

		//WriteFile(houtput, text, strlen (text), &dummy, NULL); // TODO: IDedicatedExports->Printf
	};
// QW
	//va_start(argptr, fmt);
	//vprintf(fmt, argptr);
	//va_end(argptr);
//
#elif __linux__
	va_list argptr;
	char text[1024];
	unsigned char *p;

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	if(strlen(text) > sizeof(text))
		Sys_Error("memory overwrite in Sys_Printf");

	if(nostdout)
		return;

	for(p = (unsigned char *)text; *p; p++)
	{
		*p &= 0x7f;
		if((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	};
#elif sun
	va_list argptr;

	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);
#endif // _WIN32
};

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{
#ifdef _WIN32
	DWORD flOldProtect;

	// TODO: copy on write or just read-write?
	if(!VirtualProtect((LPVOID)startaddr, length, PAGE_READWRITE, &flOldProtect))
		Sys_Error("Protection change failed\n");
#else
	int r;
	unsigned long addr;
	int psize = getpagesize();

	addr = (startaddr & ~(psize - 1)) - psize;

	//	fprintf(stderr, "writable code %lx(%lx)-%lx, length=%lx\n", startaddr,
	//			addr, startaddr+length, length);

	r = mprotect((char *)addr, length + startaddr - addr + psize, 7);

	if(r < 0)
		Sys_Error("Protection change failed\n");
#endif
};

//#ifndef _M_IX86 // TODO: check for non-Intel?

void Sys_SetFPCW()
{
};

void Sys_PushFPCW_SetHigh()
{
};

void Sys_PopFPCW()
{
};

void MaskExceptions()
{
};

//#endif // _M_IX86

void Sys_Error(const char *error, ...)
{
#ifdef _WIN32
	va_list argptr;
	char text[1024], text2[1024];
	const char *text3 = "Press Enter to exit\n";
	const char *text4 = "***********************************\n";
	const char *text5 = "\n";
	DWORD dummy;
	double starttime;
	static int in_sys_error0 = 0;
	static int in_sys_error1 = 0;
	static int in_sys_error2 = 0;
	static int in_sys_error3 = 0;

	if(!in_sys_error3)
	{
		in_sys_error3 = 1;
		//VID_ForceUnlockedAndReturnState(); // TODO
	};

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	if(isDedicated)
	{
		va_start(argptr, error);
		vsprintf(text, error, argptr);
		va_end(argptr);

		sprintf(text2, "ERROR: %s\n", text);
		//WriteFile(houtput, text5, strlen(text5), &dummy, NULL); // TODO
		//WriteFile(houtput, text4, strlen(text4), &dummy, NULL); // TODO
		//WriteFile(houtput, text2, strlen(text2), &dummy, NULL); // TODO
		//WriteFile(houtput, text3, strlen(text3), &dummy, NULL); // TODO
		//WriteFile(houtput, text4, strlen(text4), &dummy, NULL); // TODO

		starttime = Sys_FloatTime();
		sc_return_on_enter = true; // so Enter will get us out of here

		while(!Sys_ConsoleInput() && ((Sys_FloatTime() - starttime) < CONSOLE_ERROR_TIMEOUT))
		{
		};
	}
	else
	{
		// switch to windowed so the message box is visible, unless we already
		// tried that and failed
		if(!in_sys_error0)
		{
			in_sys_error0 = 1;
			//VID_SetDefaultMode(); // TODO
			MessageBox(NULL, text, "Engine Error", MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
		}
		else
			MessageBox(NULL, text, "Double Engine Error", MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
	};

	if(!in_sys_error1)
	{
		in_sys_error1 = 1;
		Host_Shutdown();
	};

	// shut down QHOST hooks if necessary
	if(!in_sys_error2)
	{
		in_sys_error2 = 1;
		//DeinitConProc(); // TODO
	};

	exit(1);
#elif __linux__
	va_list argptr;
	char string[1024];

	// change stdin to non blocking
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~FNDELAY);

	va_start(argptr, error);
	vsprintf(string, error, argptr);
	va_end(argptr);
	fprintf(stderr, "Error: %s\n", string);

	Host_Shutdown();
	exit(1);
#elif sun
	va_list argptr;

	printf("Sys_Error: ");
	va_start(argptr, error);
	vprintf(error, argptr);
	va_end(argptr);
	printf("\n");
	Host_Shutdown();
	exit(1);
#endif
};

void Sys_Quit()
{
#ifdef _WIN32
	VID_ForceUnlockedAndReturnState();

	Host_Shutdown();

#ifndef SWDS
	if(tevent)
		CloseHandle(tevent);

	//if(qwclsemaphore) // TODO
		//CloseHandle(qwclsemaphore); // TODO
#endif

	exit(0);
#elif __linux__
	Host_Shutdown();
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~FNDELAY);

	fflush(stdout);
	exit(0);
#elif sun
	Host_Shutdown();
	exit(0);
#endif
};

/*
================
Sys_FloatTime
================
*/
double Sys_FloatTime()
{
#ifdef _WIN32
	static int sametimecount;
	static unsigned int oldtime;
	static int first = 1;
	LARGE_INTEGER PerformanceCount;
	unsigned int temp, t2;
	double time;

	Sys_PushFPCW_SetHigh();

	QueryPerformanceCounter(&PerformanceCount);

	temp = ((unsigned int)PerformanceCount.LowPart >> lowshift) |
	((unsigned int)PerformanceCount.HighPart << (32 - lowshift));

	if(first)
	{
		oldtime = temp;
		first = 0;
	}
	else
	{
		// check for turnover or backward time
		if((temp <= oldtime) && ((oldtime - temp) < 0x10000000))
		{
			oldtime = temp; // so we can't get stuck
		}
		else
		{
			t2 = temp - oldtime;

			time = (double)t2 * pfreq;
			oldtime = temp;

			curtime += time;

			if(curtime == lastcurtime)
			{
				sametimecount++;

				if(sametimecount > 100000)
				{
					curtime += 1.0;
					sametimecount = 0;
				};
			}
			else
				sametimecount = 0;

			lastcurtime = curtime;
		};
	};

	Sys_PopFPCW();

	return curtime;
	
	// TODO: QW version
	/*
	static DWORD starttime;
	static qboolean first = true;
	DWORD now;
	double t;

	now = timeGetTime();

	if (first) {
		first = false;
		starttime = now;
		return 0.0;
	}
	
	if (now < starttime) // wrapped?
		return (now / 1000.0) + (LONG_MAX - starttime / 1000.0);

	if (now - starttime == 0)
		return 0.0;

	return (now - starttime) / 1000.0;
	*/
#else
	struct timeval tp;
	struct timezone tzp;
	static int secbase;

	gettimeofday(&tp, &tzp);

	if(!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec / 1000000.0;
	}

	return (tp.tv_sec - secbase) + tp.tv_usec / 1000000.0;
#endif
};

void Sys_Sleep()
{
#ifdef _WIN32
	Sleep(1); // TODO: nothing in QW
#ifdef __linux__
	usleep(1 * 1000);
#elif sun
	// Nothing?
#endif
};

#ifdef _WIN32
void Sys_SendKeyEvents()
{
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		// we always update if there are any event, even if we're paused
		scr_skipupdate = 0;

		if(!GetMessage(&msg, NULL, 0, 0))
			Sys_Quit();

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};
};
#endif // _WIN32