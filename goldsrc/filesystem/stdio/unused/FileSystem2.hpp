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