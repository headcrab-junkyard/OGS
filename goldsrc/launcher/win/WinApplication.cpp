/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018-2019 BlackPhrase
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

#include "WinApplication.hpp"

// Windows doesn't provide the setenv func impl
int setenv(const char *name, const char *value, int overwrite)
{
	if(!overwrite)
	{
#ifdef MSVC
		size_t envsize{0};
		auto errcode{getenv_s(&envsize, nullptr, 0, name)};
		if(errcode || envsize)
			return errcode;
#else
		if(getenv(name) == nullptr)
			return -1;
#endif
	};

	return _putenv_s(name, value);
};

CWinApplication::CWinApplication(const char *cmdline)
{
	msCmdLine[0] = '\0';
	
	if(cmdline && *cmdline)
		//for(int i = 0; i < strlen(cmdline); ++i) // TODO
			strcat(msCmdLine, cmdline);
};

CWinApplication::~CWinApplication() = default;

bool CWinApplication::PostInit()
{
	return true;
};