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

#include "mathlib.h"

/// entity_state_t is the information conveyed from the server
/// in an update message
typedef struct entity_state_s
{
	int		number;			///< edict index
	
	vec3_t	origin;
	vec3_t	angles;
	
	int		modelindex;
	int		frame;
	int		colormap;
	int		skin;
	int		effects;
	
	int		eflags;			///< nolerp, etc
} entity_state_t;