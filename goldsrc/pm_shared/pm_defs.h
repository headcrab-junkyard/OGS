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

#define	MAX_PHYSENTS 600

#define PM_NORMAL 0x00000000

typedef struct physent_s
{
	vec3_t	origin;
	struct model_s *model;		// only for bsp models
	//struct model_s *studiomodel ///< SOLID_BBOX, but studio clip intersections
	vec3_t	mins, maxs;	// only for non-bsp models
	int		info;		// for client or server to identify
	//vec3_t angles; // rotated entities need this info for hull testing to work
} physent_t;

typedef struct playermove_s
{
	//int player_index;
	
	qboolean server;
	//qboolean multiplayer;
	
	//float time;
	float frametime;
	
	vec3_t forward, right, up; // vectors for angles
	
	//int		sequence;	// just for debugging prints // UNUSED

	// player state
	vec3_t	origin;
	vec3_t	angles;
	//vec3_t oldangles;
	vec3_t	velocity;
	vec3_t	movedir;
	//vec3_t basevelocity;
	
	// For ducking/dead
	vec3_t view_ofs; ///< Our eye position
	
	//int effects;
	
	int flags;
	int usehull;
	//float gravity;
	//float friction;
	int		oldbuttons;
	float		waterjumptime;
	qboolean	dead;
	//int deadflag;
	int		spectator;
	int movetype;
	
	int onground;
	int waterlevel;
	int watertype;
	int oldwaterlevel;
	
	//char sztexturename[256];
	//char chtexturetype;
	
	float maxspeed;
	//float clientmaxspeed; ///< Player-specific maxspeed

	// world state
	int		numphysent; ///< Number of entities to clip against
	physent_t	physents[MAX_PHYSENTS];	// 0 should be the world

	//int nummoveent;
	//physent_t moveents[MAX_MOVEENTS]; ///< Just a list of ladders
	
	//int numvisent;
	//physent_t visents[MAX_PHYSENTS];
	
	// input
	usercmd_t	cmd;

	// results
	int		numtouch;
	pmtrace_t touchindex[MAX_PHYSENTS];
	
	//char physinfo[MAX_PHYSINFO_STRING]; // physics info string
	
	struct movevars_s *movevars;
	
	vec3_t player_mins[MAX_HULLS];
	vec3_t player_maxs[MAX_HULLS];
	
	// Common functions
	
	int (*PM_TestPlayerPosition)(float *pos, pmtrace_t *trace);
	
	int (*PM_PointContents)(float *pos, int *truecontents);
	
	pmtrace_t (*PM_PlayerTrace)(float *start, float *end, int traceFlags, int ignore_pe);
	
	//qboolean runfuncs; ///< Run functions for this frame?
	void (*PM_PlaySound)(int channel, const char *sample, float volume, float attentuation, int nFlags, int pitch);
	const char *(*PM_TraceTexture)(int ground, float *vstart, float *vend);
	void (*PM_PlaybackEventFull)(int flags, int clientindex, unsigned short eventindex, float delay,
		float *origin, float *angles,
		float fparam1, float fparam2,
		int iparam1, int param2,
		qboolean bparam1, qboolean bparam2);

	pmtrace_t (*PM_PlayerTraceEx)();
	int (*PM_TestPlayerPositionEx)();
	pmtrace_t (*PM_TraceLineEx)();
} playermove_t;