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
/// @brief non-portable functions

#pragma once

//
// memory protection
//
void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length);

//
// system IO
//
void Sys_DebugLog(const char *file, const char *fmt, ...);

void Sys_Init ();

void Sys_Error (const char *error, ...);
// an error will cause the entire program to exit

void Sys_Printf (const char *fmt, ...);
// send text to the console

void Sys_Quit ();

double Sys_FloatTime ();

char *Sys_ConsoleInput ();

void Sys_Sleep ();
// called to yield for a little bit so as
// not to hog cpu when paused or debugging

void Sys_SendKeyEvents ();
// Perform Key_Event () callbacks until the input que is empty

void Sys_LowFPPrecision ();
void Sys_HighFPPrecision ();
void Sys_SetFPCW ();

//int Sys_FileTime (char *path); // TODO: FS_FileTime

//void Sys_mkdir (char *path); // TODO: FS_mkdir