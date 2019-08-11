/*
 * This file is part of OGS Engine
 * Copyright (C) 2001 James 'Ender' Brown [ender@quakesrc.org]
 * Copyright (C) 2018-2019 BlackPhrase
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
	int		id; //filetypeid // = "IDSP"
	int		version;	//10

	char	name[64];
	int		length; //filesize

	vec3_t	eyeposition; ///< Ideal eye position
	vec3_t	min; ///< Ideal movement hull size
	vec3_t	max;

	vec3_t  bbmin; ///< Clipping bounding box
	vec3_t  bbmax; // TODO: TODO what?
	
	int		flags; // Binary flags in little-endian order
	
	// After this point, the header contains many references to offsets
	// within the MDL file and the number of items at those offsets
	
	// mstudiobone_t
	int		numbones; ///< Number of bone data sections (of type mstudiobone_t or simply bones)
	int		boneindex; ///< Offset of first data section
	
	// mstudiobonecontroller_t
	int		numbonecontrollers; //numcontrollers ///< Bone controllers
	int		bonecontrollerindex; //controllerindex
	
	// mstudiohitboxset_t
	int		numhitboxes //num_hitboxes ///< Complex bounding boxes
	int		hitboxindex; //ofs_hitboxes
	
	// mstudioanimdesc_t
	int		numseq; ///< Number of (local) animation sequences
	int		seqindex;
	
	// mstudioseqdesc_t
	int		numseqgroups; //unknown6 ///< Number of external (demand loaded) sequences
	int		seqgroupindex; //seqgroups
	
	// mstudiotexture_t
	int		numtextures; ///< Raw textures
	int		textureindex; //textures
	int		texturedataindex; // something to do with external textures
	
	int		numskinref; //skinrefs ///< Replaceable textures
	int		numskinfamilies; //skingroups
	int		skinindex; //skins
	
	// mstudiobodyparts_t
	int		numbodyparts;
	int		bodypartindex;
	
	// Local attachment points
	// mstudioattachment_t
	int		numattachments; //num_attachments ///< Queryable attachable points
	int		attachmentindex //ofs_attachments

	int		soundtable;
	int		soundindex;
	int		soundgroups;
	int		soundgroupindex;

	int		numtransitions; ///< Animation node to animation node transition graph
	int 	transitionindex;
} studiohdr_t;

// header for demand loaded sequence group data
typedef struct
{
	int magic;		//IDSQ
	int version;	//10

	char name[64];
	int length; //unk1
} studioseqhdr_t; // TODO: was hlmdl_sequencefile_t

// bones
typedef struct
{
	char	name[32]; ///< Bone name for symbolic links
	int		parent; ///< Parent bone
	int		flags; //unknown1
	int		bonecontroller[6]; ///< Bone controller index, -1 = none
	float	value[6]; ///< Default DoF values
	float	scale[6]; ///< Scale for delta DoF values
} mstudiobone_t;

// bone controllers
typedef struct
{
	int		name;
	int		type;
	float	start;
	float	end;
	int		rest; //unknown1 ///< Byte index value at rest
	int		index; ///< 0-3 user set controller, 4 mouth
} mstudiobonecontroller_t;

// intersection boxes
typedef struct
{
	int bone;
	int body;	//value reported to gamecode on impact
	vec3_t mins; ///< Bounding box
	vec3_t maxs;
} mstudiobbox_t;

// demand loaded sequence groups
typedef struct
{
	char            label[32]; ///< Textual name
	char			name[64]; ///< File name
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
	int frame; //pose
	int event; //code
	int type; //unknown1
	char options[64]; //data
} mstudioevent_t;

// pivots
typedef struct
{
	vec3_t org; ///< Pivot point
	int start;
	int end;
} mstudiopivot_t;

// attachment
typedef struct
{
	char name[32];
	int type; //unk
	int bone;
	vec3_t org; ///< Attachment point
	vec3_t vectors[3]; //unk2
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
	int		modelindex; ///< Index into models array
} mstudiobodyparts_t;

// skin info
typedef struct
{
	char	name[64];
	int		flags; /*flat, chrome, fullbright*/
	int		width; //w
	int		height; //h
	int		index; //offset
} mstudiotexture_t;

// skin families
//short index[skinfamilies][skinref]

// studio models
typedef struct
{
	char	name[64];

	int		type; //unknown1

	float	boundingradius; //unknown2

	int		nummesh;
	int		meshindex;

	int		numverts;
	int		vertinfoindex;
	int		vertindex;
	int		numnorms;
	int 	norminfoindex;
	int		normindex;

	int		numgroups; ///< Deformation groups
	int 	groupindex;
} mstudiomodel_t;

//vec3_t boundingbox[model][bone][2]; // complex intersection info

// meshes
typedef struct
{
	int numtris;
	int triindex; //index
	int skinref; //skinindex
	int numnorms; //unknown2 ///< Per-mesh normals
	int normindex; //unknown3 ///< Normal vec3_t
} mstudiomesh_t;

// triangles
#if 0
typedef struct
{
} mstudiotrivert_t;
#endif