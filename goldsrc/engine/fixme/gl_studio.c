/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Half-Life Model Renderer (Experimental) Copyright (C) 2001 James 'Ender' Brown [ender@quakesrc.org] This program is
	free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
	details. You should have received a copy of the GNU General Public License along with this program; if not, write
	to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. fromquake.h -

	render.c - apart from calculations (mostly range checking or value conversion code is a mix of standard Quake 1
	meshing, and vertex deforms. The rendering loop uses standard Quake 1 drawing, after SetupBones deforms the vertex.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/// @file

#include "quakedef.h"
//#include "shader.h" // Here in 1998 we don't use shaders!
#include "com_mesh.h"

/*
Note: this code has since been greatly modified to fix skin, submodels, hitboxes, attachments, etc.

Also, please note that it won't do all hl models....
Nor will it work 100%
*/

///////////////////////////////////////////////
// INTERNAL STUDIO MODELS HEADER
///////////////////////////////////////////////
/*
 -----------------------------------------------------------------------------------------------------------------------
    halflife model internal structure
 -----------------------------------------------------------------------------------------------------------------------
*/

// TODO
/*
#define MAX_ANIM_GROUPS	16	//submodel files containing anim data.
typedef struct	//this is stored as the cache. an hlmodel_t is generated when drawing
{
	//updated while rendering...
	float	controller[5];				// Position of bone controllers
	float	adjust[5];

	hlmdl_header_t			*header;
	hlmdl_bone_t			*bones;
	hlmdl_bonecontroller_t	*bonectls;
	hlmdl_sequencefile_t	*animcache[MAX_ANIM_GROUPS];
	zonegroup_t				*memgroup;
	struct hlmodelshaders_s
	{
		char name[MAX_QPATH];
		texnums_t defaulttex;
		shader_t *shader;
		int w, h;
	} *shaders;
	short *skinref;
	int numskinrefs;
	int numskingroups;

	int numgeomsets;
	struct
	{
		int numalternatives;
		struct hlalternative_s
		{
			mesh_t mesh;
			int numsubmeshes;
			struct
			{
				int firstindex;
				int numindexes;
			} *submesh;
		} *alternatives;
	} *geomset;
} hlmodel_t;
*/

// HL mathlib prototypes:
void	QuaternionGLAngle(const vec3_t angles, vec4_t quaternion);
void	QuaternionGLMatrix(float x, float y, float z, float w, vec4_t *GLM);
//void	UploadTexture(mstudiotexture_t *ptexture, byte *data, byte *pal);

qboolean /*QDECL*/ Mod_LoadStudioModel (model_t *mod, void *buffer, size_t fsize);

// physics stuff
void *Mod_GetHalfLifeModelData(model_t *mod);

// reflectioney things, including bone data
int HLMDL_BoneForName(model_t *mod, const char *name);
int HLMDL_FrameForName(model_t *mod, const char *name);
const char *HLMDL_FrameNameForNum(model_t *model, int surfaceidx, int num);
qboolean HLMDL_FrameInfoForNum(model_t *model, int surfaceidx, int num, char **name, int *numframes, float *duration, qboolean *loop);
qboolean HLMDL_GetModelEvent(model_t *model, int animation, int eventidx, float *timestamp, int *eventcode, char **eventdata);
int HLMDL_GetNumBones(model_t *mod, qboolean tagstoo);
int HLMDL_GetBoneParent(model_t *mod, int bonenum);
const char *HLMDL_GetBoneName(model_t *mod, int bonenum);
//int HLMDL_GetBoneData(model_t *model, int firstbone, int lastbone, framestate_t *fstate, float *result); // TODO
int HLMDL_GetAttachment(model_t *model, int tagnum, float *resultmatrix);

#ifndef SWDS
// stuff only useful for clients that need to draw stuff
void R_DrawHLModel(cl_entity_t *curent);
void HLMDL_DrawHitBoxes(cl_entity_t *ent);
#endif

///////////////////////////////////////////////

qboolean HLMDL_Trace(struct model_s *model, int hulloverride, framestate_t *framestate, vec3_t axis[3], vec3_t p1, vec3_t p2, vec3_t mins, vec3_t maxs, qboolean capsule, unsigned int against, struct trace_s *trace);
unsigned int HLMDL_Contents(struct model_s *model, int hulloverride, framestate_t *framestate, vec3_t axis[3], vec3_t p, vec3_t mins, vec3_t maxs);

void QuaternionGLMatrix(float x, float y, float z, float w, vec4_t *GLM)
{
	GLM[0][0] = 1 - 2 * y * y - 2 * z * z;
	GLM[1][0] = 2 * x * y + 2 * w * z;
	GLM[2][0] = 2 * x * z - 2 * w * y;
	GLM[0][1] = 2 * x * y - 2 * w * z;
	GLM[1][1] = 1 - 2 * x * x - 2 * z * z;
	GLM[2][1] = 2 * y * z + 2 * w * x;
	GLM[0][2] = 2 * x * z + 2 * w * y;
	GLM[1][2] = 2 * y * z - 2 * w * x;
	GLM[2][2] = 1 - 2 * x * x - 2 * y * y;
};

/*
================
QuaternionGLAngle - Convert a GL angle to a quaternion matrix
================
*/
void QuaternionGLAngle(const vec3_t angles, vec4_t quaternion)
{
	float	yaw = angles[2] * 0.5;
	float	pitch = angles[1] * 0.5;
	float	roll = angles[0] * 0.5;
	float	siny = sin(yaw);
	float	cosy = cos(yaw);
	float	sinp = sin(pitch);
	float	cosp = cos(pitch);
	float	sinr = sin(roll);
	float	cosr = cos(roll);

	quaternion[0] = sinr * cosp * cosy - cosr * sinp * siny;
	quaternion[1] = cosr * sinp * cosy + sinr * cosp * siny;
	quaternion[2] = cosr * cosp * siny - sinr * sinp * cosy;
	quaternion[3] = cosr * cosp * cosy + sinr * sinp * siny;
};

matrix3x4 transform_matrix[MAX_BONES]; // Vertex transformation matrix

#ifndef SWDS
void GL_Draw_HL_AliasFrame(short *order, vec3_t *transformed, float tex_w, float tex_h);

struct hlvremaps // TODO: mstudiotrivert_t ?
{
	unsigned short vertidx;
	unsigned short normalidx;
	unsigned short scoord;
	unsigned short tcoord;
};

static index_t HLMDL_DeDupe(unsigned short *order, struct hlvremaps *rem, size_t *count, size_t max)
{
	size_t i;
	for (i = *count; i-- > 0;)
	{
		if (rem[i].vertidx == order[0] && rem[i].normalidx == order[1] && rem[i].scoord == order[2] && rem[i].tcoord == order[3])
			return i;
	};
	i = *count;
	if (i < max)
	{
		rem[i].vertidx = order[0];
		rem[i].normalidx = order[1];
		rem[i].scoord = order[2];
		rem[i].tcoord = order[3];
	};
	*count += 1;
	return i;
};

// parse the vertex info, pull out what we can
static void HLMDL_PrepareVerticies (hlmodel_t *model, mstudiomodel_t *amodel, struct hlalternative_s *submodel)
{
	struct hlvremaps *uvert;
	size_t uvertcount, uvertstart;
	unsigned short count;
	int i;
	size_t idx = 0, v, m, maxidx=65536*3;
	size_t maxverts = 65536;

	mesh_t *mesh = &submodel->mesh;
	index_t *index;

	vec3_t *verts = (vec3_t *) ((qbyte *) model->header + amodel->vertindex);
	qbyte *bone = ((qbyte *) model->header + amodel->vertinfoindex);
	vec3_t *norms = (vec3_t *) ((qbyte *) model->header + amodel->normindex);

	uvertcount = 0;
	uvert = malloc(sizeof(*uvert)*maxverts);
	index = malloc(sizeof(*mesh->colors4b_array)*maxidx);

	for(m = 0; m < amodel->nummesh; m++)
	{
		mstudiomesh_t	*inmesh = (mstudiomesh_t *) ((qbyte *) model->header + amodel->meshindex) + m;
		unsigned short *order = (unsigned short *) ((qbyte *) model->header + inmesh->index);

		uvertstart = uvertcount;
		submodel->submesh[m].firstindex = mesh->numindexes;
		submodel->submesh[m].numindexes = 0;

		for(;;)
		{
			count = *order++;	/* get the vertex count and primitive type */
			if(!count) break;	/* done */

			if(count & 0x8000)
			{	//fan
				int first = HLMDL_DeDupe(order+0*4, uvert, &uvertcount, maxverts);
				int prev = HLMDL_DeDupe(order+1*4, uvert, &uvertcount, maxverts);
				count = (unsigned short)-(short)count;
				if (idx + (count-2)*3 > maxidx)
					break;	// would overflow. fixme: extend
				for (i = min(2,count); i < count; i++)
				{
					index[idx++] = first;
					index[idx++] = prev;
					index[idx++] = prev = HLMDL_DeDupe(order+i*4, uvert, &uvertcount, maxverts);
				};
			}
			else
			{
				int v0 = HLMDL_DeDupe(order+0*4, uvert, &uvertcount, maxverts);
				int v1 = HLMDL_DeDupe(order+1*4, uvert, &uvertcount, maxverts);
				//emit (count-2)*3 indicies as a strip
				//012 213, etc
				if (idx + (count-2)*3 > maxidx)
					break;	// would overflow. fixme: extend
				for (i = min(2,count); i < count; i++)
				{
					if (i & 1)
					{
						index[idx++] = v1;
						index[idx++] = v0;
					}
					else
					{
						index[idx++] = v0;
						index[idx++] = v1;
					};
					v0 = v1;
					index[idx++] = v1 = HLMDL_DeDupe(order+i*4, uvert, &uvertcount, maxverts);
				};
			};
			order += i*4;
		};

		if (uvertcount >= maxverts)
		{
			// if we're overflowing our verts, rewind, as we cannot generate this mesh. we'll just end up with a 0-index mesh, with no extra verts either
			uvertcount = uvertstart;
			idx = submodel->submesh[m].firstindex;
		};

		submodel->submesh[m].numindexes = idx - submodel->submesh[m].firstindex;
	};

	mesh->numindexes = idx;
	mesh->numvertexes = uvertcount;

	mesh->indexes = ZG_Malloc(model->memgroup, sizeof(*mesh->indexes)*idx);
	memcpy(mesh->indexes, index, sizeof(*mesh->indexes)*idx);

	mesh->colors4b_array = ZG_Malloc(model->memgroup, sizeof(*mesh->colors4b_array)*uvertcount);
	mesh->st_array = ZG_Malloc(model->memgroup, sizeof(*mesh->st_array)*uvertcount);
	mesh->lmst_array[0] = ZG_Malloc(model->memgroup, sizeof(*mesh->lmst_array[0])*uvertcount);
	mesh->xyz_array = ZG_Malloc(model->memgroup, sizeof(*mesh->xyz_array)*uvertcount);
	mesh->normals_array = ZG_Malloc(model->memgroup, sizeof(*mesh->normals_array)*uvertcount);
#if defined(RTLIGHTS)
	mesh->snormals_array = ZG_Malloc(model->memgroup, sizeof(*mesh->snormals_array)*uvertcount);
	mesh->tnormals_array = ZG_Malloc(model->memgroup, sizeof(*mesh->tnormals_array)*uvertcount);
#endif
	mesh->bonenums = ZG_Malloc(model->memgroup, sizeof(*mesh->bonenums)*uvertcount);
	mesh->boneweights = ZG_Malloc(model->memgroup, sizeof(*mesh->boneweights)*uvertcount);

	// prepare the verticies now that we have the mappings
	for(v = 0; v < uvertcount; v++)
	{
		mesh->bonenums[v][0] = mesh->bonenums[v][1] = mesh->bonenums[v][2] = mesh->bonenums[v][3] = bone[uvert[v].vertidx];
		Vector4Set(mesh->boneweights[v], 1, 0, 0, 0);
		Vector4Set(mesh->colors4b_array[v], 255, 255, 255, 255);	//why bytes? why not?

		mesh->lmst_array[0][v][0] = uvert[v].scoord;
		mesh->lmst_array[0][v][1] = uvert[v].tcoord;
		VectorCopy(verts[uvert[v].vertidx], mesh->xyz_array[v]);

		// Warning: these models use different tables for vertex and normals.
		// this means they might be transformed by different bones. we ignore that and just assume that the normals will want the same bone.
		VectorCopy(norms[uvert[v].normalidx], mesh->normals_array[v]);
	};

	// don't need that mapping any more
	free(uvert);
	free(index);

#if defined(RTLIGHTS)
	// treat this as the base pose, and calculate the sdir+tdir for bumpmaps.
	R_Generate_Mesh_ST_Vectors(mesh);
#endif
};
#endif

/*
================
Mod_LoadHLModel - read in the model's constituent parts
================
*/
qboolean /*QDECL*/ Mod_LoadStudioModel(model_t *mod, void *buffer)
{
	byte *pin;
	studiohdr_t *phdr;
	mstudiotexture_t *ptexture;
	
/*
#ifndef SWDS
	int i;
	int body;
	struct hlmodelshaders_s *shaders;
	mstudiotexture_t	*tex;
#endif

	//hlmodel_t *model;
	studiohdr_t *header;
	studiohdr_t *texheader;
	
	void *texmem = NULL;

	// load the model into hunk
	model = ZG_Malloc(&mod->memgroup, sizeof(hlmodel_t));
	model->memgroup = &mod->memgroup;

	header = ZG_Malloc(&mod->memgroup, fsize);
	memcpy(header, buffer, fsize);
*/

	buffer = Hunk_AllocTempName(size);
	if(!buffer)
		return 0;
	
	pin = (byte*)buffer;
	phdr = (studiohdr_t*)pin;
	ptexture = (mstudiotexture_t*)(pin + phdr->textureindex);
};

#ifdef HLSERVER
void *Mod_GetHalfLifeModelData(model_t *mod)
{
	hlmodelcache_t *mc;
	
	if (!mod || mod->type != mod_studio)
		return NULL;	// halflife models only, please

	mc = Mod_Extradata(mod);
	return (void*)mc->header;
};
#endif

int HLMDL_FrameForName(model_t *mod, const char *name)
{
	int i;
	studiohdr_t *h;
	mstudioseqdesc_t *seqs;
	hlmodel_t *mc;
	if (!mod || mod->type != mod_studio)
		return -1;	// halflife models only, please

	mc = Mod_Extradata(mod);

	h = mc->header;
	seqs = (mstudioseqdesc_t*)((char*)h+h->seqindex);

	for (i = 0; i < h->numseq; i++)
	{
		if (!strcmp(seqs[i].name, name))
			return i;
	};
	return -1;
};

qboolean HLMDL_GetModelEvent(model_t *model, int animation, int eventidx, float *timestamp, int *eventcode, char **eventdata)
{
	hlmodel_t *mc = Mod_Extradata(model);
	studiohdr_t *h = mc->header;
	mstudioevent_t *ev;
	mstudioseqdesc_t *seq = animation + (mstudioseqdesc_t*)((char*)h+h->seqindex);
	if (animation < 0 || animation >= h->numseq || eventidx < 0 || eventidx >= seq->num_events)
		return false;
	ev = eventidx + (mstudioevent_t*)((char*)h+seq->ofs_events);
	*timestamp = ev->pose / seq->timing;
	*eventcode = ev->code;
	*eventdata = ev->data;
	return true;
};

int HLMDL_BoneForName(model_t *mod, const char *name)
{
	int i;
	studiohdr_t *h;
	mstudiobone_t *bones;
	hlmodel_t *mc;
	if (!mod || mod->type != mod_studio)
		return -1;	// halflife models only, please

	mc = Mod_Extradata(mod);

	h = mc->header;
	bones = (mstudiobone_t*)((char*)h+h->boneindex);

	for (i = 0; i < h->numbones; i++)
	{
		if (!strcmp(bones[i].name, name))
			return i+1;
	};

	// FIXME: hlmdl has tags as well as bones.
	return 0;
};

/*
 ================
	HL_CalculateBones - calculate bone positions - quaternion+vector in one function
 ================
 */
void HL_CalculateBones
(
	int				frame,
	vec4_t			adjust,
	mstudiobone_t	*bone,
	mstudioanim_t	*animation,
	float			*organg
)
{
	int		i;

	/* For each vector */
	for(i = 0; i < 6; i++)
	{
		organg[i] = bone->value[i];	/* Take the bone value */

		if(animation->offset[i] != 0)
		{
			int					tempframe;
			mstudioanimvalue_t	*animvalue = (mstudioanimvalue_t *) ((qbyte *) animation + animation->offset[i]);

			/* find values including the required frame */
			tempframe = frame;
			while(animvalue->num.total <= tempframe)
			{
				tempframe -= animvalue->num.total;
				animvalue += animvalue->num.valid + 1;
			};
			if (tempframe >= animvalue->num.valid)
				tempframe = animvalue->num.valid;
			else
				tempframe += 1;

			organg[i] += animvalue[tempframe].value * bone->scale[i];
		};

		if(bone->bonecontroller[i] != -1)
		{	/* Add the programmable offset. */
			organg[i] += adjust[bone->bonecontroller[i]];
		};
	};
};

/*
 ================
	HL_CalcBoneAdj - Calculate the adjustment values for the programmable controllers
 ================
 */
void HL_CalcBoneAdj(hlmodel_t *model)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	float					value;
	mstudiobonecontroller_t	*control = (mstudiobonecontroller_t *)
									  ((qbyte *) model->header + model->header->controllerindex);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < model->header->numcontrollers; i++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		int j = control[i].index;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		if(control[i].type & 0x8000)
		{
			// wraps normally
			value = model->controller[j];// + control[i].start;
		}
		else
		{
//			value = (model->controller[j]+1)*0.5;	// shifted to give a valid range between -1 and 1, with 0 being mid-range.
//			if(value < 0)
//				value = 0;
//			else if(value > 1.0)
//				value = 1.0;
//			value = (1.0 - value) * control[i].start + value * control[i].end;

			value = model->controller[j];
			if (value < control[i].start)
				value = control[i].start;
			if (value > control[i].end)
				value = control[i].end;
		};

		/* Rotational controllers need their values converted */
		if(control[i].type >= 0x0008 && control[i].type <= 0x0020)
			model->adjust[i] = M_PI * value / 180;
		else
			model->adjust[i] = value;
	};
};

