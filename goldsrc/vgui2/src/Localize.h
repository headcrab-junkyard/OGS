/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2018 BlackPhrase
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

#include "vgui2/ILocalize.h"

namespace vgui2
{

class CLocalize : public ILocalize
{
public:
	bool AddFile(IFileSystem *fileSystem, const char *fileName);

	void RemoveAll();

	wchar_t *Find(char const *tokenName);

	int ConvertANSIToUnicode(const char *ansi, wchar_t *unicode, int unicodeBufferSizeInBytes);
	int ConvertUnicodeToANSI(const wchar_t *unicode, char *ansi, int ansiBufferSize);

	StringIndex_t FindIndex(const char *tokenName);

	void ConstructString(wchar_t *unicodeOuput, int unicodeBufferSizeInBytes, wchar_t *formatString, int numFormatParameters, ...);

	const char *GetNameByIndex(StringIndex_t index);
	wchar_t *GetValueByIndex(StringIndex_t index);

	StringIndex_t GetFirstStringIndex();
	StringIndex_t GetNextStringIndex(StringIndex_t index);

	void AddString(const char *tokenName, wchar_t *unicodeString, const char *fileName);

	void SetValueByIndex(StringIndex_t index, wchar_t *newValue);

	bool SaveToFile(IFileSystem *fileSystem, const char *fileName);

	int GetLocalizationFileCount();

	const char *GetLocalizationFileName(int index);
	const char *GetFileNameByIndex(StringIndex_t index);

	void ReloadLocalizationFiles();

	// THIS IS REALLY BAD SINCE OVERLOADED FUNCS ORDER IN VIRTUAL INTERFACES CAN BE DIFFERENT ON VARIOUS COMPILERS VALVE!!!!!!!!!!!!
	void ConstructString(wchar_t *unicodeOutput, int unicodeBufferSizeInBytes, const char *tokenName, KeyValues *localizationVariables);
	void ConstructString(wchar_t *unicodeOutput, int unicodeBufferSizeInBytes, StringIndex_t unlocalizedTextSymbol, KeyValues *localizationVariables);
};

}; // namespace vgui2