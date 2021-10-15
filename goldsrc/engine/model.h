/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
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

#pragma once

#include "modelgen.h"
#include "spritegn.h"

#include "com_model.h"

/*

d*_t structures are on-disk representations
m*_t structures are in-memory

*/

// entity effects

#define EF_BRIGHTFIELD 1
#define EF_MUZZLEFLASH 2
#define EF_BRIGHTLIGHT 4
#define EF_DIMLIGHT 8

/*
==============================================================================

BRUSH MODELS

==============================================================================
*/

//
// in memory representation
//

#define SIDE_FRONT 0
#define SIDE_BACK 1
#define SIDE_ON 2

#define SURF_PLANEBACK 2
#define SURF_DRAWSKY 4
#define SURF_DRAWSPRITE 8
#define SURF_DRAWTURB 0x10
#define SURF_DRAWTILED 0x20
#define SURF_DRAWBACKGROUND 0x40
#define SURF_UNDERWATER 0x80
//#define SURF_DONTWARP		0x100 // TODO: qw

#ifdef GLQUAKE
#define VERTEXSIZE 7

typedef struct glpoly_s
{
	struct glpoly_s *next;
	struct glpoly_s *chain;
	int numverts;
	int flags;                  // for SURF_UNDERWATER
	float verts[4][VERTEXSIZE]; // variable sized (xyz s1t1 s2t2)
} glpoly_t;
#endif

/*
==============================================================================

SPRITE MODELS

==============================================================================
*/

// FIXME: shorten these?
typedef struct mspriteframe_s
{
	int width;
	int height;
#ifndef GLQUAKE
	void *pcachespot; // remove? // TODO: software render only
#endif
	float up, down, left, right;
#ifdef GLQUAKE
	int gl_texturenum;
#else
	byte pixels[4];
#endif
} mspriteframe_t;

typedef struct
{
	int numframes;
	float *intervals;
	mspriteframe_t *frames[1];
} mspritegroup_t;

typedef struct
{
	spriteframetype_t type;
	mspriteframe_t *frameptr;
} mspriteframedesc_t;

typedef struct
{
	int type;
	int maxwidth;
	int maxheight;
	int numframes;
	float beamlength; // remove?
	void *cachespot;  // remove?
	mspriteframedesc_t frames[1];
} msprite_t;

/*
==============================================================================

ALIAS MODELS

Alias models are position independent, so the cache manager can move them.
==============================================================================
*/

typedef struct
{
#ifdef GLQUAKE
	int firstpose;
	int numposes;
	float interval;
#else
	aliasframetype_t type;
#endif
	trivertx_t bboxmin;
	trivertx_t bboxmax;
	int frame;
	char name[16];
} maliasframedesc_t;

#ifndef GLQUAKE
typedef struct
{
	aliasskintype_t type;
	void *pcachespot;
	int skin;
} maliasskindesc_t;
#endif

typedef struct
{
	trivertx_t bboxmin;
	trivertx_t bboxmax;
	int frame;
} maliasgroupframedesc_t;

typedef struct
{
	int numframes;
	int intervals;
	maliasgroupframedesc_t frames[1];
} maliasgroup_t;

#ifndef GLQUAKE
typedef struct
{
	int numskins;
	int intervals;
	maliasskindesc_t skindescs[1];
} maliasskingroup_t;
#endif

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct mtriangle_s
{
	int facesfront;
	int vertindex[3];
} mtriangle_t;

#ifdef GLQUAKE
#define MAX_SKINS 32
typedef struct
{
	int ident;
	int version;
	vec3_t scale;
	vec3_t scale_origin;
	float boundingradius;
	vec3_t eyeposition;
	int numskins;
	int skinwidth;
	int skinheight;
	int numverts;
	int numtris;
	int numframes;
	synctype_t synctype;
	int flags;
	float size;

	int numposes;
	int poseverts;
	int posedata; // numposes*poseverts trivert_t
	int commands; // gl command list with embedded s/t
	int gl_texturenum[MAX_SKINS][4];
	int texels[MAX_SKINS];       // only for player skins
	maliasframedesc_t frames[1]; // variable sized
} aliashdr_t;
#else // if not GLQUAKE
typedef struct
{
	int model;
	int stverts;
	int skindesc;
	int triangles;
	maliasframedesc_t frames[1];
} aliashdr_t;
#endif // GLQUAKE

#ifdef GLQUAKE

#define MAXALIASVERTS 1024
#define MAXALIASFRAMES 256
#define MAXALIASTRIS 2048
extern aliashdr_t *pheader;
extern stvert_t stverts[MAXALIASVERTS];
extern mtriangle_t triangles[MAXALIASTRIS];
extern trivertx_t *poseverts[MAXALIASFRAMES];

#endif

//===================================================================

//
// Whole model
//

#define EF_ROCKET 1    // leave a trail
#define EF_GRENADE 2   // leave a trail
#define EF_GIB 4       // leave a trail
#define EF_ROTATE 8    // rotate (bonus items)
#define EF_TRACER 16   // green split trail
#define EF_ZOMGIB 32   // small blood trail
#define EF_TRACER2 64  // orange split trail + rotate
#define EF_TRACER3 128 // purple trail

//============================================================================

void Mod_Init();
void Mod_ClearAll();
model_t *Mod_ForName(const char *name, qboolean crash);
void *Mod_Extradata(model_t *mod); // handles caching
void Mod_TouchModel(const char *name);

mleaf_t *Mod_PointInLeaf(float *p, model_t *model);
byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model);