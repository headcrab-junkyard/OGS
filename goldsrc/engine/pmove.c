/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2020-2021, 2023 BlackPhrase
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

//#define STEPSIZE 18

movevars_t movevars;

playermove_t *pmove;

vec3_t player_mins = {-16, -16, -24};
vec3_t player_maxs = {16, 16, 32};

void PM_InitBoxHull();

void PM_Init(playermove_t *pmove)
{
	PM_InitBoxHull();
	
	// Init vars
	
	//pmove->physinfo // TODO
	
	pmove->movevars = &movevars; // TODO
	
	// TODO: it's possible that these should be initialized by the gamedll
	//pmove->player_mins
	//pmove->player_maxs
	
	pmove->runfuncs = false;
	
	// Init the function pointers
	
	pmove->PM_Info_ValueForKey = Info_ValueForKey; // TODO: or should be a custom version here?
	
	pmove->PM_Particle = NULL; // TODO
	
	pmove->PM_TestPlayerPosition = NULL; // TODO: PM_TestPlayerPosition;
	pmove->PM_TestPlayerPositionEx = NULL; // TODO
	
	pmove->Con_NPrintf = Con_NPrintf;
	pmove->Con_DPrintf = Con_DPrintf;
	pmove->Con_Printf = Con_Printf;
	
	pmove->Sys_FloatTime = Sys_FloatTime;
	
	pmove->PM_StuckTouch = NULL; // TODO
	
	pmove->PM_PointContents = PM_PointContents;
	pmove->PM_TruePointContents = NULL; // TODO
	pmove->PM_HullPointContents = NULL; // TODO
	
	pmove->PM_PlayerTrace = NULL; // TODO: PM_PlayerTrace;
	pmove->PM_PlayerTraceEx = NULL; // TODO
	
	pmove->PM_TraceLine = NULL; // TODO
	pmove->PM_TraceLineEx = NULL; // TODO
	
	pmove->PM_TraceModel = NULL; // TODO
	
	pmove->PM_TraceTexture = NULL; // TODO
	
	pmove->RandomLong = NULL; // TODO: COM_RandomLong
	pmove->RandomFloat = NULL; // TODO: COM_RandomFloat
	
	pmove->PM_GetModelType = NULL; // TODO
	pmove->PM_GetModelBounds = NULL; // TODO
	
	pmove->PM_HullForBsp = NULL; // TODO
	
	pmove->COM_FileSize = NULL; // TODO: COM_FileSize/FS_FileSize?
	pmove->COM_LoadFile = COM_LoadFile;
	pmove->COM_FreeFile = COM_FreeFile;
	
	pmove->memfgets = NULL; // TODO
	
	pmove->PM_PlaySound = NULL; // TODO
	
	pmove->PM_PlaybackEventFull = NULL; // TODO
}