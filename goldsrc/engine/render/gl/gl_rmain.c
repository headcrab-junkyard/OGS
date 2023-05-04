/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018, 2020-2021 BlackPhrase
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

#include "quakedef.h"

cl_entity_t r_worldentity;

qboolean r_cache_thrash; // compatability

vec3_t modelorg, r_entorigin;
cl_entity_t *currententity;

int r_visframecount; // bumped when going to a new PVS
int r_framecount;    // used for dlight push checking

mplane_t frustum[4];

int c_brush_polys, c_alias_polys;

qboolean envmap; // true during envmap command capture

int currenttexture = -1; // to avoid unnecessary texture sets

int cnttextures[2] = { -1, -1 }; // cached

int particletexture; // little dot for particles
int playertextures;  // up to 16 color translated skins

int mirrortexturenum; // quake texturenum, not gltexturenum
qboolean mirror;
mplane_t *mirror_plane;

//
// view origin
//
vec3_t vup;
vec3_t vpn;
vec3_t vright;
vec3_t r_origin;

float r_world_matrix[16];
float r_base_world_matrix[16];

//
// screen size info
//
refdef_t r_refdef;

mleaf_t *r_viewleaf, *r_oldviewleaf;

texture_t *r_notexture_mip;

int d_lightstylevalue[256]; // 8.8 fraction of base light value

void R_MarkLeaves(void);

// TODO: Wrong place for cvars?

cvar_t ati_npatch = { "ati_npatch", "1.0", FCVAR_ARCHIVE }; // TODO: register

cvar_t r_bmodelinterp = { "r_bmodelinterp", "1" }; // TODO: register
cvar_t r_norefresh = { "r_norefresh", "0" };
cvar_t r_drawentities = { "r_drawentities", "1" };
cvar_t r_drawviewmodel = { "r_drawviewmodel", "1" };
cvar_t r_speeds = { "r_speeds", "0" };
cvar_t r_fullbright = { "r_fullbright", "0" };
cvar_t r_decals = { "r_decals", "4096" }; // TODO: register
cvar_t sp_decals = { "sp_decals", "4096", FCVAR_ARCHIVE }; // TODO: register
cvar_t mp_decals = { "mp_decals", "300", FCVAR_ARCHIVE }; // TODO: register
cvar_t r_lightmap = { "r_lightmap", "0" };
cvar_t r_shadows = { "r_shadows", "0" };
cvar_t r_mirroralpha = { "r_mirroralpha", "1" };
cvar_t r_wateralpha = { "r_wateralpha", "1" };
cvar_t r_dynamic = { "r_dynamic", "1" };
cvar_t r_novis = { "r_novis", "0" };
cvar_t r_mmx = { "r_mmx", "0" }; // TODO: register
cvar_t r_traceglow = { "r_traceglow", "0" }; // TODO: register
cvar_t r_wadtextures = { "r_wadtextures", "0" }; // TODO: register
cvar_t r_glowshellfreq = { "r_glowshellfreq", "2.2" }; // TODO: register

//cvar_t r_netgraph = {"r_netgraph", "0"}; // TODO: remove

cvar_t d_spriteskip = { "d_spriteskip", "0" }; // TODO: register

