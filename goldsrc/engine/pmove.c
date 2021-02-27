/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018, 2020 BlackPhrase
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

#include "quakedef.h"

//#define STEPSIZE 18

movevars_t movevars;

playermove_t *pmove;

vec3_t player_mins = { -16, -16, -24 };
vec3_t player_maxs = { 16, 16, 32 };

void PM_InitBoxHull();

void Pmove_Init(playermove_t *pmove) // TODO: was Pmove_Init, should be PM_Init
{
	PM_InitBoxHull();
	
	// TODO
	pmove->PM_TestPlayerPosition = NULL; //PM_TestPlayerPosition;
	pmove->PM_PointContents = PM_PointContents;
	pmove->PM_PlayerTrace = NULL; //PM_PlayerTrace;
}