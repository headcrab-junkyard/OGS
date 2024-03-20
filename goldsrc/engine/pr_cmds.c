/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2023 BlackPhrase
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
#include "delta.h"
#include "usermsg.h"

#include <common/Sequence.h>

extern cvar_t sv_aim;

/*
===============================================================================

						BUILT-IN FUNCTIONS

===============================================================================
*/

void PR_CheckEmptyString(const char *s)
{
	if(s[0] <= ' ')
		Host_Error("Bad string");
};

int PF_precache_model_I(const char *s)
{
	int i;

	if(sv.state != ss_loading)
		Host_Error("PF_Precache_*: Precache can only be done in spawn functions");

	PR_CheckEmptyString(s);

	for(i = 0; i < MAX_MODELS; i++)
	{
		if(!sv.model_precache[i])
		{
			sv.model_precache[i] = s;
			sv.models[i] = Mod_ForName(s, true);
			return i;
		};
		
		if(!strcmp(sv.model_precache[i], s))
			return i;
	};
	
	Host_Error("PF_precache_model: overflow");
};

int PF_precache_sound_I(const char *s)
{
	int i;

	if(sv.state != ss_loading)
		Host_Error("PF_Precache_*: Precache can only be done in spawn functions");

	PR_CheckEmptyString(s);

	for(i = 0; i < MAX_SOUNDS; i++)
	{
		if(!sv.sound_precache[i])
		{
			sv.sound_precache[i] = s;
			return i;
		};
		
		if(!strcmp(sv.sound_precache[i], s))
			return i;
	};
	
	Host_Error("PF_precache_sound: overflow");
};

void SetMinMaxSize(edict_t *e, float *min, float *max, qboolean rotate)
{
	float *angles;
	vec3_t rmin, rmax;
	float bounds[2][3];
	float xvector[2], yvector[2];
	float a;
	vec3_t base, transformed;
	int i, j, k, l;

	for(i = 0; i < 3; i++)
		if(min[i] > max[i])
			Host_Error("backwards mins/maxs");

	rotate = false; // FIXME: implement rotation properly again

	if(!rotate)
	{
		VectorCopy(min, rmin);
		VectorCopy(max, rmax);
	}
	else
	{
		// find min / max for rotations
		angles = e->v.angles;

		a = angles[1] / 180 * M_PI;

		xvector[0] = cos(a);
		xvector[1] = sin(a);
		yvector[0] = -sin(a);
		yvector[1] = cos(a);

		VectorCopy(min, bounds[0]);
		VectorCopy(max, bounds[1]);

		rmin[0] = rmin[1] = rmin[2] = 9999;
		rmax[0] = rmax[1] = rmax[2] = -9999;

		for(i = 0; i <= 1; i++)
		{
			base[0] = bounds[i][0];
			for(j = 0; j <= 1; j++)
			{
				base[1] = bounds[j][1];
				for(k = 0; k <= 1; k++)
				{
					base[2] = bounds[k][2];

					// transform the point
					transformed[0] = xvector[0] * base[0] + yvector[0] * base[1];
					transformed[1] = xvector[1] * base[0] + yvector[1] * base[1];
					transformed[2] = base[2];

					for(l = 0; l < 3; l++)
					{
						if(transformed[l] < rmin[l])
							rmin[l] = transformed[l];
						if(transformed[l] > rmax[l])
							rmax[l] = transformed[l];
					};
				};
			};
		};
	};

	// set derived values
	VectorCopy(rmin, e->v.mins);
	VectorCopy(rmax, e->v.maxs);
	VectorSubtract(max, min, e->v.size);

	SV_LinkEdict(e, false);
};

/*
=================
PF_setmodel

setmodel(entity, model)
=================
*/
void PF_setmodel_I(edict_t *e, const char *m)
{
	char **check;
	model_t *mod;
	int i;

	// check to see if model was properly precached
	for(i = 0, check = sv.model_precache; *check; i++, check++)
		if(!strcmp(*check, m))
			break;

	if(!*check)
		Host_Error("no precache: %s\n", m);

	e->v.model = m - pr_strings;
	e->v.modelindex = i; //SV_ModelIndex (m);

	mod = sv.models[(int)e->v.modelindex]; // Mod_ForName (m, true);

	if(mod)
		SetMinMaxSize(e, mod->mins, mod->maxs, true);
	else
		SetMinMaxSize(e, vec3_origin, vec3_origin, true);
};

int PF_modelindex(const char *name)
{
	return SV_ModelIndex(name);
};

int ModelFrames(int modelindex)
{
	return 0; // TODO
};

/*
=================
PF_setsize

the size box is rotated by the current angle

setsize (entity, minvector, maxvector)
=================
*/
void PF_setsize_I(edict_t *e, const float *min, const float *max)
{
	SetMinMaxSize(e, min, max, false);
};

/*
==============
PF_changelevel
==============
*/
void PF_changelevel_I(const char *s1, const char *s2)
{
	// make sure we don't issue two changelevels
	if(svs.changelevel_issued)
		return;
	svs.changelevel_issued = true;

	if(s2 && *s2)
		//if ((int)gGlobalVariables.serverflags & (SFL_NEW_UNIT | SFL_NEW_EPISODE))
			//Cbuf_AddText (va("changelevel %s %s\n",s1, s2));
		//else
			Cbuf_AddText(va("changelevel2 %s %s\n", s1, s2));
	else
		Cbuf_AddText(va("changelevel %s\n", s1));
};

/*
==============
PF_setspawnparms
==============
*/
void PF_setspawnparms_I(edict_t *ent)
{
	int i = NUM_FOR_EDICT(ent);
	if(i < 1 || i > svs.maxclients)
		Host_Error("Entity is not a client");

	// copy spawn parms out of the client_t
	client_t *client = svs.clients + (i - 1);

	//for(i = 0; i < NUM_SPAWN_PARMS; i++)
		//(&gGlobalVariables.parm1)[i] = client->spawn_parms[i];
};

