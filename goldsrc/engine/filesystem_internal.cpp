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

#include "quakedef.h"
#include "IFileSystem.hpp"

IFileSystem *gpFileSystem{nullptr};

void FileSystem_Init(char *basedir, void *filesystemFactory)
{
	// TODO
	
	if(!basedir || !*basedir)
		return;
	
	if(!filesystemFactory)
		return;
	
	gpFileSystem = (IFileSystem*)(((CreateInterfaceFn)filesystemFactory)(OGS_FILESYSTEM_INTERFACE_VERSION, nullptr)); // TODO: FILESYSTEM_INTERFACE_VERSION
	
	if(!gpFileSystem)
		return;
};

void FileSystem_Shutdown()
{
	// TODO
	
	gpFileSystem = nullptr;
};

/*
===============================================================================

FILE IO

===============================================================================
*/

int FS_FileOpenRead(const char *path, int *hndl)
{
	return 0; // TODO
};

int FS_FileOpenWrite(const char *path)
{
	return gpFileSystem->FileOpen(path, "wb");
};

void FS_FileClose(int handle)
{
	gpFileSystem->FileClose(handle);
};

void FS_FileSeek(int handle, int position)
{
	gpFileSystem->FileSeek(handle, position);
};

int FS_FileRead(int handle, void *dest, int count)
{
	return gpFileSystem->FileRead(handle, dest, count);
};

int FS_FileWrite(int handle, const void *data, int count)
{
	return gpFileSystem->FileWrite(handle, data, count);
};

int FS_FileTime(const char *path)
{
	return gpFileSystem->GetFileTime(path);
};

void FS_mkdir(const char *path)
{
	// TODO: if not dedicated?

#ifdef _WIN32
	//_mkdir(path); // TODO
#else
	mkdir(path, 0777);
#endif
};