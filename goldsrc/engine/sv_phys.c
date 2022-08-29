/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019, 2021-2022 BlackPhrase
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

/*


pushmove objects do not obey gravity, and do not interact with each other or trigger fields, but block normal movement and push normal objects when they move.

onground is set for toss objects when they come to a complete rest.  it is set for steping or walking objects 

doors, plats, etc are SOLID_BSP, and MOVETYPE_PUSH
bonus items are SOLID_TRIGGER touch, and MOVETYPE_TOSS
corpses are SOLID_NOT and MOVETYPE_TOSS
crates are SOLID_BBOX and MOVETYPE_TOSS
walking monsters are SOLID_SLIDEBOX and MOVETYPE_STEP
flying/floating monsters are SOLID_SLIDEBOX and MOVETYPE_FLY

solid_edge items only clip against bsp models.

*/

cvar_t sv_friction = { "sv_friction", "4", FCVAR_SERVER };
cvar_t sv_waterfriction = { "sv_waterfriction", "4", FCVAR_SERVER}; // TODO: set to 1?

cvar_t sv_stopspeed = { "sv_stopspeed", "100", FCVAR_SERVER };
cvar_t sv_gravity = { "sv_gravity", "800", FCVAR_SERVER };
cvar_t sv_maxvelocity = { "sv_maxvelocity", "2000" };

extern cvar_t sv_stepsize; // TODO: move here

cvar_t sv_bounce = {"sv_bounce", "1", FCVAR_SERVER};

extern cvar_t sv_maxspeed;
cvar_t	sv_spectatormaxspeed = { "sv_spectatormaxspeed", "500"};

extern cvar_t sv_accelerate;
cvar_t	sv_airaccelerate	 = { "sv_airaccelerate", "0.7", FCVAR_SERVER}; // TODO: set to 10?
cvar_t	sv_wateraccelerate	 = { "sv_wateraccelerate", "10", FCVAR_SERVER};

cvar_t sv_zmax = {"sv_zmax", "4096"};

cvar_t sv_wateramp = {"sv_wateramp", "0"};

cvar_t sv_skyname = {"sv_skyname", "desert"};

extern cvar_t sv_zmax;
extern cvar_t sv_wateramp;

extern cvar_t mp_footsteps;

extern cvar_t sv_skyname;

extern cvar_t sv_skycolor_r;
extern cvar_t sv_skycolor_g;
extern cvar_t sv_skycolor_b;

extern cvar_t sv_skyvec_x;
extern cvar_t sv_skyvec_y;
extern cvar_t sv_skyvec_z;

cvar_t sv_nostep = { "sv_nostep", "0" }; // TODO: remove?

#define MOVE_EPSILON 0.01

void SV_Physics_Toss(edict_t *ent);

