/*
 * This file is part of OGS Engine
 * Copyright (C) 1999-2005 Id Software, Inc.
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

#include "vec3.h"

class idVec5 : public idVec3
{
public:
	float s{0.0f};
	float t{0.0f};

	inline float operator[](int index) const {return (&x)[index];}
	inline float &operator[](int index) {return (&x)[index];}
};