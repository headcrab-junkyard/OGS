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
/// @brief studiorender interface implementation

#include "quakedef.h"
#include "r_studioint.h"
#include "studio.h"

int gnForceFaceFlags{0};
studiohdr_t *gpStudioHdr{nullptr};
model_t *gpRenderModel{nullptr};
int gnRenderMode{0};

struct model_s *R_GetModelByIndex(int index)
{
	// TODO
	return nullptr;
};

struct cl_entity_s *R_GetCurrentEntity()
{
	// TODO
	return nullptr;
};

struct player_info_s *R_PlayerInfo(int index)
{
	return &cl.players[index];
};

struct entity_state_s *R_GetPlayerState(int index)
{
	// TODO
	return nullptr;
};

struct cl_entity_s *R_GetViewEntity()
{
	return &cl.viewent;
};

void R_GetTimes(int *framecount, double *current, double *old)
{
	if(framecount)
		*framecount = host_framecount;
	
	if(current)
		*current = 0.0; // TODO
	
	if(old)
		*old = 0.0; // TODO
};

struct cvar_s *R_GetCvar(const char *name)
{
	return Cvar_FindVar(name);
};

void R_GetViewInfo(float *origin, float *upv, float *rightv, float *vpnv)
{
	if(origin)
		VectorCopy(r_origin, origin);
	
	if(upv)
		VectorCopy(vup, upv);
	
	if(rightv)
		VectorCopy(vright, rightv);
	
	if(vpnv)
		VectorCopy(vpn, vpnv);
};

struct model_s *R_GetChromeSprite()
{
	// TODO
	return nullptr;
};

void R_GetModelCounters(int **s, int **a)
{
	// TODO
};

void R_GetAliasScale(float *x, float *y)
{
	// TODO
};

float ****R_StudioGetBoneTransform()
{
	// TODO
	return nullptr;
};

float ****R_StudioGetLightTransform()
{
	// TODO
	return nullptr;
};

float ***R_StudioGetAliasTransform()
{
	// TODO
	return nullptr;
};

float ***R_StudioGetRotationMatrix()
{
	// TODO
	return nullptr;
};

void R_StudioSetupModel(int bodypart, void **ppBodyPart, void **ppSubModel)
{
	// TODO
};

int R_StudioCheckBBox()
{
	// TODO
	return 0;
};

void R_StudioDynamicLight(struct cl_entity_s *ent, struct alight_s *pLight)
{
	// TODO
};

void R_StudioEntityLight(struct alight_s *pLight)
{
	// TODO
};

void R_StudioSetupLighting(struct alight_s *pLighting)
{
	// TODO
};

void R_StudioDrawPoints()
{
	// TODO
};

void R_StudioDrawHulls()
{
	// TODO
};

void R_StudioDrawAbsBBox()
{
	// TODO
};

void R_StudioDrawBones()
{
	// TODO
};

void R_StudioSetupSkin(void *pTextureHeader, int index)
{
	// TODO
};

void R_StudioSetRemapColors(int top, int bottom)
{
	cl.players[cl.playernum].topcolor = top; // TODO: u sure?
	cl.players[cl.playernum].bottomcolor = bottom; // TODO: u sure?
};

struct model_s *R_SetupPlayerModel(int index)
{
	// TODO
	return nullptr;
};

void R_StudioClientEvents()
{
	// TODO
};

int R_GetForceFaceFlags()
{
	return gnForceFaceFlags;
};

void R_SetForceFaceFlags(int flags)
{
	gnForceFaceFlags = flags;
};

void R_StudioSetHeader(void *pHeader)
{
	gpStudioHdr = reinterpret_cast<studiohdr_t*>(pHeader);
};

void R_SetRenderModel(struct model_s *pModel)
{
	gpRenderModel = pModel;
};

void R_SetupRenderer(int nRenderMode)
{
	gnRenderMode = nRenderMode;
};

void R_RestoreRenderer()
{
	// TODO
	
	gnRenderMode = 0; // TODO: -1?
};

void R_SetChromeOrigin() // TODO: no args?
{
	// TODO
};

int R_IsHardware()
{
#ifdef GLQUAKE // TODO
	return true;
#else
	return false;
#endif
};

// TODO: move this
// TODO: should these be set to nullptr or implemented as stubs?

void R_GL_StudioDrawShadow()
{
#ifdef GLQUAKE
	// TODO
#endif
};

void R_GL_SetRenderMode(int mode)
{
#ifdef GLQUAKE
	// TODO
#endif
};

void R_StudioSetRenderamt(int nRenderamt)
{
	// TODO
};

void R_StudioSetCullState(int nState)
{
	// TODO
};

void R_StudioRenderShadow(int nSprite, float *p1, float *p2, float *p3, float *p4)
{
	// TODO
};

engine_studio_api_t gEngStudioAPI =
{
	Mem_Calloc,
	Cache_Check,
	COM_LoadCacheFile,
	
	Mod_ForName,
	Mod_Extradata,
	
	R_GetModelByIndex,
	
	R_GetCurrentEntity,
	
	R_PlayerInfo,
	
	R_GetPlayerState,
	
	R_GetViewEntity,
	
	R_GetTimes,
	
	R_GetCvar,
	
	R_GetViewInfo,
	
	R_GetChromeSprite,
	
	R_GetModelCounters,
	
	R_GetAliasScale,
	
	R_StudioGetBoneTransform,
	R_StudioGetLightTransform,
	R_StudioGetAliasTransform,
	R_StudioGetRotationMatrix,
	
	R_StudioSetupModel,
	
	R_StudioCheckBBox,
	
	R_StudioDynamicLight,
	R_StudioEntityLight,
	R_StudioSetupLighting,
	
	R_StudioDrawPoints,
	R_StudioDrawHulls,
	R_StudioDrawAbsBBox,
	R_StudioDrawBones,
	
	R_StudioSetupSkin,
	
	R_StudioSetRemapColors,
	
	R_SetupPlayerModel,
	
	R_StudioClientEvents,
	
	R_GetForceFaceFlags,
	R_SetForceFaceFlags,
	
	R_StudioSetHeader,
	
	R_SetRenderModel,
	
	R_SetupRenderer,
	R_RestoreRenderer,
	
	R_SetChromeOrigin,
	
	R_IsHardware,
	
	/// Only called by hardware interface
	R_GL_StudioDrawShadow,
	R_GL_SetRenderMode,
	
	R_StudioSetRenderamt,
	R_StudioSetCullState,
	R_StudioRenderShadow
};

server_studio_api_t gSvStudioAPI =
{
	Mem_Calloc,
	Cache_Check,
	COM_LoadCacheFile,
	Mod_Extradata
};