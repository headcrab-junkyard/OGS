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

#pragma once

#define STUDIO_INTERFACE_VERSION 1
#define SV_BLENDING_INTERFACE_VERSION 1

typedef struct engine_studio_api_s
{
	/// Allocate (number * size) bytes memory block and zero it
	void *(*Mem_Calloc)(int number, size_t size);
	
	/// Check to see if pointer is in the cache
	void *(*Cache_Check)(struct cache_user_s *cu);
	
	/// Load file into cache (can be swapped out on demand)
	void (*LoadCacheFile)(const char *path, struct cache_user_s *cu);
	
	/// Retrieve model pointer for the named model
	struct model_s *(*Mod_ForName)(const char *name, int crash_if_missing);
	
	/// Retrieve pointer to studio model data block from a model
	void *(*Mod_Extradata)(struct model_s *mod);
	
	/// Get indexed model from client-side model precache list
	struct model_s *(*GetModelByIndex)(int index);
	
	/// Get entity that is set for rendering
	struct cl_entity_s *(*GetCurrentEntity)();
	
	/// Get referenced player info
	struct player_info_s *(*PlayerInfo)(int index);
	
	/// Get most recently received player state data from network system
	struct entity_state_s *(*GetPlayerState)(int index);
	
	/// Get local player's view entity
	struct cl_entity_s *(*GetViewEntity)();
	
	/// Get current frame cound and last two timestamps on client
	void (*GetTimes)(int *framecount, double *current, double *old);
	
	/// Get a pointer to a cvar by name
	struct cvar_s *(*GetCvar)(const char *name);
	
	/// Get current render origin and view vectors (up, right and vpn)
	void (*GetViewInfo)(float *origin, float *upv, float *rightv, float *vpnv);
	
	/// Get sprite model used for applying chrome effect
	struct model_s *(*GetChromeSprite)();
	
	/// Get model counters so we can increment instrumentation
	void (*GetModelCounters)(int **s, int **a);
	
	/// Get software scaling coefficients
	void (*GetAliasScale)(float *x, float *y);
	
	/// Get bone, light, alias and rotation matrices
	float ****(*StudioGetBoneTransform)();
	float ****(*StudioGetLightTransform)();
	float ***(*StudioGetAliasTransform)();
	float ***(*StudioGetRotationMatrix)();
	
	/// Setup body part, and get submodel pointers
	void (*StudioSetupModel)(int bodypart, void **ppBodyPart, void **ppSubModel);
	
	/// Check if entity's bbox is in the view frustum
	int (*StudioCheckBBox)();
	
	/// Apply lighting effects to model
	void (*StudioDynamicLight)(struct cl_entity_s *ent, struct alight_s *pLight);
	void (*StudioEntityLight)(struct alight_s *pLight);
	void (*StudioSetupLighting)(struct alight_s *pLighting);
	
	/// Draw mesh vertices
	void (*StudioDrawPoints)();
	
	/// Draw hulls around bones
	void (*StudioDrawHulls)();
	
	/// Draw bbox around studio models
	void (*StudioDrawAbsBBox)();
	
	/// Draws bones
	void (*StudioDrawBones)();
	
	/// Loads in appropriate texture for model
	void (*StudioSetupSkin)(void *pTextureHeader, int index);
	
	/// Sets up for remapped colors
	void (*StudioSetRemapColors)(int top, int bottom);
	
	/// Sets player model and returns model pointer
	struct model_s *(*SetupPlayerModel)(int index);
	
	/// Fires any events embedded in animation
	void (*StudioClientEvents)();
	
	/// Get/set forced render effects flags
	int (*GetForceFaceFlags)();
	void (*SetForceFaceFlags)(int flags);
	
	/// Tell engine the value of the studio model header
	void (*StudioSetHeader)(void *pHeader);
	
	/// Tell engine which model is being rendered
	void (*SetRenderModel)(struct model_s *pModel);
	
	/// Final state setup and restore for rendering
	void (*SetupRenderer)(int nRenderMode);
	void (*RestoreRenderer)();
	
	/// Set render origin for applying chrome effect
	void (*SetChromeOrigin)();
	
	/// Try if using D3D/OpenGL
	int (*IsHardware)();
	
	/// Only called by hardware interface
	void (*GL_StudioDrawShadow)();
	void (*GL_SetRenderMode)(int mode);
	
	void (*StudioSetRenderamt)(int nRenderamt); ///< Added for rendering flass on viewmodels
	void (*StudioSetCullState)(int nState);
	void (*StudioRenderShadow)(int nSprite, float *p1, float *p2, float *p3, float *p4);
} engine_studio_api_t;

typedef struct server_studio_api_s
{
	/// Allocate (number * size) bytes memory block and zero it
	void *(*Mem_Calloc)(int number, size_t size);
	
	/// Check to see if pointer is in the cache
	void *(*Cache_Check)(struct cache_user_s *cu);
	
	/// Load file into cache (can be swapped out on demand)
	void (*LoadCacheFile)(const char *path, struct cache_user_s *cu);
	
	/// Retrieve pointer to studio model data block from a model
	void *(*Mod_Extradata)(struct model_s *mod);
} server_studio_api_t;

// client blending
typedef struct r_studio_interface_s
{
	int version;
	
	int (*StudioDrawModel)(int flags);
	int (*StudioDrawPlayer)(int flags, struct entity_state_s *pPlayer);
} r_studio_interface_t;

extern r_studio_interface_t *pStudioAPI;

// server blending
typedef struct sv_blending_interface_s
{
	int version;
	
	void (*SV_StudioSetupBones)(struct model_s *pModel,
								float frame, int sequence,
								const vec3_t angles, const vec3_t origin,
								const byte *pController, const byte *pBlending,
								int nBone, const edict_t *pEdict);
} sv_blending_interface_t;