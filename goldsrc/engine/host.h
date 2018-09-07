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

#pragma once

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

typedef struct
{
	char *basedir;
	char *cachedir; // for development over ISDN lines
	int argc;
	char **argv;
	void *membase;
	int memsize;
} quakeparms_t;

extern quakeparms_t host_parms;

extern cvar_t hostname;
extern cvar_t sys_ticrate;
extern cvar_t sys_nostdout;
extern cvar_t developer;

//extern cvar_t password; // TODO: qw

extern qboolean host_initialized; // true if into command execution
extern double host_frametime;
extern byte *host_basepal;
extern byte *host_colormap;
extern int host_framecount; // incremented every frame, never reset
extern double realtime;     // not bounded in any way, changed at
                            // start of every frame, never reset

void Host_ClearMemory();
void Host_InitCommands();
void Host_Init(quakeparms_t *parms);
void Host_Shutdown();
void Host_Error(const char *error, ...);
void Host_EndGame(const char *message, ...);
//qboolean Host_SimulationTime(float time); // TODO: qw
void Host_Frame(float time); // TODO: state, stateinfo
//void Host_ServerFrame(); // TODO: qw
void Host_Quit_f();
void Host_ClientCommands(const char *fmt, ...);
void Host_ShutdownServer(qboolean crash);
//void Host_WriteConfiguration ();
void Host_UpdateStatus(float *fps, int *nActive, /*int *nSpectators*/ int *nMaxPlayers, /*const*/ char *pszMap);