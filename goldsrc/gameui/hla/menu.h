/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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

#pragma once

#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

#define C_EXPORT extern "C" EXPORT

//
// menus
//

extern void (*vid_menudrawfn)();
extern void (*vid_menukeyfn)(int key);

void M_Init();
C_EXPORT void M_Keydown(int key);
C_EXPORT void M_Draw();

void M_ToggleMenu_f();
//qpic_t *M_CachePic(const char *path); // TODO: qw
//void M_DrawTextBox(int x, int y, int width, int lines); // TODO: qw
//void M_Menu_Quit_f(); // TODO: qw