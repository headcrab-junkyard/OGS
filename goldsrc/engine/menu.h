/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021 BlackPhrase
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

#pragma once

typedef void (*pfnM_Keydown)(int key);
typedef void (*pfnM_Draw)();

extern pfnM_Keydown fnM_Keydown;
extern pfnM_Draw fnM_Draw;

//
// menus
//
void M_Keydown(int key);
void M_Draw();

void M_ToggleMenu_f();
//qpic_t *M_CachePic(const char *path); // TODO: qw
//void M_DrawTextBox(int x, int y, int width, int lines); // TODO: qw
//void M_Menu_Quit_f(); // TODO: qw