/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018-2019 BlackPhrase
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
/// @brief event api implementation

#include "quakedef.h"
#include "event_api.h"

void EV_PlaySound(int nEnt, float *vOrigin, int nChannel, const char *sSample,
				float fVolume, float fAttenuation, int nFlags, int nPitch)
{
	// TODO
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
	// TODO
	return 0;
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
	// TODO
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
	// TODO
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

unsigned short EV_Precache(int nType, const char *sName)
{
	if(nType != 1)
		Sys_Error("EV_Precache: only file type 1 supported currently");
	
	if(!sName)
		Sys_Error("EV_Precache: NULL pointer");
	
	if(!*sName)
		Sys_Error("EV_Precache: Bad string '%s'");
	
	// TODO
	return 0;
};

void EV_Playback(int nFlags, const struct edict_s *pInvoker, unsigned short nEventIndex, float fDelay,
					float *vOrigin, float *vAngles, float fParam1, float fParam2, int nParam1, int nParam2,
					int bParam1, int bParam2)
{
	// TODO
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
	
	EV_Precache,
	EV_Playback,
	
	EV_TraceTexture,
	
	EV_StopAllSounds,
	
	EV_KillAllEvents
};