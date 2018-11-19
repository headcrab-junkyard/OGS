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
/// @brief event api

#pragma once

#define EVENT_API_VERSION 1

typedef struct event_api_s
{
	///
	int version;
	
	///
	void (*EV_PlaySound)(int nEnt, float *vOrigin, int nChannel, const char *sSample,
	                     float fVolume, float fAttenuation, int nFlags, int nPitch);
	
	///
	void (*EV_StopSound)(int nEnt, int nChannel, const char *sSample);
	
	///
	int (*EV_FindModelIndex)(const char *sName);
	
	///
	int (*EV_IsLocal)(int nPlayerNum);
	
	///
	int (*EV_LocalPlayerDucking)();
	
	///
	void (*EV_LocalPlayerViewheight)(float *vOrigin);
	
	///
	void (*EV_LocalPlayerBounds)(int nHullType, float *vMins, float *vMaxs);
	
	///
	int (*EV_IndexFromTrace)(struct pmtrace_s *pTrace);
	
	///
	struct physent_s *(*EV_GetPhysEnt)(int nIndex);
	
	///
	void (*EV_SetupPlayerPrediction)(int nDoPred, int nIncludeLocalClient);
	
	///
	void (*EV_PushPMStates)();
	
	///
	void (*EV_PopPMStates)();
	
	///
	void (*EV_SetSolidPlayers)(int nPlayerNum);
	
	///
	void (*EV_SetTraceHull)(int nHullType);
	
	///
	void (*EV_PlayerTrace)(float *vStart, float *vEnd, int nTraceFlags, int nIgnorePE, struct pmtrace_s *pTraceResult);
	
	///
	void (*EV_WeaponAnimation)(int nSequence, int nBody);
	
	///
	unsigned short (*EV_PrecacheEvent)(int nType, const char *sName);
	
	///
	void (*EV_PlaybackEvent)(int nFlags, const struct edict_s *pInvoker, unsigned short nEventIndex, float fDelay,
	                         float *vOrigin, float *vAngles, float fParam1, float fParam2, int nParam1, int nParam2,
							 int bParam1, int bParam2);
	
	///
	const char *(*EV_TraceTexture)(int nGround, float *vStart, float *vEnd);
	
	///
	void (*EV_StopAllSounds)(int nEntNum, int nEntChannel);
	
	///
	void (*EV_KillAllEvents)(int nEntNum, const char *sEventName);
} event_api_t;

extern event_api_t eventapi;