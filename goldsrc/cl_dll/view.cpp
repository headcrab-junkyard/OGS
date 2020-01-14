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
/// @brief player eye positioning

#include <cmath>

extern "C"
{
#include "mathlib.h"
};

#include "ref_params.h"
#include "cl_entity.h"
#include "engine.h"
#include "cvardef.h"
#include "usercmd.h"
#include "pm_movevars.h"
#include "exports.h"

#define AngleVectors gpEngine->pfnAngleVectors // TODO: temp

//=============================================================================

extern cvar_t *cl_forwardspeed;

cvar_t *scr_ofsx;
cvar_t *scr_ofsy;
cvar_t *scr_ofsz;

cvar_t *cl_bob;
cvar_t *cl_bobcycle;
cvar_t *cl_bobup;

cvar_t *v_centermove;
cvar_t *v_centerspeed;

// Not registered in GS to prevent cheating
cvar_t *v_iyaw_cycle;
cvar_t *v_iroll_cycle;
cvar_t *v_ipitch_cycle;

cvar_t *v_iyaw_level;
cvar_t *v_iroll_level;
cvar_t *v_ipitch_level;
//

cvar_t *v_idlescale; // TODO: float v_idlescale; in gs

/*
===============
V_CalcBob

===============
*/
float V_CalcBob (struct ref_params_s *pparams)
{
	float	bob;
	float	cycle;
	
	cycle = pparams->time - (int)(pparams->time/cl_bobcycle->value)*cl_bobcycle->value;
	cycle /= cl_bobcycle->value;
	if (cycle < cl_bobup->value)
		cycle = M_PI * cycle / cl_bobup->value;
	else
		cycle = M_PI + M_PI*(cycle-cl_bobup->value)/(1.0 - cl_bobup->value);

// bob is proportional to velocity in the xy plane
// (don't count Z, or jumping messes it up)

	bob = sqrt(pparams->simvel[0]*pparams->simvel[0] + pparams->simvel[1]*pparams->simvel[1]) * cl_bob->value;
//Con_Printf ("speed: %5.1f\n", Length(pparams->simvel));
	bob = bob*0.3 + bob*0.7*sin(cycle);
	if (bob > 4)
		bob = 4;
	else if (bob < -7)
		bob = -7;
	return bob;
}

/*
===============
V_CalcRoll

Used by view and sv_user
===============
*/
float V_CalcRoll (vec3_t angles, vec3_t velocity, float rollangle, float rollspeed)
{
	float	sign;
	float	side;
	float	value;
	vec3_t forward, right, up;
	
	AngleVectors (angles, forward, right, up);
	side = DotProduct (velocity, right);
	sign = side < 0 ? -1 : 1;
	side = fabs(side);
	
	value = rollangle;
//	if (cl.inwater)
//		value *= 6;

	if (side < rollspeed)
		side = side * value / rollspeed;
	else
		side = value;
	
	return side*sign;
}

// pitch drifting vars
struct pitchdrift_t
{
	// TODO: move these fields here from the client state structure
	float pitchvel;
	int nodrift; // TODO: qboolean
	float driftmove;
	double laststop;
};

static pitchdrift_t pd;

void V_StartPitchDrift ()
{
	if (pd.laststop == gpEngine->GetClientTime()) // TODO: was cl.time
	{
		return;		// something else is keeping it from drifting
	}

	if (pd.nodrift || !pd.pitchvel)
	{
		pd.pitchvel = v_centerspeed->value;
		pd.nodrift = false;
		pd.driftmove = 0;
	};
};

void V_StopPitchDrift ()
{
	pd.laststop = gpEngine->GetClientTime(); // TODO: was cl.time;
	pd.nodrift = true;
	pd.pitchvel = 0;
};

