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

#include <cstdlib>
#include <cassert>

#include "vector.h"

class mat3_t;
class quat_t;

class idVec3;
typedef idVec3 &vec3_p;

class angles_t
{
public:
	float pitch{0.0f};
	float yaw{0.0f};
	float roll{0.0f};

	inline angles_t() = default;

	inline angles_t(float pitch, float yaw, float roll)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
	};

	inline angles_t(const idVec3 &vec)
	{
		this->pitch = vec.x;
		this->yaw = vec.y;
		this->roll = vec.z;
	};

	friend void toAngles(idVec3 &src, angles_t &dst);
	friend void toAngles(quat_t &src, angles_t &dst);
	friend void toAngles(mat3_t &src, angles_t &dst);

	inline operator vec3_p(){return *(idVec3 *)&pitch;}

	inline float operator[](int index) const
	{
		assert((index >= 0) && (index < 3));
		return (&pitch)[index];
	};

	inline float &operator[](int index)
	{
		assert((index >= 0) && (index < 3));
		return (&pitch)[index];
	};

	inline void set(float pitch, float yaw, float roll)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
	};

	inline void operator=(angles_t const &a)
	{
		pitch = a.pitch;
		yaw = a.yaw;
		roll = a.roll;
	};

	inline void operator=(idVec3 const &a)
	{
		pitch = a[0];
		yaw = a[1];
		roll = a[2];
	};

	friend angles_t operator+(const angles_t &a, const angles_t &b);

	inline angles_t &operator+=(angles_t const &a)
	{
		pitch += a.pitch;
		yaw += a.yaw;
		roll += a.roll;

		return *this;
	};

	inline angles_t &operator+=(idVec3 const &a)
	{
		pitch += a.x;
		yaw += a.y;
		roll += a.z;

		return *this;
	};

	friend angles_t operator-(angles_t &a, angles_t &b);

	inline angles_t &operator-=(angles_t &a)
	{
		pitch -= a.pitch;
		yaw -= a.yaw;
		roll -= a.roll;

		return *this;
	};

	friend angles_t operator*(const angles_t &a, float b);
	friend angles_t operator*(float a, const angles_t &b);

	inline angles_t &operator*=(float a)
	{
		pitch *= a;
		yaw *= a;
		roll *= a;

		return *this;
	};

	friend int operator==(angles_t &a, angles_t &b);

	friend int operator!=(angles_t &a, angles_t &b);

	void toVectors(idVec3 *forward, idVec3 *right = nullptr, idVec3 *up = nullptr);
	idVec3 toForward();

	inline angles_t &Zero()
	{
		pitch = 0.0f;
		yaw = 0.0f;
		roll = 0.0f;

		return *this;
	};

	angles_t &Normalize360();
	angles_t &Normalize180();
};

extern angles_t ang_zero;

inline angles_t operator+(const angles_t &a, const angles_t &b)
{
	return angles_t(a.pitch + b.pitch, a.yaw + b.yaw, a.roll + b.roll);
};

inline angles_t operator-(angles_t &a, angles_t &b)
{
	return angles_t(a.pitch - b.pitch, a.yaw - b.yaw, a.roll - b.roll);
};

inline angles_t operator*(const angles_t &a, float b)
{
	return angles_t(a.pitch * b, a.yaw * b, a.roll * b);
};

inline angles_t operator*(float a, const angles_t &b)
{
	return angles_t(a * b.pitch, a * b.yaw, a * b.roll);
};

inline int operator==(angles_t &a, angles_t &b)
{
	return ((a.pitch == b.pitch) && (a.yaw == b.yaw) && (a.roll == b.roll));
};

inline int operator!=(angles_t &a, angles_t &b)
{
	return ((a.pitch != b.pitch) || (a.yaw != b.yaw) || (a.roll != b.roll));
};