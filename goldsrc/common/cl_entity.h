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
/// @brief client-side render-related structures

#pragma once

#include "const.h"
#include "entity_state.h"

typedef struct efrag_s
{
	struct mleaf_s		*leaf;
	struct efrag_s		*leafnext;
	struct cl_entity_s	*entity;
	struct efrag_s		*entnext;
} efrag_t;

typedef struct position_history_s
{
	vec3_t origin; // TODO: was msg_origins of entity_s struct // last two updates (0 is newest)
	vec3_t angles; // TODO: was msg_angles of entity_s struct // last two updates (0 is newest)
} position_history_t;

typedef struct cl_entity_s
{
	qboolean				forcelink;		// model changed

	int						update_type;

	entity_state_t			baseline;		// to fill in defaults in updates

	double					msgtime;		// time of last update
	
	position_history_t ph[2];
	
	vec3_t					origin;
	vec3_t					angles;
	
	//int						keynum;			// for matching entities in different frames
	
	int						frame;
	byte					*colormap;
	int						effects;		// light, particals, etc
	int						skinnum;		// for Alias models
											
	int						dlightframe;	// dynamic lighting
	int						dlightbits;
	
	int						trivial_accept;
	
	struct model_s			*model;			// NULL = no model
	struct efrag_s			*efrag;			// linked list of efrags (FIXME)
	struct mnode_s			*topnode;		// for bmodels, first world node
											//  that splits bmodel, or NULL if
											//  not split

	float					syncbase;		// for client-side animations
	
	int						visframe;		// last frame this entity was
											// found in an active leaf
											// TODO: (only used for static objects?)
} cl_entity_t;