/*
 ================
	HL_SetupBones - determine where vertex should be using bone movements
 ================
 */
void QuaternionSlerp( const vec4_t p, vec4_t q, float t, vec4_t qt );

void HL_SetupBones(hlmodel_t *model, int seqnum, int firstbone, int lastbone, float subblendfrac1, float subblendfrac2, float frametime, float *matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, j;
	vec3_t					organg1[2];
	vec3_t					organg2[2];
	vec3_t					organgb[2];
	vec4_t					quat1, quat2;

	int frame1, frame2;

	mstudioseqdesc_t	*sequence = (mstudioseqdesc_t *) ((qbyte *) model->header + model->header->seqindex) +
										 ((unsigned int)seqnum>=model->header->numseq?0:seqnum);
	mstudioseqgroup_t	*sequencedata = (mstudioseqgroup_t *)
										 ((qbyte *) model->header + model->header->seqgroups) +
										 sequence->seqindex;
	mstudioanim_t			*animation;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	matrix += firstbone*12;

	if (sequencedata->name[32])
	{
		size_t fz;
		if (sequence->seqindex >= MAX_ANIM_GROUPS)
		{
			Sys_Error("Too many animation sequence cache groups\n");
			return;
		};
		if (!model->animcache[sequence->seqindex])
			model->animcache[sequence->seqindex] = FS_LoadMallocGroupFile(model->memgroup, sequencedata->name+32, &fz);
		if (!model->animcache[sequence->seqindex] || model->animcache[sequence->seqindex]->magic != (('I'<<0)|('D'<<8)|('S'<<16)|('Q'<<24)) || model->animcache[sequence->seqindex]->version != 10)
		{
			Sys_Error("Unable to load %s\n", sequencedata->name+32);
			return;
		};
		animation = (mstudioanim_t *)((qbyte*)model->animcache[sequence->seqindex] + sequence->index);
	}
	else
		animation = (mstudioanim_t *) ((qbyte *) model->header + sequencedata->data + sequence->index);

	frametime *= sequence->timing;
	if (frametime < 0)
		frametime = 0;

	frame1 = (int)frametime;
	frametime -= frame1;
	frame2 = frame1+1;

	if (!sequence->numframes)
		return;
	// halflife seems to dupe the last frame in looping animations, so don't use it.
	if(frame1 >= sequence->numframes)
	{
		if (sequence->loop)
			frame1 %= sequence->numframes-1;
		else
			frame1 = sequence->numframes-1;
	};
	if(frame2 >= sequence->numframes)
	{
		if (sequence->loop)
			frame2 %= sequence->numframes-1;
		else
			frame2 = sequence->numframes-1;
	};

	if (frame2 < frame1)
	{
		i = frame2;
		frame2 = frame1;
		frame1 = i;
		frametime = 1-frametime;
	};

	if (lastbone > model->header->numbones)
		lastbone = model->header->numbones;

	HL_CalcBoneAdj(model);	/* Deal with programmable controllers */

	// FIXME: this is useless
	/*
	if(sequence->motiontype & 0x0001)
		positions[sequence->motionbone][0] = 0.0;
	if(sequence->motiontype & 0x0002)
		positions[sequence->motionbone][1] = 0.0;
	if(sequence->motiontype & 0x0004)
		positions[sequence->motionbone][2] = 0.0;
	*/

	/*
	this is hellish.
	a hl model blends:
		4 controllers (on a player, it seems each one of them twists a separate bone in the chest)
		a mouth (not used on players)
		its a sequence (to be smooth we need to blend between two frames in the sequence)
		up to four source animations (ironically used to pitch up/down)
		alternate sequence (walking+firing)
		frame2 (quake expectations.)

		this is madness, quite frankly.

		luckily...
		controllers and mouth control the entire thing. they should be interpolated outside, and have no affect on blending here
		alternate sequences replace. we can just call this function twice (so long as bone ranges are incremental).
		autoanimating sequence is handled inside HL_CalculateBones (sequences are weird and it has to be handled there anyway)

		this means we only have sources and alternate frames left to cope with.

		FIXME: we don't handle frame2.
	*/

	if (sequence->hasblendseq>1)
	{
		int bf0, bf1;
		unsigned int bweights;
		struct
		{
			int frame;
			float weight;
			mstudioanim_t *anim;
		} blend[8];
		// right, so, this stuff is annoying.
		// we have two different blend factors.
		// we have up to 9 blend weights. figure out which frames are what
		switch(sequence->hasblendseq)
		{
		case 0:	// erk?
			return;
		case 1: // no blending.
			bf0 = bf1 = 1;
			break;
		default:
		case 2: //mix(0, 1, weight0)
		case 3: //mix(0, 1, 2, weight0);
		case 8: //weight0 only...
			bf0 = sequence->hasblendseq;
			bf1 = 1;
			break;
		case 4: //mix(mix(0, 1, weight0), mix(2, 3, weight0), weight1)
			bf0 = bf1 = 2;
			break;
		//case 6: //???
		//	bf[0] = 3; bf[1] = 2;
		//	break;
		case 9: //mix(mix(0, 1, 2, weight0), mix(2, 3, 4, weight0), mix(5, 6, 7, weight0), weight1)
			bf0 = bf1 = 3;
			break;
		};

		subblendfrac1 = (subblendfrac1+1)/2;
		subblendfrac2 = (subblendfrac2+1)/2;
		bweights = 0;
		if (bf0 > 1)
		{
			float frac = (bf0-1) * bound(0, subblendfrac1, 1);
			int f1 = bound(0, frac, bf0-1);
			int f2 = bound(0, f1+1, bf0-1);
			frac = (frac-f1);
			frac = bound(0, frac, 1);
			if (bf1 > 1)
			{
				float frac2 = (bf1-1) * bound(0, subblendfrac2, 1);
				int a1 = bound(0, frac2, bf1-1);
				int a2 = bound(0, a1+1, bf1-1);
				frac2 = (frac2-a1);
				frac2 = bound(0, frac2, 1);

				if (frac2)
				{
					if (frac)
					{
						blend[bweights].frame = frame1;
						blend[bweights].anim = animation + model->header->numbones * (f2 + a2*bf0);
						blend[bweights++].weight = frac*frac2;
					};
					if (1-frac)
					{
						blend[bweights].frame = frame1;
						blend[bweights].anim = animation + model->header->numbones * (f1 + a2*bf0);
						blend[bweights++].weight = (1-frac)*frac2;
					};
				};

				if (1-frac2)
				{
					if (frac)
					{
						blend[bweights].frame = frame1;
						blend[bweights].anim = animation + model->header->numbones * (f2 + a1*bf0);
						blend[bweights++].weight = frac*(1-frac2);
					};
					if (1-frac)
					{
						blend[bweights].frame = frame1;
						blend[bweights].anim = animation + model->header->numbones * (f1 + a1*bf0);
						blend[bweights++].weight = (1-frac)*(1-frac2);
					};
				};
			}
			else
			{
				if (frac)
				{
					blend[bweights].frame = frame1;
					blend[bweights].anim = animation + model->header->numbones * f1;
					blend[bweights++].weight = frac;
				};
				if (1-frac)
				{
					blend[bweights].frame = frame1;
					blend[bweights].anim = animation + model->header->numbones * f2;
					blend[bweights++].weight = 1-frac;
				};
			};
		}
		else
		{
			blend[bweights].frame = frame1;
			blend[bweights].anim = animation;
			blend[bweights++].weight = 1;
		};
		if (frame1 != frame2)
		{
			// bweights can be 0-4 here..
			for (i = 0; i < bweights; i++)
			{
				blend[bweights+i].frame = frame2;
				blend[bweights+i].anim = blend[i].anim;
				blend[bweights+i].weight = blend[i].weight;

				blend[i].weight *= 1-frametime;
				blend[bweights+i].weight *= frametime;
			};
			bweights *= 2;
		};

		for(i = firstbone; i < lastbone; i++, matrix+=12)
		{
			vec3_t t;
			float len;

			HL_CalculateBones(blend[0].frame, model->adjust, model->bones + i, blend[0].anim + i, organgb[0]);
			QuaternionGLAngle(organgb[1], quat2);
			Vector4Scale(quat2, blend[0].weight, quat1);
			VectorScale(organgb[0], blend[0].weight, t);

			for (j = 1; j < bweights; j++)
			{
				HL_CalculateBones(blend[j].frame, model->adjust, model->bones + i, blend[j].anim + i, organgb[0]);
				QuaternionGLAngle(organgb[1], quat2);
				if (DotProduct4(quat2, quat1) < 0)	// negative quats are annoying
					Vector4MA(quat1, -blend[j].weight, quat2, quat1);
				else
					Vector4MA(quat1, blend[j].weight, quat2, quat1);
				VectorMA(t, blend[j].weight, organgb[0], t);
			};

			// we were lame and didn't use slerp. boo hiss. now we need to normalise the things and hope we didn't hit any singularities
			len = sqrt(DotProduct4(quat1,quat1));
			if (len && len != 1)
			{
				len = 1/len;
				quat1[0] *= len;
				quat1[1] *= len;
				quat1[2] *= len;
				quat1[3] *= len;
			};

			QuaternionGLMatrix(quat1[0], quat1[1], quat1[2], quat1[3], (vec4_t*)matrix);
			matrix[0*4+3] = t[0];
			matrix[1*4+3] = t[1];
			matrix[2*4+3] = t[2];
		};
	}
	else
	{
		for(i = firstbone; i < lastbone; i++, matrix+=12)
		{
			HL_CalculateBones(frame1, model->adjust, model->bones + i, animation + i, organg1[0]);
			QuaternionGLAngle(organg1[1], quat1);	/* A quaternion */
			if (frame1 != frame2)
			{
				HL_CalculateBones(frame2, model->adjust, model->bones + i, animation + i, organg2[0]);
				QuaternionGLAngle(organg2[1], quat2);	/* A quaternion */

				// lerp the quats properly rather than poorly lerping eular angles.
				QuaternionSlerp(quat1, quat2, frametime, quat1);
				VectorInterpolate(organg1[0], frametime, organg2[0], organg1[0]);
			};

			// figure out the relative bone matrix.
			// we probably ought to keep them as quats or something.
			QuaternionGLMatrix(quat1[0], quat1[1], quat1[2], quat1[3], (vec4_t*)matrix);
			matrix[0*4+3] = organg1[0][0];
			matrix[1*4+3] = organg1[0][1];
			matrix[2*4+3] = organg1[0][2];
		};
	};
};

