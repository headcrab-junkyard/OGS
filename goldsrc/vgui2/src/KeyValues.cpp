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

#include "KeyValues.h"

namespace vgui2
{

void CKeyValues::RegisterSizeofKeyValues(int size)
{
};

void *CKeyValues::AllocKeyValuesMemory(int size)
{
	return nullptr;
};

void CKeyValues::FreeKeyValuesMemory(void *pMem)
{
};

HKeySymbol CKeyValues::GetSymbolForString(const char *name)
{
	return 0;
};

const char *CKeyValues::GetStringForSymbol(HKeySymbol symbol)
{
	return "";
};

void CKeyValues::GetLocalizedFromANSI(const char *ansi, wchar_t *outBuf, int unicodeBufferSizeInBytes)
{
};

void CKeyValues::GetANSIFromLocalized(const wchar_t *wchar, char *outBuf, int ansiBufferSizeInBytes)
{
};

void CKeyValues::AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name)
{
};

void CKeyValues::RemoveKeyValuesFromMemoryLeakList(void *pMem)
{
};

}; // namespace vgui2