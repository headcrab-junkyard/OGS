/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2020 BlackPhrase
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

#include "GameConsole.hpp"
#include "GameConsoleDialog.hpp"

EXPOSE_SINGLE_INTERFACE(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION);

CGameConsole::CGameConsole() = default;

CGameConsole::~CGameConsole()
{
	mbInitialized = false;
};

void CGameConsole::Activate()
{
	if(!mbInitialized)
		return;
	
	//vgui2::surface()->RestrictPaintToSinglePanel(nullptr); // TODO
	mpConsole->Activate();
};

void CGameConsole::Initialize()
{
	mpConsole = vgui2::SETUP_PANEL(new CGameConsoleDialog());
	
	mbInitialized = true;
};

void CGameConsole::Hide()
{
	if(!mbInitialized)
		return;
	
	mpConsole->Hide();
};

void CGameConsole::Clear()
{
	if(!mbInitialized)
		return;
	
	mpConsole->Clear();
};

bool CGameConsole::IsConsoleVisible()
{
	if(!mbInitialized)
		return false;
	
	return mpConsole->IsVisible();
};

void CGameConsole::Printf(const char *format, ...)
{
	// TODO
};

void CGameConsole::DPrintf(const char *format, ...)
{
	// TODO
};

void CGameConsole::SetParent(int /*vgui2::VPANEL*/ parent)
{
	if(!mbInitialized)
		return;
	
	mpConsole->SetParent(static_cast<vgui2::VPANEL>(parent));
};