void SaveSpawnParms(edict_t *ent)
{
	// TODO
};

/*
=================
PF_vectoyaw

float vectoyaw(vector)
=================
*/
float PF_vectoyaw_I(const float *value1)
{
	float yaw;

	if(value1[1] == 0 && value1[0] == 0)
		yaw = 0;
	else
	{
		yaw = (int)(atan2(value1[1], value1[0]) * 180 / M_PI);
		if(yaw < 0)
			yaw += 360;
	};

	return yaw;
};

/*
=================
PF_vectoangles

vector vectoangles(vector)
=================
*/
void PF_vectoangles_I(const float *valueIn, float *valueOut)
{
	float forward;
	float yaw, pitch;
	
	if(!valueIn || !valueOut)
		return;

	if(valueIn[1] == 0 && valueIn[0] == 0)
	{
		yaw = 0;
		if(valueIn[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (int)(atan2(valueIn[1], valueIn[0]) * 180 / M_PI);
		if(yaw < 0)
			yaw += 360;

		forward = sqrt(valueIn[0] * valueIn[0] + valueIn[1] * valueIn[1]);
		pitch = (int)(atan2(valueIn[2], forward) * 180 / M_PI);
		if(pitch < 0)
			pitch += 360;
	};

	valueOut[0] = pitch;
	valueOut[1] = yaw;
	valueOut[2] = 0;
};

void SV_MoveToOrigin_I(edict_t *pEnt, const float *vGoal, float fDist, int nMoveType)
{
	if(!pEnt)
		return;
	
	// TODO
};

/*
==============
PF_changeyaw

This was a major timewaster in progs, so it was converted to C
==============
*/
void PF_changeyaw_I(edict_t *ent)
{
	//ent = PROG_TO_EDICT(gGlobalVariables.self);
	float current = anglemod(ent->v.angles[1]);
	float ideal = ent->v.ideal_yaw;
	float speed = ent->v.yaw_speed;

	if(current == ideal)
		return;
	float move = ideal - current;
	if(ideal > current)
	{
		if(move >= 180)
			move = move - 360;
	}
	else
	{
		if(move <= -180)
			move = move + 360;
	};
	
	if(move > 0)
	{
		if(move > speed)
			move = speed;
	}
	else
	{
		if(move < -speed)
			move = -speed;
	};

	ent->v.angles[1] = anglemod(current + move);
};

/*
==============
PF_changepitch
==============
*/
void PF_changepitch_I(edict_t *ent)
{
	float current = anglemod(ent->v.angles[0]);
	float ideal = ent->v.idealpitch;
	float speed = ent->v.pitch_speed;

	if(current == ideal)
		return;
	float move = ideal - current;
	if(ideal > current)
	{
		if(move >= 180)
			move = move - 360;
	}
	else
	{
		if(move <= -180)
			move = move + 360;
	};
	
	if(move > 0)
	{
		if(move > speed)
			move = speed;
	}
	else
	{
		if(move < -speed)
			move = -speed;
	};

	ent->v.angles[0] = anglemod(current + move);
};

// entity (entity start, .string field, string match) find = #5;
//void PF_Find(edict_t *start, int f, const char *s)
edict_t *FindEntityByString(edict_t *pEntStartSearchAfter, const char *sField, const char *sValue)
{
#ifdef QUAKE2
	int e;
	char *t;
	edict_t *ed;
	edict_t *first;
	edict_t *second;
	edict_t *last;

	first = second = last = (edict_t *)sv.edicts;
	e = EDICT_NUM(pEntStartSearchAfter);
	if(!s)
		Host_Error("PF_Find: bad search string");

	for(e++; e < sv.num_edicts; e++)
	{
		ed = EDICT_NUM(e);
		if(ed->free)
			continue;
		t = E_STRING(ed, sField);
		if(!t)
			continue;
		if(!strcmp(t, sValue))
		{
			if(first == (edict_t *)sv.edicts)
				first = ed;
			else if(second == (edict_t *)sv.edicts)
				second = ed;
			ed->v.chain = EDICT_TO_PROG(last);
			last = ed;
		}
	}

	if(first != last)
	{
		if(last != second)
			first->v.chain = last->v.chain;
		else
			first->v.chain = EDICT_TO_PROG(last);
		last->v.chain = EDICT_TO_PROG((edict_t *)sv.edicts);
		if(second && second != last)
			second->v.chain = EDICT_TO_PROG(last);
	}
	return first;
#else
	int e;
	char *t;
	edict_t *ed;

	e = EDICT_NUM(pEntStartSearchAfter);
	if(!sValue)
		Host_Error("PF_Find: bad search string");

	for(e++; e < sv.num_edicts; e++)
	{
		ed = EDICT_NUM(pEntStartSearchAfter);
		if(ed->free)
			continue;
		//t = E_STRING(ed, sField); // TODO
		if(!t)
			continue;
		if(!strcmp(t, sValue))
		{
			return ed;
		}
	}

	return sv.edicts;
#endif
};

int GetEntityIllum(edict_t *pEnt)
{
	if(!pEnt)
		return 0;
	
	return pEnt->v.light_level;
};

/*
=================
PF_findradius

Returns a chain of entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
edict_t *FindEntityInSphere(edict_t *pEntStartSearchAfter, const float *origin, float radius)
{
	edict_t *ent, *chain;
	vec3_t eorg;
	int i, j;

	chain = (edict_t *)sv.edicts;

	ent = NEXT_EDICT(pEntStartSearchAfter);
	for(i = 1; i < sv.num_edicts; i++, ent = NEXT_EDICT(ent))
	{
		if(ent->free)
			continue;
		if(ent->v.solid == SOLID_NOT)
			continue;
		for(j = 0; j < 3; j++)
			eorg[j] = origin[j] - (ent->v.origin[j] + (ent->v.mins[j] + ent->v.maxs[j]) * 0.5);
		if(Length(eorg) > radius)
			continue;

		ent->v.chain = EDICT_TO_PROG(chain);
		chain = ent;
	}

	return chain;
};

edict_t *PF_checkclient_I(edict_t *pEnt)
{
	// TODO
	return NULL;
};

edict_t *PVSFindEntities(edict_t *pPlayer)
{
	// TODO
	return NULL;
};

/*
==============
PF_makevectors

Writes new values for v_forward, v_up, and v_right based on angles
makevectors(vector)
==============
*/
void PF_makevectors_I(const float *v)
{
	AngleVectors(v, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
};

edict_t *PF_Spawn_I()
{
	edict_t *ed = ED_Alloc();
	return ed;
};

void PF_Remove_I(edict_t *ed)
{
	ED_Free(ed);
};

edict_t *CreateNamedEntity(int nClassName)
{
	// TODO
	edict_t *ed = ED_Alloc();
	//Q_strcpy(ed->netname, ...);
	return ed;
};

int SV_ModelIndex(const char *name); // TODO

void PF_makestatic_I(edict_t *ent)
{
	MSG_WriteByte(&sv.signon, svc_spawnstatic);

	MSG_WriteByte(&sv.signon, SV_ModelIndex(pr_strings + ent->v.model));

	MSG_WriteByte(&sv.signon, ent->v.frame);
	MSG_WriteByte(&sv.signon, ent->v.colormap);
	MSG_WriteByte(&sv.signon, ent->v.skin);

	for(int i = 0; i < 3; i++)
	{
		MSG_WriteCoord(&sv.signon, ent->v.origin[i]);
		MSG_WriteAngle(&sv.signon, ent->v.angles[i]);
	};

	// throw the entity away now
	ED_Free(ent);
};

/*
=============
PF_checkbottom
=============
*/
int PF_checkbottom_I(edict_t *ent)
{
	return SV_CheckBottom(ent);
};

/*
===============
PF_droptofloor

void() droptofloor
===============
*/
int PF_droptofloor_I(edict_t *ent)
{
	vec3_t end;
	trace_t trace;

	VectorCopy(ent->v.origin, end);
	end[2] -= 256;

	trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, false, ent);

	if(trace.fraction == 1 || trace.allsolid)
		return 0;
	else
	{
		VectorCopy(trace.endpos, ent->v.origin);
		SV_LinkEdict(ent, false);
		ent->v.flags = (int)ent->v.flags | FL_ONGROUND;
		ent->v.groundentity = EDICT_TO_PROG(trace.ent);
		return 1;
	};
};

/*
===============
PF_walkmove

float(float yaw, float dist) walkmove
===============
*/
int PF_walkmove_I(edict_t *ent, float yaw, float dist, int nMode) // TODO: nMode support
{
	vec3_t move;

	if(!((int)ent->v.flags & (FL_ONGROUND | FL_FLY | FL_SWIM)))
		return 0;

	yaw = yaw * M_PI * 2 / 360;

	move[0] = cos(yaw) * dist;
	move[1] = sin(yaw) * dist;
	move[2] = 0;

	return SV_movestep(ent, move, true);
};

/*
=================
PF_setorigin

This is the only valid way to move an object without using the physics of the world (setting velocity and waiting).  Directly changing origin will not set internal links correctly, so clipping would be messed up.  This should be called when an object is spawned, and then only if it is teleported.

setorigin (entity, origin)
=================
*/
void PF_setorigin_I(edict_t *e, const float *org)
{
	VectorCopy(org, e->v.origin);
	SV_LinkEdict(e, false);
};

/*
=================
PF_sound

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

Channel 0 is an auto-allocate channel, the others override anything
allready running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the level.
Larger attenuations will drop off.

=================
*/
void PF_sound_I(edict_t *entity, int channel, const char *sample, int volume, float attenuation)
{
	volume *= 255;

	if(volume < 0 || volume > 255)
		Sys_Error("SV_StartSound: volume = %i", volume);

	if(attenuation < 0 || attenuation > 4)
		Sys_Error("SV_StartSound: attenuation = %f", attenuation);

	if(channel < 0 || channel > 7)
		Sys_Error("SV_StartSound: channel = %i", channel);

	SV_StartSound(entity, channel, sample, volume, attenuation, PITCH_NORM);
};

/*
=================
PF_ambientsound

=================
*/
void PF_ambientsound_I(float *pos, const char *samp, float vol, float attenuation)
{
	char **check;
	int i, soundnum;

	// check to see if samp was properly precached
	for(soundnum = 0, check = sv.sound_precache; *check; check++, soundnum++)
		if(!strcmp(*check, samp))
			break;

	if(!*check)
	{
		Con_Printf("no precache: %s\n", samp);
		return;
	};

	// add an svc_spawnambient command to the level signon packet

	MSG_WriteByte(&sv.signon, svc_spawnstaticsound);
	for(i = 0; i < 3; i++)
		MSG_WriteCoord(&sv.signon, pos[i]);

	MSG_WriteByte(&sv.signon, soundnum);

	MSG_WriteByte(&sv.signon, vol * 255);
	MSG_WriteByte(&sv.signon, attenuation * 64);
};

/*
=================
PF_traceline

Used for use tracing and shot targeting
Traces are blocked by bbox and exact bsp entities, and also slide box entities
if the tryents flag is set.

traceline (vector1, vector2, tryents)
=================
*/
//void PF_traceline(float *v1, float *v2, int nomonsters, edict_t *ent)
void PF_traceline_DLL(const float *vPointA, const float *vPointB, int nNoMonsters, edict_t *pEntToSkip, TraceResult *pTraceResult) // TODO: handle pTraceResult
{
	trace_t trace;

	trace = SV_Move(vPointA, vec3_origin, vec3_origin, vPointB, nNoMonsters, pEntToSkip);

	gGlobalVariables.trace_allsolid = trace.allsolid;
	gGlobalVariables.trace_startsolid = trace.startsolid;
	gGlobalVariables.trace_fraction = trace.fraction;
	gGlobalVariables.trace_inwater = trace.inwater;
	gGlobalVariables.trace_inopen = trace.inopen;

	VectorCopy(trace.endpos, gGlobalVariables.trace_endpos);
	VectorCopy(trace.plane.normal, gGlobalVariables.trace_plane_normal);

	gGlobalVariables.trace_plane_dist = trace.plane.dist;

	if(trace.ent)
		gGlobalVariables.trace_ent = EDICT_TO_PROG(trace.ent);
	else
		gGlobalVariables.trace_ent = EDICT_TO_PROG(sv.edicts);
};

// TODO: q2
extern trace_t SV_Trace_Toss(edict_t *ent, edict_t *ignore);
//void PF_TraceToss(edict_t *ent, edict_t *ignore)
void PF_TraceToss_DLL(edict_t *pEnt, edict_t *pEntToIgnore, TraceResult *pTraceResult) // TODO: handle pTraceResult
{
	trace_t trace;

	//trace = SV_Trace_Toss(pEnt, pEntToIgnore); // TODO

	gGlobalVariables.trace_allsolid = trace.allsolid;
	gGlobalVariables.trace_startsolid = trace.startsolid;
	gGlobalVariables.trace_fraction = trace.fraction;
	gGlobalVariables.trace_inwater = trace.inwater;
	gGlobalVariables.trace_inopen = trace.inopen;

	VectorCopy(trace.endpos, gGlobalVariables.trace_endpos);
	VectorCopy(trace.plane.normal, gGlobalVariables.trace_plane_normal);

	gGlobalVariables.trace_plane_dist = trace.plane.dist;

	if(trace.ent)
		gGlobalVariables.trace_ent = EDICT_TO_PROG(trace.ent);
	else
		gGlobalVariables.trace_ent = EDICT_TO_PROG(sv.edicts);
};

int TraceMonsterHull(edict_t *pEnt, const float *vPointA, const float *vPointB, int nNoMonsters, edict_t *pEntToSkip, TraceResult *pTraceResult)
{
	// TODO
	return 0;
};

void TraceHull(const float *vPointA, const float *vPointB, int nNoMonsters, int nHullNumber, edict_t *pEntToSkip, TraceResult *pTraceResult)
{
	// TODO
};

void TraceModel(const float *vPointA, const float *vPointB, int nHullNumber, edict_t *pEnt, TraceResult *pTraceResult)
{
	// TODO
};

const char *TraceTexture(edict_t *pEntity, const float *vPointA, const float *vPointB)
{
	// TODO
	return "TODO";
};

void TraceSphere(const float *vPointA, const float *vPointB, int nNoMonsters, float fRadius, edict_t *pEntToSkip, TraceResult *pTraceResult)
{
	// TODO
};

/*
=============
PF_aim

Pick a vector for the player to shoot along
vector aim(entity, missilespeed)
=============
*/
void PF_aim_I(edict_t *ent, float speed, float *vReturn) // TODO: vReturn support
{
	edict_t *check, *bestent;
	vec3_t start, dir, end, bestdir;
	int i, j;
	trace_t tr;
	float dist, bestdist;

	VectorCopy(ent->v.origin, start);
	start[2] += 20;

	// try sending a trace straight
	VectorCopy(gGlobalVariables.v_forward, dir);
	VectorMA(start, 2048, dir, end);
	tr = SV_Move(start, vec3_origin, vec3_origin, end, false, ent);
	if(tr.ent && tr.ent->v.takedamage == DAMAGE_AIM && (!teamplay.value || ent->v.team <= 0 || ent->v.team != tr.ent->v.team))
	{
		if(vReturn)
			VectorCopy(gGlobalVariables.v_forward, vReturn);
		return;
	};

	// try all possible entities
	VectorCopy(dir, bestdir);
	bestdist = sv_aim.value;
	bestent = NULL;

	check = NEXT_EDICT(sv.edicts);
	for(i = 1; i < sv.num_edicts; i++, check = NEXT_EDICT(check))
	{
		if(check->v.takedamage != DAMAGE_AIM)
			continue;
		if(check == ent)
			continue;
		if(teamplay.value && ent->v.team > 0 && ent->v.team == check->v.team)
			continue; // don't aim at teammate
		for(j = 0; j < 3; j++)
			end[j] = check->v.origin[j] + 0.5 * (check->v.mins[j] + check->v.maxs[j]);
		VectorSubtract(end, start, dir);
		VectorNormalize(dir);
		dist = DotProduct(dir, gGlobalVariables.v_forward);
		if(dist < bestdist)
			continue; // to far to turn
		tr = SV_Move(start, vec3_origin, vec3_origin, end, false, ent);
		if(tr.ent == check)
		{ // can shoot at this one
			bestdist = dist;
			bestent = check;
		};
	};

	if(bestent)
	{
		VectorSubtract(bestent->v.origin, ent->v.origin, dir);
		dist = DotProduct(dir, gGlobalVariables.v_forward);
		VectorScale(gGlobalVariables.v_forward, dist, end);
		end[2] = dir[2];
		VectorNormalize(end);
		if(vReturn)
			VectorCopy(end, vReturn);
	}
	else
		if(vReturn)
			VectorCopy(bestdir, vReturn);
};

/*
=================
PF_localcmd

Sends text over to the client's execution buffer

localcmd (string)
=================
*/
void PF_localcmd_I(const char *str)
{
	Cbuf_AddText(str);
};

void PF_localexec_I()
{
	Cbuf_Execute();
};

/*
=================
PF_stuffcmd

Sends text over to the client's execution buffer

stuffcmd (clientent, value)
=================
*/
void PF_stuffcmd_I(edict_t *client, const char *str, ...) // TODO: fmt support
{
	int entnum;
	client_t *old;

	entnum = NUM_FOR_EDICT(client);
	if(entnum < 1 || entnum > svs.maxclients)
		Host_Error("Parm 0 not a client");

	old = host_client;
	host_client = &svs.clients[entnum - 1];
	Host_ClientCommands("%s", str);
	host_client = old;
};

/*
=================
PF_particle

particle(origin, color, count)
=================
*/
void PF_particle_I(const float *org, const float *dir, float color, float count)
{
	SV_StartParticle(org, dir, color, count);
};

/*
===============
PF_lightstyle

void(float style, string value) lightstyle
===============
*/
void PF_lightstyle_I(int style, char *val)
{
	// change the string in sv
	sv.lightstyles[style] = val;

	// send message to all clients on this server
	if(sv.state != ss_active)
		return;
	
	client_t *client;
	int j;
	
	for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
		if(client->active || client->spawned)
		{
			MSG_WriteChar(&client->netchan.message, svc_lightstyle);
			MSG_WriteChar(&client->netchan.message, style);
			MSG_WriteString(&client->netchan.message, val);
		};
};

int PF_DecalIndex(const char *name)
{
	// TODO
	return 0;
};

/*
=============
PF_pointcontents
=============
*/
int PF_pointcontents_I(const float *vPoint)
{
	return SV_PointContents(vPoint);
};

/*
===============================================================================

MESSAGE WRITING

===============================================================================
*/

sizebuf_t *WriteDest(int dest)
{
	int entnum;
	edict_t *ent;

	switch(dest)
	{
	case MSG_BROADCAST:
		return &sv.datagram;

	case MSG_ONE:
		ent = PROG_TO_EDICT(gGlobalVariables.msg_entity);
		entnum = NUM_FOR_EDICT(ent);
		if(entnum < 1 || entnum > svs.maxclients)
			Host_Error("WriteDest: not a client");
		return &svs.clients[entnum - 1].netchan.message;

	case MSG_ALL:
		return &sv.reliable_datagram;

	case MSG_INIT:
		return &sv.signon;

	default:
		Host_Error("WriteDest: bad destination");
		break;
	}

	return NULL;
}

int gnMsgDest = 0;

void PF_MessageBegin_I(int msg_dest, int msg_type, const float *vOrigin, edict_t *ed)
{
	// TODO
	
	gnMsgDest = msg_dest;
};

void PF_MessageEnd_I()
{
	// TODO
	
	gnMsgDest = 0;
};

void PF_WriteByte_I(int val)
{
	MSG_WriteByte(WriteDest(gnMsgDest), val);
};

void PF_WriteChar_I(int val)
{
	MSG_WriteChar(WriteDest(gnMsgDest), val);
};

void PF_WriteShort_I(int val)
{
	MSG_WriteShort(WriteDest(gnMsgDest), val);
};

void PF_WriteLong_I(int val)
{
	MSG_WriteLong(WriteDest(gnMsgDest), val);
};

void PF_WriteAngle_I(float val)
{
	MSG_WriteAngle(WriteDest(gnMsgDest), val);
};

void PF_WriteCoord_I(float val)
{
	MSG_WriteCoord(WriteDest(gnMsgDest), val);
};

void PF_WriteString_I(const char *s)
{
	MSG_WriteString(WriteDest(gnMsgDest), s);
};

void PF_WriteEntity_I(int val)
{
	MSG_WriteShort(WriteDest(gnMsgDest), NUM_FOR_EDICT(EDICT_NUM(val))); // TODO: wtf?
};

void CVarRegister(struct cvar_s *var)
{
	// TODO
};

/*
=================
PF_cvar

float cvar (string)
=================
*/
float CVarGetFloat(const char *name)
{
	return Cvar_VariableValue(name);
};

const char *CVarGetString(const char *name)
{
	return Cvar_VariableString(name);
};

void CVarSetFloat(const char *name, float value)
{
	Cvar_SetValue(name, value);
};

/*
=================
PF_cvar_set
=================
*/
void CVarSetString(const char *name, const char *value)
{
	Cvar_Set(name, value);
};

void AlertMessage(ALERT_TYPE aType, const char *sMsg, ...)
{
	// TODO
	//Obsolete API!!!!!!!!!!!!!!!!!!!!!
};

void EngineFprintf(void *pFile, const char *sMsg, ...)
{
	// TODO
};

void *PvAllocEntPrivateData(edict_t *pEnt, int32_t cb)
{
	if(!pEnt)
		return NULL;
	
	void *pData = Mem_Alloc((size_t)cb); // TODO
	
	if(pData)
	{
		Q_memset(pData, 0, cb);
		pEnt->pvPrivateData = pData; // TODO: I think that's the reason it passes a pointer to an entity in here...
		return pData;
	};
	
	pEnt->pvPrivateData = NULL;
	return NULL;
};

void *PvEntPrivateData(edict_t *pEnt)
{
	if(!pEnt)
		return NULL;
	
	return pEnt->pvPrivateData;
};

void FreeEntPrivateData(edict_t *pEnt)
{
	if(!pEnt)
		return;
	
	if(!pEnt->pvPrivateData)
		return;
	
	if(gNewDLLFunctions.pfnOnFreeEntPrivateData)
		gNewDLLFunctions.pfnOnFreeEntPrivateData(pEnt);
	
	Mem_Free(pEnt->pvPrivateData);
};

const char *SzFromIndex(int nString)
{
	// TODO
	return "";
};

int AllocEngineString(const char *sValue)
{
	// TODO
	return 0;
};

struct entvars_s *GetVarsOfEnt(edict_t *pEnt)
{
	if(pEnt)
		return NULL;
	
	return &pEnt->v;
};

edict_t *PEntityOfEntOffset(int nOffset)
{
	// TODO
	return NULL;
};

int EntOffsetOfPEntity(const edict_t *pEnt)
{
	// TODO
	return 0;
};

extern int IndexOfEdict(const edict_t *pEnt);
/*
int IndexOfEdict(const edict_t *pEnt)
{
	// TODO
	return 0;
};
*/

edict_t *PEntityOfEntIndex(int nIndex)
{
	if(nIndex < 0 || nIndex > sv.max_edicts)
		return NULL;
	
	return &sv.edicts[nIndex];
};

edict_t *FindEntityByVars(struct entvars_s *pVars)
{
	if(!pVars)
		return NULL;
	
	for(int i = 0; i < sv.max_edicts; i++)
		if(&sv.edicts[i].v == pVars)
			return &sv.edicts[i];
	
	return NULL;
};

void *GetModelPtr(edict_t *pEnt)
{
	if(!pEnt)
		return NULL;
	
	// TODO
	return NULL;
};

void AnimationAutomove(const edict_t *pEnt, float fTime)
{
	// TODO
};

void GetBonePosition(const edict_t *pEnt, int nBone, float *vOrigin, float *vAngles)
{
	// TODO
	//model_t *pModel = sv.models[pEnt->v.modelindex];
};

uint32_t FunctionFromName(const char *sName)
{
	// TODO
	return 0;
};

const char *NameForFunction(uint32_t nFunction)
{
	// TODO
	return "";
};

void /*QDECL*/ ClientPrintf(edict_t *pEnt, PRINT_TYPE aType, const char *sMsg)
{
	int entnum = NUM_FOR_EDICT(pEnt);

	if(entnum < 1 || entnum > svs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	};

	client_t *pClient = &svs.clients[entnum - 1];
	
	switch(aType)
	{
	case print_console:
		// TODO
		break;
	case print_center: // single print to a specific client
		MSG_WriteChar(&pClient->netchan.message, svc_centerprint);
		break;
	case print_chat:
		MSG_WriteChar(&pClient->netchan.message, svc_print);
		break;
	};
	
	MSG_WriteString(&pClient->netchan.message, sMsg);
};

/*
=================
PF_bprint

broadcast print to everyone on server

bprint(value)
=================
*/
void PF_bprint(const char *s)
{
	SV_BroadcastPrintf("%s", s);
};

void ServerPrint(const char *sMsg)
{
	PF_bprint(sMsg);
};

void GetAttachment(const edict_t *pEnt, int nAttachment, float *vOrigin, float *vAngles)
{
	// TODO
};

static float frandom()
{
	return ((float)rand() / RAND_MAX);
};

int32_t /*QDECL*/ RandomLong(int32_t nLow, int32_t nHigh)
{
	//bi_trace();
	return (int32_t)(nLow + (frandom() * (nHigh - nLow)));
};

float RandomFloat(float fLow, float fHigh)
{
	//bi_trace();
	return fLow + (frandom() * (fHigh - fLow));
};

void PF_setview_I(const edict_t *pEnt, const edict_t *pViewEnt)
{
	if(!pEnt)
		return;
	
	// TODO
	//if(!pViewEnt)
		//return;
	
	//pEnt->v.viewmodel = pViewEnt->v.modelindex; // TODO
};

float PF_Time()
{
	return sv.time;
};

void PF_crosshairangle_I(const edict_t *pClient, float pitch, float yaw)
{
	if(!pClient)
		return; // TODO: or broadcast to everyone?
	
	SV_SendCrosshairAngle(svs.clients[NUM_FOR_EDICT(pClient) - 1], pitch, yaw);
};

// TODO
//pfnCvar_RegisterVariable = 0x981fd7a4 <meta_CVarRegister(cvar_s*)>,

void PF_FadeVolume(const edict_t *pClient, int nFadePercent, int nFadeOutSeconds, int nHoldTime, int nFadeInSeconds)
{
	if(!pClient)
		return; // TODO: or broadcast to everyone?
	
	SV_SendSoundFade(svs.clients[NUM_FOR_EDICT(pClient) - 1], nFadePercent, nHoldTime, nFadeOutSeconds, nFadeInSeconds);
};

void PF_SetClientMaxspeed(const edict_t *pClient, float fSpeed)
{
	// TODO
};

edict_t *PF_CreateFakeClient_I(const char *netname)
{
	// TODO
	return NULL;
};

void PF_RunPlayerMove_I(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec)
{
	// TODO
};

int PF_NumberOfEntities_I()
{
	// TODO
	return 0;
};

char *PF_GetInfoKeyBuffer_I(edict_t *pent)
{
	if(!pent)
		return svs.info;
	
	return svs.clients[NUM_FOR_EDICT(pent)-1].userinfo;
};

char *PF_InfoKeyValue_I(char *infobuffer, char *key)
{
	return Info_ValueForKey(infobuffer, key);
};

void PF_SetKeyValue_I(char *infobuffer, char *key, char *value)
{
	Info_SetValueForKey(infobuffer, key, value, MAX_INFO_STRING);
};

void PF_SetClientKeyValue_I(int clientIndex, char *infobuffer, char *key, char *value)
{
	// TODO
};

int PF_IsMapValid_I(char *filename)
{
	// TODO
	return 0;
};

void PF_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
	// TODO
};

int PF_precache_generic_I(char *name)
{
	// TODO
	return 0;
};

int PF_GetPlayerUserId(edict_t *player)
{
	if(!player)
		return 0;
	
	return svs.clients[NUM_FOR_EDICT(player) - 1].userid;
};

void PF_BuildSoundMsg_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int nFlags, int pitch, int msg_dest, int msg_type, const float *vOrigin, edict_t *ed)
{
	// TODO
};

