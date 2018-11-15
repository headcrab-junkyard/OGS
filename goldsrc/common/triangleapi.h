/*
 * This file is part of OGS Engine
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
/// @brief triangle (direct renderer access) api

#pragma once

const int TRI_API_VERSION = 1;

typedef enum
{
	TRI_FRONT = 0,
	TRI_NONE
} TRICULLSTYLE;

typedef enum
{
	TRI_TRIANGLES = 0,
	TRI_TRIANGLE_FAN,
	TRI_QUADS,
	TRI_POLYGON,
	TRI_LINES,
	TRI_TRIANGLE_STRIP,
	TRI_QUAD_STRIP
} eRenderMode;

typedef struct triangleapi_s
{
	///
	int version;
	
	///
	void (*RenderMode)(int nMode);
	
	///
	void (*Begin)(int nPrimitiveCode);
	
	///
	void (*End)();
	
	///
	void (*Color4f)(float r, float g, float b, float a);
	
	///
	void (*Color4ub)(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	///
	void (*TexCoord2f)(float u, float v);
	
	///
	void (*Vertex3fv)(float *vWorldPoint);
	
	///
	void (*Vertex3f)(float x, float y, float z);
	
	///
	void (*Brightness)(float fBrightness);
	
	///
	void (*CullFace)(TRICULLSTYLE eStyle);
	
	///
	int (*SpriteTexture)(struct model_s *pModel, int nFrame);
	
	///
	int (*WorldToScreen)(float *vWorldCoord, float *vScreenCoord);
	
	///
	void (*Fog)(float fColor[3], float fStart, float fEnd, int nEnable);
	
	///
	void (*ScreenToWorld)(float *vScreenCoord, float *vWorldCoord);
	
	///
	void (*GetMatrix)(const int nName, float *vMatrix);
	
	///
	int (*BoxInPVS)(float *vMins, float *vMaxs);
	
	///
	void (*LightAtPoint)(float *vPos, float *vValue);
	
	///
	void (*Color4fRendermode)(float r, float g, float b, float a, int nRenderMode);
	
	///
	void (*FogParams)(float fDensity, int nFogSkyBox);
} triangleapi_t;