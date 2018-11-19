/*
 *	This file is part of OGS Engine
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
/// @brief triangle api implementation

#include "quakedef.h"
#include "triangleapi.h"

void Tri_RenderMode(int nMode)
{
#ifdef GLQUAKE
#endif
};

void Tri_Begin(int nPrimitiveCode)
{
#ifdef GLQUAKE
	qglBegin(nPrimitiveCode);
#endif
};

void Tri_End()
{
#ifdef GLQUAKE
	qglEnd();
#endif
};

void Tri_Color4f(float r, float g, float b, float a)
{
#ifdef GLQUAKE
	qglColor4f(r, g, b, a);
#endif
};

void Tri_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
#ifdef GLQUAKE
	qglColor4ub(r, g, b, a);
#endif
};

void Tri_TexCoord2f(float u, float v)
{
#ifdef GLQUAKE
	qglTexCoord2f(u, v);
#endif
};

void Tri_Vertex3fv(float *vWorldPoint)
{
#ifdef GLQUAKE
	qglVertex3fv(vWorldPoint);
#endif
};

void Tri_Vertex3f(float x, float y, float z)
{
#ifdef GLQUAKE
	qglVertex3f(x, y, z);
#endif
};

void Tri_Brightness(float fBrightness)
{
	// TODO
};

void Tri_CullFace(TRICULLSTYLE eStyle)
{
#ifdef GLQUAKE
	qglCullFace(eStyle);
#endif
};

int Tri_SpriteTexture(struct model_s *pModel, int nFrame)
{
	// TODO
	return 0;
};

int Tri_WorldToScreen(float *vWorldCoord, float *vScreenCoord)
{
	// TODO
	return 0;
};

void Tri_Fog(float fColor[3], float fStart, float fEnd, int nEnable)
{
#ifdef GLQUAKE
	GLfloat vFullColor[4];
	
	vFullColor[0] = fColor[0];
	vFullColor[1] = fColor[1];
	vFullColor[2] = fColor[2];
	vFullColor[3] = 1.0f;
	
	qglFogf(GL_FOG_COLOR, vFullColor[0]);
	qglFogf(GL_FOG_START, fStart);
	qglFogf(GL_FOG_END, fEnd);
	
	if(nEnable)
		qglEnable(GL_FOG);
	else
		qglDisable(GL_FOG);
#endif
};

void Tri_ScreenToWorld(float *vScreenCoord, float *vWorldCoord)
{
	// TODO
};

void Tri_GetMatrix(const int nName, float *vMatrix)
{
	// TODO
};

int Tri_BoxInPVS(float *vMins, float *vMaxs)
{
	// TODO
	return 0;
};

void Tri_LightAtPoint(float *vPos, float *vValue)
{
	// TODO
};

void Tri_Color4fRendermode(float r, float g, float b, float a, int nRenderMode)
{
#ifdef GLQUAKE
	Tri_RenderMode(nRenderMode);
	Tri_Color4f(r, g, b, a);
#endif
};

void Tri_FogParams(float fDensity, int nFogSkyBox)
{
#ifdef GLQUAKE
	qglFogf(GL_FOG_DENSITY, fDensity);
	// TODO: nFogSkyBox
#endif
};

triangleapi_t gTriAPI =
{
	TRI_API_VERSION,
	
	Tri_RenderMode,
	
	Tri_Begin,
	Tri_End,
	
	Tri_Color4f,
	Tri_Color4ub,
	
	Tri_TexCoord2f,
	
	Tri_Vertex3fv,
	Tri_Vertex3f,
	
	Tri_Brightness,
	
	Tri_CullFace,
	
	Tri_SpriteTexture,
	
	Tri_WorldToScreen,
	
	Tri_Fog,
	
	Tri_ScreenToWorld,
	
	Tri_GetMatrix,
	
	Tri_BoxInPVS,
	
	Tri_LightAtPoint,
	
	Tri_Color4fRendermode,
	
	Tri_FogParams
};