/*
 *	This file is part of OGS Engine
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

//#include "quakedef.h" // TODO
#include "sys_engine.h"

// TODO: temp
const int PAUSE_SLEEP = 50;     ///< sleep time on pause or minimization
const int NOT_FOCUS_SLEEP = 20; ///< sleep time when not focus

static CEngine gEngine;       // g_Engine
IEngine *gpEngine = &gEngine; // eng

// TODO: was located in system code before
#ifdef _WIN32
qboolean ActiveApp{false}, Minimized{false};
#endif

CEngine::CEngine() = default;
CEngine::~CEngine() = default;

bool CEngine::Load(bool dedicated, const char *basedir, const char *cmdline)
{
	Sys_InitGame(cmdline, basedir, nullptr /*TODO: g_Game + 8 = g_Game.GetWindowPtr()*/, dedicated); // TODO
	return true;
};

void CEngine::Unload()
{
	Host_Shutdown();
};

// TODO
#ifdef _WIN32
void SleepUntilInput(int time)
{
	//MsgWaitForMultipleObjects(1, &tevent, FALSE, time, QS_ALLINPUT); // TODO
};
#endif

void CEngine::Frame()
{
	if(!isDedicated)
	{
#ifdef _WIN32
		// yield the CPU for a little while when paused, minimized, or not the focus
		//if ((cl.paused && (!ActiveApp && !DDActive)) || Minimized || block_drawing) // TODO
		{
			SleepUntilInput(PAUSE_SLEEP);
			scr_skipupdate = 1; // no point in bothering to draw
		}
		//else if (!ActiveApp && !DDActive) // TODO
			SleepUntilInput(NOT_FOCUS_SLEEP);
#endif // _WIN32
	};

	newtime = Sys_FloatTime();
	frametime = newtime - oldtime;

	if(isDedicated)
	{
		while(frametime < sys_ticrate.value)
		{
			Sys_Sleep();
			newtime = Sys_FloatTime();
			frametime = newtime - oldtime;
		};
	};

	Host_Frame(frametime);
	oldtime = newtime;
};