int HLMDL_GetNumBones(model_t *mod, qboolean tags)
{
	hlmodel_t *mc;
	if (!mod || mod->type != mod_studio)
		return -1;	// halflife models only, please

	mc = Mod_Extradata(mod);
	if (tags)
		return mc->header->numbones + mc->header->num_attachments;
	return mc->header->numbones;
};

int HLMDL_GetBoneParent(model_t *mod, int bonenum)
{
	hlmodel_t *model = Mod_Extradata(mod);

	if (bonenum >= 0 && bonenum < model->header->numbones)
		return model->bones[bonenum].parent;
	bonenum -= model->header->numbones;
	if (bonenum >= 0 && bonenum < model->header->num_attachments)
	{
		mstudioattachment_t *attachments = bonenum+(mstudioattachment_t*)((char*)model->header + model->header->ofs_attachments);
		return attachments->bone;
	};
	return -1;
};

const char *HLMDL_GetBoneName(model_t *mod, int bonenum)
{
	hlmodel_t *model = Mod_Extradata(mod);

	if (bonenum >= 0 && bonenum < model->header->numbones)
		return model->bones[bonenum].name;
	bonenum -= model->header->numbones;
	if (bonenum >= 0 && bonenum < model->header->num_attachments)
	{
		mstudioattachment_t *attachments = bonenum+(mstudioattachment_t*)((char*)model->header + model->header->ofs_attachments);
		if (*attachments->name)
			return attachments->name;
		return "Unnamed Attachment";
	};
	return NULL;
};

