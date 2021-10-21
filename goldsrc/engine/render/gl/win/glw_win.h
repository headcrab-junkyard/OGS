/*
 * This file is part of OGS Engine
 * Copyright (C) 1997-2001 Id Software, Inc.
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

#ifndef _WIN32
#  error You should not be including this file on this platform
#endif

#pragma once

typedef struct
{
	HINSTANCE	hInstance;
	void	*wndproc;

	HDC     hDC;			// handle to device context
	HWND    hWnd;			// handle to window
	HGLRC   hGLRC;			// handle to GL rendering context

	HINSTANCE hinstOpenGL;	// HINSTANCE for the OpenGL library

	qboolean minidriver;
	qboolean allowdisplaydepthchange;
	qboolean mcd_accelerated;

	FILE *log_fp;
} glwstate_t;

extern glwstate_t glw_state;