/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018-2019, 2021-2022, 2024 BlackPhrase
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

#ifdef _WIN32
#include <io.h>
#endif

#ifdef _WIN32
qboolean WinNT;

static double pfreq;
static double curtime;
static double lastcurtime;
static int lowshift;
#endif

int starttime;

qboolean isDedicated;

void Sys_InitFloatTime();

void MaskExceptions();
void Sys_SetFPCW();
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
	// touch all the memory to make sure it's there. The 16-page skip is to
	// keep Win 95 from thinking we're trying to page ourselves in (we are
	// doing that, of course, but there's no reason we shouldn't)
	byte *x = (byte *)ptr;

	for(int n = 0; n < 4; n++)
	{
		for(int m = 0; m < (size - 16 * 0x1000); m += 4)
		{
			sys_checksum += *(int *)&x[m];
			sys_checksum += *(int *)&x[m + 16 * 0x1000];
		};
	};
};
#endif

/*
================
Sys_InitFloatTime
================
*/
#ifdef _WIN32
void Sys_InitFloatTime()
{
	Sys_FloatTime();

	int j = COM_CheckParm("-starttime");

	if(j)
		curtime = (double)(Q_atof(com_argv[j + 1]));
	else
		curtime = 0.0;

	lastcurtime = curtime;
};
#endif // _WIN32

/*
==================
ParseCommandLine

==================
*/
const char *argv[MAX_NUM_ARGVS];
static const char *empty_string = "";

void Sys_InitArgv(char *lpCmdLine)
{
	if(!lpCmdLine)
		return;
	
	host_parms.argc = 1;
	argv[0] = empty_string;

	while(*lpCmdLine && (host_parms.argc < MAX_NUM_ARGVS))
	{
		while(*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126)))
			lpCmdLine++;

		if(*lpCmdLine)
		{
			argv[host_parms.argc] = lpCmdLine;
			host_parms.argc++;

			while(*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126)))
				lpCmdLine++;

			if(*lpCmdLine)
			{
				*lpCmdLine = 0;
				lpCmdLine++;
			};
		};
	};
	
	host_parms.argv = argv;
	
	COM_InitArgv(host_parms.argc, host_parms.argv);
	
	host_parms.argc = com_argc;
	host_parms.argv = com_argv;
};

/*
================
Sys_ShutdownArgv
================
*/
void Sys_ShutdownArgv()
{
	// TODO: nothing?
};

/*
================
Sys_Printf
================
*/
void Sys_Printf(const char *fmt, ...)
{
	va_list argptr;

#ifdef _WIN32

#ifdef SWDS
	//if (sys_nostdout.value)
		//return;
	
	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);
#else
	char text[1024];

//
	if(isDedicated)
	{
		va_start(argptr, fmt);
		vsprintf(text, fmt, argptr);
		va_end(argptr);

		// TODO
		DedicatedExports_Print(text);
		//printf("%s", text);
	};
	
	//DebugOutputString(text); // TODO
	
// QW
	//va_start(argptr, fmt);
	//vprintf(fmt, argptr);
	//va_end(argptr);
//
#endif // SWDS

#elif __linux__
	char text[1024];
	byte *p;

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	if(strlen(text) > sizeof(text))
		Sys_Error("memory overwrite in Sys_Printf");

	if(nostdout)
		return;

	for(p = (byte *)text; *p; p++)
	{
		*p &= 0x7f;
		if((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	};
#elif sun
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

#ifndef SWDS
	DWORD flOldProtect;

	// TODO: copy on write or just read-write?
	if(!VirtualProtect((LPVOID)startaddr, length, PAGE_READWRITE, &flOldProtect))
		Sys_Error("Protection change failed.\n");
#endif

#else
	int psize = getpagesize();

	unsigned long addr = (startaddr & ~(psize - 1)) - psize;

	//	fprintf(stderr, "writable code %lx(%lx)-%lx, length=%lx\n", startaddr,
	//			addr, startaddr+length, length);

	int r = mprotect((char *)addr, length + startaddr - addr + psize, 7);

	if(r < 0)
		Sys_Error("Protection change failed\n");
#endif
};

//#ifndef _M_IX86 // TODO: check for non-Intel?

/*
================
Sys_SetFPCW
================
*/
void Sys_SetFPCW()
{
};

/*
================
Sys_PushFPCW_SetHigh
================
*/
void Sys_PushFPCW_SetHigh()
{
};

/*
================
Sys_PopFPCW
================
*/
void Sys_PopFPCW()
{
};

/*
================
MaskExceptions
================
*/
void MaskExceptions()
{
};

//#endif // _M_IX86

/*
================
Sys_Error
================
*/
void Sys_Error(const char *error, ...)
{
#ifdef _WIN32
	va_list argptr;
	char text[1024];
	
	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);
	
#ifdef SWDS
	//MessageBox(NULL, text, "Error", 0 /* MB_OK */ );
	printf("ERROR: %s\n", text);
#else
	char text2[1024];
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

	gEntityInterface.pfnSys_Error(text);
	
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
		//sc_return_on_enter = true; // so Enter will get us out of here // TODO

		//while(!Sys_ConsoleInput() && ((Sys_FloatTime() - starttime) < CONSOLE_ERROR_TIMEOUT)) // TODO
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
#endif // SWDS

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

	gEntityInterface.pfnSys_Error(string);
	
	Host_Shutdown();
	exit(1);
#elif sun
	va_list argptr;

	printf("Sys_Error: ");
	va_start(argptr, error);
	vprintf(error, argptr);
	va_end(argptr);
	printf("\n");
	gEntityInterface.pfnSys_Error(error);
	Host_Shutdown();
	exit(1);
#endif
};

