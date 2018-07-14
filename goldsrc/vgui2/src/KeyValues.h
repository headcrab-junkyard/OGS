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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include "vgui/IKeyValues.h"

class CKeyValues final : public vgui2::IKeyValues
{
public:
	CKeyValues();
	~CKeyValues();
	
	void RegisterSizeofKeyValues(int size) override;

	void *AllocKeyValuesMemory(int size) override;
	void FreeKeyValuesMemory(void *pMem) override;

	HKeySymbol GetSymbolForString(const char *name) override;
	const char *GetStringForSymbol(HKeySymbol symbol) override;

	void GetLocalizedFromANSI(const char *ansi, wchar_t *outBuf, int unicodeBufferSizeInBytes) override;

	void GetANSIFromLocalized(const wchar_t *wchar, char *outBuf, int ansiBufferSizeInBytes) override;

	void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name) override;
	void RemoveKeyValuesFromMemoryLeakList(void *pMem) override;
};