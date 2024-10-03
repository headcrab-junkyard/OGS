/*
 * This file is part of OGS Engine
 * Copyright (C) 2024 BlackPhrase
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

#include <vstdlib/ikeyvaluessystem.h>

class CKeyValuesSystem : public IKeyValuesSystem
{
public:
	void RegisterSizeofKeyValues(int anSize) override;
	
	void *AllocKeyValuesMemory(int anSize) override;
	void FreeKeyValuesMemory(void *apMemory) override;
	
	HKeySymbol GetSymbolForString(const char *asName, bool abCreate) override;
	const char *GetStringForSymbol(HKeySymbol anSymbol) override;
	
	void AddKeyValuesToMemoryLeakList(void *apMemory, HKeySymbol anName) override;
	void RemoveKeyValuesFromMemoryLeakList(void *apMemory) override;
};

void CKeyValuesSystem::RegisterSizeofKeyValues(int anSize)
{
	// TODO
};

void *CKeyValuesSystem::AllocKeyValuesMemory(int anSize)
{
	// TODO
	return nullptr;
};

void CKeyValuesSystem::FreeKeyValuesMemory(void *apMemory)
{
	// TODO
};

HKeySymbol CKeyValuesSystem::GetSymbolForString(const char *asName, bool abCreate)
{
	// TODO
	return INVALID_KEY_SYMBOL;
};

const char *CKeyValuesSystem::GetStringForSymbol(HKeySymbol anSymbol)
{
	// TODO
	return "";
};

void CKeyValuesSystem::AddKeyValuesToMemoryLeakList(void *apMemory, HKeySymbol anName)
{
	// TODO
};

void CKeyValuesSystem::RemoveKeyValuesFromMemoryLeakList(void *apMemory)
{
	// TODO
};

CKeyValuesSystem g_KeyValuesSystem;

IKeyValuesSystem *KeyValuesSystem()
{
	return &g_KeyValuesSystem;
};