/*
================
SV_CheckAllEnts
================
*/
void SV_CheckAllEnts()
{
	int e;
	edict_t *check;

	// see if any solid entities are inside the final position
	check = NEXT_EDICT(sv.edicts);
	for(e = 1; e < sv.num_edicts; e++, check = NEXT_EDICT(check))
	{
		if(check->free)
			continue;
		if(check->v.movetype == MOVETYPE_PUSH || check->v.movetype == MOVETYPE_NONE
		   || check->v.movetype == MOVETYPE_FOLLOW // TODO: not present in qw
		   || check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		if(SV_TestEntityPosition(check))
			Con_Printf("entity in invalid position\n");
	}
}

/*
================
SV_CheckVelocity
================
*/
void SV_CheckVelocity(edict_t *ent)
{
	int i;

	//
	// bound velocity
	//
	for(i = 0; i < 3; i++)
	{
		if(IS_NAN(ent->v.velocity[i]))
		{
			Con_Printf("Got a NaN velocity on %s\n", PR_GetString(ent->v.classname));
			ent->v.velocity[i] = 0;
		}
		if(IS_NAN(ent->v.origin[i]))
		{
			Con_Printf("Got a NaN origin on %s\n", PR_GetString(ent->v.classname));
			ent->v.origin[i] = 0;
		}
		if(ent->v.velocity[i] > sv_maxvelocity.value)
			ent->v.velocity[i] = sv_maxvelocity.value;
		else if(ent->v.velocity[i] < -sv_maxvelocity.value)
			ent->v.velocity[i] = -sv_maxvelocity.value;
	}
}

/*
=============
SV_RunThink

Runs thinking code if time.  There is some play in the exact time the think
function will be called, because it is called before any movement is done
in a frame.  Not used for pushmove objects, because they must be exact.
Returns false if the entity removed itself.
=============
*/
qboolean SV_RunThink(edict_t *ent)
{
	float thinktime;

	// TODO: qw
//
/*
	do
	{
		thinktime = ent->v.nextthink;
		if (thinktime <= 0)
			return true;
		if (thinktime > sv.time + host_frametime)
			return true;
		
		if (thinktime < sv.time)
			thinktime = sv.time;	// don't let things stay in the past.
									// it is possible to start that way
									// by a trigger with a local time.
		ent->v.nextthink = 0;
		pr_global_struct->time = thinktime;
		PR_ExecuteProgram (ent->v.think);

		if (ent->free)
			return false;
	} while (1);

	return true;
*/
//
	thinktime = ent->v.nextthink;
	if(thinktime <= 0 || thinktime > sv.time + host_frametime)
		return true;

	if(thinktime < sv.time)
		thinktime = sv.time; // don't let things stay in the past.
	                         // it is possible to start that way
	                         // by a trigger with a local time.
	ent->v.nextthink = 0;
	gGlobalVariables.time = thinktime;
	gEntityInterface.pfnThink(ent);
	return !ent->free;
//
}

/*
==================
SV_Impact

Two entities have touched, so run their touch functions
==================
*/
void SV_Impact(edict_t *e1, edict_t *e2)
{
	gGlobalVariables.time = sv.time;
	
	// TODO: should there be only one call to the pfnShouldCollide func?
	
	if(e1->v.solid != SOLID_NOT)
	{
		if(gNewDLLFunctions.pfnShouldCollide)
			if(!gNewDLLFunctions.pfnShouldCollide(e1, e2))
				return;
		
		gEntityInterface.pfnTouch(e1, e2);
	};

	if(e2->v.solid != SOLID_NOT)
	{
		if(gNewDLLFunctions.pfnShouldCollide)
			if(!gNewDLLFunctions.pfnShouldCollide(e2, e1))
				return;
		
		gEntityInterface.pfnTouch(e2, e1);
	};
}

/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
#define STOP_EPSILON 0.1

int ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
	float backoff;
	float change;
	int i, blocked;

	blocked = 0;
	if(normal[2] > 0)
		blocked |= 1; // floor
	if(!normal[2])
		blocked |= 2; // step

	backoff = DotProduct(in, normal) * overbounce;

	for(i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if(out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}

/*
============
SV_FlyMove

The basic solid body movement clip that slides along multiple planes
Returns the clipflags if the velocity was modified (hit something solid)
1 = floor
2 = wall / step
4 = dead stop
If steptrace is not NULL, the trace of any vertical wall hit will be stored
============
*/
#define MAX_CLIP_PLANES 5
int SV_FlyMove(edict_t *ent, float time, trace_t *steptrace)
{
	int bumpcount, numbumps;
	vec3_t dir;
	float d;
	int numplanes;
	vec3_t planes[MAX_CLIP_PLANES];
	vec3_t primal_velocity, original_velocity, new_velocity;
	int i, j;
	trace_t trace;
	vec3_t end;
	float time_left;
	int blocked;

	numbumps = 4;

	blocked = 0;
	VectorCopy(ent->v.velocity, original_velocity);
	VectorCopy(ent->v.velocity, primal_velocity);
	numplanes = 0;

	time_left = time;

	for(bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if(!ent->v.velocity[0] && !ent->v.velocity[1] && !ent->v.velocity[2])
			break;

		for(i = 0; i < 3; i++)
			end[i] = ent->v.origin[i] + time_left * ent->v.velocity[i];

		trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, false, ent);

		if(trace.allsolid)
		{ // entity is trapped in another solid
			VectorCopy(vec3_origin, ent->v.velocity);
			return 3;
		}

		if(trace.fraction > 0)
		{ // actually covered some distance
			VectorCopy(trace.endpos, ent->v.origin);
			VectorCopy(ent->v.velocity, original_velocity);
			numplanes = 0;
		}

		if(trace.fraction == 1)
			break; // moved the entire distance

		if(!trace.ent)
			Sys_Error("SV_FlyMove: !trace.ent");

		if(trace.plane.normal[2] > 0.7)
		{
			blocked |= 1; // floor
			if(trace.ent->v.solid == SOLID_BSP)
			{
				ent->v.flags = (int)ent->v.flags | FL_ONGROUND;
				ent->v.groundentity = trace.ent;
			}
		}
		if(!trace.plane.normal[2])
		{
			blocked |= 2; // step
			if(steptrace)
				*steptrace = trace; // save for player extrafriction
		}

		//
		// run the impact function
		//
		SV_Impact(ent, trace.ent);
		if(ent->free)
			break; // removed by the impact function

		time_left -= time_left * trace.fraction;

		// cliped to another plane
		if(numplanes >= MAX_CLIP_PLANES)
		{ // this shouldn't really happen
			VectorCopy(vec3_origin, ent->v.velocity);
			return 3;
		}

		VectorCopy(trace.plane.normal, planes[numplanes]);
		numplanes++;

		//
		// modify original_velocity so it parallels all of the clip planes
		//
		for(i = 0; i < numplanes; i++)
		{
			ClipVelocity(original_velocity, planes[i], new_velocity, 1);
			for(j = 0; j < numplanes; j++)
				if(j != i)
				{
					if(DotProduct(new_velocity, planes[j]) < 0)
						break; // not ok
				}
			if(j == numplanes)
				break;
		}

		if(i != numplanes)
		{ // go along this plane
			VectorCopy(new_velocity, ent->v.velocity);
		}
		else
		{ // go along the crease
			if(numplanes != 2)
			{
				//				Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
				VectorCopy(vec3_origin, ent->v.velocity);
				return 7;
			}
			CrossProduct(planes[0], planes[1], dir);
			d = DotProduct(dir, ent->v.velocity);
			VectorScale(dir, d, ent->v.velocity);
		}

		//
		// if original velocity is against the original velocity, stop dead
		// to avoid tiny occilations in sloping corners
		//
		if(DotProduct(ent->v.velocity, primal_velocity) <= 0)
		{
			VectorCopy(vec3_origin, ent->v.velocity);
			return blocked;
		}
	}

	return blocked;
}

/*
============
SV_AddGravity

============
*/
// TODO: void SV_AddGravity(edict_t *ent, float scale) in qw
void SV_AddGravity(edict_t *ent)
{
	float ent_gravity = 1.0;

	if(ent->v.gravity)
		ent_gravity = ent->v.gravity;

	ent->v.velocity[2] -= ent_gravity * movevars.gravity * host_frametime;
	// TODO: ent->v.velocity[2] -= scale * movevars.gravity * host_frametime; in qw
}

/*
===============================================================================

PUSHMOVE

===============================================================================
*/

/*
============
SV_PushEntity

Does not change the entities velocity at all
============
*/
trace_t SV_PushEntity(edict_t *ent, vec3_t push)
{
	trace_t trace;
	vec3_t end;

	VectorAdd(ent->v.origin, push, end);

	if(ent->v.movetype == MOVETYPE_FLYMISSILE)
		trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_MISSILE, ent);
	else if(ent->v.solid == SOLID_TRIGGER || ent->v.solid == SOLID_NOT)
		// only clip against bmodels
		trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_NOMONSTERS, ent);
	else
		trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent);

	VectorCopy(trace.endpos, ent->v.origin);
	SV_LinkEdict(ent, true);

	if(trace.ent)
		SV_Impact(ent, trace.ent);

	return trace;
}

