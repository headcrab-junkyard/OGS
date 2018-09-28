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

#include "quakedef.h"
#include "EngineSurface.h"

EXPOSE_SINGLE_INTERFACE(CEngineSurface, IEngineSurface, ENGINESURFACE_INTERFACE_VERSION);

CEngineSurface::CEngineSurface() = default;
CEngineSurface::~CEngineSurface() = default;

void CEngineSurface::pushMakeCurrent(int *p1, int *p2, int *r, bool useInsets)
{
	// TODO
};

void CEngineSurface::popMakeCurrent()
{
	// TODO
};

void CEngineSurface::drawFilledRect(int x0, int y0, int x1, int y1)
{
	// TODO
};

void CEngineSurface::drawOutlinedRect(int x0, int y0, int x1, int y1)
{
	// TODO
};

void CEngineSurface::drawLine(int x0, int y0, int x1, int y1)
{
	// TODO
};

void CEngineSurface::drawPolyLine(int *px, int *py, int numPoints)
{
	// TODO
};

void CEngineSurface::drawTexturedPolygon(int *p, int n)
{
	// TODO
};

void CEngineSurface::drawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	// TODO
};

void CEngineSurface::drawSetTexture(int id)
{
	// TODO
};

void CEngineSurface::drawTexturedRect(int x0, int y0, int x1, int y1)
{
	// TODO
};

int CEngineSurface::createNewTextureID()
{
	// TODO
	return 0;
};

void CEngineSurface::drawSetColor(int r, int g, int b, int a)
{
	// TODO
};

void CEngineSurface::drawSetTextColor(int r, int g, int b, int a)
{
	// TODO
};

void CEngineSurface::drawSetTextPos(int x, int y)
{
	// TODO
};

void CEngineSurface::drawGetTextPos(int &x, int &y)
{
	// TODO
};

void CEngineSurface::drawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1)
{
	// TODO
};

void CEngineSurface::drawPrintCharAdd(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1)
{
	// TODO
};

void CEngineSurface::drawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
	// TODO
};

void CEngineSurface::drawGetTextureSize(int id, int &wide, int &tall)
{
	// TODO
};

bool CEngineSurface::isTextureIDValid(int id)
{
	// TODO
	return true;
};

bool CEngineSurface::drawSetSubTextureRGBA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall)
{
	// TODO
	return true;
};

void CEngineSurface::drawFlushText()
{
	// TODO
};

void CEngineSurface::resetViewPort()
{
	// TODO
};

void CEngineSurface::drawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceUpload)
{
	// TODO
};

void CEngineSurface::drawUpdateRegionTextureBGRA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall)
{
	// TODO
};