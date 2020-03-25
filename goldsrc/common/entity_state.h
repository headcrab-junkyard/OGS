/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019 BlackPhrase
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

#include "mathlib.h"
#include "weaponinfo.h"

#define MAX_WEAPONS 64

/// entity_state_t is the information conveyed from the server
/// in an update message
typedef struct entity_state_s
{
	int		number;			///< edict index
	
	int messagenum; // all player's won't be updated each frame
	
	vec3_t	origin;
	vec3_t	angles;
	
	int		modelindex;
	int		frame;
	int		colormap;
	int		skin;
	int		effects;
	
	byte	eflags;			///< nolerp, etc
	
	int movetype;
	vec3_t velocity;
	
	int oldbuttons;
	int onground; // -1 = in air, else pmove entity number
} entity_state_t;

typedef struct clientdata_s
{
	vec3_t origin;
	vec3_t velocity;
	
	int flags; // dead, gib, etc
	int waterlevel;
	int watertype;
	vec3_t view_ofs;
	float health; // cl.stats[STAT_HEALTH]
	
	float waterjumptime;
} clientdata_t;

typedef struct local_state_s
{
	entity_state_t playerstate;
	clientdata_t client;
	weapon_data_t weapondata[MAX_WEAPONS];
} local_state_t;