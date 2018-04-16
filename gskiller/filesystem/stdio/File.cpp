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

#include "File.hpp"

CFile::CFile(const char *asName) : msName(asName), mpHandle(fopen(asName))
{
	assert(mpHandle);
	
	//if(!mpHandle)
		//throw std::runtime_error("Nice joke!");
};

CFile::~CFile()
{
	if(mpHandle)
	{
		fflush(mpHandle);
		fclose(mpHandle);
	};
};

void CFile::Write(const void *data, int count)
{
	if(!data)
		return;
	
	if(mpHandle)
		fwrite(data, sizeof(char), count, mpHandle);
};

int CFile::Read(void *dest, int count) const
{
	if(!dest)
		return -1;
	
	if(mpHandle)
		return fread(dest, sizeof(char), count, mpHandle);

	return 0;
};

void CFile::Printf(const char *text, ...)
{
	if(!text || !*text)
		return;
	
	if(mpHandle)
		fputs(text, mpHandle);
};

int CFile::Seek(int position) const
{
	if(mpHandle)
		return fseek(mpHandle, position, SEEK_SET);

	return 0;
};

int CFile::Tell() const
{
	if(mpHandle)
		return ftell(mpHandle);

	return 0;
};

const char *CFile::GetName() const
{
	if(mpHandle)
		return msName;

	return "";
};

const char *CFile::GetExt() const
{
	return ""; // TODO
};

const char *CFile::GetPath() const
{
	return ""; // TODO
};

int CFile::GetTime() const
{
	if(mpHandle)
		return 1;
	
	return -1;
};

int CFile::GetSize() const
{
	return 0; // TODO
};