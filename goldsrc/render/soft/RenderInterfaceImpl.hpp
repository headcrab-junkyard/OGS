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

#pragma once

#include "RenderInterface.h"

class CRender final : public IRender
{
public:
	CRender();
	~CRender();

	bool Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWnd, void *apWndProc) override;
	void Shutdown() override;

	void AppActivate(bool abActivate) override;

	void BeginRegistration(const char *asMap) override;
	void EndRegistration() override;

	struct model_s *RegisterModel(const char *asName) override;
	//struct image_s *RegisterSkin(const char *asName) override;
	struct image_s *RegisterPic(const char *asName) override;

	void SetSky(const char *asName, float afRotate, vec3_t avAxis) override;

	void BeginFrame(float afCameraSeparation) override;
	void RenderFrame(refdef_t *apRenderSettings) override;
	void EndFrame() override;

	void DrawGetPicSize(int *w, int *h, const char *name) override;
	void DrawPic(int x, int y, const char *name) override;
	void DrawStretchPic(int x, int y, int w, int h, const char *name) override;
	void DrawChar(int x, int y, int c) override;
	void DrawTileClear(int x, int y, int w, int h, const char *name) override;
	void DrawFill(int x, int y, int w, int h, int c) override;
	void DrawFadeScreen() override;

	void DrawStretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data) override;

	void CinematicSetPalette(const unsigned short *palette) override;
};