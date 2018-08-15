/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"

/*
===============================================================================

						BUILT-IN FUNCTIONS

===============================================================================
*/

void PF_precache_model_I(const char *s)
{
	int i;

	if(sv.state != ss_loading)
		Host_Error("PF_Precache_*: Precache can only be done in spawn functions");

	G_INT(OFS_RETURN) = G_INT(OFS_PARM0);
	PR_CheckEmptyString(s);

	for(i = 0; i < MAX_MODELS; i++)
	{
		if(!sv.model_precache[i])
		{
			sv.model_precache[i] = s;
			sv.models[i] = Mod_ForName(s, true);
			return;
		};
		
		if(!strcmp(sv.model_precache[i], s))
			return;
	};
	
	Host_Error("PF_precache_model: overflow");
};

int PF_precache_sound_I(const char *s)
{
	int i;

	if(sv.state != ss_loading)
		Host_Error("PF_Precache_*: Precache can only be done in spawn functions");

	G_INT(OFS_RETURN) = G_INT(OFS_PARM0);
	PR_CheckEmptyString(s);

	for(i = 0; i < MAX_SOUNDS; i++)
	{
		if(!sv.sound_precache[i])
		{
			sv.sound_precache[i] = s;
			return;
		};
		
		if(!strcmp(sv.sound_precache[i], s))
			return;
	};
	
	Host_Error("PF_precache_sound: overflow");
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

int PF_modelindex()
{
	return 0;
};

int ModelFrames()
{
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
PF_setsize

the size box is rotated by the current angle

setsize (entity, minvector, maxvector)
=================
*/
void PF_setsize_I(edict_t *e, float *min, float *max)
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
	int i;
	client_t *client;

	i = NUM_FOR_EDICT(ent);
	if(i < 1 || i > svs.maxclients)
		Host_Error("Entity is not a client");

	// copy spawn parms out of the client_t
	client = svs.clients + (i - 1);

	for(i = 0; i < NUM_SPAWN_PARMS; i++)
		(&gGlobalVariables.parm1)[i] = client->spawn_parms[i];
};

void SaveSpawnParms()
{
};

/*
=================
PF_vectoyaw

float vectoyaw(vector)
=================
*/
float PF_vectoyaw_I(float *value1)
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
float *PF_vectoangles_I(float *value1)
{
	float forward;
	float yaw, pitch;

	if(value1[1] == 0 && value1[0] == 0)
	{
		yaw = 0;
		if(value1[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (int)(atan2(value1[1], value1[0]) * 180 / M_PI);
		if(yaw < 0)
			yaw += 360;

		forward = sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = (int)(atan2(value1[2], forward) * 180 / M_PI);
		if(pitch < 0)
			pitch += 360;
	};

	G_FLOAT(OFS_RETURN + 0) = pitch;
	G_FLOAT(OFS_RETURN + 1) = yaw;
	G_FLOAT(OFS_RETURN + 2) = 0;
};

void SV_MoveToOrigin_I()
{
};

/*
==============
PF_changeyaw

This was a major timewaster in progs, so it was converted to C
==============
*/
void PF_changeyaw_I(edict_t *ent)
{
	float ideal, current, move, speed;

	//ent = PROG_TO_EDICT(gGlobalVariables.self);
	current = anglemod(ent->v.angles[1]);
	ideal = ent->v.ideal_yaw;
	speed = ent->v.yaw_speed;

	if(current == ideal)
		return;
	move = ideal - current;
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
	float ideal, current, move, speed;

	current = anglemod(ent->v.angles[0]);
	ideal = ent->v.idealpitch;
	speed = ent->v.pitch_speed;

	if(current == ideal)
		return;
	move = ideal - current;
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

edict_t *FindEntityByString()
{
	return NULL;
};

int GetEntityIllum()
{
	return 0;
};

FindEntityInSphere()
{
};

PF_checkclient_I()
{
};

PVSFindEntities()
{
};

/*
==============
PF_makevectors

Writes new values for v_forward, v_up, and v_right based on angles
makevectors(vector)
==============
*/
void PF_makevectors_I(float *v)
{
	AngleVectors(v, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
};

void AngleVectors()
{
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

CreateNamedEntity()
{
};

int SV_ModelIndex(const char *name); // TODO

void PF_makestatic_I(edict_t *ent)
{
	int i;

	MSG_WriteByte(&sv.signon, svc_spawnstatic);

	MSG_WriteByte(&sv.signon, SV_ModelIndex(pr_strings + ent->v.model));

	MSG_WriteByte(&sv.signon, ent->v.frame);
	MSG_WriteByte(&sv.signon, ent->v.colormap);
	MSG_WriteByte(&sv.signon, ent->v.skin);

	for(i = 0; i < 3; i++)
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
float PF_checkbottom_I(edict_t *ent)
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
float PF_walkmove_I(edict_t *ent, float yaw, float dist)
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
void PF_setorigin_I(edict_t *e, float *org)
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

	SV_StartSound(entity, channel, sample, volume, attenuation);
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

void PF_traceline_DLL()
{
};

void PF_TraceToss_DLL()
{
};

void TraceMonsterHull()
{
};

void TraceHull()
{
};

void TraceModel()
{
};

void TraceTexture()
{
};

void TraceSphere()
{
};

/*
=============
PF_aim

Pick a vector for the player to shoot along
vector aim(entity, missilespeed)
=============
*/
cvar_t sv_aim = { "sv_aim", "0.93" };
void PF_aim_I(edict_t *ent, float speed)
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
		VectorCopy(gGlobalVariables.v_forward, G_VECTOR(OFS_RETURN));
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
		VectorCopy(end, G_VECTOR(OFS_RETURN));
	}
	else
		VectorCopy(bestdir, G_VECTOR(OFS_RETURN));
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
};

/*
=================
PF_stuffcmd

Sends text over to the client's execution buffer

stuffcmd (clientent, value)
=================
*/
void PF_stuffcmd_I(edict_t *client, const char *str)
{
	int entnum;
	client_t *old;

	entnum = EDICT_NUM(OFS_PARM0);
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
void PF_particle_I(float *org, float *dir, float color, float count)
{
	SV_StartParticle(org, dir, color, count);
};

/*
===============
PF_lightstyle

void(float style, string value) lightstyle
===============
*/
void PF_lightstyle_I(/*float*/ int style, const char *val)
{
	client_t *client;
	int j;

	// change the string in sv
	sv.lightstyles[style] = val;

	// send message to all clients on this server
	if(sv.state != ss_active)
		return;

	for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
		if(client->active || client->spawned)
		{
			MSG_WriteChar(&client->message, svc_lightstyle);
			MSG_WriteChar(&client->message, style);
			MSG_WriteString(&client->message, val);
		};
};

int PF_DecalIndex()
{
	return 0;
};

/*
=============
PF_pointcontents
=============
*/
float PF_pointcontents_I(float *v)
{
	return SV_PointContents(v);
};

void PF_MessageBegin_I()
{
};

void PF_MessageEnd_I()
{
};

void PF_WriteByte_I(int val)
{
	MSG_WriteByte(WriteDest(), val);
};

void PF_WriteChar_I(int val)
{
	MSG_WriteChar(WriteDest(), val);
};

void PF_WriteShort_I(int val)
{
	MSG_WriteShort(WriteDest(), val);
};

void PF_WriteLong_I(int val)
{
	MSG_WriteLong(WriteDest(), val);
};

void PF_WriteAngle_I(float val)
{
	MSG_WriteAngle(WriteDest(), val);
};

void PF_WriteCoord_I(float val)
{
	MSG_WriteCoord(WriteDest(), val);
};

void PF_WriteString_I(const char *s)
{
	MSG_WriteString(WriteDest(), s);
};

void PF_WriteEntity_I(edict_t *ent)
{
	MSG_WriteShort(WriteDest(), EDICT_NUM(ent));
};

pfnCVarRegister = 0x981fd7a4 <meta_CVarRegister(cvar_s*)>,

float CVarGetFloat()
{
	return 0.0f;
};

const char *CVarGetString()
{
	return "";
};

void CVarSetFloat()
{
};

void CVarSetString()
{
};

void AlertMessage()
{
};

void EngineFprintf()
{
};

void *PvAllocEntPrivateData()
{
	return NULL;
};

void *PvEntPrivateData()
{
	return NULL;
};

void FreeEntPrivateData()
{
};

const char *SzFromIndex()
{
	return "";
};

AllocEngineString()
{
};

GetVarsOfEnt,
{
};

PEntityOfEntOffset,
{
};

EntOffsetOfPEntity,
{
};

IndexOfEdict,
{
};

PEntityOfEntIndex,
{
};

FindEntityByVars,
{
};

GetModelPtr()
{
};

pfnRegUserMsg = 0x981fd94a <meta_RegUserMsg(char const*, int)>,

AnimationAutomove()
{
};

GetBonePosition()
{
};

FunctionFromName,
{
};

NameForFunction,
{
};

void ClientPrintf()
{
};

void ServerPrint()
{
};

GetAttachment()
{
};

CRC32_Init,
{
};

CRC32_ProcessBuffer,
{
};

CRC32_ProcessByte,
{
};

CRC32_Final,
{
};

RandomLong,
{
};

RandomFloat,
{
};

void PF_setview_I()
{
};

float PF_Time()
{
	return 0.0f;
};

void PF_crosshairangle_I()
{
};

COM_LoadFileForMe,
{
};

COM_FreeFile,
{
};

void Host_EndSection()
{
};

int COM_CompareFileTime()
{
	return 0;
};

COM_GetGameDir()
{
};

pfnCvar_RegisterVariable = 0x981fd7a4 <meta_CVarRegister(cvar_s*)>,

void PF_FadeVolume()
{
};

void PF_SetClientMaxspeed()
{
};

client_t *PF_CreateFakeClient_I()
{
	return NULL;
};

void PF_RunPlayerMove_I()
{
};

int PF_NumberOfEntities_I()
{
	return 0;
};

char *PF_GetInfoKeyBuffer_I()
{
	return NULL;
};

PF_InfoKeyValue_I()
{
};

void PF_SetKeyValue_I()
{
};

void PF_SetClientKeyValue_I()
{
};

int PF_IsMapValid_I()
{
	return 0;
};

void PF_StaticDecal()
{
};

int PF_precache_generic_I()
{
	return 0;
};

PF_GetPlayerUserId()
{
};

void PF_BuildSoundMsg_I()
{
};

int PF_IsDedicatedServer()
{
	return isDedicated;
};

cvar_t *CVarGetPointer(const char *name)
{
	return NULL;
};

PF_GetPlayerWONId()
{
};

void PF_RemoveKey_I(char *infobuffer, const char *key)
{
};

const char *PF_GetPhysicsKeyValue(const char *key)
{
	return "";
};

void PF_SetPhysicsKeyValue(const char *key, const char *value)
{
};

char *PF_GetPhysicsInfoString()
{
	return NULL;
};

EV_Precache,
{
};

EV_Playback,
{
};

SV_FatPVS,
{
};

SV_FatPAS,
{
};

SV_CheckVisibility,
{
};

void DELTA_SetField()
{
};

void DELTA_UnsetField()
{
};

void DELTA_AddEncoder()
{
};

PF_GetCurrentPlayer()
{
};

int PF_CanSkipPlayer()
{
	return 0;
};

DELTA_FindFieldIndex()
{
};

void DELTA_SetFieldByIndex()
{
};

void DELTA_UnsetFieldByIndex()
{
};

void PF_SetGroupMask()
{
};

void PF_CreateInstancedBaseline()
{
};

void PF_Cvar_DirectSet()
{
};

void PF_ForceUnmodified()
{
};

PF_GetPlayerStats()
{
};

pfnAddServerCommand = 0x981fd63f <meta_AddServerCommand(char*, void (*)())>,

Voice_GetClientListening()
{
};

void Voice_SetClientListening()
{
};

pfnGetPlayerAuthId = 0,
{
};

pfnSequenceGet = 0,
{
};

pfnSequencePickSentence = 0,
{
};

pfnGetFileSize = 0,
{
};

pfnGetApproxWavePlayLen = 0,
{
};

pfnIsCareerMatch = 0,
{
};

pfnGetLocalizedStringLength = 0,
{
};

pfnRegisterTutorMessageShown = 0,
{
};

pfnGetTimesTutorMessageShown = 0,
{
};

ProcessTutorMessageDecayBuffer = 0,
{
};

ConstructTutorMessageDecayBuffer = 0,
{
};

ResetTutorMessageDecayData = 0,
{
};

pfnQueryClientCvarValue = 0,
{
};

pfnQueryClientCvarValue2 = 0,
{
};

pfnCheckParm = 0
{
};

/*
enginefuncs_t gEngineFuncs[] =
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
	PF_WriteByte_I,
	PF_WriteChar_I,
	PF_WriteShort_I,
	PF_WriteLong_I,
	PF_WriteAngle_I,
	PF_WriteCoord_I,
	PF_WriteString_I,
	PF_WriteEntity_I,
	pfnCVarRegister = 0x981fd7a4 <meta_CVarRegister(cvar_s*)>,
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
	pfnRegUserMsg = 0x981fd94a <meta_RegUserMsg(char const*, int)>,
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
	pfnCvar_RegisterVariable = 0x981fd7a4 <meta_CVarRegister(cvar_s*)>,
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
	EV_Playback,
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
	pfnAddServerCommand = 0x981fd63f <meta_AddServerCommand(char*, void (*)())>,
	Voice_GetClientListening,
	Voice_SetClientListening,
	pfnGetPlayerAuthId = 0,
	pfnSequenceGet = 0,
	pfnSequencePickSentence = 0,
	pfnGetFileSize = 0,
	pfnGetApproxWavePlayLen = 0,
	pfnIsCareerMatch = 0,
	pfnGetLocalizedStringLength = 0,
	pfnRegisterTutorMessageShown = 0,
	pfnGetTimesTutorMessageShown = 0,
	ProcessTutorMessageDecayBuffer = 0,
	ConstructTutorMessageDecayBuffer = 0,
	ResetTutorMessageDecayData = 0,
	pfnQueryClientCvarValue = 0,
	pfnQueryClientCvarValue2 = 0,
	pfnCheckParm = 0
};
*/