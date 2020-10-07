/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

extern cvar_t v_gamma;

extern byte gammatable[256]; // palette is sent through this
extern byte ramps[3][256];

//#ifdef GLQUAKE // TODO: qw
extern float v_blend[4];
//#endif // TODO: qw

extern cvar_t lcd_x;

void V_Init();
void V_RenderView();
float V_CalcRoll(vec3_t angles, vec3_t velocity);
void V_UpdatePalette();
void V_CalcShake(); // TODO
void V_ApplyShake(float *avOrigin, float *avAngles, float afFactor); // TODO