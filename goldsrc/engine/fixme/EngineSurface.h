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

#pragma once

#include "IEngineSurface.h"

class CEngineSurface : public IEngineSurface
{
public:
	CEngineSurface();
	~CEngineSurface();
	
	void pushMakeCurrent(int *p1, int *p2, int *r, bool useInsets) override;
	void popMakeCurrent() override;
	
	void drawFilledRect(int x0, int y0, int x1, int y1) override;
	void drawOutlinedRect(int x0, int y0, int x1, int y1) override;
	
	void drawLine(int x0, int y0, int x1, int y1) override;
	void drawPolyLine(int *px, int *py, int numPoints) override;
	
	void drawTexturedPolygon(int *p, int n) override;
	
	void drawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload) override;
	void drawSetTexture(int id) override;
	
	void drawTexturedRect(int x0, int y0, int x1, int y1) override;
	
	int createNewTextureID() override;
	
	void drawSetColor(int r, int g, int b, int a) override;
	void drawSetTextColor(int r, int g, int b, int a) override;
	
	void drawSetTextPos(int x, int y) override;
	void drawGetTextPos(int &x, int &y) override;
	
	void drawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1) override;
	void drawPrintCharAdd(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1) override;
	
	void drawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) override;
	void drawGetTextureSize(int id, int &wide, int &tall) override;
	
	bool isTextureIDValid(int id) override;
	
	bool drawSetSubTextureRGBA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall) override;
	
	void drawFlushText() override;
	
	void resetViewPort() override;
	
	void drawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceUpload) override;
	void drawUpdateRegionTextureBGRA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall) override;
};