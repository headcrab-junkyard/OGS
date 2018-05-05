/*
 *	This file is part of Open GoldSrc Project
 *	Copyright (C) 2018 Headcrab Garage
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of this program with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve"). You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve. If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so. If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include <cstring>
#include "FileSystem.hpp"

// TODO: temp
void Sys_Error(const char *msg, ...)
{
};

EXPOSE_SINGLE_INTERFACE(CFileSystem, IFileSystem, OGS_FILESYSTEM_INTERFACE_VERSION);

CFileSystem::CFileSystem() = default;
CFileSystem::~CFileSystem() = default;

void CFileSystem::Mount()
{
};

void CFileSystem::Unmount()
{
	ClearAllSearchPaths();
};

void CFileSystem::AddSearchPath(const char *path, const char *alias, bool nowrite)
{
	mlstSearchPaths.emplace_back(alias, path, nowrite);
};

IFile *CFileSystem::OpenFile(const char *asName)
{
	IFile *pFile{nullptr}; //new CFile(asName);
	//mlstOpenHandles.push_back(pFile);
	return pFile;
};

void CFileSystem::CloseFile(IFile *apFile)
{
	if(!apFile)
		return;
	
	auto It{mlstOpenHandles.find(apFile)};
	
	if(It)
	{
		delete It;
		mlstOpenHandles.erase(It);
	};
};

int CFileSystem::FileOpen(const char *path, const char *mode)
{
	// FS_FileOpenRead
	/*
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
	
	*/
	//
	
	FILE *f{fopen(path, "wb")};
	
	if(!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	
	int i{findhandle()};
	
	sys_handles[i] = f;
	
	return i;
};

void CFileSystem::FileClose(int handle)
{
	if(!handle)
		return;

#ifdef _WIN32
	fclose(sys_handles[handle]);
	sys_handles[handle] = nullptr;
#else
	close(handle);
#endif
};

void CFileSystem::FileSeek(int handle, int position) // TODO: seek mode
{
#ifdef _WIN32
	fseek(sys_handles[handle], position, SEEK_SET);
#else
	lseek(handle, position, SEEK_SET);
#endif
};

int CFileSystem::FileRead(int handle, void *dest, int count)
{
	if(!dest)
		return -1;
	
#ifdef _WIN32
	return fread(dest, 1, count, sys_handles[handle]);
#else
	return read(handle, dest, count);
#endif
};

int CFileSystem::FileWrite(int handle, const void *data, int count)
{
	if(!data)
		return -1;
	
#ifdef _WIN32
	return fwrite(data, 1, count, sys_handles[handle]);
#else
	return write(handle, src, count);
#endif
};

int CFileSystem::GetFileTime(const char *path) const
{
	FILE *f{fopen(path, "rb")};
	
	if(f)
	{
		fclose(f);
		return 1;
	};
	
	return -1;
};

int CFileSystem::findhandle()
{
	for(int i = 1; i < MAX_HANDLES; i++)
		if(!sys_handles[i])
			return i;
	
	Sys_Error("out of handles");
	return -1;
};

/*
================
filelength
================
*/
int CFileSystem::filelength(FILE *f)
{
	int pos = ftell(f);
	fseek(f, 0, SEEK_END);
	
	int end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
};