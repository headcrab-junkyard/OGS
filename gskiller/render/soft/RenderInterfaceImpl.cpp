/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve"). You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve. If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so. If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
*/

/// @file

#include "RenderInterfaceImpl.hpp"

static CRender gRender;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRender, IRender, OGS_RENDER_INTERFACE_VERSION, gRender);

IFileSystem *gpFileSystem = nullptr;

CRender::CRender() = default;
CRender::~CRender() = default;

bool CRender::Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWnd, void *apWndProc)
{
	gpFileSystem = (IFileSystem *)afnEngineFactory(FILESYSTEM_INTERFACE_VERSION, nullptr);

	if(!gpFileSystem)
		return false;

	R_Init();
	return true;
};

void CRender::Shutdown()
{
	R_Shutdown();
};

void CRender::AppActivate(bool abActivate)
{
	SWimp_AppActivate(abActivate);
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
	R_RegisterModel(asName);
};

/*
struct image_s *CRender::RegisterSkin(const char *asName)
{
	R_RegisterSkin(asName);
};
*/

struct image_s *CRender::RegisterPic(const char *asName)
{
	Draw_FindPic(asName);
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
	SWimp_EndFrame();
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
	Draw_TileClar(x, y, w, h, name);
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
	R_CinematicSetPalette(palette);
};