/*
============
SV_PushMove

============
*/
void SV_PushMove(edict_t *pusher, float movetime)
{
	int i, e;
	edict_t *check, *block;
	vec3_t mins, maxs, move;
	vec3_t entorig, pushorig;
	int num_moved;
	edict_t *moved_edict[MAX_EDICTS];
	vec3_t moved_from[MAX_EDICTS];

	if(!pusher->v.velocity[0] && !pusher->v.velocity[1] && !pusher->v.velocity[2])
	{
		pusher->v.ltime += movetime;
		return;
	}

	for(i = 0; i < 3; i++)
	{
		move[i] = pusher->v.velocity[i] * movetime;
		mins[i] = pusher->v.absmin[i] + move[i];
		maxs[i] = pusher->v.absmax[i] + move[i];
	}

	VectorCopy(pusher->v.origin, pushorig);

	// move the pusher to it's final position

	VectorAdd(pusher->v.origin, move, pusher->v.origin);
	pusher->v.ltime += movetime;
	SV_LinkEdict(pusher, false);

	// see if any solid entities are inside the final position
	num_moved = 0;
	check = NEXT_EDICT(sv.edicts);
	for(e = 1; e < sv.num_edicts; e++, check = NEXT_EDICT(check))
	{
		if(check->free)
			continue;
		if(check->v.movetype == MOVETYPE_PUSH || check->v.movetype == MOVETYPE_NONE
		   || check->v.movetype == MOVETYPE_FOLLOW
		   || check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		// if the entity is standing on the pusher, it will definately be moved
		if(!(((int)check->v.flags & FL_ONGROUND) && check->v.groundentity == pusher))
		{
			if(check->v.absmin[0] >= maxs[0] || check->v.absmin[1] >= maxs[1] || check->v.absmin[2] >= maxs[2] || check->v.absmax[0] <= mins[0] || check->v.absmax[1] <= mins[1] || check->v.absmax[2] <= mins[2])
				continue;

			// see if the ent's bbox is inside the pusher's final position
			if(!SV_TestEntityPosition(check))
				continue;
		}

		// remove the onground flag for non-players
		if(check->v.movetype != MOVETYPE_WALK)
			check->v.flags = (int)check->v.flags & ~FL_ONGROUND;

		VectorCopy(check->v.origin, entorig);
		VectorCopy(check->v.origin, moved_from[num_moved]);
		moved_edict[num_moved] = check;
		num_moved++;

		// try moving the contacted entity
		pusher->v.solid = SOLID_NOT;
		SV_PushEntity(check, move);
		pusher->v.solid = SOLID_BSP;

		// if it is still inside the pusher, block
		block = SV_TestEntityPosition(check);
		if(block)
		{ // fail the move
			if(check->v.mins[0] == check->v.maxs[0])
				continue;
			if(check->v.solid == SOLID_NOT || check->v.solid == SOLID_TRIGGER)
			{ // corpse
				check->v.mins[0] = check->v.mins[1] = 0;
				VectorCopy(check->v.mins, check->v.maxs);
				continue;
			}

			VectorCopy(entorig, check->v.origin);
			SV_LinkEdict(check, true);

			VectorCopy(pushorig, pusher->v.origin);
			SV_LinkEdict(pusher, false);
			pusher->v.ltime -= movetime;

			// if the pusher has a "blocked" function, call it
			// otherwise, just stay in place until the obstacle is gone
			if(gEntityInterface.pfnBlocked)
			{
				gEntityInterface.pfnBlocked(pusher, check);
			}

			// move back any entities we already moved
			for(i = 0; i < num_moved; i++)
			{
				VectorCopy(moved_from[i], moved_edict[i]->v.origin);
				SV_LinkEdict(moved_edict[i], false);
			}
			return;
		}
	}
}

/*
============
SV_PushRotate

============
*/
void SV_PushRotate(edict_t *pusher, float movetime)
{
	int i, e;
	edict_t *check, *block;
	vec3_t move, a, amove;
	vec3_t entorig, pushorig;
	int num_moved;
	edict_t *moved_edict[MAX_EDICTS];
	vec3_t moved_from[MAX_EDICTS];
	vec3_t org, org2;
	vec3_t forward, right, up;

	if(!pusher->v.avelocity[0] && !pusher->v.avelocity[1] && !pusher->v.avelocity[2])
	{
		pusher->v.ltime += movetime;
		return;
	}

	for(i = 0; i < 3; i++)
		amove[i] = pusher->v.avelocity[i] * movetime;

	VectorSubtract(vec3_origin, amove, a);
	AngleVectors(a, forward, right, up);

	VectorCopy(pusher->v.angles, pushorig);

	// move the pusher to it's final position

	VectorAdd(pusher->v.angles, amove, pusher->v.angles);
	pusher->v.ltime += movetime;
	SV_LinkEdict(pusher, false);

	// see if any solid entities are inside the final position
	num_moved = 0;
	check = NEXT_EDICT(sv.edicts);
	for(e = 1; e < sv.num_edicts; e++, check = NEXT_EDICT(check))
	{
		if(check->free)
			continue;
		if(check->v.movetype == MOVETYPE_PUSH || check->v.movetype == MOVETYPE_NONE || check->v.movetype == MOVETYPE_FOLLOW || check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		// if the entity is standing on the pusher, it will definately be moved
		if(!(((int)check->v.flags & FL_ONGROUND) && check->v.groundentity == pusher))
		{
			if(check->v.absmin[0] >= pusher->v.absmax[0] || check->v.absmin[1] >= pusher->v.absmax[1] || check->v.absmin[2] >= pusher->v.absmax[2] || check->v.absmax[0] <= pusher->v.absmin[0] || check->v.absmax[1] <= pusher->v.absmin[1] || check->v.absmax[2] <= pusher->v.absmin[2])
				continue;

			// see if the ent's bbox is inside the pusher's final position
			if(!SV_TestEntityPosition(check))
				continue;
		}

		// remove the onground flag for non-players
		if(check->v.movetype != MOVETYPE_WALK)
			check->v.flags = (int)check->v.flags & ~FL_ONGROUND;

		VectorCopy(check->v.origin, entorig);
		VectorCopy(check->v.origin, moved_from[num_moved]);
		moved_edict[num_moved] = check;
		num_moved++;

		// calculate destination position
		VectorSubtract(check->v.origin, pusher->v.origin, org);
		org2[0] = DotProduct(org, forward);
		org2[1] = -DotProduct(org, right);
		org2[2] = DotProduct(org, up);
		VectorSubtract(org2, org, move);

		// try moving the contacted entity
		pusher->v.solid = SOLID_NOT;
		SV_PushEntity(check, move);
		pusher->v.solid = SOLID_BSP;

		// if it is still inside the pusher, block
		block = SV_TestEntityPosition(check);
		if(block)
		{ // fail the move
			if(check->v.mins[0] == check->v.maxs[0])
				continue;
			if(check->v.solid == SOLID_NOT || check->v.solid == SOLID_TRIGGER)
			{ // corpse
				check->v.mins[0] = check->v.mins[1] = 0;
				VectorCopy(check->v.mins, check->v.maxs);
				continue;
			}

			VectorCopy(entorig, check->v.origin);
			SV_LinkEdict(check, true);

			VectorCopy(pushorig, pusher->v.angles);
			SV_LinkEdict(pusher, false);
			pusher->v.ltime -= movetime;

			// if the pusher has a "blocked" function, call it
			// otherwise, just stay in place until the obstacle is gone
			gEntityInterface.pfnBlocked(pusher, check);

			// move back any entities we already moved
			for(i = 0; i < num_moved; i++)
			{
				VectorCopy(moved_from[i], moved_edict[i]->v.origin);
				VectorSubtract(moved_edict[i]->v.angles, amove, moved_edict[i]->v.angles);
				SV_LinkEdict(moved_edict[i], false);
			}
			return;
		}
		else
		{
			VectorAdd(check->v.angles, amove, check->v.angles);
		}
	}
}

/*
================
SV_Physics_Pusher

================
*/
void SV_Physics_Pusher(edict_t *ent)
{
	float thinktime;
	float oldltime;
	float movetime;

	oldltime = ent->v.ltime;

	thinktime = ent->v.nextthink;
	if(thinktime < ent->v.ltime + host_frametime)
	{
		movetime = thinktime - ent->v.ltime;
		if(movetime < 0)
			movetime = 0;
	}
	else
		movetime = host_frametime;

	if(movetime)
	{
		if(ent->v.avelocity[0] || ent->v.avelocity[1] || ent->v.avelocity[2])
			SV_PushRotate(ent, movetime);
		else
			SV_PushMove(ent, movetime); // advances ent->v.ltime if not blocked
	}

	if(thinktime > oldltime && thinktime <= ent->v.ltime)
	{
		ent->v.nextthink = 0;
		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnThink(ent);
		if(ent->free)
			return;
	}
}

/*
===============================================================================

CLIENT MOVEMENT

===============================================================================
*/

/*
=============
SV_CheckStuck

This is a big hack to try and fix the rare case of getting stuck in the world
clipping hull.
=============
*/
void SV_CheckStuck(edict_t *ent)
{
	int i, j;
	int z;
	vec3_t org;

	if(!SV_TestEntityPosition(ent))
	{
		VectorCopy(ent->v.origin, ent->v.oldorigin);
		return;
	}

	VectorCopy(ent->v.origin, org);
	VectorCopy(ent->v.oldorigin, ent->v.origin);
	if(!SV_TestEntityPosition(ent))
	{
		Con_DPrintf("Unstuck.\n");
		SV_LinkEdict(ent, true);
		return;
	}

	for(z = 0; z < 18; z++)
		for(i = -1; i <= 1; i++)
			for(j = -1; j <= 1; j++)
			{
				ent->v.origin[0] = org[0] + i;
				ent->v.origin[1] = org[1] + j;
				ent->v.origin[2] = org[2] + z;
				if(!SV_TestEntityPosition(ent))
				{
					Con_DPrintf("Unstuck.\n");
					SV_LinkEdict(ent, true);
					return;
				}
			}

	VectorCopy(org, ent->v.origin);
	Con_DPrintf("player is stuck.\n");
}

/*
=============
SV_CheckWater
=============
*/
qboolean SV_CheckWater(edict_t *ent)
{
	vec3_t point;
	int cont;
#ifdef QUAKE2
	int truecont;
#endif

	point[0] = ent->v.origin[0];
	point[1] = ent->v.origin[1];
	point[2] = ent->v.origin[2] + ent->v.mins[2] + 1;

	ent->v.waterlevel = 0;
	ent->v.watertype = CONTENTS_EMPTY;
	cont = SV_PointContents(point);
	if(cont <= CONTENTS_WATER)
	{
#ifdef QUAKE2
		truecont = SV_TruePointContents(point);
#endif
		ent->v.watertype = cont;
		ent->v.waterlevel = 1;
		point[2] = ent->v.origin[2] + (ent->v.mins[2] + ent->v.maxs[2]) * 0.5;
		cont = SV_PointContents(point);
		if(cont <= CONTENTS_WATER)
		{
			ent->v.waterlevel = 2;
			point[2] = ent->v.origin[2] + ent->v.view_ofs[2];
			cont = SV_PointContents(point);
			if(cont <= CONTENTS_WATER)
				ent->v.waterlevel = 3;
		}
#ifdef QUAKE2
		if(truecont <= CONTENTS_CURRENT_0 && truecont >= CONTENTS_CURRENT_DOWN)
		{
			static vec3_t current_table[] =
			{
			  { 1, 0, 0 },
			  { 0, 1, 0 },
			  { -1, 0, 0 },
			  { 0, -1, 0 },
			  { 0, 0, 1 },
			  { 0, 0, -1 }
			};

			VectorMA(ent->v.basevelocity, 150.0 * ent->v.waterlevel / 3.0, current_table[CONTENTS_CURRENT_0 - truecont], ent->v.basevelocity);
		}
#endif
	}

	return ent->v.waterlevel > 1;
}

/*
============
SV_WallFriction

============
*/
void SV_WallFriction(edict_t *ent, trace_t *trace)
{
	vec3_t forward, right, up;
	float d, i;
	vec3_t into, side;

	AngleVectors(ent->v.v_angle, forward, right, up);
	d = DotProduct(trace->plane.normal, forward);

	d += 0.5;
	if(d >= 0)
		return;

	// cut the tangential velocity
	i = DotProduct(trace->plane.normal, ent->v.velocity);
	VectorScale(trace->plane.normal, i, into);
	VectorSubtract(ent->v.velocity, into, side);

	ent->v.velocity[0] = side[0] * (1 + d);
	ent->v.velocity[1] = side[1] * (1 + d);
}

/*
=====================
SV_TryUnstick

Player has come to a dead stop, possibly due to the problem with limited
float precision at some angle joins in the BSP hull.

Try fixing by pushing one pixel in each direction.

This is a hack, but in the interest of good gameplay...
======================
*/
int SV_TryUnstick(edict_t *ent, vec3_t oldvel)
{
	int i;
	vec3_t oldorg;
	vec3_t dir;
	int clip;
	trace_t steptrace;

	VectorCopy(ent->v.origin, oldorg);
	VectorCopy(vec3_origin, dir);

	for(i = 0; i < 8; i++)
	{
		// try pushing a little in an axial direction
		switch(i)
		{
		case 0:
			dir[0] = 2;
			dir[1] = 0;
			break;
		case 1:
			dir[0] = 0;
			dir[1] = 2;
			break;
		case 2:
			dir[0] = -2;
			dir[1] = 0;
			break;
		case 3:
			dir[0] = 0;
			dir[1] = -2;
			break;
		case 4:
			dir[0] = 2;
			dir[1] = 2;
			break;
		case 5:
			dir[0] = -2;
			dir[1] = 2;
			break;
		case 6:
			dir[0] = 2;
			dir[1] = -2;
			break;
		case 7:
			dir[0] = -2;
			dir[1] = -2;
			break;
		}

		SV_PushEntity(ent, dir);

		// retry the original move
		ent->v.velocity[0] = oldvel[0];
		ent->v.velocity[1] = oldvel[1];
		ent->v.velocity[2] = 0;
		clip = SV_FlyMove(ent, 0.1, &steptrace);

		if(fabs(oldorg[1] - ent->v.origin[1]) > 4 || fabs(oldorg[0] - ent->v.origin[0]) > 4)
		{
			//Con_DPrintf ("unstuck!\n");
			return clip;
		}

		// go back to the original pos and try again
		VectorCopy(oldorg, ent->v.origin);
	}

	VectorCopy(vec3_origin, ent->v.velocity);
	return 7; // still not moving
}

/*
=====================
SV_WalkMove

Only used by players
======================
*/
#define STEPSIZE 18
void SV_WalkMove(edict_t *ent)
{
	vec3_t upmove, downmove;
	vec3_t oldorg, oldvel;
	vec3_t nosteporg, nostepvel;
	int clip;
	int oldonground;
	trace_t steptrace, downtrace;

	//
	// do a regular slide move unless it looks like you ran into a step
	//
	oldonground = (int)ent->v.flags & FL_ONGROUND;
	ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;

	VectorCopy(ent->v.origin, oldorg);
	VectorCopy(ent->v.velocity, oldvel);

	clip = SV_FlyMove(ent, host_frametime, &steptrace);

	if(!(clip & 2))
		return; // move didn't block on a step

	if(!oldonground && ent->v.waterlevel == 0)
		return; // don't stair up while jumping

	if(ent->v.movetype != MOVETYPE_WALK)
		return; // gibbed by a trigger

	if(sv_nostep.value)
		return;

	if((int)sv_player->v.flags & FL_WATERJUMP)
		return;

	VectorCopy(ent->v.origin, nosteporg);
	VectorCopy(ent->v.velocity, nostepvel);

	//
	// try moving up and forward to go up a step
	//
	VectorCopy(oldorg, ent->v.origin); // back to start pos

	VectorCopy(vec3_origin, upmove);
	VectorCopy(vec3_origin, downmove);
	upmove[2] = sv_stepsize.value;
	downmove[2] = -sv_stepsize.value + oldvel[2] * host_frametime;

	// move up
	SV_PushEntity(ent, upmove); // FIXME: don't link?

	// move forward
	ent->v.velocity[0] = oldvel[0];
	ent->v.velocity[1] = oldvel[1];
	ent->v.velocity[2] = 0;
	clip = SV_FlyMove(ent, host_frametime, &steptrace);

	// check for stuckness, possibly due to the limited precision of floats
	// in the clipping hulls
	if(clip)
	{
		if(fabs(oldorg[1] - ent->v.origin[1]) < 0.03125 && fabs(oldorg[0] - ent->v.origin[0]) < 0.03125)
		{ // stepping up didn't make any progress
			clip = SV_TryUnstick(ent, oldvel);
		}
	}

	// extra friction based on view angle
	if(clip & 2)
		SV_WallFriction(ent, &steptrace);

	// move down
	downtrace = SV_PushEntity(ent, downmove); // FIXME: don't link?

	if(downtrace.plane.normal[2] > 0.7)
	{
		if(ent->v.solid == SOLID_BSP)
		{
			ent->v.flags = (int)ent->v.flags | FL_ONGROUND;
			ent->v.groundentity = downtrace.ent;
		}
	}
	else
	{
		// if the push down didn't end up on good ground, use the move without
		// the step up.  This happens near wall / slope combinations, and can
		// cause the player to hop up higher on a slope too steep to climb
		VectorCopy(nosteporg, ent->v.origin);
		VectorCopy(nostepvel, ent->v.velocity);
	}
}

/*
================
SV_Physics_Client

Player character actions
================
*/
/*
void SV_Physics_Client(edict_t *ent, int num)
{
	if(!svs.clients[num - 1].active)
		return; // unconnected slot

	//
	// call standard client pre-think
	//
	gGlobalVariables.time = sv.time;
	gEntityInterface.pfnPlayerPreThink(ent);

	//
	// do a move
	//
	SV_CheckVelocity(ent);

	//
	// decide which move function to call
	//
	switch((int)ent->v.movetype)
	{
	case MOVETYPE_NONE:
		if(!SV_RunThink(ent))
			return;
		break;

	case MOVETYPE_WALK:
		if(!SV_RunThink(ent))
			return;
		if(!SV_CheckWater(ent) && !((int)ent->v.flags & FL_WATERJUMP))
			SV_AddGravity(ent);
		SV_CheckStuck(ent);
#ifdef QUAKE2
		VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
#endif
		SV_WalkMove(ent);

#ifdef QUAKE2
		VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
#endif
		break;

	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
		SV_Physics_Toss(ent);
		break;

	case MOVETYPE_FLY:
		if(!SV_RunThink(ent))
			return;
		SV_FlyMove(ent, host_frametime, NULL);
		break;

	case MOVETYPE_NOCLIP:
		if(!SV_RunThink(ent))
			return;
		VectorMA(ent->v.origin, host_frametime, ent->v.velocity, ent->v.origin);
		break;

	default:
		Sys_Error("SV_Physics_client: bad movetype %i", (int)ent->v.movetype);
	}

	//
	// call standard player post-think
	//
	SV_LinkEdict(ent, true);

	gGlobalVariables.time = sv.time;
	gEntityInterface.pfnPlayerPostThink(ent);
}
*/

//============================================================================

/*
=============
SV_Physics_None

Non moving objects can only think
=============
*/
void SV_Physics_None(edict_t *ent)
{
	// regular thinking
	SV_RunThink(ent);
}

/*
=============
SV_Physics_Follow

Entities that are "stuck" to another entity
=============
*/
void SV_Physics_Follow(edict_t *ent)
{
	// regular thinking
	SV_RunThink(ent);
	VectorAdd(ent->v.aiment->v.origin, ent->v.v_angle, ent->v.origin);
	SV_LinkEdict(ent, true);
}

/*
=============
SV_Physics_Noclip

A moving object that doesn't obey physics
=============
*/
void SV_Physics_Noclip(edict_t *ent)
{
	// regular thinking
	if(!SV_RunThink(ent))
		return;

	VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);
	VectorMA(ent->v.origin, host_frametime, ent->v.velocity, ent->v.origin);

	SV_LinkEdict(ent, false);
}

/*
==============================================================================

TOSS / BOUNCE

==============================================================================
*/

/*
=============
SV_CheckWaterTransition

=============
*/
void SV_CheckWaterTransition(edict_t *ent)
{
	int cont;
#ifdef QUAKE2
	vec3_t point;

	point[0] = ent->v.origin[0];
	point[1] = ent->v.origin[1];
	point[2] = ent->v.origin[2] + ent->v.mins[2] + 1;
	cont = SV_PointContents(point);
#else
	cont = SV_PointContents(ent->v.origin);
#endif
	if(!ent->v.watertype)
	{ // just spawned here
		ent->v.watertype = cont;
		ent->v.waterlevel = 1;
		return;
	}

	if(cont <= CONTENTS_WATER)
	{
		if(ent->v.watertype == CONTENTS_EMPTY)
		{ // just crossed into water
			SV_StartSound(ent, 0, "misc/h2ohit1.wav", 255, 1, PITCH_NORM);
		}
		ent->v.watertype = cont;
		ent->v.waterlevel = 1;
	}
	else
	{
		if(ent->v.watertype != CONTENTS_EMPTY)
		{ // just crossed into water
			SV_StartSound(ent, 0, "misc/h2ohit1.wav", 255, 1, PITCH_NORM);
		}
		ent->v.watertype = CONTENTS_EMPTY;
		ent->v.waterlevel = cont;
	}
}

/*
=============
SV_Physics_Toss

Toss, bounce, and fly movement.  When onground, do nothing.
=============
*/
void SV_Physics_Toss(edict_t *ent)
{
	trace_t trace;
	vec3_t move;
	float backoff;
#ifdef QUAKE2
	edict_t *groundentity;

	groundentity = ent->v.groundentity;
	if((int)groundentity->v.flags & FL_CONVEYOR)
		VectorScale(groundentity->v.movedir, groundentity->v.speed, ent->v.basevelocity);
	else
		VectorCopy(vec3_origin, ent->v.basevelocity);
	SV_CheckWater(ent);
#endif
	// regular thinking
	if(!SV_RunThink(ent))
		return;

#ifdef QUAKE2
	if(ent->v.velocity[2] > 0)
		ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;

	if(((int)ent->v.flags & FL_ONGROUND))
		//@@
		if(VectorCompare(ent->v.basevelocity, vec3_origin))
			return;

	SV_CheckVelocity(ent);

	// add gravity
	if(!((int)ent->v.flags & FL_ONGROUND) && ent->v.movetype != MOVETYPE_FLY && ent->v.movetype != MOVETYPE_BOUNCEMISSILE && ent->v.movetype != MOVETYPE_FLYMISSILE)
		SV_AddGravity(ent);

#else
	// if onground, return without moving
	if(((int)ent->v.flags & FL_ONGROUND))
		return;

	SV_CheckVelocity(ent);

	// add gravity
	if(ent->v.movetype != MOVETYPE_FLY && ent->v.movetype != MOVETYPE_FLYMISSILE)
		SV_AddGravity(ent);
#endif

	// move angles
	VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);

// move origin
#ifdef QUAKE2
	VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
#endif
	VectorScale(ent->v.velocity, host_frametime, move);
	trace = SV_PushEntity(ent, move);
#ifdef QUAKE2
	VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
#endif
	if(trace.fraction == 1)
		return;
	if(ent->free)
		return;

	if(ent->v.movetype == MOVETYPE_BOUNCE)
		backoff = 1.5;
	else if(ent->v.movetype == MOVETYPE_BOUNCEMISSILE) // TODO: not present in qw
		backoff = 2.0;
	else
		backoff = 1;

	ClipVelocity(ent->v.velocity, trace.plane.normal, ent->v.velocity, backoff);

	// stop if on ground
	if(trace.plane.normal[2] > 0.7)
	{
		if(ent->v.velocity[2] < 60 || (ent->v.movetype != MOVETYPE_BOUNCE && ent->v.movetype != MOVETYPE_BOUNCEMISSILE)) // TODO: no bouncemissile support here in qw
		{
			ent->v.flags = (int)ent->v.flags | FL_ONGROUND;
			ent->v.groundentity = trace.ent; // TODO: was = EDICT_TO_PROG(trace.ent)
			VectorCopy(vec3_origin, ent->v.velocity);
			VectorCopy(vec3_origin, ent->v.avelocity);
		}
	}

	// check for in water
	SV_CheckWaterTransition(ent);
}

