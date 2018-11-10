/*
 * This file is part of OGS Engine
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

#pragma once

#include "mathlib.h"

typedef struct ref_params_s
{
	vec3_t vieworg;
	vec3_t viewangles;
	
	vec3_t forward;
	vec3_t right;
	vec3_t up;
	
	float frametime;
	float time;
	
	int intermission;
	int paused;
	int spectator;
	int onground;
	int waterlevel;
	
	vec3_t simvel;
	vec3_t simorg;
	
	vec3_t viewheight;
	
	int idealpitch;
	
	vec3_t cl_viewangles;
	
	int health;
	
	vec3_t crosshairangle;
	
	float viewsize;
	
	vec3_t punchangle;
	
	int maxclients;
	int viewentity;
	int playernum;
	int max_entities;
	int demoplayback;
	int hardware;
	int smoothing;
	
	struct usercmd_s *cmd;
	struct movevars_s *movevars;
	
	int viewport[4];
	
	int nextView;
	
	int onlyClientDraw;
} ref_params_t;