/*
 * This file is part of OGS Engine
 * Copyright (C) 2001 James 'Ender' Brown [ender@quakesrc.org]
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
/// @brief studio model structure

#pragma once

#include "const.h"

#define IDSTUDIOHEADER (('T' << 24) + ('S' << 16) + ('D' << 8) + 'I') // little-endian "IDST"
#define HLMDLHEADER "IDST"

#define HLPOLYHEADER IDSTUDIOHEADER

#define MAXSTUDIOBONES 128
#define MAXSTUDIOCONTROLLERS 8

#define STUDIO_VERSION 10 // TODO: temp

typedef float vec4_t[4];

/*
 -----------------------------------------------------------------------------------------------------------------------
    main model header
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
	int		filetypeid;	//IDSP
	int		version;	//10
	char	name[64];
	int		filesize;
	vec3_t	unknown3[3];
	vec3_t  bbmin; // TODO
	vec3_t  bbmax; // TODO
	int		flags; // TODO
	int		numbones;
	int		boneindex;
	int		numcontrollers;
	int		controllerindex;
	int		num_hitboxes;
	int		ofs_hitboxes;
	int		numseq;
	int		seqindex;
	int		unknown6;		//external sequences
	int		seqgroups;
	int		numtextures;
	int		textures;
	int		unknown7;	//something to do with external textures
	int		skinrefs;
	int		skingroups;
	int		skins;
	int		numbodyparts;
	int		bodypartindex;
	int		num_attachments;
	int		ofs_attachments;
	int		unknown9[6];	//sounds, transitions
} studiohdr_t;

// header for demand loaded sequence group data
typedef struct
{
	int magic;		//IDSQ
	int version;	//10
	char name[64];
	int unk1;
} studioseqhdr_t; // TODO: was hlmdl_sequencefile_t

// bones
typedef struct
{
	char	name[32];
	int		parent;
	int		unknown1; // TODO
	int		bonecontroller[6];
	float	value[6];
	float	scale[6];
} mstudiobone_t;

// bone controllers
typedef struct
{
	int		name;
	int		type;
	float	start;
	float	end;
	int		unknown1;
	int		index;
} mstudiobonecontroller_t;

// intersection boxes
typedef struct
{
	int bone;
	int body;	//value reported to gamecode on impact
	vec3_t mins;
	vec3_t maxs;
} mstudiobbox_t;

// demand loaded sequence groups
typedef struct
{
	char            label[32];
	char			name[64];
	unsigned int	cache;
	int				data;
} mstudioseqgroup_t;

// sequence descriptions
typedef struct
{
	char	name[32];
	float	timing;
	int		loop;
	int		unknown1[2];
	int		num_events;
	int		ofs_events;
	int		numframes;
	int		unknown2[2];
	int		motiontype;
	int		motionbone;
	vec3_t	unknown3;
	int		unknown4[2];
	vec3_t	bbox[2];
	int		hasblendseq;
	int		index;
	int		unknown7[2];
	float	unknown[4];
	int		unknown8;
	unsigned int		seqindex;
	int		unknown9[4];
} mstudioseqdesc_t;

// events
//#include "studio_event.h"
typedef struct
{
	int pose;
	int code;
	int unknown1;
	char data[64];
} mstudioevent_t;

// pivots
typedef struct
{
} mstudiopivot_t;

// attachment
typedef struct
{
	char name[32];	//I assume
	int unk;
	int bone;
	vec3_t org;
	vec3_t unk2[3];
} mstudioattachment_t;

typedef struct
{
	unsigned short	offset[6];
} mstudioanim_t;

// animation frames
typedef union
{
	struct {
		byte	valid;
		byte	total;
	} num;
	short	value;
} mstudioanimvalue_t;

// body part index
typedef struct
{
	char	name[64];
	int		nummodels;
	int		base;
	int		modelindex;
} mstudiobodyparts_t;

// skin info
typedef struct
{
	char	name[64];
	int		flags; /*flat, chrome, fullbright*/
	int		w;	/* width */
	int		h;	/* height */
	int		offset;	/* index */
} mstudiotexture_t;

// skin families
//short index[skinfamilies][skinref]

// studio models
typedef struct
{
	char	name[64];
	int		unknown1;
	float	unknown2;
	int		nummesh;
	int		meshindex;
	int		numverts;
	int		vertinfoindex;
	int		vertindex;
	int		unknown3[2];
	int		normindex;
	int		unknown4[2];
} mstudiomodel_t;

//vec3_t boundingbox[model][bone][2]; // complex intersection info

// meshes
typedef struct
{
	int numtris;
	int index;
	int skinindex;
	int unknown2;
	int unknown3;
} mstudiomesh_t;

// triangles
#if 0
typedef struct
{
} mstudiotrivert_t;
#endif