/*
===============
V_DriftPitch

Moves the client pitch angle towards cl.idealpitch sent by the server.

If the user is adjusting pitch manually, either with lookup/lookdown,
mlook and mouse, or klook and keyboard, pitch drifting is constantly stopped.

Drifting is enabled when the center view key is hit, mlook is released and
lookspring is non 0, or when 
===============
*/
void V_DriftPitch (struct ref_params_s *pparams)
{
	float		delta, move;

	if (gpEngine->IsNoClipping() || !pparams->onground || pparams->demoplayback )
	{
		pd.driftmove = 0;
		pd.pitchvel = 0;
		return;
	}

// don't count small mouse motion
	if (pd.nodrift)
	{
		if ( fabs(pparams->cmd->forwardmove) < cl_forwardspeed->value)
			pd.driftmove = 0;
		else
			pd.driftmove += pparams->frametime;
	
		if ( pd.driftmove > v_centermove->value)
		{
			V_StartPitchDrift ();
		}
		return;
	}
	
	delta = pparams->idealpitch - pparams->cl_viewangles[PITCH];

	if (!delta)
	{
		pd.pitchvel = 0;
		return;
	};

	move = pparams->frametime * pd.pitchvel;
	pd.pitchvel += pparams->frametime * v_centerspeed->value;
	
//Con_Printf ("move: %f (%f)\n", move, pparams->frametime);

	if (delta > 0)
	{
		if (move > delta)
		{
			pd.pitchvel = 0;
			move = delta;
		};
		pparams->cl_viewangles[PITCH] += move;
	}
	else if (delta < 0)
	{
		if (move > -delta)
		{
			pd.pitchvel = 0;
			move = -delta;
		};
		pparams->cl_viewangles[PITCH] -= move;
	};
};

/* 
============================================================================== 
 
						VIEW RENDERING 
 
============================================================================== 
*/

float angledelta(float a)
{
	a = anglemod(a);
	if(a > 180)
		a -= 360;
	return a;
}

/*
==================
CalcGunAngle
==================
*/
void CalcGunAngle(struct ref_params_s *pparams)
{
	float yaw, pitch, move;
	static float oldyaw = 0;
	static float oldpitch = 0;
	
	cl_entity_t *viewent = gpEngine->GetViewModel();
	if(!viewent)
		return;

	yaw = pparams->viewangles[YAW];
	pitch = -pparams->viewangles[PITCH];

	yaw = angledelta(yaw - pparams->viewangles[YAW]) * 0.4;
	if(yaw > 10)
		yaw = 10;
	if(yaw < -10)
		yaw = -10;
	pitch = angledelta(-pitch - pparams->viewangles[PITCH]) * 0.4;
	if(pitch > 10)
		pitch = 10;
	if(pitch < -10)
		pitch = -10;
	move = pparams->frametime * 20;
	if(yaw > oldyaw)
	{
		if(oldyaw + move < yaw)
			yaw = oldyaw + move;
	}
	else
	{
		if(oldyaw - move > yaw)
			yaw = oldyaw - move;
	}

	if(pitch > oldpitch)
	{
		if(oldpitch + move < pitch)
			pitch = oldpitch + move;
	}
	else
	{
		if(oldpitch - move > pitch)
			pitch = oldpitch - move;
	}

	oldyaw = yaw;
	oldpitch = pitch;

	viewent->angles[YAW] = pparams->viewangles[YAW] + yaw;
	viewent->angles[PITCH] = -(pparams->viewangles[PITCH] + pitch);

	viewent->angles[ROLL] -= v_idlescale->value * sin(pparams->time * v_iroll_cycle->value) * v_iroll_level->value;
	viewent->angles[PITCH] -= v_idlescale->value * sin(pparams->time * v_ipitch_cycle->value) * v_ipitch_level->value;
	viewent->angles[YAW] -= v_idlescale->value * sin(pparams->time * v_iyaw_cycle->value) * v_iyaw_level->value;
}

/*
==============
V_AddIdle

Idle swaying
==============
*/
void V_AddIdle(struct ref_params_s *pparams)
{
	pparams->viewangles[ROLL] += v_idlescale->value * sin(pparams->time * v_iroll_cycle->value) * v_iroll_level->value;
	pparams->viewangles[PITCH] += v_idlescale->value * sin(pparams->time * v_ipitch_cycle->value) * v_ipitch_level->value;
	pparams->viewangles[YAW] += v_idlescale->value * sin(pparams->time * v_iyaw_cycle->value) * v_iyaw_level->value;
};

/*
==============
V_CalcViewRoll

Roll is induced by movement and damage
==============
*/
void V_CalcViewRoll(struct ref_params_s *pparams)
{
	float side;

	side = V_CalcRoll(gpEngine->GetEntityByIndex(pparams->viewentity)->angles, pparams->simvel, pparams->movevars->rollangle, pparams->movevars->rollspeed);
	pparams->viewangles[ROLL] += side;

	// TODO: unused?
	/*
	if(v_dmg_time > 0)
	{
		pparams->viewangles[ROLL] += v_dmg_time / v_kicktime->value * v_dmg_roll;
		pparams->viewangles[PITCH] += v_dmg_time / v_kicktime->value * v_dmg_pitch;
		v_dmg_time -= pparams->frametime;
	};
	*/

	if(pparams->health <= 0)
	{
		pparams->viewangles[ROLL] = 80; // dead view angle
		return;
	};
};

