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

#include "const.h"

typedef struct
{
	int	pad[28];
	int	world;
	float	time;
	float	frametime;
	float	force_retouch;
	string_t	mapname;
	string_t	startspot;
	float	deathmatch;
	float	coop;
	float	teamplay;
	float	serverflags;
	
	float	total_secrets;
	float	total_monsters;
	
	float	found_secrets; // number of secrets found
	float	killed_monsters; // number of monsters killed
	
	// spawnparms are used to encode information about clients across server level changes
	float	parm1;
	float	parm2;
	float	parm3;
	float	parm4;
	float	parm5;
	float	parm6;
	float	parm7;
	float	parm8;
	float	parm9;
	float	parm10;
	float	parm11;
	float	parm12;
	float	parm13;
	float	parm14;
	float	parm15;
	float	parm16;
	
	// set by makevectors()
	vec3_t	v_forward;
	vec3_t	v_up;
	vec3_t	v_right;
	
	// set by traceline / tracebox
	float	trace_allsolid;
	float	trace_startsolid;
	float	trace_fraction;
	vec3_t	trace_endpos;
	vec3_t	trace_plane_normal;
	float	trace_plane_dist;
	int	trace_ent;
	float	trace_inopen;
	float	trace_inwater;
	
	int	msg_entity; // destination of single entity writes
	
	//string_t	null;
} globalvars_t;

typedef struct
{
	string_t	classname; // spawn function
	
	vec3_t	origin; // ***
	vec3_t	oldorigin; // ***
	vec3_t	velocity;
	vec3_t	angles;
	vec3_t	avelocity;
	vec3_t	punchangle;
	
	float	modelindex; // *** model index in the precached list
	string_t	model;
	
	string_t	weaponmodel;
	
	vec3_t	absmin; // *** origin + mins / maxs
	vec3_t	absmax;
	vec3_t	mins; // bounding box extents relative to origin
	vec3_t	maxs;
	vec3_t	size; // maxs - mins
	
	float	ltime; // local time for entity
	float	nextthink;
	
	float	movetype;
	float	solid;
	
	float	skin;
	float	effects;
	
	float	gravity;
	int	groundentity;
	
	float	frame;
	
	float	health;
	float	frags;
	float	weapon; // one of the IT_SHOTGUN, etc flags
	float	weaponframe;
	float	currentammo;
	float	takedamage;
	
	int	deadflag;
	int	chain;
	float	fixangle;
	vec3_t	v_angle;
	float	idealpitch;
	int	enemy;
	vec3_t	view_ofs; // add to origin to get eye point
	
	int	button0; // fire
	int	button1; // use
	int	button2; // jump
	int	impulse; // weapon changes
	float	flags;
	float	colormap;
	float	team;
	
	float	max_health; // players maximum health is stored here
	float	teleport_time; // don't back up
	
	float	armortype; // save this fraction of incoming damage
	float	armorvalue;
	float	ideal_yaw;
	float	yaw_speed;
	int	aiment;
	int	goalentity;
	float	spawnflags;
	float	waterlevel; // 0 = not in, 1 = feet, 2 = wast, 3 = eyes
	float	watertype; // a contents value
	
	string_t	target;
	string_t	targetname;
	string_t	netname;
	string_t	message; // trigger messages
	
	float	dmg_take;
	float	dmg_save;
	int	dmg_inflictor;
	int	owner;
	vec3_t	movedir;
	float	sounds;
	string_t	noise;
	string_t	noise1;
	string_t	noise2;
	string_t	noise3;
} entvars_t;
#endif	//float	ammo_shells;
	///////////////////////////////////
	
	//float	ammo_nails;
	//float	ammo_rockets;
	//float	ammo_cells;
	
	//float	items; // bit flags