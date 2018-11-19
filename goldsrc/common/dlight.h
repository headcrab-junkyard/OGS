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

#pragma once

#include "const.h"

typedef struct dlight_s
{
	vec3_t	origin;
	float	radius;
	
	color24 color;
	
	float	die;				///< stop lighting after this time
	float	decay;				///< drop this each second
	
	float	minlight;			///< don't add when contributing less
	int		key;				///< so entities can reuse same entry
	
	qboolean	dark;			///< subtracts light instead of adding
} dlight_t;