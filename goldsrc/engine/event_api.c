/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2021 BlackPhrase
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
/// @brief event api implementation

#include "quakedef.h"
#include "event_api.h"
#include "event_args.h"

void EV_PlaySound(int nEnt, float *vOrigin, int nChannel, const char *sSample,
				float fVolume, float fAttenuation, int nFlags, int nPitch)
{
	// TODO: S_StartDynamicSound(nEnt, nChannel, vOrigin, sSample, fVolume, fAttenuation, nFlags, nPitch);
};

void EV_StopSound(int nEnt, int nChannel, const char *sSample)
{
	// TODO
};

int EV_FindModelIndex(const char *sName)
{
	// TODO
	return 0;
};

int EV_IsLocal(int nPlayerNum)
{
	return cl.playernum == nPlayerNum;
};

int EV_LocalPlayerDucking()
{
	// TODO
	return 0;
};

void EV_LocalPlayerViewheight(float *vOrigin)
{
	// TODO
};

void EV_LocalPlayerBounds(int nHullType, float *vMins, float *vMaxs)
{
	switch(nHullType)
	{
		// TODO
	};
};

int EV_IndexFromTrace(struct pmtrace_s *pTrace)
{
	// TODO
	return -1;
};

struct physent_s *EV_GetPhysEnt(int nIndex)
{
	// TODO
	return NULL;
};

void EV_SetupPlayerPrediction(int nDoPred, int nIncludeLocalClient)
{
	// TODO: nIncludeLocalClient support
	CL_SetUpPlayerPrediction(nDoPred);
};

void EV_PushPMStates()
{
	// TODO
};

void EV_PopPMStates()
{
	// TODO
};

void EV_SetSolidPlayers(int nPlayerNum)
{
	CL_SetSolidPlayers(nPlayerNum);
};

void EV_SetTraceHull(int nHullType)
{
	// TODO
};

void EV_PlayerTrace(float *vStart, float *vEnd, int nTraceFlags, int nIgnorePE, struct pmtrace_s *pTraceResult)
{
	// TODO: PM_PlayerTrace(vStart, vEnd, nTraceFlags, nIgnorePE, pTraceResult);
};

void EV_WeaponAnimation(int nSequence, int nBody)
{
	CL_WeaponAnim(nSequence, nBody);
};

unsigned short EV_CL_Precache(int anType, const char *asName)
{
	static unsigned short nLast = 0;
	
	if(anType != 1) // TODO: for some reason this always should (must?) be 1
		return -1; // TODO: EVENT_INVALID/INVALID_EVENT?
	
	if(!asName || !*asName)
		return -1; // TODO: EVENT_INVALID/INVALID_EVENT?
	
	Q_strcpy(cl.event_precache[nLast]->name, asName);
	
	return nLast++;
};

void EV_Playback(int anFlags, const struct edict_s *apInvoker, unsigned short nIndex, float afDelay, 
						float *avOrigin, float *avAngles, 
						float afParam1, float afParam2,
						int anParam1, int anParam2,
						int abParam1, int abParam2)
{
	// We don't support invokers on client-side
	if(apInvoker != NULL)
		apInvoker = NULL;
	
	event_args_t args;
	Q_memset(&args, 0, sizeof(args));
	
	VectorCopy(avOrigin, args.origin);
	VectorCopy(avAngles, args.angles);
	
	args.fparam1 = afParam1;
	args.fparam2 = afParam2;
	
	args.iparam1 = anParam1;
	args.iparam2 = anParam2;
	
	args.bparam1 = abParam1;
	args.bparam2 = abParam2;
	
	args.flags = anFlags; // TODO
	
	CL_QueueEvent(anFlags, nIndex, afDelay, &args);
};

const char *EV_TraceTexture(int nGround, float *vStart, float *vEnd)
{
	// TODO
	return "";
};

void EV_StopAllSounds(int nEntNum, int nEntChannel)
{
	S_StopSound(nEntNum, nEntChannel);
};

void EV_KillAllEvents(int nEntNum, const char *sEventName)
{
	// TODO
};

event_api_t eventapi =
{
	EVENT_API_VERSION,
	
	EV_PlaySound,
	EV_StopSound,
	
	EV_FindModelIndex,
	
	EV_IsLocal,
	EV_LocalPlayerDucking,
	
	EV_LocalPlayerViewheight,
	EV_LocalPlayerBounds,
	
	EV_IndexFromTrace,
	
	EV_GetPhysEnt,
	
	EV_SetupPlayerPrediction,
	
	EV_PushPMStates,
	EV_PopPMStates,
	
	EV_SetSolidPlayers,
	
	EV_SetTraceHull,
	
	EV_PlayerTrace,
	
	EV_WeaponAnimation,
	
	EV_CL_Precache,
	EV_Playback,
	
	EV_TraceTexture,
	
	EV_StopAllSounds,
	
	EV_KillAllEvents
};