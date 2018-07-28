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
/// @brief Sun system driver

#include "quakedef.h"
#include "errno.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES 10

typedef struct
{
	FILE *hFile;
	char *pMap;
	int nLen;
	int nPos;
} MEMFILE;

MEMFILE sys_handles[MAX_HANDLES];

int findhandle(void)
{
	int i;

	for(i = 1; i < MAX_HANDLES; i++)
		if(!sys_handles[i].hFile)
			return i;
	Sys_Error("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength(FILE *f)
{
	int pos;
	int end;

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead(char *path, int *hndl)
{
	FILE *f;
	int i;

	i = findhandle();

	f = fopen(path, "rb");
	if(!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i].hFile = f;
	sys_handles[i].nLen = filelength(f);
	sys_handles[i].nPos = 0;
	sys_handles[i].pMap = mmap(0, sys_handles[i].nLen, PROT_READ, MAP_SHARED, fileno(sys_handles[i].hFile), 0);
	if(!sys_handles[i].pMap || (sys_handles[i].pMap == (char *)-1))
	{
		printf("mmap %s failed!", path);
		sys_handles[i].pMap = NULL;
	}

	*hndl = i;

	return (sys_handles[i].nLen);
}

int Sys_FileOpenWrite(char *path)
{
	FILE *f;
	int i;

	i = findhandle();

	f = fopen(path, "wb");
	if(!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	sys_handles[i].hFile = f;
	sys_handles[i].nLen = 0;
	sys_handles[i].nPos = 0;
	sys_handles[i].pMap = NULL;

	return i;
}

void Sys_FileClose(int handle)
{
	if(sys_handles[handle].pMap)
		if(munmap(sys_handles[handle].pMap, sys_handles[handle].nLen) != 0)
			printf("failed to unmap handle %d\n", handle);

	fclose(sys_handles[handle].hFile);
	sys_handles[handle].hFile = NULL;
}

void Sys_FileSeek(int handle, int position)
{
	if(sys_handles[handle].pMap)
	{
		sys_handles[handle].nPos = position;
	}
	else
		fseek(sys_handles[handle].hFile, position, SEEK_SET);
}

int Sys_FileRead(int handle, void *dest, int count)
{
	if(sys_handles[handle].pMap)
	{
		int nPos = sys_handles[handle].nPos;
		if(count < 0)
			count = 0;
		if(nPos + count > sys_handles[handle].nLen)
			count = sys_handles[handle].nLen - nPos;
		memcpy(dest, &sys_handles[handle].pMap[nPos], count);
		sys_handles[handle].nPos = nPos + count;
		return (count);
	}
	else
		return fread(dest, 1, count, sys_handles[handle].hFile);
}

int Sys_FileWrite(int handle, void *data, int count)
{
	if(sys_handles[handle].pMap)
		Sys_Error("Attempted to write to read-only file %d!\n", handle);
	return fwrite(data, 1, count, sys_handles[handle].hFile);
}

int Sys_FileTime(char *path)
{
	FILE *f;

	f = fopen(path, "rb");
	if(f)
	{
		fclose(f);
		return 1;
	}

	return -1;
}

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

char *Sys_ConsoleInput(void)
{
	static char text[256];
	int len;
	fd_set readfds;
	int ready;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	ready = select(1, &readfds, 0, 0, &timeout);

	if(ready > 0)
	{
		len = read(0, text, sizeof(text));
		if(len >= 1)
		{
			text[len - 1] = 0; // rip off the /n and terminate
			return text;
		}
	}

	return 0;
}

//=============================================================================

int main(int argc, char **argv)
{
	Sys_Init();

	// unroll the simulation loop to give the video side a chance to see _vid_default_mode
	Host_Frame(0.1);
	VID_SetDefaultMode();

	oldtime = Sys_FloatTime();
	while(1)
	{
	}
}