int PF_IsDedicatedServer()
{
	return isDedicated;
};

cvar_t *CVarGetPointer(const char *name)
{
	return Cvar_FindVar(name);
};

uint PF_GetPlayerWONId(edict_t *player)
{
	if(!player)
		return 0;
	
	// TODO
	return 0;
};

void PF_RemoveKey_I(char *infobuffer, const char *key)
{
	Info_RemoveKey(infobuffer, key);
};

const char *PF_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
	if(!pClient)
		return NULL;
	
	return Info_ValueForKey(svs.clients[NUM_FOR_EDICT(pClient) - 1].physinfo, key);
};

void PF_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
	if(!pClient)
		return;
	
	Info_SetValueForKey(svs.clients[NUM_FOR_EDICT(pClient) - 1].physinfo, key, value, MAX_INFO_STRING);
};

const char *PF_GetPhysicsInfoString(const edict_t *pClient)
{
	if(!pClient)
		return NULL;
	
	return svs.clients[NUM_FOR_EDICT(pClient) - 1].physinfo;
};


unsigned short EV_Precache(int nType, const char *sName)
{
	static unsigned short nLast = 0;
	
	if(nType != 1)
		Sys_Error("EV_Precache: only file type 1 supported currently");
	
	if(!sName)
		Sys_Error("EV_Precache: NULL pointer");
	
	if(!*sName)
		Sys_Error("EV_Precache: Bad string '%s'"); // TODO
	
	if(!FS_FileExists(sName))
		Sys_Error("EV_Precache: file %s missing from server", sName);

	Q_strcpy(sv.event_precache[nLast]->name, sName);
	
	return nLast++;
};

