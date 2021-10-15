/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021 BlackPhrase
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
/// @brief render effects api implementation

#include "quakedef.h"
#include "r_efx.h"

particle_t *R_AllocParticle(pfnParticleCallback afnCallback)
{
	return NULL;
};

/*
void R_BlobExplosion(float *org)
{
};
*/

void R_Blood(float *org, float *dir, int pcolor, int speed)
{
};

void R_BloodSprite(float *org, int colorindex, int modelindex, int modelindex2, float size)
{
};

void R_BloodStream(float *org, float *dir, int pcolor, int speed)
{
};

void R_BreakModel(float *pos, float *size, float *dir, float random, float life, int count, int modelindex, char flags)
{
};

void R_Bubbles(float *mins, float *maxs, float height, int modelindex, int count, float speed)
{
};

void R_BubbleTrail(float *start, float *end, float height, int modelindex, int count, float speed)
{
};

void R_BulletImpactParticles(float *pos)
{
};

/*
void R_EntityParticles(struct cl_entity_s *ent)
{
};
*/

void R_Explosion(float *pos, int model, float scale, float framerate, int flags)
{
};

void R_FizzEffect(struct cl_entity_s *pent, int modelindex, int density)
{
};

void R_FireField(float *org, int radius, int modelindex, int count, int flags, float life)
{
};

void R_FlickerParticles(float *org)
{
};

void R_FunnelSprite(float *org, int modelindex, int reverse)
{
};

void R_Implosion(float *end, float radius, int count, float life)
{
};

void R_LargeFunnel(float *org, int reverse)
{
};

void R_LavalSplash(float *org)
{
};

void R_MultiGunshot(float *org, float *dir, float *noise, int count, int decalcount, int *decalindices)
{
};

void R_MuzzleFlash(float *pos, int type)
{
};

void R_ParticleBox(float *mins, float *maxs, byte r, byte g, byte b, float life)
{
};

void R_ParticleBurst(float *pos, int size, int color, float life)
{
};

/*
void R_ParticleExplosion(float *org)
{
};

void R_ParticleExplosion2(float *org, int colorstart, int colorlength)
{
};
*/

void R_ParticleLine(float *start, float *end, byte r, byte g, byte b, float life)
{
};

void R_PlayerSprites(int client, int modelindex, int count, int size)
{
};

void R_Projectile(float *org, float *vel, int modelindex, int life, int owner, void (*hitcallback)(struct tempent_s *ent, struct pmtrace_s *ptrace))
{
};

void R_RicochetSound(float *pos)
{
};

void R_RicochetSprite(float *pos, struct model_s *pmodel, float duration, float scale)
{
};

void R_RocketFlare(float *pos)
{
};

/*
void R_RocketTrail(float *start, float *end, int type)
{
};

void R_RunParticleEffect(float *org, float *dir, int color, int count)
{
};
*/

void R_ShowLine(float *start, float *end)
{
};

void R_SparkEffect(float *pos, int count, int velocityMin, int velocityMax)
{
};

void R_SparkShower(float *pos)
{
};

void R_SparkStreaks(float *pos, int count, int velocityMin, int velocityMax)
{
};

void R_Spray(float *pos, float *dir, int modelindex, int count, int speed, int spread, int rendermode)
{
};

void R_Sprite_Explode(TEMPENTITY *pTemp, float scale, int flags)
{
};

void R_Sprite_Smoke(TEMPENTITY *pTemp, float scale)
{
};

void R_Sprite_Spray(float *pos, float *dir, int modelindex, int count, int speed, int nRand)
{
};

void R_Sprite_Trail(int type, float *start, float *end, int modelindex, int count, float life, float size, float amplitude, int renderamt, float speed)
{
};

void R_Sprite_WallPuff(TEMPENTITY *pTemp, float scale)
{
};

void R_StreakSplash(float *pos, float *dir, int color, int count, float speed, int velocityMin, int velocityMax)
{
};

void R_TracerEffect(float *start, float *end)
{
};

void R_UserTracerParticle(float *org, float *vel, float life, int colorindex, float length, byte deathcontext, void (*deathfunc)(struct particle_s *particle))
{
};

particle_t *R_TracerParticles(float *org, float *vel, float life)
{
	return NULL;
};

/*
void R_TeleportSplash(float *org)
{
};
*/

void R_TempSphereModel(float *pos, float speed, float life, int count, int modelindex)
{
};

TEMPENTITY *R_TempModel(float *pos, float *dir, float *angles, float life, int modelindex, int soundtype)
{
	return NULL;
};

TEMPENTITY *R_DefaultSprite(float *pos, int spriteindex, float framerate)
{
	return NULL;
};

TEMPENTITY *R_TempSprite(float *pos, float *dir, float scale, int modelindex, int rendermode, int renderfx, float a, float life, int flags)
{
	return NULL;
};

int Draw_DecalIndex(int id)
{
	return 0;
};

int Draw_DecalIndexFromName(char *name)
{
	return 0;
};

