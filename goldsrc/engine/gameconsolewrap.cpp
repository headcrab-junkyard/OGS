/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2021 BlackPhrase
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

#include "quakedef.h"
#include "GameUI/IGameConsole.h"

IGameConsole *gpGameConsole{ nullptr };

//void GameConsole_Initialize();

void GameConsole_Clear()
{
	if(gpGameConsole)
		gpGameConsole->Clear();
};

qboolean GameConsole_IsVisible()
{
	if(gpGameConsole)
		return gpGameConsole->IsConsoleVisible();

	return false;
};

void GameConsole_Printf(const char *format, ...)
{
	char msg[256]{};
	va_list argptr;
	
	va_start(argptr, format);
	vsprintf(msg, format, argptr);
	va_end(argptr);

	if(gpGameConsole)
		gpGameConsole->Printf(msg);
};

void GameConsole_DPrintf(const char *format, ...)
{
	char msg[256]{};
	va_list argptr;
	
	va_start(argptr, format);
	vsprintf(msg, format, argptr);
	va_end(argptr);

	if(gpGameConsole)
		gpGameConsole->DPrintf(msg);
};