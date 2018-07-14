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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include "vgui2/ISurface.h"

namespace vgui2
{

class CSurface : public ISurface
{
public:
	void Shutdown();

	void RunFrame();

	VPANEL GetEmbeddedPanel();
	void SetEmbeddedPanel(VPANEL pPanel);

	void PushMakeCurrent(VPANEL panel, bool useInsets);
	void PopMakeCurrent(VPANEL panel);

	void DrawSetColor(int r, int g, int b, int a);
	void DrawSetColor(Color col);

	void DrawFilledRect(int x0, int y0, int x1, int y1);
	void DrawOutlinedRect(int x0, int y0, int x1, int y1);

	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawPolyLine(int *px, int *py, int numPoints);

	void DrawSetTextFont(HFont font);
	void DrawSetTextColor(int r, int g, int b, int a);
	void DrawSetTextColor(Color col);

	void DrawSetTextPos(int x, int y);
	void DrawGetTextPos(int &x, int &y);

	void DrawPrintText(const wchar_t *text, int textLen);

	void DrawUnicodeChar(wchar_t wch);
	void DrawUnicodeCharAdd(wchar_t wch);

	void DrawFlushText();

	IHTML *CreateHTMLWindow(IHTMLEvents *events, VPANEL context);
	void PaintHTMLWindow(IHTML *htmlwin);
	void DeleteHTMLWindow(IHTML *htmlwin);

	void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload);
	void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
	void DrawSetTexture(int id);
	void DrawGetTextureSize(int id, int &wide, int &tall);

	void DrawTexturedRect(int x0, int y0, int x1, int y1);

	bool IsTextureIDValid(int id);
	int CreateNewTextureID(bool procedural = false);

	void GetScreenSize(int &wide, int &tall);

	void SetAsTopMost(VPANEL panel, bool state);

	void BringToFront(VPANEL panel);

	void SetForegroundWindow(VPANEL panel);

	void SetPanelVisible(VPANEL panel, bool state);

	void SetMinimized(VPANEL panel, bool state);
	bool IsMinimized(VPANEL panel);

	void FlashWindow(VPANEL panel, bool state);

	void SetTitle(VPANEL panel, const wchar_t *title);

	void SetAsToolBar(VPANEL panel, bool state);

	void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true);

	void SwapBuffers(VPANEL panel);

	void Invalidate(VPANEL panel);

	void SetCursor(HCursor cursor);

	bool IsCursorVisible();

	void ApplyChanges();

	bool IsWithin(int x, int y);

	bool HasFocus();

	bool SupportsFeature(SurfaceFeature_e feature);

	void RestrictPaintToSinglePanel(VPANEL panel);

	void SetModalPanel(VPANEL panel);
	VPANEL GetModalPanel();

	void UnlockCursor();
	void LockCursor();

	void SetTranslateExtendedKeys(bool state);

	VPANEL GetTopmostPopup();

	void SetTopLevelFocus(VPANEL panel);

	HFont CreateFont();

	bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
	bool AddCustomFontFile(const char *fontFileName);

	int GetFontTall(HFont font);
	void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c);
	int GetCharacterWidth(HFont font, int ch);
	void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall);

	VPANEL GetNotifyPanel();

	void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text);

	void PlaySound(const char *fileName);

	int GetPopupCount();
	VPANEL GetPopup(int index);

	bool ShouldPaintChildPanel(VPANEL childPanel);

	bool RecreateContext(VPANEL panel);

	void AddPanel(VPANEL panel);
	void ReleasePanel(VPANEL panel);

	void MovePopupToFront(VPANEL panel);
	void MovePopupToBack(VPANEL panel);

	void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false);
	void PaintTraverse(VPANEL panel);

	void EnableMouseCapture(VPANEL panel, bool state);

	void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall);
	void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall);

	void GetProportionalBase(int &width, int &height);

	void CalculateMouseVisible();

	bool NeedKBInput();

	bool HasCursorPosFunctions();

	void SurfaceGetCursorPos(int &x, int &y);
	void SurfaceSetCursorPos(int x, int y);

	void DrawTexturedPolygon(int *p, int n);

	int GetFontAscent(HFont font, wchar_t wch);

	void SetAllowHTMLJavaScript(bool state);
};

}; // namespace vgui2