int HLMDL_GetAttachment(model_t *mod, int tagnum, float *resultmatrix)
{
	hlmodel_t *model = Mod_Extradata(mod);
	if (tagnum >= 0 && tagnum < model->header->num_attachments)
	{
		mstudioattachment_t *attachments = tagnum+(mstudioattachment_t*)((char*)model->header + model->header->ofs_attachments);
		resultmatrix[3] = attachments->org[0];
		resultmatrix[7] = attachments->org[1];
		resultmatrix[11] = attachments->org[2];
		return attachments->bone;
	};
	return -1;
};

static int HLMDL_GetBoneData_Internal(hlmodel_t *model, int firstbone, int lastbone, framestate_t *fstate, float *result)
{
	int b, cbone, bgroup;

	for (b = 0; b < MAX_BONE_CONTROLLERS; b++)
		model->controller[b] = fstate->bonecontrols[b];
	for (cbone = 0, bgroup = 0; bgroup < FS_COUNT; bgroup++)
	{
		lastbone = fstate->g[bgroup].endbone;
		if (bgroup == FS_COUNT-1)
			lastbone = model->header->numbones;
		if (cbone >= lastbone)
			continue;
		HL_SetupBones(model, fstate->g[bgroup].frame[0], cbone, lastbone, fstate->g[bgroup].subblendfrac, fstate->g[bgroup].subblend2frac, fstate->g[bgroup].frametime[0], result);	/* Setup the bones */
		cbone = lastbone;
	};
	return cbone;
};

