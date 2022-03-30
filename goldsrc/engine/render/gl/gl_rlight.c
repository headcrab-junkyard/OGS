/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019, 2021 BlackPhrase
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

int r_dlightframecount;

/*
==================
R_AnimateLight
==================
*/
void R_AnimateLight(void)
{
	int i, j, k;

	//
	// light animations
	// 'm' is normal light, 'a' is no light, 'z' is double bright
	i = (int)(cl.time * 10);
	for(j = 0; j < MAX_LIGHTSTYLES; j++)
	{
		if(!cl_lightstyle[j].length)
		{
			d_lightstylevalue[j] = 256;
			continue;
		}
		k = i % cl_lightstyle[j].length;
		k = cl_lightstyle[j].map[k] - 'a';
		k = k * 22;
		d_lightstylevalue[j] = k;
	}
}

/*
=============================================================================

DYNAMIC LIGHTS BLEND RENDERING

=============================================================================
*/

void AddLightBlend(float r, float g, float b, float a2)
{
	float a;

	v_blend[3] = a = v_blend[3] + a2 * (1 - v_blend[3]);

	a2 = a2 / a;

	v_blend[0] = v_blend[1] * (1 - a2) + r * a2;
	v_blend[1] = v_blend[1] * (1 - a2) + g * a2;
	v_blend[2] = v_blend[2] * (1 - a2) + b * a2;

	//Con_Printf("AddLightBlend(): %4.2f %4.2f %4.2f %4.6f\n", v_blend[0], v_blend[1], v_blend[2], v_blend[3]);
}

float bubble_sintable[17], bubble_costable[17];

void R_InitBubble()
{
	float a;
	int i;
	float *bub_sin, *bub_cos;

	bub_sin = bubble_sintable;
	bub_cos = bubble_costable;

	for (i=16 ; i>=0 ; i--)
	{
		a = i/16.0 * M_PI*2;
		*bub_sin++ = sin(a);
		*bub_cos++ = cos(a);
	}
}

void R_RenderDlight(dlight_t *light)
{
	int i, j;
	float a;
	vec3_t v;
	float rad;
	//float	*bub_sin, *bub_cos; // TODO

	//bub_sin = bubble_sintable; // TODO
	//bub_cos = bubble_costable; // TODO

	rad = light->radius * 0.35;

	VectorSubtract(light->origin, r_origin, v);
	
	if(Length(v) < rad)
	{
		// view is inside the dlight
		AddLightBlend(1, 0.5, 0, light->radius * 0.0003);
		return;
	};

	qglBegin(GL_TRIANGLE_FAN);
	qglColor3f(0.2, 0.1, 0.0); // TODO: unused in qw, qglColor3f (light->color[0]*0.2, light->color[1]*0.2, light->color[2]*0.2); in q2
	//qglColor3f (0.2,0.1,0.05); // changed dimlight effect
	//qglColor4f (light->color[0], light->color[1], light->color[2], light->color[3]); // TODO: qw
	
	for(i = 0; i < 3; i++)
		v[i] = light->origin[i] - vpn[i] * rad;
	
	qglVertex3fv(v);
	qglColor3f(0, 0, 0);
	
	for(i = 16; i >= 0; i--)
	{
		a = i / 16.0 * M_PI * 2; // TODO: unused in qw

		for(j = 0; j < 3; j++)
			v[j] = light->origin[j] + vright[j] * cos(a) * rad + vup[j] * sin(a) * rad;
			// TODO: qw
			//v[j] = light->origin[j] + (vright[j]*(*bub_cos) +
				//+ vup[j]*(*bub_sin)) * rad;
		
		//bub_sin++; // TODO
		//bub_cos++; // TODO

		qglVertex3fv(v);
	}
	qglEnd();
}

/*
=============
R_RenderDlights
=============
*/
void R_RenderDlights(void)
{
	int i;
	dlight_t *l;

	if(!gl_flashblend.value) // TODO: if(!gl_flashblend->value) in q2
		return;

	r_dlightframecount = r_framecount + 1; // because the count hasn't
	                                       //  advanced yet for this frame
	qglDepthMask(0);
	qglDisable(GL_TEXTURE_2D);
	qglShadeModel(GL_SMOOTH);
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_ONE, GL_ONE);

	l = cl_dlights; // TODO: = r_newrefdef.dlights in q2
	for(i = 0; i < MAX_DLIGHTS; i++, l++) // TODO: for (i=0 ; i<r_newrefdef.num_dlights ; i++, l++) in q2
	{
		if(l->die < cl.time || !l->radius) // TODO: not present in q2
			continue; // TODO: not present in q2
		R_RenderDlight(l);
	}

	qglColor3f(1, 1, 1);
	qglDisable(GL_BLEND);
	qglEnable(GL_TEXTURE_2D);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDepthMask(1);
}

/*
=============================================================================

DYNAMIC LIGHTS

=============================================================================
*/

/*
=============
R_MarkLights
=============
*/
void R_MarkLights(dlight_t *light, int bit, mnode_t *node)
{
	mplane_t *splitplane;
	float dist;
	msurface_t *surf;
	int i;

	if(node->contents < 0) // TODO: if(node->contents != -1) in q2
		return;

	splitplane = node->plane;
	dist = DotProduct(light->origin, splitplane->normal) - splitplane->dist;

	if(dist > light->radius)
	//if (dist > light->intensity-DLIGHT_CUTOFF) // TODO: q2
	{
		R_MarkLights(light, bit, node->children[0]);
		return;
	}
	if(dist < -light->radius)
	//if (dist < -light->intensity+DLIGHT_CUTOFF) // TODO: q2
	{
		R_MarkLights(light, bit, node->children[1]);
		return;
	}

	// mark the polygons
	surf = cl.worldmodel->surfaces + node->firstsurface;
	//surf = r_worldmodel->surfaces + node->firstsurface; // TODO: q2
	for(i = 0; i < node->numsurfaces; i++, surf++)
	{
		if(surf->dlightframe != r_dlightframecount)
		{
			surf->dlightbits = 0;
			surf->dlightframe = r_dlightframecount;
		}
		surf->dlightbits |= bit;
	}

	R_MarkLights(light, bit, node->children[0]);
	R_MarkLights(light, bit, node->children[1]);
}

