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

#include "GameConsole.hpp"

EXPOSE_SINGLE_INTERFACE(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION);

CGameConsole::CGameConsole() = default;
CGameConsole::~CGameConsole() = default;

void CGameConsole::Activate()
{
};

void CGameConsole::Initialize()
{
};

void CGameConsole::Hide()
{
};

void CGameConsole::Clear()
{
};

bool CGameConsole::IsConsoleVisible()
{
};

void CGameConsole::Printf(const char *format, ...)
{
};

void CGameConsole::DPrintf(const char *format, ...)
{
};

void CGameConsole::SetParent(vgui2::VPANEL parent)
{
};