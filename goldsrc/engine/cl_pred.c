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
#include "winquake.h"

cvar_t cl_nopred = { "cl_nopred", "0" };
cvar_t cl_pushlatency = { "pushlatency", "-999" };

extern frame_t *view_frame;

/*
===================
CL_CheckPredictionError
===================
*/
// TODO: fix
/*
void CL_CheckPredictionError()
{
	int frame;
	int delta[3];
	int i;
	int len;

	if(!cl_predict.value || (cl.frame.playerstate.pmove.pm_flags & PMF_NO_PREDICTION))
		return;

	// calculate the last usercmd_t we sent that the server has processed
	frame = cls.netchan.incoming_acknowledged;
	frame &= (CMD_BACKUP - 1);

	// compare what the server returned with what we had predicted it to be
	VectorSubtract(cl.frame.playerstate.pmove.origin, cl.predicted_origins[frame], delta);

	// save the prediction error for interpolation
	len = abs(delta[0]) + abs(delta[1]) + abs(delta[2]);
	if(len > 640) // 80 world units
	{             // a teleport or something
		VectorClear(cl.prediction_error);
	}
	else
	{
		if(cl_showmiss.value && (delta[0] || delta[1] || delta[2]))
			Con_Printf("prediction miss on %i: %i\n", cl.frame.serverframe,
			           delta[0] + delta[1] + delta[2]);

		VectorCopy(cl.frame.playerstate.pmove.origin, cl.predicted_origins[frame]);

		// save for error itnerpolation
		for(i = 0; i < 3; i++)
			cl.prediction_error[i] = delta[i] * 0.125;
	}
}
*/

/*
=================
CL_NudgePosition

If pmove.origin is in a solid position,
try nudging slightly on all axis to
allow for the cut precision of the net coordinates
=================
*/
void CL_NudgePosition()
{
	vec3_t base;
	int x, y;

	if(PM_HullPointContents(&cl.model_precache[1]->hulls[1], 0, clpmove.origin) == CONTENTS_EMPTY)
		return;

	VectorCopy(clpmove.origin, base);
	for(x = -1; x <= 1; x++)
	{
		for(y = -1; y <= 1; y++)
		{
			clpmove.origin[0] = base[0] + x * 1.0 / 8;
			clpmove.origin[1] = base[1] + y * 1.0 / 8;
			if(PM_HullPointContents(&cl.model_precache[1]->hulls[1], 0, clpmove.origin) == CONTENTS_EMPTY)
				return;
		}
	}
	Con_DPrintf("CL_NudgePosition: stuck\n");
}

/*
==============
CL_PredictUsercmd
==============
*/
void CL_PredictUsercmd(local_state_t *from, local_state_t *to, usercmd_t *u, qboolean spectator) // TODO: local_state_t was player_state_t
{
	// split up very long moves
	if(u->msec > 50)
	{
		local_state_t temp;
		usercmd_t split;

		split = *u;
		split.msec /= 2;

		CL_PredictUsercmd(from, &temp, &split, spectator);
		CL_PredictUsercmd(&temp, to, &split, spectator);
		return;
	}

	// TODO: clpmove was pmove
	VectorCopy(from->playerstate.origin, clpmove.origin);
	//	VectorCopy (from->playerstate.angles, clpmove.angles);
	VectorCopy(u->viewangles, clpmove.angles);
	VectorCopy(from->playerstate.velocity, clpmove.velocity);
	//VectorCopy(from->playerstate.movedir, clpmove.movedir); // TODO

	clpmove.flags = from->client.flags;
	clpmove.oldbuttons = from->playerstate.oldbuttons;
	clpmove.waterjumptime = from->client.waterjumptime;
	clpmove.dead = cl.stats[STAT_HEALTH] <= 0;
	clpmove.spectator = spectator;
	clpmove.movetype = from->playerstate.movetype;

	clpmove.onground = from->playerstate.onground;
	clpmove.waterlevel = from->client.waterlevel;
	clpmove.watertype = from->client.watertype;
	VectorCopy(clpmove.view_ofs, from->client.view_ofs);
	
	clpmove.maxspeed = from->client.maxspeed;
	
	clpmove.cmd = *u;

	ClientDLL_MoveClient(&clpmove); // TODO: was PlayerMove();
	
	//for (i=0 ; i<3 ; i++)
		//clpmove.origin[i] = ((int)(clpmove.origin[i]*8))*0.125;
	
	to->client.waterjumptime = clpmove.waterjumptime;
	to->playerstate.oldbuttons = clpmove.cmd.buttons;
	VectorCopy(clpmove.origin, to->playerstate.origin);
	VectorCopy(clpmove.angles, to->playerstate.angles);
	VectorCopy(clpmove.velocity, to->playerstate.velocity);
	to->playerstate.onground = clpmove.onground;

	//to->weaponframe = from->weaponframe;
	
	qboolean runfuncs = true; // TODO
	double time = 1.0; // TODO
	uint random_seed = 0; // TODO
	ClientDLL_PostRunCmd(from, to, u, runfuncs, time, random_seed);
}

