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

class Bounds
{
public:
	inline Bounds() = default;
	inline Bounds(const idVec3 &mins, const idVec3 &maxs)
	{
		b[0] = mins;
		b[1] = maxs;
	};

	inline void Clear()
	{
		b[0][0] = b[0][1] = b[0][2] = 99999;
		b[1][0] = b[1][1] = b[1][2] = -99999;
	};
	
	inline void Zero()
	{
		b[0][0] = b[0][1] = b[0][2] = 0;
		b[1][0] = b[1][1] = b[1][2] = 0;
	};
	
	/// radius from origin, not from center
	float GetRadius() const;
	inline idVec3 GetCenter(){return idVec3((b[1][0] + b[0][0]) * 0.5f, (b[1][1] + b[0][1]) * 0.5f, (b[1][2] + b[0][2]) * 0.5f);}
	
	void AddPoint(const idVec3 &v);
	void AddBounds(const Bounds &bb);
	
	inline bool IsCleared() const {return b[0][0] > b[1][0];}
	
	inline bool ContainsPoint(const idVec3 &p) const
	{
		if(p[0] < b[0][0] || p[1] < b[0][1] || p[2] < b[0][2] || p[0] > b[1][0] || p[1] > b[1][1] || p[2] > b[1][2])
			return false;
		return true;
	};
	
	// touching is NOT intersecting
	inline bool IntersectsBounds(const Bounds &b2) const
	{
		if(b2.b[1][0] < b[0][0] || b2.b[1][1] < b[0][1] || b2.b[1][2] < b[0][2] || b2.b[0][0] > b[1][0] || b2.b[0][1] > b[1][1] || b2.b[0][2] > b[1][2])
			return false;
		return true;
	};
	
	idVec3 b[2];
};

extern Bounds boundsZero;

inline void Bounds::AddPoint(const idVec3 &v)
{
	if(v[0] < b[0][0])
		b[0][0] = v[0];

	if(v[0] > b[1][0])
		b[1][0] = v[0];

	if(v[1] < b[0][1])
		b[0][1] = v[1];

	if(v[1] > b[1][1])
		b[1][1] = v[1];

	if(v[2] < b[0][2])
		b[0][2] = v[2];

	if(v[2] > b[1][2])
		b[1][2] = v[2];
};

inline void Bounds::AddBounds(const Bounds &bb)
{
	if(bb.b[0][0] < b[0][0])
		b[0][0] = bb.b[0][0];

	if(bb.b[0][1] < b[0][1])
		b[0][1] = bb.b[0][1];

	if(bb.b[0][2] < b[0][2])
		b[0][2] = bb.b[0][2];

	if(bb.b[1][0] > b[1][0])
		b[1][0] = bb.b[1][0];

	if(bb.b[1][1] > b[1][1])
		b[1][1] = bb.b[1][1];

	if(bb.b[1][2] > b[1][2])
		b[1][2] = bb.b[1][2];
};

inline float Bounds::GetRadius()
{
	float total{0.0f};
	float a, aa;

	for(int i = 0; i < 3; ++i)
	{
		a = (float)fabs(b[0][i]);
		aa = (float)fabs(b[1][i]);
		
		if(aa > a)
			a = aa;

		total += a * a;
	};

	return (float)idSqrt(total);
};