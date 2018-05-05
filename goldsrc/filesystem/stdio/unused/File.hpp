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
/// @brief IFile interface implementation

#pragma once

#include <cstdio>
#include "IFile.hpp"

class CFile final : public IFile
{
public:
	CFile(const char *asName);
	~CFile();
	
	void Write(const void *data, int count) override;
	int Read(void *dest, int count) const override;

	void Printf(const char *text, ...) override;

	int Seek(int position) const override;

	int Tell() const override;

	const char *GetName() const override;
	const char *GetExt() const override;
	const char *GetPath() const override;

	int GetTime() const override;
	int GetSize() const override;
private:
	const char *msName{""};
	
	FILE *mpHandle{nullptr};
};