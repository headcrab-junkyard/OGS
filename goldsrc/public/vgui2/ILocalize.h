/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include "VGUI.h"
#include "interface.h"

const char VGUI_LOCALIZE_INTERFACE_VERSION[] = "VGUI_Localize003";

class KeyValues;
struct IFileSystem;

namespace vgui2
{

using StringIndex_t = unsigned long;
const StringIndex_t INVALID_STRING_INDEX = (unsigned long)-1;

struct ILocalize : public IBaseInterface
{
	///
	virtual bool AddFile(IFileSystem *apFileSystem, const char *asFileName) = 0;
	
	///
	virtual void RemoveAll() = 0;
	
	///
	virtual wchar_t *Find(char const *asTokenName) = 0;
	
	///
	virtual int ConvertANSIToUnicode(const char *ansi, wchar_t *unicode, int unicodeBufferSizeInBytes) = 0;
	
	///
	virtual int ConvertUnicodeToANSI(const wchar_t *unicode, char *ansi, int ansiBufferSize) = 0;
	
	///
	virtual StringIndex_t FindIndex(const char *asTokenName) = 0;
	
	///
	virtual void ConstructString(wchar_t *unicodeOutput, int unicodeBufferSizeInBytes, wchar_t *formatString, int numFormatParameters, ...) = 0;
	
	///
	virtual const char *GetNameByIndex(StringIndex_t anIndex) = 0;
	
	///
	virtual wchar_t *GetValueByIndex(StringIndex_t anIndex) = 0;
	
	///
	virtual StringIndex_t GetFirstStringIndex() = 0;
	
	///
	virtual StringIndex_t GetNextStringIndex(StringIndex_t anIndex) = 0;
	
	///
	virtual void AddString(const char *asTokenName, wchar_t *unicodeString, const char *asFileName) = 0;
	
	///
	virtual void SetValueByIndex(StringIndex_t anIndex, wchar_t *asNewValue) = 0;
	
	///
	virtual bool SaveToFile(IFileSystem *apFileSystem, const char *asFileName) = 0;
	
	///
	virtual int GetLocalizationFileCount() = 0;
	
	///
	virtual const char *GetLocalizationFileName(int anIndex) = 0;
	
	///
	virtual const char *GetFileNameByIndex(StringIndex_t anIndex) = 0;
	
	///
	virtual void ReloadLocalizationFiles() = 0;
	
	///
	virtual void ConstructString(wchar_t *unicodeOutput, int unicodeBufferSizeInBytes, const char *tokenName, KeyValues *pLocalizationVariables) = 0;
	
	///
	virtual void ConstructString(wchar_t *unicodeOutput, int unicodeBufferSizeInBytes, StringIndex_t nUnlocalizedTextSymbol, KeyValues *pLocalizationVariables) = 0;
};

}; // namespace vgui2