/*
===============================================================================

STEPPING MOVEMENT

===============================================================================
*/

/*
=============
SV_Physics_Step

Monsters freefall when they don't have a ground entity, otherwise
all movement is done with discrete steps.

This is also used for objects that have become still on the ground, but
will fall if the floor is pulled out from under them.
=============
*/
#ifdef QUAKE2
void SV_Physics_Step(edict_t *ent)
{
	qboolean wasonground;
	qboolean inwater;
	qboolean hitsound = false;
	float *vel;
	float speed, newspeed, control;
	float friction;
	edict_t *groundentity;

	groundentity = ent->v.groundentity;
	if((int)groundentity->v.flags & FL_CONVEYOR)
		VectorScale(groundentity->v.movedir, groundentity->v.speed, ent->v.basevelocity);
	else
		VectorCopy(vec3_origin, ent->v.basevelocity);
	//@@
	gGlobalVariables.time = sv.time;
	gGlobalVariables.self = EDICT_TO_PROG(ent);
	PF_WaterMove();

	SV_CheckVelocity(ent);

	wasonground = (int)ent->v.flags & FL_ONGROUND;
	//	ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;

	// add gravity except:
	//   flying monsters
	//   swimming monsters who are in the water
	inwater = SV_CheckWater(ent);
	if(!wasonground)
		if(!((int)ent->v.flags & FL_FLY))
			if(!(((int)ent->v.flags & FL_SWIM) && (ent->v.waterlevel > 0)))
			{
				if(ent->v.velocity[2] < sv_gravity.value * -0.1)
					hitsound = true;
				if(!inwater)
					SV_AddGravity(ent);
			}

	if(!VectorCompare(ent->v.velocity, vec3_origin) || !VectorCompare(ent->v.basevelocity, vec3_origin))
	{
		ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;
		// apply friction
		// let dead monsters who aren't completely onground slide
		if(wasonground)
			if(!(ent->v.health <= 0.0 && !SV_CheckBottom(ent)))
			{
				vel = ent->v.velocity;
				speed = sqrt(vel[0] * vel[0] + vel[1] * vel[1]);
				if(speed)
				{
					friction = sv_friction.value;

					control = speed < sv_stopspeed.value ? sv_stopspeed.value : speed;
					newspeed = speed - host_frametime * control * friction;

					if(newspeed < 0)
						newspeed = 0;
					newspeed /= speed;

					vel[0] = vel[0] * newspeed;
					vel[1] = vel[1] * newspeed;
				}
			}

		VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
		SV_FlyMove(ent, host_frametime, NULL);
		VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);

		// determine if it's on solid ground at all
		{
			vec3_t mins, maxs, point;
			int x, y;

			VectorAdd(ent->v.origin, ent->v.mins, mins);
			VectorAdd(ent->v.origin, ent->v.maxs, maxs);

			point[2] = mins[2] - 1;
			for(x = 0; x <= 1; x++)
				for(y = 0; y <= 1; y++)
				{
					point[0] = x ? maxs[0] : mins[0];
					point[1] = y ? maxs[1] : mins[1];
					if(SV_PointContents(point) == CONTENTS_SOLID)
					{
						ent->v.flags = (int)ent->v.flags | FL_ONGROUND;
						break;
					}
				}
		}

		SV_LinkEdict(ent, true);

		if((int)ent->v.flags & FL_ONGROUND)
			if(!wasonground)
				if(hitsound)
					SV_StartSound(ent, 0, "demon/dland2.wav", 255, 1, PITCH_NORM);
	}

	// regular thinking
	SV_RunThink(ent);
	SV_CheckWaterTransition(ent);
}
#else
void SV_Physics_Step(edict_t *ent)
{
	qboolean hitsound;

	// freefall if not onground
	if(!((int)ent->v.flags & (FL_ONGROUND | FL_FLY | FL_SWIM)))
	{
		if(ent->v.velocity[2] < sv_gravity.value * -0.1) // TODO: sv_gravity.value -> movevars.gravity in qw
			hitsound = true;
		else
			hitsound = false;

		SV_AddGravity(ent); // TODO: scale = 1.0 in qw
		SV_CheckVelocity(ent);
		SV_FlyMove(ent, host_frametime, NULL);
		SV_LinkEdict(ent, true);

		if((int)ent->v.flags & FL_ONGROUND) // just hit ground
		{
			if(hitsound)
				SV_StartSound(ent, 0, "demon/dland2.wav", 255, 1, PITCH_NORM);
		}
	}

	// regular thinking
	SV_RunThink(ent);

	SV_CheckWaterTransition(ent);
}
#endif