/*
=============
R_PushDlights
=============
*/
void R_PushDlights(void)
{
	int i;
	dlight_t *l;

	if(gl_flashblend.value) // TODO: if(gl_flashblend->value) in q2
		return;

	r_dlightframecount = r_framecount + 1; // because the count hasn't
	                                       //  advanced yet for this frame
	l = cl_dlights; // TODO: = r_newrefdef.dlights in q2

	for(i = 0; i < MAX_DLIGHTS; i++, l++) // TODO: for (i=0 ; i<r_newrefdef.num_dlights ; i++, l++) in q2
	{
		if(l->die < cl.time || !l->radius) // TODO: not present in q2
			continue; // TODO: not present in q2
		R_MarkLights(l, 1 << i, cl.worldmodel->nodes); // TODO: cl.worldmodel -> r_worldmodel
	}
}

/*
=============================================================================

LIGHT SAMPLING

=============================================================================
*/

//vec3_t pointcolor; // TODO: q2
mplane_t *lightplane; // used as shadow plane
vec3_t lightspot;

int RecursiveLightPoint(mnode_t *node, vec3_t start, vec3_t end)
{
	int r;
	float front, back, frac;
	int side;
	mplane_t *plane;
	vec3_t mid;
	msurface_t *surf;
	int s, t, ds, dt;
	int i;
	mtexinfo_t *tex;
	byte *lightmap;
	unsigned scale; // TODO: not present in q2
	int maps;

	if(node->contents < 0) // TODO: if(node->contents != -1) in q2
		return -1; // didn't hit anything

	// calculate mid point

	// FIXME: optimize for axial
	plane = node->plane;
	front = DotProduct(start, plane->normal) - plane->dist;
	back = DotProduct(end, plane->normal) - plane->dist;
	side = front < 0;

	if((back < 0) == side)
		return RecursiveLightPoint(node->children[side], start, end);

	frac = front / (front - back);
	mid[0] = start[0] + (end[0] - start[0]) * frac;
	mid[1] = start[1] + (end[1] - start[1]) * frac;
	mid[2] = start[2] + (end[2] - start[2]) * frac;

	// go down front side
	r = RecursiveLightPoint(node->children[side], start, mid);
	if(r >= 0)
		return r; // hit something

	if((back < 0) == side)
		return -1; // didn't hit anuthing

	// check for impact on this node
	VectorCopy(mid, lightspot);
	lightplane = plane;

	surf = cl.worldmodel->surfaces + node->firstsurface;
	for(i = 0; i < node->numsurfaces; i++, surf++)
	{
		if(surf->flags & SURF_DRAWTILED)
			continue; // no lightmaps

		tex = surf->texinfo;

		s = DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3];
		t = DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3];

		if(s < surf->texturemins[0] || t < surf->texturemins[1])
			continue;

		ds = s - surf->texturemins[0];
		dt = t - surf->texturemins[1];

		if(ds > surf->extents[0] || dt > surf->extents[1])
			continue;

		if(!surf->samples)
			return 0;

		ds >>= 4;
		dt >>= 4;

		lightmap = surf->samples;
		r = 0;
		if(lightmap)
		{
			lightmap += dt * ((surf->extents[0] >> 4) + 1) + ds;

			for(maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255;
			    maps++)
			{
				scale = d_lightstylevalue[surf->styles[maps]];
				r += *lightmap * scale;
				lightmap += ((surf->extents[0] >> 4) + 1) *
				((surf->extents[1] >> 4) + 1);
			}

			r >>= 8;
		}

		return r;
	}

	// go down back side
	return RecursiveLightPoint(node->children[!side], mid, end);
}

/*
===============
R_LightPoint
===============
*/
int R_LightPoint(vec3_t p)
//void R_LightPoint (vec3_t p, vec3_t color) // TODO: q2
{
	vec3_t end;
	int r; // TODO: float in q2
	// TODO: q2
	/*
	int			lnum;
	dlight_t	*dl;
	vec3_t		dist;
	float		add;
	*/

	if(!cl.worldmodel->lightdata) // TODO: r_worldmodel in q2
		return 255;
	// TODO: q2
	/*
	{
		color[0] = color[1] = color[2] = 1.0;
		return;
	};
	*/

	end[0] = p[0];
	end[1] = p[1];
	end[2] = p[2] - 2048;

	r = RecursiveLightPoint(cl.worldmodel->nodes, p, end); // TODO: r_worldmodel in q2

	if(r == -1)
		r = 0;
	// TODO: q2
	/*
		VectorCopy (vec3_origin, color);
	else
		VectorCopy (pointcolor, color);
	*/

	// TODO: q2
	/*
	//
	// add dynamic lights
	//
	dl = r_newrefdef.dlights;
	for (lnum=0 ; lnum<r_newrefdef.num_dlights ; lnum++, dl++)
	{
		VectorSubtract (currententity->origin, dl->origin, dist);
		add = dl->intensity - VectorLength(dist);
		add *= (1.0/256);
		if (add > 0)
			VectorMA (color, add, dl->color, color);
	};

	VectorScale (color, gl_modulate->value, color);
	*/
	
	return r;
};