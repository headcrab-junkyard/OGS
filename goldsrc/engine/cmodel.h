/*
 * This file is part of OGS Engine
 * Copyright (C) 1997-2001 Id Software, Inc.
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

/*
==============================================================

CMODEL

==============================================================
*/

#include "../qcommon/qfiles.h"

cmodel_t *CM_LoadMap(const char *name, qboolean clientload, unsigned *checksum);
cmodel_t *CM_InlineModel(const char *name); // *1, *2, etc

int CM_NumClusters();
int CM_NumInlineModels();
char *CM_EntityString();

// creates a clipping hull for an arbitrary box
int CM_HeadnodeForBox(vec3_t mins, vec3_t maxs);

// returns an ORed contents mask
int CM_PointContents(vec3_t p, int headnode);
int CM_TransformedPointContents(vec3_t p, int headnode, vec3_t origin, vec3_t angles);

trace_t CM_BoxTrace(vec3_t start, vec3_t end,
                    vec3_t mins, vec3_t maxs,
                    int headnode, int brushmask);
trace_t CM_TransformedBoxTrace(vec3_t start, vec3_t end,
                               vec3_t mins, vec3_t maxs,
                               int headnode, int brushmask,
                               vec3_t origin, vec3_t angles);

byte *CM_ClusterPVS(int cluster);
byte *CM_ClusterPHS(int cluster);

int CM_PointLeafnum(vec3_t p);

// call with topnode set to the headnode, returns with topnode
// set to the first node that splits the box
int CM_BoxLeafnums(vec3_t mins, vec3_t maxs, int *list,
                   int listsize, int *topnode);

int CM_LeafContents(int leafnum);
int CM_LeafCluster(int leafnum);
int CM_LeafArea(int leafnum);

void CM_SetAreaPortalState(int portalnum, qboolean open);
qboolean CM_AreasConnected(int area1, int area2);

int CM_WriteAreaBits(byte *buffer, int area);
qboolean CM_HeadnodeVisible(int headnode, byte *visbits);

void CM_WritePortalState(FILE *f);
void CM_ReadPortalState(FILE *f);