//============================================================================

/*
================
SV_Physics

================
*/
void SV_Physics()
{
	int i;
	edict_t *ent;

	// let the progs know that a new frame has started
	gGlobalVariables.time = sv.time;
	gEntityInterface.pfnStartFrame();

	//SV_CheckAllEnts ();

	//
	// treat each object in turn
	//
	ent = sv.edicts;
	for(i = 0; i < sv.num_edicts; i++, ent = NEXT_EDICT(ent))
	{
		if(ent->free)
			continue;

		if(gGlobalVariables.force_retouch)
			SV_LinkEdict(ent, true); // force retouch even for stationary
		
		switch(ent->v.movetype)
		{
		//case MOVETYPE_WALK:
			//if(i > 0 && i <= svs.maxclients)
			//SV_Physics_Client(ent, i);
			//break;
		case MOVETYPE_PUSH:
			SV_Physics_Pusher(ent);
			break;
		case MOVETYPE_PUSHSTEP:
			SV_Physics_Pusher(ent); // TODO: The physics of this movetype is very similar to that of MOVETYPE_PUSH, except that MOVETYPE_PUSHSTEP uses a slightly different way to collide with other entities.
			break;
		case MOVETYPE_NONE:
			SV_Physics_None(ent);
			break;
		case MOVETYPE_FOLLOW:
			SV_Physics_Follow(ent);
			break;
		case MOVETYPE_NOCLIP:
			SV_Physics_Noclip(ent);
			break;
		case MOVETYPE_STEP:
			SV_Physics_Step(ent);
			break;
		case MOVETYPE_TOSS:
		case MOVETYPE_BOUNCE:
		case MOVETYPE_BOUNCEMISSILE:
		case MOVETYPE_FLY:
		case MOVETYPE_FLYMISSILE:
			SV_Physics_Toss(ent);
			break;
		// TODO: case MOVETYPE_COMPOUND:
			// TODO: unused?
			//break;
		default:
			Sys_Error("SV_Physics: bad movetype %i", (int)ent->v.movetype);
		};
	}

	if(gGlobalVariables.force_retouch)
		gGlobalVariables.force_retouch--;

	sv.time += host_frametime; // TODO: not present in qw
}

