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

#include "quakedef.h"

#ifdef GLTEST

typedef struct
{
	plane_t *plane;
	vec3_t origin;
	vec3_t normal;
	vec3_t up;
	vec3_t right;
	vec3_t reflect;
	float length;
} puff_t;

#define MAX_PUFFS 64

puff_t puffs[MAX_PUFFS];

void Test_Init(void)
{
}

plane_t junk;
plane_t *HitPlane(vec3_t start, vec3_t end)
{
	trace_t trace;

	// fill in a default trace
	memset(&trace, 0, sizeof(trace_t));
	trace.fraction = 1;
	trace.allsolid = true;
	VectorCopy(end, trace.endpos);

	SV_RecursiveHullCheck(cl.worldmodel->hulls, 0, 0, 1, start, end, &trace);

	junk = trace.plane;
	return &junk;
}

void Test_Spawn(vec3_t origin)
{
	int i;
	puff_t *p;
	vec3_t temp;
	vec3_t normal;
	vec3_t incoming;
	plane_t *plane;
	float d;

	for(i = 0, p = puffs; i < MAX_PUFFS; i++, p++)
	{
		if(p->length <= 0)
			break;
	}
	if(i == MAX_PUFFS)
		return;

	VectorSubtract(r_refdef.vieworg, origin, incoming);
	VectorSubtract(origin, incoming, temp);
	plane = HitPlane(r_refdef.vieworg, temp);

	VectorNormalize(incoming);
	d = DotProduct(incoming, plane->normal);
	VectorSubtract(vec3_origin, incoming, p->reflect);
	VectorMA(p->reflect, d * 2, plane->normal, p->reflect);

	VectorCopy(origin, p->origin);
	VectorCopy(plane->normal, p->normal);

	CrossProduct(incoming, p->normal, p->up);

	CrossProduct(p->up, p->normal, p->right);

	p->length = 8;
}

void DrawPuff(puff_t *p)
{
	vec3_t pts[2][3];
	int i, j;
	float s, d;

	for(i = 0; i < 2; i++)
	{
		if(i == 1)
		{
			s = 6;
			d = p->length;
		}
		else
		{
			s = 2;
			d = 0;
		}

		for(j = 0; j < 3; j++)
		{
			pts[i][0][j] = p->origin[j] + p->up[j] * s + p->reflect[j] * d;
			pts[i][1][j] = p->origin[j] + p->right[j] * s + p->reflect[j] * d;
			pts[i][2][j] = p->origin[j] + -p->right[j] * s + p->reflect[j] * d;
		}
	}

	qglColor3f(1, 0, 0);

#if 0
	qglBegin (GL_LINES);
	qglVertex3fv (p->origin);
	qglVertex3f (p->origin[0] + p->length*p->reflect[0],
		p->origin[1] + p->length*p->reflect[1],
		p->origin[2] + p->length*p->reflect[2]);

	qglVertex3fv (pts[0][0]);
	qglVertex3fv (pts[1][0]);

	qglVertex3fv (pts[0][1]);
	qglVertex3fv (pts[1][1]);

	qglVertex3fv (pts[0][2]);
	qglVertex3fv (pts[1][2]);

	qglEnd ();
#endif

	qglBegin(GL_QUADS);
	for(i = 0; i < 3; i++)
	{
		j = (i + 1) % 3;
		qglVertex3fv(pts[0][j]);
		qglVertex3fv(pts[1][j]);
		qglVertex3fv(pts[1][i]);
		qglVertex3fv(pts[0][i]);
	}
	qglEnd();

	qglBegin(GL_TRIANGLES);
	qglVertex3fv(pts[1][0]);
	qglVertex3fv(pts[1][1]);
	qglVertex3fv(pts[1][2]);
	qglEnd();

	p->length -= host_frametime * 2;
}

void Test_Draw(void)
{
	int i;
	puff_t *p;

	for(i = 0, p = puffs; i < MAX_PUFFS; i++, p++)
	{
		if(p->length > 0)
			DrawPuff(p);
	}
}

#endif
