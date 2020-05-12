/// @file

#pragma once

#include "tier1/interface.h"

const char ENGINE_SURFACE_VERSION[] = "EngineSurface007";

class IEngineSurface : public IBaseInterface
{
public:
	virtual void pushMakeCurrent(int *p1, int *p2, int *r, bool useInsets) = 0;
	virtual void popMakeCurrent() = 0;
	
	virtual void drawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void drawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	
	virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void drawPolyLine(int *px, int *py, int numPoints) = 0;
	
	virtual void drawTexturedPolygon(int *p, int n) = 0;
	
	virtual void drawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload) = 0;
	virtual void drawSetTexture(int id) = 0;
	
	virtual void drawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	
	virtual int createNewTextureID() = 0;
	
	virtual void drawSetColor(int r, int g, int b, int a) = 0;
	virtual void drawSetTextColor(int r, int g, int b, int a) = 0;
	
	virtual void drawSetTextPos(int x, int y) = 0;
	virtual void drawGetTextPos(int &x, int &y) = 0;
	
	virtual void drawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1) = 0;
	virtual void drawPrintCharAdd(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1) = 0;
	
	virtual void drawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
	virtual void drawGetTextureSize(int id, int &wide, int &tall) = 0;
	
	virtual bool isTextureIDValid(int id) = 0;
	
	virtual bool drawSetSubTextureRGBA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall) = 0;
	
	virtual void drawFlushText() = 0;
	
	virtual void resetViewPort() = 0;
	
	virtual void drawSetTextureBGRA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceUpload) = 0;
	virtual void drawUpdateRegionTextureBGRA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall) = 0;
};