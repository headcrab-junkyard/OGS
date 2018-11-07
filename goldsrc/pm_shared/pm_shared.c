/// @file

#include <assert.h>
#include <math.h>

#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_movevars.h"
//#include "pm_debug.h"
#include "com_model.h"
#include "in_buttons.h"

#define STOP_EPSILON 0.1

#ifndef NULL
#define NULL (void*)0
#endif

// #define	PM_GRAVITY			800
// #define	PM_STOPSPEED		100
// #define	PM_MAXSPEED			320
// #define	PM_SPECTATORMAXSPEED	500
// #define	PM_ACCELERATE		10
// #define	PM_AIRACCELERATE	0.7
// #define	PM_WATERACCELERATE	10
// #define	PM_FRICTION			6
// #define	PM_WATERFRICTION	1

static qboolean pm_shared_initialized = false;

playermove_t *pmove = NULL;

/*
==================
PM_ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
int PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce)
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
PM_FlyMove

The basic solid body movement clip that slides along multiple planes
============
*/
#define MAX_CLIP_PLANES 5

int PM_FlyMove(void)
{
	int bumpcount, numbumps;
	vec3_t dir;
	float d;
	int numplanes;
	vec3_t planes[MAX_CLIP_PLANES];
	vec3_t primal_velocity, original_velocity;
	int i, j;
	pmtrace_t trace;
	vec3_t end;
	float time_left;
	int blocked;

	numbumps = 4;

	blocked = 0;
	VectorCopy(pmove->velocity, original_velocity);
	VectorCopy(pmove->velocity, primal_velocity);
	numplanes = 0;

	time_left = pmove->frametime;

	for(bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		for(i = 0; i < 3; i++)
			end[i] = pmove->origin[i] + time_left * pmove->velocity[i];

		trace = pmove->PM_PlayerTrace(pmove->origin, end, PM_NORMAL, -1);

		if(trace.startsolid || trace.allsolid)
		{ // entity is trapped in another solid
			VectorCopy(vec3_origin, pmove->velocity);
			return 3;
		}

		if(trace.fraction > 0)
		{ // actually covered some distance
			VectorCopy(trace.endpos, pmove->origin);
			numplanes = 0;
		}

		if(trace.fraction == 1)
			break; // moved the entire distance

		// save entity for contact
		pmove->touchindex[pmove->numtouch] = trace.ent;
		pmove->numtouch++;

		if(trace.plane.normal[2] > 0.7)
		{
			blocked |= 1; // floor
		}
		if(!trace.plane.normal[2])
		{
			blocked |= 2; // step
		}

		time_left -= time_left * trace.fraction;

		// cliped to another plane
		if(numplanes >= MAX_CLIP_PLANES)
		{ // this shouldn't really happen
			VectorCopy(vec3_origin, pmove->velocity);
			break;
		}

		VectorCopy(trace.plane.normal, planes[numplanes]);
		numplanes++;

		//
		// modify original_velocity so it parallels all of the clip planes
		//
		for(i = 0; i < numplanes; i++)
		{
			PM_ClipVelocity(original_velocity, planes[i], pmove->velocity, 1);
			for(j = 0; j < numplanes; j++)
				if(j != i)
				{
					if(DotProduct(pmove->velocity, planes[j]) < 0)
						break; // not ok
				}
			if(j == numplanes)
				break;
		}

		if(i != numplanes)
		{ // go along this plane
		}
		else
		{ // go along the crease
			if(numplanes != 2)
			{
				//				Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
				VectorCopy(vec3_origin, pmove->velocity);
				break;
			}
			CrossProduct(planes[0], planes[1], dir);
			d = DotProduct(dir, pmove->velocity);
			VectorScale(dir, d, pmove->velocity);
		}

		//
		// if original velocity is against the original velocity, stop dead
		// to avoid tiny occilations in sloping corners
		//
		if(DotProduct(pmove->velocity, primal_velocity) <= 0)
		{
			VectorCopy(vec3_origin, pmove->velocity);
			break;
		}
	}

	if(pmove->waterjumptime)
	{
		VectorCopy(primal_velocity, pmove->velocity);
	}
	return blocked;
}

