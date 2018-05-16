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