/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018 BlackPhrase
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
/// @brief player movement module definitions

#pragma once

#include "const.h"
#include "pm_info.h"
#include "pmtrace.h"
#include "usercmd.h"

#define PM_NORMAL 0x00000000

#define	MAX_PHYSENTS 600

typedef struct physent_s
{
	vec3_t	origin;
	struct model_s *model;		// only for bsp models
	vec3_t	mins, maxs;	// only for non-bsp models
	int		info;		// for client or server to identify
} physent_t;

typedef struct playermove_s
{
	qboolean server;
	
	float frametime;
	
	vec3_t forward, right, up;
	
	int		sequence;	// just for debugging prints

	// player state
	vec3_t	origin;
	vec3_t	angles;
	vec3_t	velocity;
	
	int usehull;
	
	int		oldbuttons;
	float		waterjumptime;
	qboolean	dead;
	int		spectator;
	
	int onground;
	int waterlevel;
	int watertype;
	int oldwaterlevel;
	
	float maxspeed;

	// world state
	int		numphysent;
	physent_t	physents[MAX_PHYSENTS];	// 0 should be the world

	// input
	usercmd_t	cmd;

	// results
	int		numtouch;
	int		touchindex[MAX_PHYSENTS];
	
	struct movevars_s *movevars;
	
	vec3_t player_mins[MAX_HULLS];
	vec3_t player_maxs[MAX_HULLS];
	
	// Common functions
	
	int (*PM_TestPlayerPosition)(float *pos, pmtrace_t *trace);
	
	int (*PM_PointContents)(float *pos, int *truecontents);
	
	pmtrace_t (*PM_PlayerTrace)(float *start, float *end, int traceFlags, int ignore_pe);
} playermove_t;