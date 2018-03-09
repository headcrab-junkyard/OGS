/*
Copyright (C) 1996-2001 Id Software, Inc.

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

#pragma once

//
// key / value info strings
//
#define	MAX_INFO_KEY		64
#define	MAX_INFO_VALUE		64
//#define	MAX_INFO_STRING		512 // TODO: com_model

char *Info_ValueForKey (char *s, char *key);

void Info_RemoveKey (char *s, char *key);
void Info_RemovePrefixedKeys (char *start, char prefix);

//void Info_SetValueForKey (char *s, char *key, char *value); // TODO: Q2 version
void Info_SetValueForKey (char *s, char *key, char *value, int maxsize);
void Info_SetValueForStarKey (char *s, char *key, char *value, int maxsize);

void Info_Print (char *s);

//qboolean Info_Validate (char *s); // TODO: Q2