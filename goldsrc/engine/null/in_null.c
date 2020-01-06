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
/// @brief for systems without a mouse

#include "quakedef.h"

void IN_Init()
{
};

void IN_Shutdown()
{
};

void IN_ClearStates()
{
};

// TODO
//#ifdef _WIN32

qboolean mouseactive;

#ifdef _WIN32
unsigned int uiWheelMessage;
#endif

void IN_ActivateMouse()
{
};

void IN_DeactivateMouse()
{
};

void IN_ShowMouse()
{
};

void IN_HideMouse()
{
};

void IN_Accumulate()
{
};

void IN_MouseEvent(int key, qboolean down)
{
};

void IN_UpdateClipCursor()
{
};

//#endif // _WIN32