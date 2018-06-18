/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2018 BlackPhrase
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

#include "RenderInterfaceImpl.hpp"
#include "gl_local.h"
#include "public/FileSystem.h"

static CRender gRender;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRender, IRender, OGS_RENDER_INTERFACE_VERSION, gRender);

IFileSystem *gpFileSystem = nullptr;

CRender::CRender() = default;
CRender::~CRender() = default;

bool CRender::Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWnd /*, void *apWndProc*/)
{
	gpFileSystem = (IFileSystem *)afnEngineFactory(FILESYSTEM_INTERFACE_VERSION, nullptr);

	if(!gpFileSystem)
		return false;

	// Pass a window ptr from the engine
	// If it's valid then use it (might be passed from launcher code)
	// Otherwise create it ourself
	// Note: Mb better to always create a window in engine/video module and pass it's ptr here?
	R_Init(ahInstance, apWnd /*, apWndProc*/);
	return true;
};

void CRender::Shutdown()
{
	R_Shutdown();
};

void CRender::AppActivate(bool abActivate)
{
	GLimp_AppActivate(abActivate);
};

void CRender::BeginRegistration(const char *asMap)
{
	R_BeginRegistration(asMap);
};

void CRender::EndRegistration()
{
	R_EndRegistration();
};

struct model_s *CRender::RegisterModel(const char *asName)
{
	return R_RegisterModel(asName);
};

/*
struct image_s *CRender::RegisterSkin(const char *asName)
{
	return R_RegisterSkin(asName);
};
*/

struct image_s *CRender::RegisterPic(const char *asName)
{
	return Draw_FindPic(asName);
};

void CRender::SetSky(const char *asName, float afRotate, vec3_t avAxis)
{
	R_SetSky(asName, afRotate, avAxis);
};

void CRender::BeginFrame(float afCameraSeparation)
{
	R_BeginFrame(afCameraSeparation);
};

void CRender::RenderFrame(refdef_t *apRenderSettings)
{
	R_RenderFrame(apRenderSettings);
};

void CRender::EndFrame()
{
	GLimp_EndFrame();
};

void CRender::DrawGetPicSize(int *w, int *h, const char *name)
{
	Draw_GetPicSize(w, h, name);
};

void CRender::DrawPic(int x, int y, const char *name)
{
	Draw_Pic(x, y, name);
};

void CRender::DrawStretchPic(int x, int y, int w, int h, const char *name)
{
	Draw_StretchPic(x, y, w, h, name);
};

void CRender::DrawChar(int x, int y, int c)
{
	Draw_Char(x, y, c);
};

void CRender::DrawTileClear(int x, int y, int w, int h, const char *name)
{
	Draw_TileClear(x, y, w, h, name);
};

void CRender::DrawFill(int x, int y, int w, int h, int c)
{
	Draw_Fill(x, y, w, h, c);
};

void CRender::DrawFadeScreen()
{
	Draw_FadeScreen();
};

void CRender::DrawStretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data)
{
	Draw_StretchRaw(x, y, w, h, cols, rows, data);
};

void CRender::CinematicSetPalette(const unsigned short *palette)
{
	R_SetPalette(palette);
};