int HLMDL_GetBoneData(model_t *mod, int firstbone, int lastbone, framestate_t *fstate, float *result)
{
	return HLMDL_GetBoneData_Internal(Mod_Extradata(mod), firstbone, lastbone, fstate, result);
};

const char *HLMDL_FrameNameForNum(model_t *mod, int surfaceidx, int seqnum)
{
	hlmodel_t *model = Mod_Extradata(mod);
	mstudioseqdesc_t	*sequence = (mstudioseqdesc_t *) ((qbyte *) model->header + model->header->seqindex) +
										 ((unsigned int)seqnum>=model->header->numseq?0:seqnum);
	return sequence->name;
};

qboolean HLMDL_FrameInfoForNum(model_t *mod, int surfaceidx, int seqnum, char **name, int *numframes, float *duration, qboolean *loop)
{
	hlmodel_t *model = Mod_Extradata(mod);
	mstudioseqdesc_t	*sequence = (mstudioseqdesc_t *) ((qbyte *) model->header + model->header->seqindex) +
										 ((unsigned int)seqnum>=model->header->numseq?0:seqnum);

	*name = sequence->name;
	*numframes = sequence->numframes;
	*duration = (sequence->numframes-1)/sequence->timing;
	*loop = sequence->loop;
	return true;
};