/*
==============
CL_PredictMove
==============
*/
void CL_PredictMove()
{
#ifndef SWDS
	if(cls.state == ca_dedicated)
		return;
	
	int ack, current;
	int i;
	float f;
	frame_t *from, *to = NULL;
	int oldphysent;

	if(cl_pushlatency.value > 0)
		Cvar_Set("pushlatency", "0");

	if(cls.state != ca_active)
		return;
	
	if(cl.paused)
		return;

	cl.time = realtime - cl_pushlatency.value * 0.001; // TODO: was cl.time = realtime - cls.latency - cl_pushlatency.value * 0.001;
	if(cl.time > realtime)
		cl.time = realtime;

	if(cl.intermission)
		return;

	if(!cl.validsequence)
		return;

	ack = cls.netchan.incoming_acknowledged;
	current = cls.netchan.outgoing_sequence;
	
	// if we are too far out of date, just freeze
	if(current - ack >= UPDATE_BACKUP - 1)
	{
		return;
	}

	VectorCopy(cl.viewangles, cl.simangles);

	// this is the last frame received from the server
	from = &cl.frames[ack & UPDATE_MASK];

	// we can now render a frame
	//if(cls.state == ca_onserver) // TODO
	{
		// first update is the final signon stage
		//cls.state = ca_active; // TODO
	}

	if(cl_nopred.value)
	{
		VectorCopy(from->playerstate[cl.playernum].playerstate.velocity, cl.simvel);
		VectorCopy(from->playerstate[cl.playernum].playerstate.origin, cl.simorg);
		return;
	}

	// predict forward until cl.time <= to->senttime
	oldphysent = clpmove.numphysent;
	CL_SetSolidPlayers(cl.playernum);

	//to = &cl.frames[ack & UPDATE_MASK];

	// run frames
	for(i = 1; i < UPDATE_BACKUP - 1 && ack + i < current; i++)
	{
		to = &cl.frames[(ack + i) & UPDATE_MASK];
		CL_PredictUsercmd(&from->playerstate[cl.playernum], &to->playerstate[cl.playernum], &to->cmd, cl.spectator); // TODO: shouldn't the cmd be from the "from" state?
		if(to->senttime >= cl.time)
			break;
		from = to;
		
		// save for debug checking
		//VectorCopy(pm.s.origin, cl.predicted_origins[frame]);
	}

	clpmove.numphysent = oldphysent;

	if(i == UPDATE_BACKUP - 1 || !to)
		return; // net hasn't deliver packets in a long time...

	// now interpolate some fraction of the final frame
	if(to->senttime == from->senttime)
		f = 0;
	else
	{
		f = (cl.time - from->senttime) / (to->senttime - from->senttime);

		if(f < 0)
			f = 0;
		if(f > 1)
			f = 1;
	}

	for(i = 0; i < 3; i++)
		if(fabs(from->playerstate[cl.playernum].playerstate.origin[i] - to->playerstate[cl.playernum].playerstate.origin[i]) > 128)
		{ // teleported, so don't lerp
			VectorCopy(to->playerstate[cl.playernum].playerstate.velocity, cl.simvel);
			VectorCopy(to->playerstate[cl.playernum].playerstate.origin, cl.simorg);
			return;
		}

	for(i = 0; i < 3; i++)
	{
		cl.simorg[i] = from->playerstate[cl.playernum].playerstate.origin[i] + f * (to->playerstate[cl.playernum].playerstate.origin[i] - from->playerstate[cl.playernum].playerstate.origin[i]);
		cl.simvel[i] = from->playerstate[cl.playernum].playerstate.velocity[i] + f * (to->playerstate[cl.playernum].playerstate.velocity[i] - from->playerstate[cl.playernum].playerstate.velocity[i]);
	}
#endif // SWDS
}

/*
==============
CL_InitPrediction
==============
*/
void CL_InitPrediction()
{
	Cvar_RegisterVariable(&cl_pushlatency);
	Cvar_RegisterVariable(&cl_nopred);
}