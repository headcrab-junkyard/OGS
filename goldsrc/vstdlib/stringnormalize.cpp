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

#include <vstdlib/stringnormalize.h>

void CStringNormalization::Initialize()
{
	if(m_bInitialized)
		return;
	
	// TODO
	
	m_bInitialized = true;
};

int CStringNormalization::Normalize(bool abSomething, const char *src, char *dest, int buflen)
{
	// TODO
	return 0;
};

int CStringNormalization::Normalize(bool abSomething, const wchar_t *src, wchar_t *dest, int buflen)
{
	// TODO
	return 0;
};

bool CStringNormalization::Test_BValidateStructures()
{
	// TODO
	return false;
};

void CStringNormalization::ValidateStatics(CValidator &aValidator, char *asSomething)
{
	// TODO
};