qboolean HLMDL_Trace(model_t *model, int hulloverride, framestate_t *framestate, vec3_t axis[3], vec3_t p1, vec3_t p2, vec3_t mins, vec3_t maxs, qboolean capsule, unsigned int against, struct trace_s *trace)
{
	hlmodel_t *hm = Mod_Extradata(model);
	float *relbones;
	float calcrelbones[MAX_BONES*12];
	int bonecount;
	int b, i;
	vec3_t norm, p1l, p2l;
	float inverse[12];
	mstudiobbox_t *hitbox = (mstudiobbox_t*)((char*)hm->header+hm->header->ofs_hitboxes);
	float dist, d1, d2, f, enterfrac, enterdist, exitfrac;
	qboolean startout, endout;
	int enterplane;

	memset (trace, 0, sizeof(trace_t));
	trace->fraction = trace->truefraction = 1;
	if (!(against & FTECONTENTS_BODY))
		return false;

	if (framestate->bonestate && framestate->skeltype == SKEL_ABSOLUTE)
	{
		relbones = framestate->bonestate;
		bonecount = framestate->bonecount;
		if (axis)
		{
			for (b = 0; b < bonecount; b++)
				R_ConcatTransformsAxis(axis, (void*)(relbones+b*12), transform_matrix[b]);
		}
		else
			memcpy(transform_matrix, relbones, bonecount * 12 * sizeof(float));
	}
	else
	{
		// get relative bones from th emodel.
		if (framestate->bonestate)
		{
			relbones = framestate->bonestate;
			bonecount = framestate->bonecount;
		}
		else
		{
			relbones = calcrelbones;
			bonecount = HLMDL_GetBoneData(model, 0, MAX_BONES, framestate, calcrelbones);
		};

		// convert relative to absolutes
		for (b = 0; b < bonecount; b++)
		{
			// If we have a parent, take the addition. Otherwise just copy the values
			if(hm->bones[b].parent>=0)
				R_ConcatTransforms(transform_matrix[hm->bones[b].parent], (void*)(relbones+b*12), transform_matrix[b]);
			else if (axis)
				R_ConcatTransformsAxis(axis, (void*)(relbones+b*12), transform_matrix[b]);
			else
				memcpy(transform_matrix[b], relbones+b*12, 12 * sizeof(float));
		};
	};

	for (b = 0; b < hm->header->num_hitboxes; b++, hitbox++)
	{
		startout = false;
		endout = false;
		enterplane = 0;
		enterfrac = -1;
		exitfrac = 10;
		enterdist = 0;

		// FIXME: would be nice to check if there's a possible collision a bit faster, without needing to do lots of excess maths.

		// transform start+end into the bbox, so everything is axial and simple.
		Matrix3x4_Invert_Simple((void*)transform_matrix[hitbox->bone], inverse);
		Matrix3x4_RM_Transform3(inverse, p1, p1l);
		Matrix3x4_RM_Transform3(inverse, p2, p2l);
		// FIXME: would it be faster to just generate the plane and transform that, colliding non-axially? would probably be better for sized impactors.

		// clip against the 6 axial faces
		for (i = 0; i < 6; i++)
		{
			if (i < 3)
			{	// normal>0
				dist = hitbox->maxs[i] - mins[i];
				d1 = p1l[i] - dist;
				d2 = p2l[i] - dist;
			}
			else
			{
				// normal<0
				dist = maxs[i-3] - hitbox->mins[i-3];
				d1 = -p1l[i-3] - dist;
				d2 = -p2l[i-3] - dist;
			};
			
			// FIXME: if the trace has size, we should insert 6 extra planes for the shape of the impactor
			// FIXME: capsules

			if (d1 >= 0)
				startout = true;
			if (d2 > 0)
				endout = true;

			// if we're fully outside any plane, then we cannot possibly enter the brush, skip to the next one
			if (d1 > 0 && d2 >= 0)
				goto nextbrush;

			// if we're fully inside the plane, then whatever is happening is not relevent for this plane
			if (d1 < 0 && d2 <= 0)
				continue;

			f = d1 / (d1-d2);
			if (d1 > d2)
			{
				// entered the brush. favour the furthest fraction to avoid extended edges (yay for convex shapes)
				if (enterfrac < f)
				{
					enterfrac = f;
					enterplane = i;
					enterdist = dist;
				};
			}
			else
			{
				// left the brush, favour the nearest plane (smallest frac)
				if (exitfrac > f)
				{
					exitfrac = f;
				};
			};
		};

		if (!startout)
		{
			trace->startsolid = true;
			if (!endout)
				trace->allsolid = true;
			trace->contents = FTECONTENTS_BODY;

			trace->brush_face = 0;
			trace->bone_id = hitbox->bone+1;
			trace->brush_id = b+1;
			trace->surface_id = hitbox->body;
			break;
		};
		if (enterfrac != -1 && enterfrac < exitfrac)
		{
			// impact!
			if (enterfrac < trace->fraction)
			{
				trace->fraction = trace->truefraction = enterfrac;
				trace->plane.dist = enterdist;
				trace->contents = FTECONTENTS_BODY;

				trace->brush_face = enterplane+1;
				trace->bone_id = hitbox->bone+1;
				trace->brush_id = b+1;
				trace->surface_id = hitbox->body;
			};
		};
nextbrush:
		;
	};

	if (trace->brush_face)
	{
		VectorClear(norm);
		if (trace->brush_face < 4)
			norm[trace->brush_face-1] = 1;
		else
			norm[trace->brush_face-4] = -1;
		Matrix3x4_RM_Transform3x3((void*)transform_matrix[trace->bone_id-1], norm, trace->plane.normal);
	}
	else
		VectorClear(trace->plane.normal);
	VectorInterpolate(p1, trace->fraction, p2, trace->endpos);

	return trace->truefraction != 1;
};

