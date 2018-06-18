/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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

#pragma once

//
// the net drivers should just set the apropriate bits in m_activenet,
// instead of having the menu code look through their internal tables
//
#define MNET_IPX 1
#define MNET_TCP 2

extern int m_activenet;

//
// menus
//
void M_Init();
void M_Keydown(int key);
void M_Draw();

void M_ToggleMenu_f();