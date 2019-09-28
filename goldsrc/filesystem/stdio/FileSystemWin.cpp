/*
 * This file is part of OGS Engine
 * Copyright (C) 1997-2001 Id Software, Inc.
 * Copyright (C) 2019 BlackPhrase
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

#include "FileSystem.hpp"

char findbase[MAX_OSPATH];
char findpath[MAX_OSPATH];
int findhandle;

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

char *Sys_FindFirst (const char *path, unsigned int musthave, unsigned int canthave )
{
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
	snprintf (findpath, sizeof(findpath), "%s/%s", findbase, findinfo.name);
	return findpath;
};

char *Sys_FindNext ( unsigned int musthave, unsigned int canthave )
{
	struct _finddata_t findinfo;

	if (findhandle == -1)
		return NULL;
	if (_findnext (findhandle, &findinfo) == -1)
		return NULL;
	if ( !CompareAttributes( findinfo.attrib, musthave, canthave ) )
		return NULL;

	snprintf (findpath, sizeof(findpath), "%s/%s", findbase, findinfo.name);
	return findpath;
};

void Sys_FindClose ()
{
	if (findhandle != -1)
		_findclose (findhandle);
	findhandle = 0;
};