unsigned int HLMDL_Contents	(model_t *model, int hulloverride, framestate_t *framestate, vec3_t axis[3], vec3_t p, vec3_t mins, vec3_t maxs)
{
	trace_t tr;
	HLMDL_Trace(model, hulloverride, framestate, axis, p, p, mins, maxs, false, ~0, &tr);
	return tr.contents;
};

#ifndef SWDS
void R_HL_BuildFrame(hlmodel_t *model, mstudiomodel_t *amodel, entity_t *curent, int bodypart, int bodyidx, int meshidx, float tex_s, float tex_t, mesh_t *mesh, qboolean gpubones)
{
	int b;
	int cbone;
//	int bgroup;
//	int lastbone;
	int v;

	*mesh = model->geomset[bodypart].alternatives[bodyidx].mesh;

	// FIXME: cache this!
	if (curent->framestate.bonecount >= model->header->numbones)
	{
		if (curent->framestate.skeltype == SKEL_RELATIVE)
		{
			mesh->numbones = model->header->numbones;
			for (b = 0; b < mesh->numbones; b++)
			{
				// If we have a parent, take the addition. Otherwise just copy the values
				if(model->bones[b].parent>=0)
				{
					R_ConcatTransforms(transform_matrix[model->bones[b].parent], (void*)(curent->framestate.bonestate+b*12), transform_matrix[b]);
				}
				else
				{
					memcpy(transform_matrix[b], curent->framestate.bonestate+b*12, 12 * sizeof(float));
				};
			};
			mesh->bones = transform_matrix[0][0];
		}
		else
		{
			mesh->bones = curent->framestate.bonestate;
			mesh->numbones = curent->framestate.bonecount;
		};
	}
	else
	{
		float relatives[12*MAX_BONES];
		mesh->bones = transform_matrix[0][0];
		mesh->numbones = model->header->numbones;

/*		// FIXME: needs caching.
		for (b = 0; b < MAX_BONE_CONTROLLERS; b++)
			model->controller[b] = curent->framestate.bonecontrols[b];
		for (cbone = 0, bgroup = 0; bgroup < FS_COUNT; bgroup++)
		{
			lastbone = curent->framestate.g[bgroup].endbone;
			if (bgroup == FS_COUNT-1)
				lastbone = model->header->numbones;
			if (cbone >= lastbone)
				continue;
			HL_SetupBones(model, curent->framestate.g[bgroup].frame[0], cbone, lastbone, curent->framestate.g[bgroup].subblendfrac, curent->framestate.g[bgroup].frametime[0], relatives);	// Setup the bones
			cbone = lastbone;
		};
*/
		cbone = HLMDL_GetBoneData_Internal(model, 0, model->header->numbones, &curent->framestate, relatives);

		// convert relative to absolutes
		for (b = 0; b < cbone; b++)
		{
			// If we have a parent, take the addition. Otherwise just copy the values
			if(model->bones[b].parent>=0)
				R_ConcatTransforms(transform_matrix[model->bones[b].parent], (void*)(relatives+b*12), transform_matrix[b]);
			else
				memcpy(transform_matrix[b], relatives+b*12, 12 * sizeof(float));
		};
	};

	mesh->indexes += model->geomset[bodypart].alternatives[bodyidx].submesh[meshidx].firstindex;
	mesh->numindexes = model->geomset[bodypart].alternatives[bodyidx].submesh[meshidx].numindexes;

	if (gpubones)
	{	// get the backend to do the skeletal stuff (read: glsl)
		for(v = 0; v < mesh->numvertexes; v++)
		{	// should really come up with a better way to deal with this, like rect textures.
			mesh->st_array[v][0] = mesh->lmst_array[0][v][0] * tex_s;
			mesh->st_array[v][1] = mesh->lmst_array[0][v][1] * tex_t;
		};
	}
	else
	{	// backend can't handle it, apparently. do it in software.
		static vecV_t nxyz[2048];
		static vec3_t nnorm[2048];
		for(v = 0; v < mesh->numvertexes; v++)
		{	// should really come up with a better way to deal with this, like rect textures.
			mesh->st_array[v][0] = mesh->lmst_array[0][v][0] * tex_s;
			mesh->st_array[v][1] = mesh->lmst_array[0][v][1] * tex_t;

			VectorTransform(mesh->xyz_array[v], (void *)transform_matrix[mesh->bonenums[v][0]], nxyz[v]);

			nnorm[v][0] = DotProduct(mesh->normals_array[v], transform_matrix[mesh->bonenums[v][0]][0]);
			nnorm[v][1] = DotProduct(mesh->normals_array[v], transform_matrix[mesh->bonenums[v][0]][1]);
			nnorm[v][2] = DotProduct(mesh->normals_array[v], transform_matrix[mesh->bonenums[v][0]][2]);

			// FIXME: svector, tvector!
		};
		mesh->xyz_array = nxyz;
		mesh->normals_array = nnorm;
		mesh->bonenums = NULL;
		mesh->boneweights = NULL;
		mesh->bones = NULL;
		mesh->numbones = 0;
	};
};

static void R_HalfLife_WalkMeshes(entity_t *rent, batch_t *b, batch_t **batches);

static void R_HL_BuildMesh(struct batch_s *b)
{
	R_HalfLife_WalkMeshes(b->ent, b, NULL);
};