void EV_SV_Playback(int nFlags, int nClientIndex, unsigned short nEventIndex, float fDelay,
					float *vOrigin, float *vAngles, float fParam1, float fParam2, int nParam1, int nParam2,
					int bParam1, int bParam2)
{
	event_args_t args;
	Q_memset(&args, 0, sizeof(args));
	
	VectorCopy(vOrigin, args.origin);
	VectorCopy(vAngles, args.angles);
	
	args.fparam1 = fParam1;
	args.fparam2 = fParam2;
	
	args.iparam1 = nParam1;
	args.iparam2 = nParam2;
	
	args.bparam1 = bParam1;
	args.bparam2 = bParam2;
	
	args.flags = nFlags; // TODO
	
	// TODO: nClientIndex
	
	SV_BroadcastEvent(nFlags, nEventIndex, fDelay, &args); // TODO
};

extern byte *SV_FatPVS(float *org);
/*
byte *SV_FatPVS(float *org)
{
	// TODO
	return NULL;
};
*/

byte *SV_FatPAS(float *org)
{
	// TODO
	return NULL;
};

int SV_CheckVisibility(const edict_t *entity, byte *pset)
{
	// TODO
	return 0;
};

int PF_GetCurrentPlayer()
{
	// TODO
	return 0;
};