#ifdef QUAKE2
trace_t SV_Trace_Toss(edict_t *ent, edict_t *ignore)
{
	edict_t tempent, *tent;
	trace_t trace;
	vec3_t move;
	vec3_t end;
	double save_frametime;
	//	extern particle_t	*active_particles, *free_particles;
	//	particle_t	*p;

	save_frametime = host_frametime;
	host_frametime = 0.05;

	memcpy(&tempent, ent, sizeof(edict_t));
	tent = &tempent;

	while(1)
	{
		SV_CheckVelocity(tent);
		SV_AddGravity(tent);
		VectorMA(tent->v.angles, host_frametime, tent->v.avelocity, tent->v.angles);
		VectorScale(tent->v.velocity, host_frametime, move);
		VectorAdd(tent->v.origin, move, end);
		trace = SV_Move(tent->v.origin, tent->v.mins, tent->v.maxs, end, MOVE_NORMAL, tent);
		VectorCopy(trace.endpos, tent->v.origin);

		//		p = free_particles;
		//		if (p)
		//		{
		//			free_particles = p->next;
		//			p->next = active_particles;
		//			active_particles = p;
		//
		//			p->die = 256;
		//			p->color = 15;
		//			p->type = pt_static;
		//			VectorCopy (vec3_origin, p->vel);
		//			VectorCopy (tent->v.origin, p->org);
		//		}

		if(trace.ent)
			if(trace.ent != ignore)
				break;
	}
	//	p->color = 224;
	host_frametime = save_frametime;
	return trace;
}
#endif

