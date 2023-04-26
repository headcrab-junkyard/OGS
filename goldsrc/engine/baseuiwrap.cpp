/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021-2022 BlackPhrase
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

#include <BaseUI/IBaseUI.h>

extern IBaseUI *gpBaseUI;

/*
void BaseUI_Initialize(void *factories, int count)
{
	gpBaseUI->Initialize(reinterpret_cast<CreateInterfaceFn*>(factories), count);
};
*/

void BaseUI_Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion)
{
	gpBaseUI->Start(engineFuncs, interfaceVersion);
};

/*
void BaseUI_Shutdown()
{
	gpBaseUI->Shutdown();
};
*/

int BaseUI_Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	return gpBaseUI->Key_Event(down, keynum, pszCurrentBinding);
};

void BaseUI_CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam)
{
	gpBaseUI->CallEngineSurfaceProc(hwnd, msg, wparam, lparam);
};

void BaseUI_Paint(int x, int y, int right, int bottom)
{
	gpBaseUI->Paint(x, y, right, bottom);
};

void BaseUI_HideGameUI()
{
	gpBaseUI->HideGameUI();
};

void BaseUI_ActivateGameUI()
{
	gpBaseUI->ActivateGameUI();
};

qboolean BaseUI_IsGameUIVisible()
{
	return gpBaseUI->IsGameUIVisible();
};

void BaseUI_HideConsole()
{
	gpBaseUI->HideConsole();
};

void BaseUI_ShowConsole()
{
	gpBaseUI->ShowConsole();
};