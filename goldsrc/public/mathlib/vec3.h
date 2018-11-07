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

//class idVec3 : public idHeap<idVec3>
class idVec3
{
public:
#ifndef FAT_VEC3
	float x, y, z;
#else
	float x, y, z, dist{0.0f};
#endif

	idVec3() = default;

	inline idVec3(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	};

	inline operator float *(){return &x;}

	inline float operator[](const int index) const {return (&x)[index];}
	inline float &operator[](const int index){return (&x)[index];}

	inline void set(const float x, const float y, const float z);
	{
		this->x = x;
		this->y = y;
		this->z = z;
	};

	inline idVec3 operator-() const {return idVec3(-x, -y, -z);}

	inline idVec3 &operator=(const idVec3 &a)
	{
		x = a.x;
		y = a.y;
		z = a.z;

		return *this;
	};

	inline float operator*(const idVec3 &a) const {return x * a.x + y * a.y + z * a.z;}
	inline idVec3 operator*(const float a) const {return idVec3(x * a, y * a, z * a);}

	friend idVec3 operator*(float a, idVec3 b);

	inline idVec3 operator+(const idVec3 &a) const {return idVec3(x + a.x, y + a.y, z + a.z);}
	inline idVec3 operator-(const idVec3 &a) const {return idVec3(x - a.x, y - a.y, z - a.z);}

	inline idVec3 &operator+=(const idVec3 &a)
	{
		x += a.x;
		y += a.y;
		z += a.z;

		return *this;
	};

	inline idVec3 &operator-=(const idVec3 &a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;

		return *this;
	};

	inline idVec3 &operator*=(const float a)
	{
		x *= a;
		y *= a;
		z *= a;

		return *this;
	};

	inline int operator==(const idVec3 &a) const
	{
		if(Q_fabs(x - a.x) > EQUAL_EPSILON || Q_fabs(y - a.y) > EQUAL_EPSILON || Q_fabs(z - a.z) > EQUAL_EPSILON)
			return false;
		return true;
	};

	inline int operator!=(const idVec3 &a) const
	{
		if(Q_fabs(x - a.x) > EQUAL_EPSILON || Q_fabs(y - a.y) > EQUAL_EPSILON || Q_fabs(z - a.z) > EQUAL_EPSILON)
			return true;
		return false;
	};

	inline idVec3 Cross(const idVec3 &a) const {return idVec3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);}

	inline idVec3 &Cross(const idVec3 &a, const idVec3 &b)
	{
		x = a.y * b.z - a.z * b.y;
		y = a.z * b.x - a.x * b.z;
		z = a.x * b.y - a.y * b.x;

		return *this;
	};

	inline float Length() const
	{
		float length{x * x + y * y + z * z};
		return (float)idSqrt(length);
	};
	
	inline float Normalize()
	{
		float length{this->Length()};
		if(length)
		{
			float ilength{1.0f / length};

			x *= ilength;
			y *= ilength;
			z *= ilength;
		};

		return length;
	};

	inline void Zero(){x = y = z = 0.0f;}

	inline void Snap()
	{
		x = float(int(x));
		y = float(int(y));
		z = float(int(z));
	};

	/*
	======================
	SnapTowards

	Round a vector to integers for more efficient network
	transmission, but make sure that it rounds towards a given point
	rather than blindly truncating.  This prevents it from truncating 
	into a wall.
	======================
	*/
	inline void SnapTowards(const idVec3 &to)
	{
		x = float(int(x) + 1);
		y = float(int(y) + 1);
		z = float(int(z) + 1);

		if(to.x <= x)
			x = float(int(x));

		if(to.y <= y)
			y = float(int(y));

		if(to.z <= z)
			z = float(int(z));
	};

	float toYaw();
	float toPitch();
	angles_t toAngles();
	friend idVec3 LerpVector(const idVec3 &w1, const idVec3 &w2, const float t);

	char *string();
};

extern idVec3 vec_zero;

inline idVec3 operator*(float a, idVec3 b)
{
	return idVec3(b.x * a, b.y * a, b.z * a);
};