// TODO: move to sv_phys.h?
extern cvar_t sv_maxspeed;
extern cvar_t sv_accelerate;

void SV_SetMoveVars()
{
	movevars.gravity			= sv_gravity.value;
	
	movevars.stopspeed		    = sv_stopspeed.value;
	
	movevars.maxspeed			= sv_maxspeed.value;
	movevars.spectatormaxspeed  = sv_spectatormaxspeed.value;
	
	movevars.accelerate		    = sv_accelerate.value;
	movevars.airaccelerate	    = sv_airaccelerate.value;
	movevars.wateraccelerate	= sv_wateraccelerate.value;
	
	movevars.friction			= sv_friction.value;
	movevars.edgefriction		= sv_edgefriction.value;
	movevars.waterfriction	    = sv_waterfriction.value;
	
	movevars.entgravity			= 1.0;
	
	movevars.bounce				= sv_bounce.value;
	movevars.stepsize			= sv_stepsize.value;
	movevars.maxvelocity		= sv_maxvelocity.value;
	
	movevars.zmax				= sv_zmax.value;
	movevars.waveHeight			= sv_wateramp.value;
	
	movevars.footsteps			= mp_footsteps.value;
	
	Q_strcpy(movevars.skyName, "2desert"); //sv_skyname.string; // TODO
	
	movevars.rollangle			= 0.0f; // TODO
	movevars.rollspeed			= 0.0f; // TODO
	
	movevars.skycolor_r			= sv_skycolor_r.value;
	movevars.skycolor_g			= sv_skycolor_g.value;
	movevars.skycolor_b			= sv_skycolor_b.value;
	
	movevars.skyvec_x			= sv_skyvec_x.value;
	movevars.skyvec_y			= sv_skyvec_y.value;
	movevars.skyvec_z			= sv_skyvec_z.value;
}