void R_DecalShoot(int textureindex, int entity, int modelindex, float *position, int flags)
{
};

void R_AttachTentToPlayer(int client, int modelindex, float zoffset, float life)
{
};

void R_KillAttachedTents(int client)
{
};

BEAM *R_BeamCirclePoints(int type, float *start, float *end, int modelindex, float life, float width, float amplitude, float brightness, float speed, int startframe, float framerate, float r, float g, float b)
{
	return NULL;
};

BEAM *R_BeamEntPoint(int startEnt, float *end, int modelindex, float life, float width, float amplitude, float brightness, float speed, int startframe, float framerate, float r, float g, float b)
{
	return NULL;
};

BEAM *R_BeamEnts(int startEnt, int endEnt, int modelindex, float life, float width, float amplitude, float brightness, float speed, int startframe, float framerate, float r, float g, float b)
{
	return NULL;
};

BEAM *R_BeamFollow(int startEnt, int modelindex, float life, float width, float r, float g, float b, float brightness)
{
	return NULL;
};

void R_BeamKill(int deadEntity)
{
};

BEAM *R_BeamLightning(float *start, float *end, int modelindex, float life, float width, float amplitude, float brightness, float speed)
{
	return NULL;
};

BEAM *R_BeamPoints(float *start, float *end, int modelindex, float life, float width, float amplitude, float brightness, float speed, int startframe, float framerate, float r, float g, float b)
{
	return NULL;
};

BEAM *R_BeamRing(int startEnt, int endEnt, int modelindex, float life, float width, float amplitude, float brightness, float speed, int startframe, float framerate, float r, float g, float b)
{
	return NULL;
};

dlight_t *CL_AllocElight(int key)
{
	return NULL;
};

TEMPENTITY *CL_TempEntAlloc(float *origin, struct model_s *pmodel)
{
	return NULL;
};

TEMPENTITY *CL_TempEntAllocNoModel(float *origin)
{
	return NULL;
};

TEMPENTITY *CL_TempEntAllocHigh(float *origin, struct model_s *pmodel)
{
	return NULL;
};

TEMPENTITY *CL_TempEntAllocCustom(float *origin, struct model_s *pmodel, int high, void (*callback)(struct tempent_s *ent, float frametime, float currenttime))
{
	return NULL;
};

void R_GetPackedColor(short *packed, short color)
{
};

short R_LookupColor(byte r, byte g, byte b)
{
	return 0;
};

void R_DecalRemoveAll(int textureIndex)
{
};

void R_FireCustomDecal(int textureIndex, int entity, int modelindex, float *position, int flags, float scale)
{
};

efx_api_t efx =
{
	R_AllocParticle,
	
	R_BlobExplosion,
	
	R_Blood,
	R_BloodSprite,
	R_BloodStream,
	
	R_BreakModel,
	
	R_Bubbles,
	R_BubbleTrail,
	
	R_BulletImpactParticles,
	R_EntityParticles,
	
	R_Explosion,
	
	R_FizzEffect,
	
	R_FireField,
	R_FlickerParticles,
	R_FunnelSprite,
	R_Implosion,
	R_LargeFunnel,
	R_LavalSplash,
	R_MultiGunshot,
	R_MuzzleFlash,
	
	R_ParticleBox,
	R_ParticleBurst,
	
	R_ParticleExplosion,
	R_ParticleExplosion2,
	
	R_ParticleLine,
	R_PlayerSprites,
	R_Projectile,
	
	R_RicochetSound,
	R_RicochetSprite,
	
	R_RocketFlare,
	R_RocketTrail,
	
	R_RunParticleEffect,
	
	R_ShowLine,
	
	R_SparkEffect,
	R_SparkShower,
	R_SparkStreaks,
	
	R_Spray,
	
	R_Sprite_Explode,
	R_Sprite_Smoke,
	R_Sprite_Spray,
	R_Sprite_Trail,
	R_Sprite_WallPuff,
	
	R_StreakSplash,
	R_TracerEffect,
	R_UserTracerParticle,
	R_TracerParticles,
	R_TeleportSplash,
	R_TempSphereModel,
	R_TempModel,
	R_DefaultSprite,
	R_TempSprite,
	
	Draw_DecalIndex,
	Draw_DecalIndexFromName,
	
	R_DecalShoot,
	R_AttachTentToPlayer,
	R_KillAttachedTents,
	R_BeamCirclePoints,
	R_BeamEntPoint,
	R_BeamEnts,
	R_BeamFollow,
	R_BeamKill,
	R_BeamLightning,
	R_BeamPoints,
	R_BeamRing,
	
	CL_AllocDlight,
	CL_AllocElight,
	
	CL_TempEntAlloc,
	CL_TempEntAllocNoModel,
	CL_TempEntAllocHigh,
	CL_TempEntAllocCustom,
	
	R_GetPackedColor,
	R_LookupColor,
	R_DecalRemoveAll,
	R_FireCustomDecal
};