cvar_t gl_finish = { "gl_finish", "0" }; // TODO: remove?
cvar_t gl_clear = { "gl_clear", "0" };
cvar_t gl_cull = { "gl_cull", "1" };
cvar_t gl_texsort = { "gl_texsort", "1" }; // TODO: remove
cvar_t gl_smoothmodels = { "gl_smoothmodels", "1" }; // TODO: remove
cvar_t gl_affinemodels = { "gl_affinemodels", "0" };
cvar_t gl_polyblend = { "gl_polyblend", "1" }; // TODO: remove
cvar_t gl_flashblend = { "gl_flashblend", "0" };
cvar_t gl_playermip = { "gl_playermip", "0" }; // TODO: remove
cvar_t gl_nocolors = { "gl_nocolors", "0" }; // TODO: remove
cvar_t gl_keeptjunctions = { "gl_keeptjunctions", "1", FCVAR_ARCHIVE };
cvar_t gl_wateramp = { "gl_wateramp", "0.3", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_dither = { "gl_dither", "1", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_spriteblend = { "gl_spriteblend", "1", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_polyoffset = { "gl_polyoffset", "4", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_lightholes = { "gl_lightholes", "1", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_zmax = { "gl_zmax", "4096" }; // TODO: unused
cvar_t gl_alphamin = { "gl_alphamin", "0.25" }; // TODO: unused
cvar_t gl_overdraw = { "gl_overdraw", "0" }; // TODO: unused
cvar_t gl_watersides = { "gl_watersides", "0" }; // TODO: unused
cvar_t gl_overbright = { "gl_overbright", "1", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_envmapsize = { "gl_envmapsize", "256" }; // TODO: unused
cvar_t gl_flipmatrix = { "gl_flipmatrix", "0", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_monolights = { "gl_monolights", "0", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_fog = { "gl_fog", "1", FCVAR_ARCHIVE }; // TODO: unused
cvar_t gl_wireframe = { "gl_wireframe", "0" }; // TODO: unused

cvar_t gl_reporttjunctions = { "gl_reporttjunctions", "0" }; // TODO: remove
cvar_t gl_doubleeyes = { "gl_doubleeys", "1" }; // TODO: not present in qw & gs, remove

cvar_t gl_ztrick_old = {"gl_ztrick_old", "0"}; // TODO: unused, wrong place

cvar_t gl_vsync = {"gl_vsync", "0", FCVAR_ARCHIVE}; // TODO: wrong place

extern cvar_t gl_ztrick;
//extern	cvar_t	scr_fov; // TODO

/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
	int i;

	for(i = 0; i < 4; i++)
		if(BoxOnPlaneSide(mins, maxs, &frustum[i]) == 2)
			return true;
	return false;
}

void R_RotateForEntity(cl_entity_t *e)
{
	qglTranslatef(e->origin[0], e->origin[1], e->origin[2]);

	qglRotatef(e->angles[1], 0, 0, 1);
	qglRotatef(-e->angles[0], 0, 1, 0);
	//ZOID: fixed z angle
	qglRotatef(e->angles[2], 1, 0, 0);
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/

/*
================
R_GetSpriteFrame
================
*/
mspriteframe_t *R_GetSpriteFrame(cl_entity_t *currententity)
{
	msprite_t *psprite;
	mspritegroup_t *pspritegroup;
	mspriteframe_t *pspriteframe;
	int i, numframes, frame;
	float *pintervals, fullinterval, targettime, time;

	psprite = currententity->model->cache.data;
	frame = currententity->frame;

	if((frame >= psprite->numframes) || (frame < 0))
	{
		Con_Printf("R_DrawSprite: no such frame %d\n", frame);
		frame = 0;
	}

	if(psprite->frames[frame].type == SPR_SINGLE)
	{
		pspriteframe = psprite->frames[frame].frameptr;
	}
	else
	{
		pspritegroup = (mspritegroup_t *)psprite->frames[frame].frameptr;
		pintervals = pspritegroup->intervals;
		numframes = pspritegroup->numframes;
		fullinterval = pintervals[numframes - 1];

		time = cl.time + currententity->syncbase;

		// when loading in Mod_LoadSpriteGroup, we guaranteed all interval values
		// are positive, so we don't have to worry about division by 0
		targettime = time - ((int)(time / fullinterval)) * fullinterval;

		for(i = 0; i < (numframes - 1); i++)
		{
			if(pintervals[i] > targettime)
				break;
		}

		pspriteframe = pspritegroup->frames[i];
	}

	return pspriteframe;
}

/*
=================
R_DrawSpriteModel

=================
*/
void R_DrawSpriteModel(cl_entity_t *e)
{
	vec3_t point;
	mspriteframe_t *frame;
	float *up, *right;
	vec3_t v_forward, v_right, v_up;
	msprite_t *psprite;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache
	frame = R_GetSpriteFrame(e);
	psprite = currententity->model->cache.data;

	if(psprite->type == SPR_ORIENTED)
	{ // bullet marks on walls
		AngleVectors(currententity->angles, v_forward, v_right, v_up);
		up = v_up;
		right = v_right;
	}
	else
	{ // normal sprite
		up = vup;
		right = vright;
	}

	qglColor3f(1, 1, 1);

	GL_DisableMultitexture();

	GL_Bind(frame->gl_texturenum);

	qglEnable(GL_ALPHA_TEST);
	qglBegin(GL_QUADS);

	qglTexCoord2f(0, 1);
	VectorMA(e->origin, frame->down, up, point);
	VectorMA(point, frame->left, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(0, 0);
	VectorMA(e->origin, frame->up, up, point);
	VectorMA(point, frame->left, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 0);
	VectorMA(e->origin, frame->up, up, point);
	VectorMA(point, frame->right, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 1);
	VectorMA(e->origin, frame->down, up, point);
	VectorMA(point, frame->right, right, point);
	qglVertex3fv(point);

	qglEnd();

	qglDisable(GL_ALPHA_TEST);
}

/*
=============================================================

  ALIAS MODELS

=============================================================
*/

#define NUMVERTEXNORMALS 162

float r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};

vec3_t shadevector;
float shadelight, ambientlight;

// precalculated dot products for quantized angles
#define SHADEDOT_QUANT 16
float r_avertexnormal_dots[SHADEDOT_QUANT][256] =
#include "anorm_dots.h"
;

float *shadedots = r_avertexnormal_dots[0];

int lastposenum;

/*
=============
GL_DrawAliasFrame
=============
*/
void GL_DrawAliasFrame(aliashdr_t *paliashdr, int posenum)
{
	float l;
	trivertx_t *verts;
	int *order;
	int count;

	lastposenum = posenum;

	verts = (trivertx_t *)((byte *)paliashdr + paliashdr->posedata);
	verts += posenum * paliashdr->poseverts;
	order = (int *)((byte *)paliashdr + paliashdr->commands);

	while(1)
	{
		// get the vertex count and primitive type
		count = *order++;
		if(!count)
			break; // done
		if(count < 0)
		{
			count = -count;
			qglBegin(GL_TRIANGLE_FAN);
		}
		else
			qglBegin(GL_TRIANGLE_STRIP);

		do
		{
			// texture coordinates come from the draw list
			qglTexCoord2f(((float *)order)[0], ((float *)order)[1]);
			order += 2;

			// normals and vertexes come from the frame list
			l = shadedots[verts->lightnormalindex] * shadelight;
			qglColor3f(l, l, l);
			qglVertex3f(verts->v[0], verts->v[1], verts->v[2]);
			verts++;
		} while(--count);

		qglEnd();
	}
}

/*
=============
GL_DrawAliasShadow
=============
*/
extern vec3_t lightspot;

void GL_DrawAliasShadow(aliashdr_t *paliashdr, int posenum)
{
	trivertx_t *verts;
	int *order;
	vec3_t point;
	float height, lheight;
	int count;

	lheight = currententity->origin[2] - lightspot[2];

	height = 0;
	verts = (trivertx_t *)((byte *)paliashdr + paliashdr->posedata);
	verts += posenum * paliashdr->poseverts;
	order = (int *)((byte *)paliashdr + paliashdr->commands);

	height = -lheight + 1.0;

	while(1)
	{
		// get the vertex count and primitive type
		count = *order++;
		if(!count)
			break; // done
		if(count < 0)
		{
			count = -count;
			qglBegin(GL_TRIANGLE_FAN);
		}
		else
			qglBegin(GL_TRIANGLE_STRIP);

		do
		{
			// texture coordinates come from the draw list
			// (skipped for shadows) glTexCoord2fv ((float *)order);
			order += 2;

			// normals and vertexes come from the frame list
			point[0] = verts->v[0] * paliashdr->scale[0] + paliashdr->scale_origin[0];
			point[1] = verts->v[1] * paliashdr->scale[1] + paliashdr->scale_origin[1];
			point[2] = verts->v[2] * paliashdr->scale[2] + paliashdr->scale_origin[2];

			point[0] -= shadevector[0] * (point[2] + lheight);
			point[1] -= shadevector[1] * (point[2] + lheight);
			point[2] = height;
			//			height -= 0.001;
			qglVertex3fv(point);

			verts++;
		} while(--count);

		qglEnd();
	}
}

/*
=================
R_SetupAliasFrame

=================
*/
void R_SetupAliasFrame(int frame, aliashdr_t *paliashdr)
{
	int pose, numposes;
	float interval;

	if((frame >= paliashdr->numframes) || (frame < 0))
	{
		Con_DPrintf("R_AliasSetupFrame: no such frame %d\n", frame);
		frame = 0;
	}

	pose = paliashdr->frames[frame].firstpose;
	numposes = paliashdr->frames[frame].numposes;

	if(numposes > 1)
	{
		interval = paliashdr->frames[frame].interval;
		pose += (int)(cl.time / interval) % numposes;
	}

	GL_DrawAliasFrame(paliashdr, pose);
}

/*
=================
R_DrawAliasModel

=================
*/
void R_DrawAliasModel(cl_entity_t *e)
{
	int i;
	int lnum;
	vec3_t dist;
	float add;
	model_t *clmodel;
	vec3_t mins, maxs;
	aliashdr_t *paliashdr;
	float an;
	int anim;

	clmodel = currententity->model;

	VectorAdd(currententity->origin, clmodel->mins, mins);
	VectorAdd(currententity->origin, clmodel->maxs, maxs);

	if(R_CullBox(mins, maxs))
		return;

	VectorCopy(currententity->origin, r_entorigin);
	VectorSubtract(r_origin, r_entorigin, modelorg);

	//
	// get lighting information
	//

	ambientlight = shadelight = R_LightPoint(currententity->origin);

	// allways give the gun some light
	if(e == &cl.viewent && ambientlight < 24)
		ambientlight = shadelight = 24;

	for(lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		if(cl_dlights[lnum].die >= cl.time)
		{
			VectorSubtract(currententity->origin,
			               cl_dlights[lnum].origin,
			               dist);
			add = cl_dlights[lnum].radius - Length(dist);

			if(add > 0)
			{
				ambientlight += add;
				//ZOID models should be affected by dlights as well
				shadelight += add;
			}
		}
	}

	// clamp lighting so it doesn't overbright as much
	if(ambientlight > 128)
		ambientlight = 128;
	if(ambientlight + shadelight > 192)
		shadelight = 192 - ambientlight;

	// ZOID: never allow players to go totally black
// TODO
	i = currententity - cl_entities;
	if(i >= 1 && i <= cl.maxclients /* && !strcmp (currententity->model->name, "progs/player.mdl") */)
// TODO: qw
	//if (!strcmp(clmodel->name, "progs/player.mdl"))
	//{
//
		if(ambientlight < 8)
			ambientlight = shadelight = 8;

	//} // TODO: qw
	// HACK HACK HACK -- no fullbright colors, so make torches full light
	/*else*/ if(!strcmp(clmodel->name, "progs/flame2.mdl") || !strcmp(clmodel->name, "progs/flame.mdl"))
		ambientlight = shadelight = 256;

	shadedots = r_avertexnormal_dots[((int)(e->angles[1] * (SHADEDOT_QUANT / 360.0))) & (SHADEDOT_QUANT - 1)];
	shadelight = shadelight / 200.0;

	an = e->angles[1] / 180 * M_PI;
	shadevector[0] = cos(-an);
	shadevector[1] = sin(-an);
	shadevector[2] = 1;
	VectorNormalize(shadevector);

	//
	// locate the proper data
	//
	paliashdr = (aliashdr_t *)Mod_Extradata(currententity->model);

	c_alias_polys += paliashdr->numtris;

	//
	// draw all the triangles
	//

	GL_DisableMultitexture();

	qglPushMatrix();
	R_RotateForEntity(e);

	//if (!strcmp (clmodel->name, "progs/eyes.mdl") ) // TODO: qw
	if(!strcmp(clmodel->name, "progs/eyes.mdl") && gl_doubleeyes.value)
	{
		qglTranslatef(paliashdr->scale_origin[0], paliashdr->scale_origin[1], paliashdr->scale_origin[2] - (22 + 8));
		// double size of eyes, since they are really hard to see in gl
		qglScalef(paliashdr->scale[0] * 2, paliashdr->scale[1] * 2, paliashdr->scale[2] * 2);
	}
	else
	{
		qglTranslatef(paliashdr->scale_origin[0], paliashdr->scale_origin[1], paliashdr->scale_origin[2]);
		qglScalef(paliashdr->scale[0], paliashdr->scale[1], paliashdr->scale[2]);
	}

	anim = (int)(cl.time * 10) & 3;
	GL_Bind(paliashdr->gl_texturenum[currententity->skinnum][anim]);

	// we can't dynamically colormap textures, so they are cached
	// seperately for the players.  Heads are just uncolored.
	//if (currententity->scoreboard && !gl_nocolors.value) // TODO: qw
	if(currententity->colormap != vid.colormap && !gl_nocolors.value)
	{
		//i = currententity->scoreboard - cl.players; // TODO: qw
		i = currententity - cl_entities;

		// TODO: qw
		//if (!currententity->scoreboard->skin)
		{
			//Skin_Find(currententity->scoreboard);
			//R_TranslatePlayerSkin(i);
		}

		if(i >= 1 && i <= cl.maxclients /* && !strcmp (currententity->model->name, "progs/player.mdl") */) // TODO: if (i >= 0 && i< cl.maxclients) in qw
			GL_Bind(playertextures - 1 + i); // TODO: GL_Bind(playertextures + i); in qw
	}

	if(gl_smoothmodels.value)
		qglShadeModel(GL_SMOOTH);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(gl_affinemodels.value)
		qglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	R_SetupAliasFrame(currententity->frame, paliashdr);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	qglShadeModel(GL_FLAT);
	if(gl_affinemodels.value)
		qglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	qglPopMatrix();

	if(r_shadows.value)
	{
		qglPushMatrix();
		R_RotateForEntity(e);
		qglDisable(GL_TEXTURE_2D);
		qglEnable(GL_BLEND);
		qglColor4f(0, 0, 0, 0.5);
		GL_DrawAliasShadow(paliashdr, lastposenum);
		qglEnable(GL_TEXTURE_2D);
		qglDisable(GL_BLEND);
		qglColor4f(1, 1, 1, 1);
		qglPopMatrix();
	}
}

/*
=============================================================

  STUDIO MODELS

=============================================================
*/

/*
=================
R_DrawStudioModel
=================
*/
void R_DrawStudioModel(cl_entity_t *e)
{
	// TODO
};

//==================================================================================

/*
=============
R_DrawEntitiesOnList
=============
*/
void R_DrawEntitiesOnList()
{
	int i;

	if(!r_drawentities.value)
		return;

	// TODO: q2
/*
	// draw non-transparent first
	for (i=0 ; i<r_newrefdef.num_entities ; i++)
	{
		currententity = &r_newrefdef.entities[i];

		if (currententity->flags & RF_TRANSLUCENT)
			continue; // solid

		if ( currententity->flags & RF_BEAM )
			R_DrawBeam( currententity );
		else
		{
			currentmodel = currententity->model;

			if (!currentmodel)
			{
				R_DrawNullModel ();
				continue;
			};
			
			switch (currentmodel->type)
			{
			case mod_alias:
				R_DrawAliasModel (currententity);
				break;
			case mod_brush:
				R_DrawBrushModel (currententity);
				break;
			case mod_sprite:
				R_DrawSpriteModel (currententity);
				break;
			case mod_studio:
				R_DrawStudioModel(currententity);
				break;
			default:
				/*ri.Sys_Error (/*ERR_DROP, "Bad modeltype");
				break;
			};
		};
	};

	// draw transparent entities
	// we could sort these if it ever becomes a problem...
	qglDepthMask (0); // no z writes

	for (i=0 ; i<r_newrefdef.num_entities ; i++)
	{
		currententity = &r_newrefdef.entities[i];

		if (!(currententity->flags & RF_TRANSLUCENT))
			continue;	// solid

		if ( currententity->flags & RF_BEAM )
			R_DrawBeam( currententity );
		else
		{
			currentmodel = currententity->model;

			if (!currentmodel)
			{
				R_DrawNullModel ();
				continue;
			};
			
			switch (currentmodel->type)
			{
			case mod_alias:
				R_DrawAliasModel (currententity);
				break;
			case mod_brush:
				R_DrawBrushModel (currententity);
				break;
			case mod_sprite:
				R_DrawSpriteModel (currententity);
				break;
			case mod_studio:
				R_DrawStudioModel(currententity);
				break;
			default:
				/*ri.Sys_Error (/*ERR_DROP, "Bad modeltype");
				break;
			};
		};
	};

	qglDepthMask (1); // back to writing
*/

	// draw sprites seperately, because of alpha blending
	for(i = 0; i < cl_numvisedicts; i++)
	{
		currententity = cl_visedicts[i]; // TODO: = &cl_visedicts[i]; in qw

		switch(currententity->model->type)
		{
		case mod_alias:
			R_DrawAliasModel(currententity);
			break;

		case mod_brush:
			R_DrawBrushModel(currententity);
			break;

		case mod_studio:
			R_DrawStudioModel(currententity);

		default:
			break;
		}
	}

	for(i = 0; i < cl_numvisedicts; i++)
	{
		currententity = cl_visedicts[i]; // TODO: = &cl_visedicts[i]; in qw

		switch(currententity->model->type)
		{
		case mod_sprite:
			R_DrawSpriteModel(currententity);
			break;
		};
	};
};

/*
=============
R_DrawViewModel
=============
*/
void R_DrawViewModel(void)
{
	float ambient[4], diffuse[4];
	int j;
	int lnum;
	vec3_t dist;
	float add;
	dlight_t *dl;
	int ambientlight, shadelight;

	//if (!r_drawviewmodel.value || !Cam_DrawViewModel()) // TODO: qw
	if(!r_drawviewmodel.value)
		return;

	// TODO: not present in qw
	if(chase_active.value)
		return;

	if(envmap)
		return;

	if(!r_drawentities.value)
		return;

	//if (cl.stats[STAT_ITEMS] & IT_INVISIBILITY) // TODO: qw
	if(cl.items & IT_INVISIBILITY)
		return;

	if(cl.stats[STAT_HEALTH] <= 0)
		return;

	currententity = &cl.viewent;
	if(!currententity->model)
		return;

	j = R_LightPoint(currententity->origin);

	if(j < 24)
		j = 24; // allways give some light on gun
	ambientlight = j;
	shadelight = j;

	// add dynamic lights
	for(lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		dl = &cl_dlights[lnum];
		if(!dl->radius)
			continue;
		if(!dl->radius)
			continue;
		if(dl->die < cl.time)
			continue;

		VectorSubtract(currententity->origin, dl->origin, dist);
		add = dl->radius - Length(dist);
		if(add > 0)
			ambientlight += add;
	}

	ambient[0] = ambient[1] = ambient[2] = ambient[3] = (float)ambientlight / 128;
	diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] = (float)shadelight / 128;

	// hack the depth range to prevent view model from poking into walls
	qglDepthRange(gldepthmin, gldepthmin + 0.3 * (gldepthmax - gldepthmin));
	R_DrawAliasModel(currententity);
	qglDepthRange(gldepthmin, gldepthmax);
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend(void)
{
	if(!gl_polyblend.value)
		return;
	if(!v_blend[3])
		return;

	//Con_Printf("R_PolyBlend(): %4.2f %4.2f %4.2f %4.2f\n",v_blend[0], v_blend[1],	v_blend[2],	v_blend[3]);

	GL_DisableMultitexture();

	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_BLEND);
	qglDisable(GL_DEPTH_TEST);
	qglDisable(GL_TEXTURE_2D);

	qglLoadIdentity();

	qglRotatef(-90, 1, 0, 0); // put Z going up
	qglRotatef(90, 0, 0, 1);  // put Z going up

	qglColor4fv(v_blend);

	qglBegin(GL_QUADS);

	qglVertex3f(10, 100, 100);
	qglVertex3f(10, -100, 100);
	qglVertex3f(10, -100, -100);
	qglVertex3f(10, 100, -100);
	qglEnd();

	qglDisable(GL_BLEND);
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_ALPHA_TEST);
}

int SignbitsForPlane(mplane_t *out)
{
	int bits, j;

	// for fast box on planeside test

	bits = 0;
	for(j = 0; j < 3; j++)
	{
		if(out->normal[j] < 0)
			bits |= 1 << j;
	}
	return bits;
}

void R_SetFrustum(void)
{
	int i;

#if 0
	/*
	** this code is wrong, since it presume a 90 degree FOV both in the
	** horizontal and vertical plane
	*/
	if(r_refdef.fov_x == 90)
	{
		// front side is visible

		VectorAdd(vpn, vright, frustum[0].normal);
		VectorSubtract(vpn, vright, frustum[1].normal);

		VectorAdd(vpn, vup, frustum[2].normal);
		VectorSubtract(vpn, vup, frustum[3].normal);
		
		// we theoretically don't need to normalize these vectors, but I do it
		// anyway so that debugging is a little easier
		VectorNormalize( frustum[0].normal );
		VectorNormalize( frustum[1].normal );
		VectorNormalize( frustum[2].normal );
		VectorNormalize( frustum[3].normal );
	}
	else
#endif // 0
	{
		// rotate VPN right by FOV_X/2 degrees
		RotatePointAroundVector(frustum[0].normal, vup, vpn, -(90 - r_refdef.fov_x / 2));
		// rotate VPN left by FOV_X/2 degrees
		RotatePointAroundVector(frustum[1].normal, vup, vpn, 90 - r_refdef.fov_x / 2);
		// rotate VPN up by FOV_X/2 degrees
		RotatePointAroundVector(frustum[2].normal, vright, vpn, 90 - r_refdef.fov_y / 2);
		// rotate VPN down by FOV_X/2 degrees
		RotatePointAroundVector(frustum[3].normal, vright, vpn, -(90 - r_refdef.fov_y / 2));
	}

	for(i = 0; i < 4; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct(r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane(&frustum[i]);
	}
}

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame(void)
{
	// don't allow cheats in multiplayer
	if(cl.maxclients > 1)
		Cvar_Set("r_fullbright", "0");

// TODO: qw
/*
	r_fullbright.value = 0;
	r_lightmap.value = 0;
	if (!atoi(Info_ValueForKey(cl.serverinfo, "watervis")))
		r_wateralpha.value = 1;
*/

	R_AnimateLight();

	r_framecount++;

	// build the transformation matrix for the given view angles
	VectorCopy(r_refdef.vieworg, r_origin);

	AngleVectors(r_refdef.viewangles, vpn, vright, vup);

	// current viewleaf
	r_oldviewleaf = r_viewleaf;
	r_viewleaf = Mod_PointInLeaf(r_origin, cl.worldmodel);

	V_SetContentsColor(r_viewleaf->contents);
	V_CalcBlend();

	r_cache_thrash = false;

	c_brush_polys = 0;
	c_alias_polys = 0;
}

void MYgluPerspective(GLdouble fovy, GLdouble aspect,
                      GLdouble zNear, GLdouble zFar)
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	qglFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

/*
=============
R_SetupGL
=============
*/
void R_SetupGL(void)
{
	float screenaspect;
	extern int glwidth, glheight;
	int x, x2, y2, y, w, h;

	//
	// set up viewpoint
	//
	qglMatrixMode(GL_PROJECTION);
	qglLoadIdentity();
	x = r_refdef.vrect.x * glwidth / vid.width;
	x2 = (r_refdef.vrect.x + r_refdef.vrect.width) * glwidth / vid.width;
	y = (vid.height - r_refdef.vrect.y) * glheight / vid.height;
	y2 = (vid.height - (r_refdef.vrect.y + r_refdef.vrect.height)) * glheight / vid.height;

	// fudge around because of frac screen scale
	if(x > 0)
		x--;
	if(x2 < glwidth)
		x2++;
	if(y2 < 0)
		y2--;
	if(y < glheight)
		y++;

	w = x2 - x;
	h = y - y2;

	if(envmap)
	{
		x = y2 = 0;
		w = h = 256;
	}

	qglViewport(glx + x, gly + y2, w, h);
	screenaspect = (float)r_refdef.vrect.width / r_refdef.vrect.height;
	//	yfov = 2*atan((float)r_refdef.vrect.height/r_refdef.vrect.width)*180/M_PI;
	//	yfov = (2.0 * tan (scr_fov.value/360*M_PI)) / screenaspect;
	//	yfov = 2*atan((float)r_refdef.vrect.height/r_refdef.vrect.width)*(scr_fov.value*2)/M_PI;
	//    MYgluPerspective (yfov,  screenaspect,  4,  4096);
	MYgluPerspective(r_refdef.fov_y, screenaspect, 4, 4096);

	if(mirror)
	{
		if(mirror_plane->normal[2])
			qglScalef(1, -1, 1);
		else
			qglScalef(-1, 1, 1);
		qglCullFace(GL_BACK);
	}
	else
		qglCullFace(GL_FRONT);

	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();

	qglRotatef(-90, 1, 0, 0); // put Z going up
	qglRotatef(90, 0, 0, 1);  // put Z going up
	qglRotatef(-r_refdef.viewangles[2], 1, 0, 0);
	qglRotatef(-r_refdef.viewangles[0], 0, 1, 0);
	qglRotatef(-r_refdef.viewangles[1], 0, 0, 1);
	qglTranslatef(-r_refdef.vieworg[0], -r_refdef.vieworg[1], -r_refdef.vieworg[2]);

	qglGetFloatv(GL_MODELVIEW_MATRIX, r_world_matrix);

	//
	// set drawing parms
	//
	if(gl_cull.value)
		qglEnable(GL_CULL_FACE);
	else
		qglDisable(GL_CULL_FACE);

	qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_DEPTH_TEST);
}

/*
================
R_RenderScene

r_refdef must be set before the first call
================
*/
void R_RenderScene(void)
{
	R_SetupFrame();

	R_SetFrustum();

	R_SetupGL();

	R_MarkLeaves(); // done here so we know if we're in water

	R_DrawWorld(); // adds static entities to the list

	S_ExtraUpdate(); // don't let sound get messed up if going slow

	R_DrawEntitiesOnList();

	GL_DisableMultitexture();

	R_RenderDlights();

	R_DrawParticles();

#ifdef GLTEST
	Test_Draw();
#endif
}

/*
=============
R_Clear
=============
*/
void R_Clear(void)
{
	if(r_mirroralpha.value != 1.0)
	{
		if(gl_clear.value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);
		gldepthmin = 0;
		gldepthmax = 0.5;
		qglDepthFunc(GL_LEQUAL);
	}
	else if(gl_ztrick.value)
	{
		static int trickframe;

		if(gl_clear.value)
			qglClear(GL_COLOR_BUFFER_BIT);

		trickframe++;
		if(trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			qglDepthFunc(GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			qglDepthFunc(GL_GEQUAL);
		}
	}
	else
	{
		if(gl_clear.value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);
		gldepthmin = 0;
		gldepthmax = 1;
		qglDepthFunc(GL_LEQUAL);
	}

	qglDepthRange(gldepthmin, gldepthmax);
}

//#if 0 //!!! FIXME, Zoid, mirror is disabled for now
/*
=============
R_Mirror
=============
*/
void R_Mirror(void)
{
	float d;
	msurface_t *s;
	cl_entity_t *ent;

	if(!mirror)
		return;

	memcpy(r_base_world_matrix, r_world_matrix, sizeof(r_base_world_matrix));

	d = DotProduct(r_refdef.vieworg, mirror_plane->normal) - mirror_plane->dist;
	VectorMA(r_refdef.vieworg, -2 * d, mirror_plane->normal, r_refdef.vieworg);

	d = DotProduct(vpn, mirror_plane->normal);
	VectorMA(vpn, -2 * d, mirror_plane->normal, vpn);

	r_refdef.viewangles[0] = -asin(vpn[2]) / M_PI * 180;
	r_refdef.viewangles[1] = atan2(vpn[1], vpn[0]) / M_PI * 180;
	r_refdef.viewangles[2] = -r_refdef.viewangles[2];

	ent = &cl_entities[cl.viewentity];
	if(cl_numvisedicts < MAX_VISEDICTS)
	{
		cl_visedicts[cl_numvisedicts] = ent;
		cl_numvisedicts++;
	}

	gldepthmin = 0.5;
	gldepthmax = 1;
	qglDepthRange(gldepthmin, gldepthmax);
	qglDepthFunc(GL_LEQUAL);

	R_RenderScene();
	R_DrawWaterSurfaces();

	gldepthmin = 0;
	gldepthmax = 0.5;
	qglDepthRange(gldepthmin, gldepthmax);
	qglDepthFunc(GL_LEQUAL);

	// blend on top
	qglEnable(GL_BLEND);
	qglMatrixMode(GL_PROJECTION);
	if(mirror_plane->normal[2])
		qglScalef(1, -1, 1);
	else
		qglScalef(-1, 1, 1);
	qglCullFace(GL_FRONT);
	qglMatrixMode(GL_MODELVIEW);

	qglLoadMatrixf(r_base_world_matrix);

	qglColor4f(1, 1, 1, r_mirroralpha.value);
	s = cl.worldmodel->textures[mirrortexturenum]->texturechain;
	for(; s; s = s->texturechain)
		R_RenderBrushPoly(s);
	cl.worldmodel->textures[mirrortexturenum]->texturechain = NULL;
	qglDisable(GL_BLEND);
	qglColor4f(1, 1, 1, 1);
}
//#endif

/*
================
R_RenderView

r_refdef must be set before the first call
================
*/
void R_RenderView(void)
{
	double time1 = 0, time2;
	GLfloat colors[4] = { (GLfloat)0.0, (GLfloat)0.0, (GLfloat)1, (GLfloat)0.20 };

	if(r_norefresh.value)
		return;

	if(!r_worldentity.model || !cl.worldmodel)
		Sys_Error("R_RenderView: NULL worldmodel");

	if(r_speeds.value)
	{
		qglFinish();
		time1 = Sys_FloatTime();
		c_brush_polys = 0;
		c_alias_polys = 0;
	}

	mirror = false;

	if(gl_finish.value)
		qglFinish();

	R_Clear();

	// render normal view

	/***** Experimental silly looking fog ******
****** Use r_fullbright if you enable ******
	qglFogi(GL_FOG_MODE, GL_LINEAR);
	qglFogfv(GL_FOG_COLOR, colors);
	qglFogf(GL_FOG_END, 512.0);
	qglEnable(GL_FOG);
********************************************/

	R_RenderScene();
	R_DrawViewModel();
	R_DrawWaterSurfaces();

	// More fog right here :)
	//qglDisable(GL_FOG);
	// End of all fog code...

	// render mirror view
	R_Mirror(); // TODO: commented out in qw

	R_PolyBlend();

	if(r_speeds.value)
	{
		//		glFinish ();
		time2 = Sys_FloatTime();
		Con_Printf("%3i ms  %4i wpoly %4i epoly\n", (int)((time2 - time1) * 1000), c_brush_polys, c_alias_polys);
	}
}