static void R_HalfLife_WalkMeshes(entity_t *rent, batch_t *b, batch_t **batches)
{
	hlmodel_t *model = Mod_Extradata(rent->model);
	int						body, m;
	int batchid = 0;
	static mesh_t bmesh, *mptr = &bmesh;
	skinfile_t *sk = NULL;

	unsigned int entity_body = 0;

	if (rent->customskin)
		sk = Mod_LookupSkin(rent->customskin);
	//entity_body = rent->body;	// hey, if its there, let's use it

	for (body = 0; body < model->numgeomsets; body++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		mstudiobodyparts_t	*bodypart = (mstudiobodyparts_t *) ((qbyte *) model->header + model->header->bodypartindex) + body;
		int					bodyindex = ((sk && body < MAX_GEOMSETS && sk->geomset[body] >= 1)?sk->geomset[body]-1:(entity_body / bodypart->base)) % bodypart->nummodels;
		mstudiomodel_t	*amodel = (mstudiomodel_t *) ((qbyte *) model->header + bodypart->modelindex) + bodyindex;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		// Draw each mesh
		for(m = 0; m < amodel->nummesh; m++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			mstudiomesh_t	*mesh = (mstudiomesh_t *) ((qbyte *) model->header + amodel->meshindex) + m;
			float			tex_w;
			float			tex_h;
			struct hlmodelshaders_s *s;
			int skinidx = mesh->skinindex;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if (skinidx >= model->numskinrefs)
				continue;	// can happen from bad mesh/skin mixing
			if (rent->skinnum < model->numskingroups)
				skinidx += rent->skinnum * model->numskinrefs;
			s = &model->shaders[model->skinref[skinidx]];

			if (batches)
			{
				int sort, j;

				b = BE_GetTempBatch();
				if (!b)
					return;

				if (!s->shader)
				{
					s->shader = R_RegisterSkin(s->name, rent->model->name);
					R_BuildDefaultTexnums(&s->defaulttex, s->shader);
				};
				b->skin = NULL;
				b->shader = s->shader;
				if (sk)
				{
					int i;
					for (i = 0; i < sk->nummappings; i++)
					{
						if (!strcmp(sk->mappings[i].surface, s->name))
						{
							b->skin = &sk->mappings[i].texnums;
							b->shader = sk->mappings[i].shader;
							break;
						};
					};
				};
				
				if ( rent->forcedshader )
					b->shader = rent->forcedshader; 

				b->buildmeshes = R_HL_BuildMesh;
				b->ent = rent;
				b->mesh = NULL;
				b->firstmesh = 0;
				b->meshes = 1;
				b->texture = NULL;
				for (j = 0; j < MAXRLIGHTMAPS; j++)
					b->lightmap[j] = -1;
				b->surf_first = batchid;
				b->flags = 0;
				sort = b->shader->sort;
				// FIXME: we probably need to force some blend modes based on the surface flags.
				if (rent->flags & RF_FORCECOLOURMOD)
					b->flags |= BEF_FORCECOLOURMOD;
				if (rent->flags & RF_ADDITIVE)
				{
					b->flags |= BEF_FORCEADDITIVE;
					if (sort < SHADER_SORT_ADDITIVE)
						sort = SHADER_SORT_ADDITIVE;
				};
				if (rent->flags & RF_TRANSLUCENT)
				{
					b->flags |= BEF_FORCETRANSPARENT;
					if (SHADER_SORT_PORTAL < sort && sort < SHADER_SORT_BLEND)
						sort = SHADER_SORT_BLEND;
				};
				if (rent->flags & RF_NODEPTHTEST)
				{
					b->flags |= BEF_FORCENODEPTH;
					if (sort < SHADER_SORT_NEAREST)
						sort = SHADER_SORT_NEAREST;
				};
				if (rent->flags & RF_NOSHADOW)
					b->flags |= BEF_NOSHADOWS;
				b->vbo = NULL;
				b->next = batches[sort];
				batches[sort] = b;
			}
			else
			{
				if (batchid == b->surf_first)
				{
					tex_w = 1.0f / s->w;
					tex_h = 1.0f / s->h;
	
					b->mesh = &mptr;
					R_HL_BuildFrame(model, amodel, b->ent, body, bodyindex, m, tex_w, tex_h, b->mesh[0], b->shader->prog && (b->shader->prog->supportedpermutations & PERMUTATION_SKELETAL));
					return;
				};
			};

			batchid++;
		};
	};
};

qboolean R_CalcModelLighting(entity_t *e, model_t *clmodel);

void R_HalfLife_GenerateBatches(entity_t *e, batch_t **batches)
{
	R_CalcModelLighting(e, e->model);
	R_HalfLife_WalkMeshes(e, NULL, batches);
};

void HLMDL_DrawHitBoxes(entity_t *rent)
{
	hlmodel_t *model = Mod_Extradata(rent->model);
	mstudiobbox_t *hitbox = (mstudiobbox_t*)((char*)model->header+model->header->ofs_hitboxes);
	matrix3x4 entitymatrix;

	shader_t *shader = R_RegisterShader("hitbox_nodepth", SUF_NONE,
			"{\n"
				"polygonoffset\n"
				"{\n"
					"map $whiteimage\n"
					"blendfunc gl_src_alpha gl_one\n"
					"rgbgen vertex\n"
					"alphagen vertex\n"
					"nodepthtest\n"
				"}\n"
			"}\n");

	float relbones[MAX_BONES*12];
	int bonecount = HLMDL_GetBoneData(rent->model, 0, MAX_BONES, &rent->framestate, relbones);
	int b;

	entitymatrix[0][0] = rent->axis[0][0];
	entitymatrix[0][1] = rent->axis[1][0];
	entitymatrix[0][2] = rent->axis[2][0];
	entitymatrix[1][0] = rent->axis[0][1];
	entitymatrix[1][1] = rent->axis[1][1];
	entitymatrix[1][2] = rent->axis[2][1];
	entitymatrix[2][0] = rent->axis[0][2];
	entitymatrix[2][1] = rent->axis[1][2];
	entitymatrix[2][2] = rent->axis[2][2];
	entitymatrix[0][3] = rent->origin[0];
	entitymatrix[1][3] = rent->origin[1];
	entitymatrix[2][3] = rent->origin[2];

	// convert relative to absolutes
	for (b = 0; b < bonecount; b++)
	{
		// If we have a parent, take the addition. Otherwise just copy the values
		if(model->bones[b].parent>=0)
			R_ConcatTransforms(transform_matrix[model->bones[b].parent], (void*)(relbones+b*12), transform_matrix[b]);
		else
			R_ConcatTransforms(entitymatrix, (void*)(relbones+b*12), transform_matrix[b]);
	};

	for (b = 0; b < model->header->num_hitboxes; b++, hitbox++)
		CLQ1_AddOrientedCube(shader, hitbox->mins, hitbox->maxs, transform_matrix[hitbox->bone][0], 1, 1, 1, 0.2);
};
#endif // SWDS

#endif