int PF_CanSkipPlayer(const edict_t *player)
{
	// TODO
	return 0;
};

void PF_SetGroupMask(int mask, int op)
{
	// TODO
};

int PF_CreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	// TODO
	return 0;
};

void PF_Cvar_DirectSet(struct cvar_s *var, char *value)
{
	//Cvar_DirectSet(var, value); // TODO
};

void PF_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	// TODO
	
	switch(type)
	{
	case force_exactfile:
		break;
	case force_model_samebounds:
		break;
	case force_model_specifybounds:
		break;
	case force_model_specifybounds_if_avail:
		break;
	};
};

void PF_GetPlayerStats(const edict_t *pPlayer, int *ping, int *packet_loss)
{
	if(!pPlayer)
		return;
	
	if(ping)
		*ping = 0;
	
	if(packet_loss)
		*packet_loss = 0;
};

void Cmd_AddServerCommand(char *cmd_name, void (*function)())
{
	// TODO
	//Cmd_AddCommand(name, function, FCMD_EXTDLL);
};

qboolean Voice_GetClientListening(int nReceiver, int nSender)
{
	// TODO
	return false; // vShouldReceiveFromSender[nReceiver][nSender];
};

qboolean Voice_SetClientListening(int nReceiver, int nSender, qboolean bListen)
{
	// TODO: vShouldReceiveFromSender[nReceiver][nSender] = bListen;
	return false;
};

