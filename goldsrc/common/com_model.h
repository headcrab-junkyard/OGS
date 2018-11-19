/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
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

#include "const.h"
#include "mathlib.h"
#include "bspfile.h"

// TODO: defined in bspfile header
//#define MAX_MAP_HULLS 4
//#define MIPLEVELS 4
//#define NUM_AMBIENTS 4
//

typedef enum
{
	mod_brush,
	mod_sprite,
	mod_alias,
	mod_studio
} modtype_t;

// must match definition in both modelgen.h and spritegn.h
#ifndef SYNCTYPE_T
#define SYNCTYPE_T
typedef enum {ST_SYNC=0, ST_RAND } synctype_t;
#endif

// plane_t structure
// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct mplane_s
{
	vec3_t	normal;
	float	dist;
	byte	type;			// for texture axis selection and fast side tests
	byte	signbits;		// signx + signy<<1 + signz<<1
	byte	pad[2];
} mplane_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
	vec3_t		position;
} mvertex_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
	unsigned short	v[2];
	unsigned int	cachededgeoffset;
} medge_t;

typedef struct texture_s
{
	char		name[16];
	unsigned	width, height;
	int			anim_total;				// total tenths in sequence ( 0 = no)
	int			anim_min, anim_max;		// time for this frame min <=time< max
	struct texture_s *anim_next;		// in the animation sequence
	struct texture_s *alternate_anims;	// bmodels in frame 1 use these
	unsigned	offsets[MIPLEVELS];		// four mip maps stored

#ifdef GLQUAKE
	int gl_texturenum;
	struct msurface_s *texturechain;	// for gl_texsort drawing
#endif
} texture_t;

typedef struct
{
	float		vecs[2][4];
	float		mipadjust;
	texture_t	*texture;
	int			flags;
} mtexinfo_t;

typedef struct mnode_s
{
// common with leaf
	int			contents;		// 0, to differentiate from leafs
	int			visframe;		// node needs to be traversed if current
	
	short		minmaxs[6];		// for bounding box culling // TODO: float for gl quake render

	struct mnode_s	*parent;

// node specific
	mplane_t	*plane;
	struct mnode_s	*children[2];	

	unsigned short		firstsurface;
	unsigned short		numsurfaces;
} mnode_t;

typedef struct decal_s
{
	// TODO
} decal_t;

typedef struct msurface_s
{
	int visframe; // should be drawn when node is crossed

	int dlightframe;
	int dlightbits;

	mplane_t *plane;
	int flags;

	int firstedge; // look up in model->surfedges[], negative numbers
	int numedges;  // are backwards edges

	// surface generation data
	struct surfcache_s *cachespots[MIPLEVELS];

	short texturemins[2];
	short extents[2];

	mtexinfo_t *texinfo;
	
	// lighting info
	byte styles[MAXLIGHTMAPS];
	color24 *samples; // [numstyles*surfsize]
	
	decal_t *pdecals; // TODO
	
#ifdef GLQUAKE
	int light_s, light_t; // gl lightmap coordinates

	struct glpoly_s *polys; // multiple if warped
	struct msurface_s *texturechain;

	int lightmaptexturenum;
	
	int cached_light[MAXLIGHTMAPS]; // values currently used in lightmap
	qboolean cached_dlight;         // true if dynamic light in cache
#endif
} msurface_t;

typedef struct mleaf_s
{
// common with node
	int			contents;		// wil be a negative contents number
	int			visframe;		// node needs to be traversed if current

	short		minmaxs[6];		// for bounding box culling // TODO: float for gl quake render

	struct mnode_s	*parent;

// leaf specific
	byte		*compressed_vis;
	struct efrag_s *efrags;

	msurface_t	**firstmarksurface;
	int			nummarksurfaces;
	int			key;			// BSP sequence number for leaf's contents
	byte		ambient_sound_level[NUM_AMBIENTS];
} mleaf_t;

// TODO: struct dclipnode_t
//typedef struct dclipnode_s dclipnode_t;

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct
{
	dclipnode_t	*clipnodes;
	mplane_t	*planes;
	int			firstclipnode;
	int			lastclipnode;
	vec3_t		clip_mins;
	vec3_t		clip_maxs;
} hull_t;

#if !defined(CACHE_USER) && !defined(QUAKEDEF_H)
#define CACHE_USER
typedef struct cache_user_s
{
	void *data;
} cache_user_t;
#endif

typedef struct model_s
{
	char		name[MAX_QPATH];
	qboolean	needload;		// bmodels and sprites don't cache normally

	modtype_t	type;
	int			numframes;
	synctype_t	synctype;
	
	int			flags;

//
// volume occupied by the model (graphics)
//		
	vec3_t		mins, maxs;
	float		radius;

//
// solid volume for clipping 
//
	// TODO: from gl_model header
	//qboolean	clipbox;
	//vec3_t		clipmins, clipmaxs;

//
// brush model
//
	int			firstmodelsurface, nummodelsurfaces;

	int			numsubmodels;
	dmodel_t	*submodels;

	int			numplanes;
	mplane_t	*planes;

	int			numleafs;		// number of visible leafs, not counting 0
	mleaf_t		*leafs;

	int			numvertexes;
	mvertex_t	*vertexes;

	int			numedges;
	medge_t		*edges;

	int			numnodes;
	mnode_t		*nodes;

	int			numtexinfo;
	mtexinfo_t	*texinfo;

	int			numsurfaces;
	msurface_t	*surfaces;

	int			numsurfedges;
	int			*surfedges;

	int			numclipnodes;
	dclipnode_t	*clipnodes;

	int			nummarksurfaces;
	msurface_t	**marksurfaces;

	hull_t		hulls[MAX_MAP_HULLS];

	int			numtextures;
	texture_t	**textures;

	byte		*visdata;
	color24		*lightdata;
	char		*entities;

	// TODO: qw
	//unsigned	checksum;
	//unsigned	checksum2;

//
// additional model data
//
	cache_user_t	cache;		// only access through Mod_Extradata

} model_t;

// TODO: #include "engine/custom.hpp"

#define	MAX_INFO_STRING	256
#define	MAX_SCOREBOARDNAME	32

typedef struct skin_s skin_t;

typedef struct player_info_s
{
	int		userid;
	char	userinfo[MAX_INFO_STRING];

	// scoreboard information
	char	name[MAX_SCOREBOARDNAME];
	float	entertime;
	int		frags;
	int		ping;
	byte	pl;

	// skin information
	int		topcolor;
	int		bottomcolor;

	int		spectator;
} player_info_t;