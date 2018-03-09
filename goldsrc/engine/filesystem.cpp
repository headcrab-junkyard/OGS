/// @file

#include "quakedef.h"
#include "IFileSystem.hpp"

IFileSystem *gpFileSystem{nullptr};

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES             10
FILE    *sys_handles[MAX_HANDLES];

int             findhandle ()
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength (FILE *f)
{
	int             pos;
	int             end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int FS_FileOpenRead (const char *path, int *hndl)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
}

int FS_FileOpenWrite (const char *path)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	return i;
}

void FS_FileClose (int handle)
{
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void FS_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}

int FS_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int FS_FileWrite (int handle, void *data, int count)
{
	return fwrite (data, 1, count, sys_handles[handle]);
}

int    FS_FileTime (const char *path)
{
	FILE    *f;
	
	f = fopen(path, "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}
	
	return -1;
}

void FS_mkdir (const char *path)
{
}