/*
==================
V_CalcIntermissionRefdef

==================
*/
void V_CalcIntermissionRefdef(struct ref_params_s *pparams)
{
	//cl_entity_t *ent;
	cl_entity_t *view;
	float old;

	// ent is the player model (visible when out of body)
	//ent = gpEngine->GetEntityByIndex(pparams->viewentity);
	// view is the weapon model (only visible from inside body)
	view = gpEngine->GetViewModel(); // TODO: was &cl.viewent;

	VectorCopy(pparams->simorg, pparams->vieworg);
	VectorCopy(pparams->viewangles, pparams->viewangles);
	view->model = nullptr;

	// always idle in intermission
	old = v_idlescale->value;
	v_idlescale->value = 1;
	V_AddIdle(pparams);
	v_idlescale->value = old;
};

void V_CalcNormalRefdef(struct ref_params_s *pparams)
{
	//cl_entity_t *ent;
	cl_entity_t *view;
	int i;
	vec3_t forward, right, up;
	vec3_t angles;
	float bob;
	
	static float oldz = 0.0f;
	static float lasttime = 0.0f; // TODO: unused in qw

	V_DriftPitch(pparams);

	// ent is the player model (visible when out of body)
	//ent = gpEngine->GetEntityByIndex(pparams->viewentity);
	// view is the weapon model (only visible from inside body)
	view = gpEngine->GetViewModel(); // TODO: was &cl.viewent

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view
	//ent->angles[YAW] = pparams->cl_viewangles[YAW];      // the model should face
	                                            // the view dir
	//ent->angles[PITCH] = -pparams->cl_viewangles[PITCH]; // the model should face
	                                            // the view dir

	bob = V_CalcBob(pparams);
	
	// refresh position from simulated origin
	VectorCopy(pparams->simorg, pparams->vieworg);
	pparams->vieworg[2] += bob;
	//VectorAdd(pparams->vieworg, pparams->viewheight, pparams->vieworg); // TODO: commented in, uncomment when fixed (viewheight is too high)
	
	// never let it sit exactly on a node line, because a water plane can
	// dissapear when viewed with the eye exactly on it.
	// the server protocol only specifies to 1/16 pixel, so add 1/32 in each axis
	pparams->vieworg[0] += 1.0 / 32; // TODO: 16 in qw
	pparams->vieworg[1] += 1.0 / 32; // TODO: 16 in qw
	pparams->vieworg[2] += 1.0 / 32; // TODO: 16 in qw

	VectorCopy(pparams->cl_viewangles, pparams->viewangles);

	// TODO: temp
	pparams->health = 100;
	
	V_CalcViewRoll(pparams);
	V_AddIdle(pparams);

	// TODO: qw
/*
	if (view_message->flags & PF_GIB)
		r_refdef.vieworg[2] += 8;	// gib view height
	else if (view_message->flags & PF_DEAD)
		r_refdef.vieworg[2] -= 16;	// corpse view height
	else
		r_refdef.vieworg[2] += 22;	// view height

	if (view_message->flags & PF_DEAD)		// PF_GIB will also set PF_DEAD
		r_refdef.viewangles[ROLL] = 80;	// dead view angle
*/
	
	// offsets
	VectorCopy(pparams->cl_viewangles, angles); // TODO: gs, but i can't see anything in this case
	//angles[PITCH] = -pparams->viewangles[PITCH]; // because entity pitches are actually backward
	//angles[YAW] = pparams->viewangles[YAW];
	//angles[ROLL] = pparams->viewangles[ROLL];

	AngleVectors(angles, pparams->forward, pparams->right, pparams->up);

	for(i = 0; i < 3; i++)
		pparams->vieworg[i] += scr_ofsx->value * pparams->forward[i] + scr_ofsy->value * pparams->right[i] + scr_ofsz->value * pparams->up[i]; // TODO: not present in qw

	//V_BoundOffsets(); // TODO

	// set up gun position
	VectorCopy(pparams->cl_viewangles, view->angles);

	CalcGunAngle(pparams);

	VectorCopy(pparams->simorg, view->origin);
	view->origin[2] += 22; // TODO: += waterOffset;
	VectorAdd(view->origin, pparams->viewheight, view->origin); // TODO: not present in qw

	for(i = 0; i < 3; i++)
	{
		view->origin[i] += pparams->forward[i] * bob * 0.4;
		//view->origin[i] += pparams->right[i]*bob*0.4;
		//view->origin[i] += pparams->up[i]*bob*0.8;
	};
	
	view->origin[2] += bob;

	// fudge position around to keep amount of weapon visible
	// roughly equal with different FOV
	if(pparams->viewsize == 110)
		view->origin[2] += 1;
	else if(pparams->viewsize == 100)
		view->origin[2] += 2;
	else if(pparams->viewsize == 90)
		view->origin[2] += 1;
	else if(pparams->viewsize == 80)
		view->origin[2] += 0.5;

	view->model = NULL; // TODO: cl.model_precache[cl.stats[STAT_WEAPON]];
	view->frame = 0; // TODO: cl.stats[STAT_WEAPONFRAME]; // TODO: no stats...
	//view->colormap = vid.colormap; // TODO: unused?

	// set up the refresh position
	VectorAdd(pparams->viewangles, pparams->punchangle, pparams->viewangles);

	// smooth out stair step ups
	if(pparams->onground && pparams->simorg[2] - oldz > 0)
	{
		float steptime;

		steptime = pparams->time - lasttime;
		if(steptime < 0)
			//FIXME		I_Error ("steptime < 0");
			steptime = 0;

		oldz += steptime * 80;
		if(oldz > pparams->simorg[2])
			oldz = pparams->simorg[2];
		if(pparams->simorg[2] - oldz > 12)
			oldz = pparams->simorg[2] - 12;
		pparams->vieworg[2] += oldz - pparams->simorg[2];
		view->origin[2] += oldz - pparams->simorg[2];
	}
	else
		oldz = pparams->simorg[2];

	//if(chase_active->value) // TODO
		//Chase_Update();
	
	lasttime = pparams->time;
};