/*
================
Sys_Quit
================
*/
void Sys_Quit()
{
#ifdef _WIN32

#ifndef SWDS
	//VID_ForceUnlockedAndReturnState(); // TODO

	Host_Shutdown();

	//if(tevent) // TODO
		//CloseHandle(tevent); // TODO

	//if(qwclsemaphore) // TODO
		//CloseHandle(qwclsemaphore); // TODO
#endif

#elif __linux__
	Host_Shutdown();
	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~FNDELAY);

	fflush(stdout);
#elif sun
	Host_Shutdown();
#endif

	exit(0);
};

/*
================
Sys_FloatTime
================
*/
double Sys_FloatTime()
{
#ifdef _WIN32

#ifdef SWDS
	struct _timeb tstruct;
	static int starttime;

	_ftime(&tstruct);

	if(!starttime)
		starttime = tstruct.time;

	double t = (tstruct.time - starttime) + tstruct.millitm * 0.001;

	return t;
#else
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
#endif // SWDS

#else // if not Windows
	struct timeval tp;
	struct timezone tzp;
	static int secbase;

	gettimeofday(&tp, &tzp);

	if(!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec / 1000000.0;
	};

	return (tp.tv_sec - secbase) + tp.tv_usec / 1000000.0;
#endif // _WIN32
};

/*
================
Sys_Sleep
================
*/
void Sys_Sleep()
{
#ifdef _WIN32

#ifndef SWDS
	Sleep(1); // TODO: nothing in QW
#endif

#elif __linux__
	usleep(1 * 1000);
#elif sun
	// Nothing?
#endif
};

/*
================
Sys_SendKeyEvents

Send Key_Event calls
================
*/
void Sys_SendKeyEvents()
{
#ifdef OGS_USE_SDL
	//SDL_PumpEvents();
	
	HandleSDLEvents();
#elif _WIN32

#ifndef SWDS
	static MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		// we always update if there are any event, even if we're paused
		scr_skipupdate = 0;

		if(!GetMessage(&msg, NULL, 0, 0))
			Sys_Quit();

		//sys_msg_time = msg.time;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};
	
	// grab frame time 
	//sys_frame_time = timeGetTime(); // FIXME: should this be at start?
#endif // SWDS
#endif

#endif // OGS_USE_SDL
};

//============================================

#ifdef _WIN32
char findbase[MAX_OSPATH];
char findpath[MAX_OSPATH];
int findhandle;
#else
static char findbase[MAX_OSPATH];
static char findpath[MAX_OSPATH];
static char findpattern[MAX_OSPATH];
static DIR *fdir;
#endif

// directory searching
#define SFF_ARCH    0x01
#define SFF_HIDDEN  0x02
#define SFF_RDONLY  0x04
#define SFF_SUBDIR  0x08
#define SFF_SYSTEM  0x10

