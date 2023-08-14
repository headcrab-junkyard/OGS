/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019, 2021, 2023 BlackPhrase
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
/// @brief sdl2 input impl

#include "quakedef.h"

void IN_Init()
{
};

void IN_Shutdown()
{
};

// TODO: IN_ClearStates?

void IN_SetCursorPos(int x, int y)
{
	SDL_WarpMouseInWindow(mainwind, x, y);
};

void IN_GetCursorPos(int *x, int *y)
{
	SDL_GetMouseState(x, y); // TODO: SDL_GetRelativeMouseState?
};