/*
==============
PM_Accelerate
==============
*/
void PM_Accelerate(vec3_t wishdir, float wishspeed, float accel)
{
	int i;
	float addspeed, accelspeed, currentspeed;

	if(pmove->dead)
		return;
	if(pmove->waterjumptime)
		return;

	currentspeed = DotProduct(pmove->velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if(addspeed <= 0)
		return;
	accelspeed = accel * pmove->frametime * wishspeed;
	if(accelspeed > addspeed)
		accelspeed = addspeed;

	for(i = 0; i < 3; i++)
		pmove->velocity[i] += accelspeed * wishdir[i];
}

/*
=============
PM_GroundMove

Player is on ground, with no upwards velocity
=============
*/
void PM_WalkMove()
{
	vec3_t start, dest;
	pmtrace_t trace;
	vec3_t original, originalvel, down, up, downvel;
	float downdist, updist;

	pmove->velocity[2] = 0;
	if(!pmove->velocity[0] && !pmove->velocity[1] && !pmove->velocity[2])
		return;

	// first try just moving to the destination
	dest[0] = pmove->origin[0] + pmove->velocity[0] * pmove->frametime;
	dest[1] = pmove->origin[1] + pmove->velocity[1] * pmove->frametime;
	dest[2] = pmove->origin[2];

	// first try moving directly to the next spot
	VectorCopy(dest, start);
	trace = pmove->PM_PlayerTrace(pmove->origin, dest, PM_NORMAL, -1);
	if(trace.fraction == 1)
	{
		VectorCopy(trace.endpos, pmove->origin);
		return;
	}

	// try sliding forward both on ground and up 16 pixels
	// take the move that goes farthest
	VectorCopy(pmove->origin, original);
	VectorCopy(pmove->velocity, originalvel);

	// slide move
	PM_FlyMove();

	VectorCopy(pmove->origin, down);
	VectorCopy(pmove->velocity, downvel);

	VectorCopy(original, pmove->origin);
	VectorCopy(originalvel, pmove->velocity);

	// move up a stair height
	VectorCopy(pmove->origin, dest);
	dest[2] += pmove->movevars->stepsize;
	trace = pmove->PM_PlayerTrace(pmove->origin, dest, PM_NORMAL, -1);
	if(!trace.startsolid && !trace.allsolid)
	{
		VectorCopy(trace.endpos, pmove->origin);
	}

	// slide move
	PM_FlyMove();

	// press down the stepheight
	VectorCopy(pmove->origin, dest);
	dest[2] -= pmove->movevars->stepsize;
	trace = pmove->PM_PlayerTrace(pmove->origin, dest, PM_NORMAL, -1);
	if(trace.plane.normal[2] < 0.7)
		goto usedown;
	if(!trace.startsolid && !trace.allsolid)
	{
		VectorCopy(trace.endpos, pmove->origin);
	}
	VectorCopy(pmove->origin, up);

	// decide which one went farther
	downdist = (down[0] - original[0]) * (down[0] - original[0]) + (down[1] - original[1]) * (down[1] - original[1]);
	updist = (up[0] - original[0]) * (up[0] - original[0]) + (up[1] - original[1]) * (up[1] - original[1]);

	if(downdist > updist)
	{
	usedown:
		VectorCopy(down, pmove->origin);
		VectorCopy(downvel, pmove->velocity);
	}
	else // copy z value from slide move
		pmove->velocity[2] = downvel[2];

	// if at a dead stop, retry the move with nudges to get around lips
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
void PM_Friction(void)
{
	float *vel;
	float speed, newspeed, control;
	float friction;
	float drop;
	vec3_t start, stop;
	pmtrace_t trace;

	if(pmove->waterjumptime)
		return;

	vel = pmove->velocity;

	speed = sqrt(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);
	if(speed < 1)
	{
		vel[0] = 0;
		vel[1] = 0;
		return;
	}

	friction = pmove->movevars->friction;

	// if the leading edge is over a dropoff, increase friction
	if(pmove->onground != -1)
	{
		start[0] = stop[0] = pmove->origin[0] + vel[0] / speed * 16;
		start[1] = stop[1] = pmove->origin[1] + vel[1] / speed * 16;
		start[2] = pmove->origin[2] + pmove->player_mins[pmove->usehull][2];
		stop[2] = start[2] - 34;

		trace = pmove->PM_PlayerTrace(start, stop, PM_NORMAL, -1);

		if(trace.fraction == 1)
		{
			friction *= 2;
		}
	}

	drop = 0;

	if(pmove->waterlevel >= 2) // apply water friction
		drop += speed * pmove->movevars->waterfriction * pmove->waterlevel * pmove->frametime;
	else if(pmove->onground != -1) // apply ground friction
	{
		control = speed < pmove->movevars->stopspeed ? pmove->movevars->stopspeed : speed;
		drop += control * friction * pmove->frametime;
	}

	// scale the velocity
	newspeed = speed - drop;
	if(newspeed < 0)
		newspeed = 0;
	newspeed /= speed;

	vel[0] = vel[0] * newspeed;
	vel[1] = vel[1] * newspeed;
	vel[2] = vel[2] * newspeed;
}

void PM_AirAccelerate(vec3_t wishdir, float wishspeed, float accel)
{
	int i;
	float addspeed, accelspeed, currentspeed, wishspd = wishspeed;

	if(pmove->dead)
		return;
	if(pmove->waterjumptime)
		return;

	if(wishspd > 30)
		wishspd = 30;
	currentspeed = DotProduct(pmove->velocity, wishdir);
	addspeed = wishspd - currentspeed;
	if(addspeed <= 0)
		return;
	accelspeed = accel * wishspeed * pmove->frametime;
	if(accelspeed > addspeed)
		accelspeed = addspeed;

	for(i = 0; i < 3; i++)
		pmove->velocity[i] += accelspeed * wishdir[i];
}

/*
===================
PM_WaterMove

===================
*/
void PM_WaterMove(void)
{
	int i;
	vec3_t wishvel;
	float wishspeed;
	vec3_t wishdir;
	vec3_t start, dest;
	pmtrace_t trace;

	//
	// user intentions
	//
	for(i = 0; i < 3; i++)
		wishvel[i] = pmove->forward[i] * pmove->cmd.forwardmove + pmove->right[i] * pmove->cmd.sidemove;

	if(!pmove->cmd.forwardmove && !pmove->cmd.sidemove && !pmove->cmd.upmove)
		wishvel[2] -= 60; // drift towards bottom
	else
		wishvel[2] += pmove->cmd.upmove;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if(wishspeed > pmove->movevars->maxspeed)
	{
		VectorScale(wishvel, pmove->movevars->maxspeed / wishspeed, wishvel);
		wishspeed = pmove->movevars->maxspeed;
	}
	wishspeed *= 0.7;

	//
	// water acceleration
	//
	//	if (pmove->waterjumptime)
	//		Con_Printf ("wm->%f, %f, %f\n", pmove->velocity[0], pmove->velocity[1], pmove->velocity[2]);
	PM_Accelerate(wishdir, wishspeed, pmove->movevars->wateraccelerate);

	// assume it is a stair or a slope, so press down from stepheight above
	VectorMA(pmove->origin, pmove->frametime, pmove->velocity, dest);
	VectorCopy(dest, start);
	start[2] += pmove->movevars->stepsize + 1;
	trace = pmove->PM_PlayerTrace(start, dest, PM_NORMAL, -1);
	if(!trace.startsolid && !trace.allsolid) // FIXME: check steep slope?
	{                                        // walked up the step
		VectorCopy(trace.endpos, pmove->origin);
		return;
	}

	PM_FlyMove();
	//	if (pmove->waterjumptime)
	//		Con_Printf ("<-wm%f, %f, %f\n", pmove->velocity[0], pmove->velocity[1], pmove->velocity[2]);
}

/*
===================
PM_AirMove

===================
*/
void PM_AirMove(void)
{
	int i;
	vec3_t wishvel;
	float fmove, smove;
	vec3_t wishdir;
	float wishspeed;

	fmove = pmove->cmd.forwardmove;
	smove = pmove->cmd.sidemove;

	pmove->forward[2] = 0;
	pmove->right[2] = 0;
	VectorNormalize(pmove->forward);
	VectorNormalize(pmove->right);

	for(i = 0; i < 2; i++)
		wishvel[i] = pmove->forward[i] * fmove + pmove->right[i] * smove;
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	//
	// clamp to server defined max speed
	//
	if(wishspeed > pmove->maxspeed) // TODO: was movevars.maxspeed, so it actually should pmove->movevars->maxspeed?
	{
		VectorScale(wishvel, pmove->maxspeed / wishspeed, wishvel); // TODO: was movevars.maxspeed, so it actually should pmove->movevars->maxspeed?
		wishspeed = pmove->maxspeed; // TODO: was movevars.maxspeed, so it actually should pmove->movevars->maxspeed?
	}

	//	if (pmove->waterjumptime)
	//		Con_Printf ("am->%f, %f, %f\n", pmove->velocity[0], pmove->velocity[1], pmove->velocity[2]);

	if(pmove->onground != -1)
	{
		pmove->velocity[2] = 0;
		PM_Accelerate(wishdir, wishspeed, pmove->movevars->accelerate);
		pmove->velocity[2] -= pmove->movevars->entgravity * pmove->movevars->gravity * pmove->frametime;
		PM_WalkMove();
	}
	else
	{ // not on ground, so little effect on velocity
		PM_AirAccelerate(wishdir, wishspeed, pmove->movevars->accelerate);

		// add gravity
		pmove->velocity[2] -= pmove->movevars->entgravity * pmove->movevars->gravity * pmove->frametime;

		PM_FlyMove();
	}

	//Con_Printf("airmove:vec: %4.2f %4.2f %4.2f\n",
	//			pmove->velocity[0],
	//			pmove->velocity[1],
	//			pmove->velocity[2]);
	//

	//	if (pmove->waterjumptime)
	//		Con_Printf ("<-am%f, %f, %f\n", pmove->velocity[0], pmove->velocity[1], pmove->velocity[2]);
}

/*
=============
PM_CatagorizePosition
=============
*/
void PM_CatagorizePosition(void)
{
	vec3_t point;
	int cont;
	pmtrace_t tr;

	// if the player hull point one unit down is solid, the player
	// is on ground

	// see if standing on something solid
	point[0] = pmove->origin[0];
	point[1] = pmove->origin[1];
	point[2] = pmove->origin[2] - 1;
	if(pmove->velocity[2] > 180)
	{
		pmove->onground = -1;
	}
	else
	{
		tr = pmove->PM_PlayerTrace(pmove->origin, point, PM_NORMAL, -1);
		if(tr.plane.normal[2] < 0.7)
			pmove->onground = -1; // too steep
		else
			pmove->onground = tr.ent;
		if(pmove->onground != -1)
		{
			pmove->waterjumptime = 0;
			if(!tr.startsolid && !tr.allsolid)
				VectorCopy(tr.endpos, pmove->origin);
		}

		// standing on an entity other than the world
		if(tr.ent > 0)
		{
			pmove->touchindex[pmove->numtouch] = tr.ent;
			pmove->numtouch++;
		}
	}

	//
	// get waterlevel
	//
	pmove->waterlevel = 0;
	pmove->watertype = CONTENTS_EMPTY;

	point[2] = pmove->origin[2] + pmove->player_mins[pmove->usehull][2] + 1;
	cont = pmove->PM_PointContents(point, NULL);

	if(cont <= CONTENTS_WATER)
	{
		pmove->watertype = cont;
		pmove->waterlevel = 1;
		point[2] = pmove->origin[2] + (pmove->player_mins[pmove->usehull][2] + pmove->player_maxs[pmove->usehull][2]) * 0.5;
		cont = pmove->PM_PointContents(point, NULL);
		if(cont <= CONTENTS_WATER)
		{
			pmove->waterlevel = 2;
			point[2] = pmove->origin[2] + 22;
			cont = pmove->PM_PointContents(point, NULL);
			if(cont <= CONTENTS_WATER)
				pmove->waterlevel = 3;
		}
	}
}

/*
=================
NudgePosition

If pmove->origin is in a solid position,
try nudging slightly on all axis to
allow for the cut precision of the net coordinates
=================
*/
void PM_CheckStuck()
{
	vec3_t base;
	int x, y, z;
	int i;
	static int sign[3] = { 0, -1, 1 };
	pmtrace_t traceresult;

	VectorCopy(pmove->origin, base);

	for(i = 0; i < 3; i++)
		pmove->origin[i] = ((int)(pmove->origin[i] * 8)) * 0.125;
	//	pmove->origin[2] += 0.124;

	//	if (pmove->dead)
	//		return;		// might be a squished point, so don'y bother
	//	if (pmove->PM_TestPlayerPosition (pmove->origin) )
	//		return;

	for(z = 0; z <= 2; z++)
	{
		for(x = 0; x <= 2; x++)
		{
			for(y = 0; y <= 2; y++)
			{
				pmove->origin[0] = base[0] + (sign[x] * 1.0 / 8);
				pmove->origin[1] = base[1] + (sign[y] * 1.0 / 8);
				pmove->origin[2] = base[2] + (sign[z] * 1.0 / 8);
				if(pmove->PM_TestPlayerPosition(pmove->origin, &traceresult))
					return;
			}
		}
	}
	VectorCopy(base, pmove->origin);
	//	Con_DPrintf ("NudgePosition: stuck\n");
}

/*
===============
SpectatorMove
===============
*/
void PM_SpectatorMove()
{
	float speed, drop, friction, control, newspeed, accel;
	float currentspeed, addspeed, accelspeed;
	int i;
	vec3_t wishvel;
	float fmove, smove;
	vec3_t wishdir;
	float wishspeed;
#ifndef SWDS
	extern float server_version; // version of server we connected to
#endif

	// friction

	speed = Length(pmove->velocity);
	if(speed < 1)
	{
		VectorCopy(vec3_origin, pmove->velocity)
	}
	else
	{
		drop = 0;

		friction = pmove->movevars->friction * 1.5; // extra friction
		control = speed < pmove->movevars->stopspeed ? pmove->movevars->stopspeed : speed;
		drop += control * friction * pmove->frametime;

		// scale the velocity
		newspeed = speed - drop;
		if(newspeed < 0)
			newspeed = 0;
		newspeed /= speed;

		VectorScale(pmove->velocity, newspeed, pmove->velocity);
	}

	// accelerate
	fmove = pmove->cmd.forwardmove;
	smove = pmove->cmd.sidemove;

	VectorNormalize(pmove->forward);
	VectorNormalize(pmove->right);

	for(i = 0; i < 3; i++)
		wishvel[i] = pmove->forward[i] * fmove + pmove->right[i] * smove;
	wishvel[2] += pmove->cmd.upmove;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	//
	// clamp to server defined max speed
	//
	if(wishspeed > pmove->movevars->spectatormaxspeed)
	{
		VectorScale(wishvel, pmove->movevars->spectatormaxspeed / wishspeed, wishvel);
		wishspeed = pmove->movevars->spectatormaxspeed;
	}

	currentspeed = DotProduct(pmove->velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if(addspeed <= 0)
		return;
	accelspeed = pmove->movevars->accelerate * pmove->frametime * wishspeed;
	if(accelspeed > addspeed)
		accelspeed = addspeed;

	for(i = 0; i < 3; i++)
		pmove->velocity[i] += accelspeed * wishdir[i];

	// move
	VectorMA(pmove->origin, pmove->frametime, pmove->velocity, pmove->origin);
}

/*
=============
JumpButton
=============
*/
void PM_Jump(void)
{
	if(pmove->dead)
	{
		pmove->oldbuttons |= IN_JUMP; // don't jump again until released
		return;
	}

	if(pmove->waterjumptime)
	{
		pmove->waterjumptime -= pmove->frametime;
		if(pmove->waterjumptime < 0)
			pmove->waterjumptime = 0;
		return;
	}

	if(pmove->waterlevel >= 2)
	{ // swimming, not jumping
		pmove->onground = -1;

		if(pmove->watertype == CONTENTS_WATER)
			pmove->velocity[2] = 100;
		else if(pmove->watertype == CONTENTS_SLIME)
			pmove->velocity[2] = 80;
		else
			pmove->velocity[2] = 50;
		return;
	}

	if(pmove->onground == -1)
		return; // in air, so no effect

	if(pmove->oldbuttons & IN_JUMP)
		return; // don't pogo stick

	pmove->onground = -1;
	pmove->velocity[2] += 270;

	pmove->oldbuttons |= IN_JUMP; // don't jump again until released
}

/*
=============
CheckWaterJump
=============
*/
void PM_CheckWaterJump(void)
{
	vec3_t spot;
	int cont;
	vec3_t flatforward;

	if(pmove->waterjumptime)
		return;

	// ZOID, don't hop out if we just jumped in
	if(pmove->velocity[2] < -180)
		return; // only hop out if we are moving up

	// see if near an edge
	flatforward[0] = pmove->forward[0];
	flatforward[1] = pmove->forward[1];
	flatforward[2] = 0;
	VectorNormalize(flatforward);

	VectorMA(pmove->origin, 24, flatforward, spot);
	spot[2] += 8;
	cont = pmove->PM_PointContents(spot, NULL);
	if(cont != CONTENTS_SOLID)
		return;
	spot[2] += 24;
	cont = pmove->PM_PointContents(spot, NULL);
	if(cont != CONTENTS_EMPTY)
		return;
	// jump out of water
	VectorScale(flatforward, 50, pmove->velocity);
	pmove->velocity[2] = 310;
	pmove->waterjumptime = 2;         // safety net
	pmove->oldbuttons |= IN_JUMP; // don't jump again until released
};

/*
=============
PlayerMove

Returns with origin, angles, and velocity modified in place.

Numtouch and touchindex[] will be set if any of the physents
were contacted during the move.
=============
*/
void PM_PlayerMove(qboolean server)
{
	// Are we running server code?
	pmove->server = server;
	
	pmove->frametime = pmove->cmd.msec * 0.001;
	pmove->numtouch = 0;

	AngleVectors(pmove->angles, pmove->forward, pmove->right, pmove->up);

	if(pmove->spectator)
	{
		PM_SpectatorMove();
		return;
	};

	PM_CheckStuck();

	// take angles directly from command
	VectorCopy(pmove->cmd.viewangles, pmove->angles);

	// set onground, watertype, and waterlevel
	PM_CatagorizePosition();

	if(pmove->waterlevel == 2)
		PM_CheckWaterJump();

	if(pmove->velocity[2] < 0)
		pmove->waterjumptime = 0;

	if(pmove->cmd.buttons & IN_JUMP)
		PM_Jump();
	else
		pmove->oldbuttons &= ~IN_JUMP;

	PM_Friction();

	if(pmove->waterlevel >= 2)
		PM_WaterMove();
	else
		PM_AirMove();

	// set onground, watertype, and waterlevel for final spot
	PM_CatagorizePosition();
}

void PM_Move(struct playermove_s *ppmove, int server)
{
	assert(pm_shared_initialized);
	
	pmove = ppmove;
	
	PM_PlayerMove(server);
}

void PM_Init(struct playermove_s *ppmove)
{
	assert(!pm_shared_initialized);
	
	pmove = ppmove;
	
	pm_shared_initialized = true;
};

char PM_GetTextureType(const char *name)
{
	// TODO
	return '\0';
};