/*
==================
V_CalcRefdef

==================
*/
/*C_EXPORT*/ void V_CalcRefdef(struct ref_params_s *pparams)
{
	if(pparams->intermission)
	{
		// intermission / finale rendering
		V_CalcIntermissionRefdef(pparams);
	}
	else
	{
		if(!pparams->paused /* && (pparams->maxclients > 1 || key_dest == key_game) */) // TODO
			V_CalcNormalRefdef(pparams);
	};
};

//============================================================================

/*
=============
V_Init
=============
*/
void V_Init()
{
	gpEngine->pfnAddCommand("centerview", V_StartPitchDrift);

	scr_ofsx = gpEngine->pfnRegisterVariable("scr_ofsx", "0", 0);
	scr_ofsy = gpEngine->pfnRegisterVariable("scr_ofsy", "0", 0);
	scr_ofsz = gpEngine->pfnRegisterVariable("scr_ofsz", "0", 0);
	
	v_centermove = gpEngine->pfnRegisterVariable("v_centermove", "0.15", 0);
	v_centerspeed = gpEngine->pfnRegisterVariable("v_centerspeed", "500", 0);
	
	cl_bob = gpEngine->pfnRegisterVariable("cl_bob", "0.02", 0);
	cl_bobcycle = gpEngine->pfnRegisterVariable("cl_bobcycle", "0.6", 0);
	cl_bobup = gpEngine->pfnRegisterVariable("cl_bobup", "0.5", 0);
	
	// TODO: unregistered in gs
	v_iyaw_cycle = gpEngine->pfnRegisterVariable("v_iyaw_cycle", "2", 0);
	v_iroll_cycle = gpEngine->pfnRegisterVariable("v_iroll_cycle", "0.5", 0);
	v_ipitch_cycle = gpEngine->pfnRegisterVariable("v_ipitch_cycle", "1", 0);
	
	v_iyaw_level = gpEngine->pfnRegisterVariable("v_iyaw_level", "0.3", 0);
	v_iroll_level = gpEngine->pfnRegisterVariable("v_iroll_level", "0.1", 0);
	v_ipitch_level = gpEngine->pfnRegisterVariable("v_ipitch_level", "0.3", 0);
	//
	
	v_idlescale = gpEngine->pfnRegisterVariable("v_idlescale", "0", 0);
};