const char *pfnGetPlayerAuthId(edict_t *player)
{
	// TODO
	return "";
};

sequenceEntry_s *pfnSequenceGet(const char *sFileName, const char *sEntryName)
{
	// TODO
	return SequenceGet(sFileName, sEntryName);
};

sentenceEntry_s *pfnSequencePickSentence(const char *sGroupName, int nPickMethod, int *pPicked)
{
	// TODO
	return SequencePickSentence(sGroupName, nPickMethod, pPicked);
};

int pfnGetFileSize(char *filename)
{
	return FS_FileSize(filename);
};

uint pfnGetApproxWavePlayLen(const char *filepath)
{
	// TODO
	return 0; // TODO: COM_GetApproxWavePlayLen(filepath);
};

int pfnIsCareerMatch()
{
	// TODO: implemented by GameUI's ICareerUI interface?
	return 0;
};

int pfnGetLocalizedStringLength(const char *label)
{
	// TODO: this is probably asks the VGUI2's ILocalize interface for that
	return 0;
};

void pfnRegisterTutorMessageShown(int mid)
{
	// TODO
};

int pfnGetTimesTutorMessageShown(int mid)
{
	// TODO
};

void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	// TODO
};

void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	// TODO
};

void ResetTutorMessageDecayData()
{
	// TODO
};

