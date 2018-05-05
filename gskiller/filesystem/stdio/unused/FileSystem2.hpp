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
/// @brief IFileSystem interface implementation (OGS version)

#pragma once

#include <cstdio>
#include <string>
#include <list>
#include <unordered_map>
#include "IFileSystem.hpp"

struct SSearchPathEntry
{
	std::string name{""};
	
	bool readonly{true}; // TODO: nowrite
};

using tSearchPathEntryList = std::list<SSearchPathEntry>;

struct SSearchPathGroup
{
	std::string name{""}; // TODO: pathid
	tSearchPathEntryList mlstSearchPaths;
};

using tSearchPathGroupList = std::list<SSearchPathGroup>;

class CFileSystem final : public IFileSystem
{
public:
	CFileSystem();
	~CFileSystem();
	
	void Mount() override;
	void Unmount() override;
	
	void AddSearchPath(const char *path, const char *alias, bool nowrite) override;
	
	IFile *OpenFile(const char *asName) override;
	void CloseFile(IFile *apFile) override;
	
	int FileOpen(const char *path, const char *mode) override;
	void FileClose(int handle) override;
	
	void FileSeek(int handle, int position) override;
	
	int FileRead(int handle, void *dest, int count) override;
	int FileWrite(int handle, const void *data, int count) override;
	
	int GetFileTime(const char *path) const override;
private:
	static constexpr auto MAX_HANDLES{10};
	
	tSearchPathGroupList mlstSearchPaths;
	
	int findhandle();
	int filelength(FILE *f);

	// TODO: if WIN only?
	FILE *sys_handles[MAX_HANDLES]{nullptr};
};