/*
 * This file is part of OGS Engine
 * Copyright (C) 1999-2005 Id Software, Inc.
 * Copyright (C) 2018-2019 BlackPhrase
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

#include <cassert>
#include <cmath>

class idVec3;
class angles_t;
class mat3_t;

class quat_t
{
public:
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
	float w{0.0f};

	inline quat_t() = default;

	inline quat_t(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};

	friend void toQuat(idVec3 &src, quat_t &dst);
	friend void toQuat(angles_t &src, quat_t &dst);
	friend void toQuat(mat3_t &src, quat_t &dst);

	inline float *vec4(){return &x;}

	inline float operator[](int index) const
	{
		assert((index >= 0) && (index < 4));
		return (&x)[index];
	};

	inline float &operator[](int index)
	{
		assert((index >= 0) && (index < 4));
		return (&x)[index];
	};

	inline void set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};

	inline void operator=(quat_t a)
	{
		x = a.x;
		y = a.y;
		z = a.z;
		w = a.w;
	};

	friend quat_t operator+(quat_t a, quat_t b);

	inline quat_t &operator+=(quat_t a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;

		return *this;
	};

	friend quat_t operator-(quat_t a, quat_t b);

	inline quat_t &operator-=(quat_t a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;

		return *this;
	};

	friend quat_t operator*(quat_t a, float b);
	friend quat_t operator*(float a, quat_t b);

	inline quat_t &operator*=(float a)
	{
		x *= a;
		y *= a;
		z *= a;
		w *= a;

		return *this;
	};

	friend int operator==(quat_t a, quat_t b);
	friend int operator!=(quat_t a, quat_t b);

	inline float Length()
	{
		float length = x * x + y * y + z * z + w * w;
		return (float)sqrt(length);
	};

	inline quat_t &Normalize()
	{
		float length = this->Length();
		if(length)
		{
			float ilength = 1 / length;
			
			x *= ilength;
			y *= ilength;
			z *= ilength;
			w *= ilength;
		};

		return *this;
	};

	inline quat_t operator-(){return quat_t(-x, -y, -z, -w);}
};

inline quat_t operator+(quat_t a, quat_t b)
{
	return quat_t(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
};

inline quat_t operator-(quat_t a, quat_t b)
{
	return quat_t(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
};

inline quat_t operator*(quat_t a, float b)
{
	return quat_t(a.x * b, a.y * b, a.z * b, a.w * b);
};

inline quat_t operator*(float a, quat_t b)
{
	return b * a;
};

inline int operator==(quat_t a, quat_t b)
{
	return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w));
};

inline int operator!=(quat_t a, quat_t b)
{
	return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z) && (a.w != b.w));
};