void pfnQueryClientCvarValue(const edict_t *player, const char *cvarName)
{
	if(!player)
		return;
	
	if(!cvarName || !*cvarName)
		return;
	
	SV_QueryCvarValue(svs.clients[NUM_FOR_EDICT(pClient) - 1], cvarName);
};

void pfnQueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
	if(!player)
		return;
	
	if(!cvarName || !*cvarName)
		return;
	
	SV_QueryCvarValueEx(svs.clients[NUM_FOR_EDICT(pClient) - 1], cvarName, requestID);
};

int pfnCheckParm(const char *sCmdLineToken, char **ppnext)
{
	// TODO
	return 0;
};

enginefuncs_t gEngineFuncs =
{
	PF_precache_model_I,
	PF_precache_sound_I,
	
	PF_setmodel_I,
	PF_modelindex,
	ModelFrames,
	
	PF_setsize_I,
	
	PF_changelevel_I,
	
	PF_setspawnparms_I,
	SaveSpawnParms,

	PF_vectoyaw_I,
	PF_vectoangles_I,
	
	SV_MoveToOrigin_I,
	
	PF_changeyaw_I,
	PF_changepitch_I,
	
	FindEntityByString,
	
	GetEntityIllum,
	
	FindEntityInSphere,
	PF_checkclient_I,
	PVSFindEntities,
	
	PF_makevectors_I,
	AngleVectors,
	
	PF_Spawn_I,
	PF_Remove_I,
	
	CreateNamedEntity,
	
	PF_makestatic_I,
	PF_checkbottom_I,
	PF_droptofloor_I,
	PF_walkmove_I,
	PF_setorigin_I,
	
	PF_sound_I,
	PF_ambientsound_I,
	
	PF_traceline_DLL,
	PF_TraceToss_DLL,
	
	TraceMonsterHull,
	TraceHull,
	TraceModel,
	TraceTexture,
	TraceSphere,
	
	PF_aim_I,
	
	PF_localcmd_I,
	PF_localexec_I,
	
	PF_stuffcmd_I,
	
	PF_particle_I,
	
	PF_lightstyle_I,
	
	PF_DecalIndex,
	
	PF_pointcontents_I,
	
	PF_MessageBegin_I,
	PF_MessageEnd_I,
	
	NULL, //PF_WriteByte_I,
	NULL, //PF_WriteChar_I,
	NULL, //PF_WriteShort_I,
	NULL, //PF_WriteLong_I,
	NULL, //PF_WriteAngle_I,
	NULL, //PF_WriteCoord_I,
	NULL, //PF_WriteString_I,
	NULL, //PF_WriteEntity_I,
	
	CVarRegister,
	
	CVarGetFloat,
	CVarGetString,
	
	CVarSetFloat,
	CVarSetString,
	
	AlertMessage,
	EngineFprintf,
	
	PvAllocEntPrivateData,
	PvEntPrivateData,
	FreeEntPrivateData,
	
	SzFromIndex,
	AllocEngineString,
	
	GetVarsOfEnt,
	PEntityOfEntOffset,
	EntOffsetOfPEntity,
	IndexOfEdict,
	PEntityOfEntIndex,
	FindEntityByVars,
	
	GetModelPtr,
	
	SV_RegUserMsg,
	
	AnimationAutomove,
	
	GetBonePosition,
	
	FunctionFromName,
	NameForFunction,
	
	ClientPrintf,
	ServerPrint,
	
	Cmd_Args,
	Cmd_Argv,
	Cmd_Argc,
	
	GetAttachment,
	
	CRC32_Init,
	CRC32_ProcessBuffer,
	CRC32_ProcessByte,
	CRC32_Final,
	
	RandomLong,
	RandomFloat,
	
	PF_setview_I,
	
	PF_Time,
	
	PF_crosshairangle_I,
	
	COM_LoadFileForMe,
	COM_FreeFile,
	
	Host_EndSection,
	
	COM_CompareFileTime,
	COM_GetGameDir,
	
	NULL, // TODO: pfnCvar_RegisterVariable = 0x981fd7a4 <meta_CVarRegister(cvar_s*)>,
	
	PF_FadeVolume,
	
	PF_SetClientMaxspeed,
	
	PF_CreateFakeClient_I,
	
	PF_RunPlayerMove_I,
	
	PF_NumberOfEntities_I,
	
	PF_GetInfoKeyBuffer_I,
	
	PF_InfoKeyValue_I,
	PF_SetKeyValue_I,
	PF_SetClientKeyValue_I,
	
	PF_IsMapValid_I,
	
	PF_StaticDecal,

	PF_precache_generic_I,
	
	PF_GetPlayerUserId,
	
	PF_BuildSoundMsg_I,
	
	PF_IsDedicatedServer,
	
	CVarGetPointer,
	
	PF_GetPlayerWONId,
	
	PF_RemoveKey_I,
	
	PF_GetPhysicsKeyValue,
	PF_SetPhysicsKeyValue,
	PF_GetPhysicsInfoString,
	
	EV_Precache,
	EV_SV_Playback,
	
	SV_FatPVS,
	SV_FatPAS,
	
	SV_CheckVisibility,
	
	DELTA_SetField,
	DELTA_UnsetField,
	DELTA_AddEncoder,
	
	PF_GetCurrentPlayer,
	
	PF_CanSkipPlayer,
	
	DELTA_FindFieldIndex,
	
	DELTA_SetFieldByIndex,
	DELTA_UnsetFieldByIndex,
	
	PF_SetGroupMask,
	
	PF_CreateInstancedBaseline,
	
	PF_Cvar_DirectSet,
	
	PF_ForceUnmodified,
	
	PF_GetPlayerStats,
	
	Cmd_AddServerCommand,
	
	Voice_GetClientListening,
	Voice_SetClientListening,
	
	pfnGetPlayerAuthId,
	
	pfnSequenceGet,
	pfnSequencePickSentence,
	
	pfnGetFileSize,
	
	pfnGetApproxWavePlayLen,
	
	pfnIsCareerMatch,
	
	pfnGetLocalizedStringLength,
	
	pfnRegisterTutorMessageShown,
	pfnGetTimesTutorMessageShown,
	
	ProcessTutorMessageDecayBuffer,
	ConstructTutorMessageDecayBuffer,
	
	ResetTutorMessageDecayData,
	
	pfnQueryClientCvarValue,
	pfnQueryClientCvarValue2,
	
	pfnCheckParm
};