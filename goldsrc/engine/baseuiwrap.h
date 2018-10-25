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
/// @brief IBaseUI interface wrapper

#pragma once

#include "const.h"

void BaseUI_Initialize(void *factories, int count);
void BaseUI_Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion);
void BaseUI_Shutdown();

int BaseUI_Key_Event(int down, int keynum, const char *pszCurrentBinding);

void BaseUI_CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam);

void BaseUI_Paint(int x, int y, int right, int bottom);

void BaseUI_HideGameUI();
void BaseUI_ActivateGameUI();

qboolean BaseUI_IsGameUIVisible();

void BaseUI_HideConsole();
void BaseUI_ShowConsole();