#ifdef _WIN32
static qboolean CompareAttributes( unsigned int found, unsigned int musthave, unsigned int canthave )
{
	if ( ( found & _A_RDONLY ) && ( canthave & SFF_RDONLY ) )
		return false;
	if ( ( found & _A_HIDDEN ) && ( canthave & SFF_HIDDEN ) )
		return false;
	if ( ( found & _A_SYSTEM ) && ( canthave & SFF_SYSTEM ) )
		return false;
	if ( ( found & _A_SUBDIR ) && ( canthave & SFF_SUBDIR ) )
		return false;
	if ( ( found & _A_ARCH ) && ( canthave & SFF_ARCH ) )
		return false;

	if ( ( musthave & SFF_RDONLY ) && !( found & _A_RDONLY ) )
		return false;
	if ( ( musthave & SFF_HIDDEN ) && !( found & _A_HIDDEN ) )
		return false;
	if ( ( musthave & SFF_SYSTEM ) && !( found & _A_SYSTEM ) )
		return false;
	if ( ( musthave & SFF_SUBDIR ) && !( found & _A_SUBDIR ) )
		return false;
	if ( ( musthave & SFF_ARCH ) && !( found & _A_ARCH ) )
		return false;

	return true;
};
#else
static qboolean CompareAttributes(char *path, char *name, unsigned musthave, unsigned canthave )
{
	struct stat st;
	char fn[MAX_OSPATH];

	// . and .. never match
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
		return false;

	sprintf(fn, "%s/%s", path, name);
	if (stat(fn, &st) == -1)
		return false; // shouldn't happen

	if ( ( st.st_mode & S_IFDIR ) && ( canthave & SFF_SUBDIR ) )
		return false;

	if ( ( musthave & SFF_SUBDIR ) && !( st.st_mode & S_IFDIR ) )
		return false;

	return true;
};
#endif // _WIN32

char *Sys_FindFirst (const char *path, unsigned int musthave, unsigned int canthave )
{
#ifdef _WIN32
	struct _finddata_t findinfo;

	if (findhandle)
		Sys_Error ("Sys_FindFirst without close");
	findhandle = 0;

	//COM_FilePath (path, findbase); // TODO
	findhandle = _findfirst (path, &findinfo);
	if (findhandle == -1)
		return NULL;
	if ( !CompareAttributes( findinfo.attrib, musthave, canthave ) )
		return NULL;
	sprintf (findpath, sizeof(findpath), "%s/%s", findbase, findinfo.name);
	return findpath;
#else
	struct dirent *d;
	char *p;

	if (fdir)
		Sys_Error ("Sys_FindFirst without close");

//	COM_FilePath (path, findbase);
	strcpy(findbase, path);

	if ((p = strrchr(findbase, '/')) != NULL)
	{
		*p = 0;
		strcpy(findpattern, p + 1);
	}
	else
		strcpy(findpattern, "*");

	if (strcmp(findpattern, "*.*") == 0)
		strcpy(findpattern, "*");
	
	if ((fdir = opendir(findbase)) == NULL)
		return NULL;
	
	while ((d = readdir(fdir)) != NULL)
	{
		if (!*findpattern || glob_match(findpattern, d->d_name))
		{
//			if (*findpattern)
//				printf("%s matched %s\n", findpattern, d->d_name);
			if (CompareAttributes(findbase, d->d_name, musthave, canhave))
			{
				sprintf (findpath, "%s/%s", findbase, d->d_name);
				return findpath;
			}
		}
	}
	return NULL;
#endif
};

char *Sys_FindNext ( unsigned int musthave, unsigned int canthave )
{
#ifdef _WIN32
	struct _finddata_t findinfo;

	if (findhandle == -1)
		return NULL;
	if (_findnext (findhandle, &findinfo) == -1)
		return NULL;
	if ( !CompareAttributes( findinfo.attrib, musthave, canthave ) )
		return NULL;

	sprintf (findpath, sizeof(findpath), "%s/%s", findbase, findinfo.name);
	return findpath;
#else
	struct dirent *d;

	if (fdir == NULL)
		return NULL;
	while ((d = readdir(fdir)) != NULL)
	{
		if (!*findpattern || glob_match(findpattern, d->d_name))
		{
//			if (*findpattern)
//				printf("%s matched %s\n", findpattern, d->d_name);
			if (CompareAttributes(findbase, d->d_name, musthave, canhave))
			{
				sprintf (findpath, "%s/%s", findbase, d->d_name);
				return findpath;
			}
		}
	}
	return NULL;
#endif
};

void Sys_FindClose ()
{
#ifdef _WIN32
	if (findhandle != -1)
		_findclose (findhandle);
	findhandle = 0;
#else
	if (fdir != NULL)
		closedir(fdir);
	fdir = NULL;
#endif
};

//============================================