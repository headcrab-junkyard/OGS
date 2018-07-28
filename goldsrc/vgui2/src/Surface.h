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
	void Shutdown() override;

	void RunFrame() override;

	VPANEL GetEmbeddedPanel() override;
	void SetEmbeddedPanel(VPANEL pPanel) override;

	void PushMakeCurrent(VPANEL panel, bool useInsets) override;
	void PopMakeCurrent(VPANEL panel) override;

	void DrawSetColor(int r, int g, int b, int a) override;
	void DrawSetColor(Color col) override;

	void DrawFilledRect(int x0, int y0, int x1, int y1) override;
	void DrawOutlinedRect(int x0, int y0, int x1, int y1) override;

	void DrawLine(int x0, int y0, int x1, int y1) override;
	void DrawPolyLine(int *px, int *py, int numPoints) override;

	void DrawSetTextFont(HFont font) override;
	void DrawSetTextColor(int r, int g, int b, int a) override;
	void DrawSetTextColor(Color col) override;

	void DrawSetTextPos(int x, int y) override;
	void DrawGetTextPos(int &x, int &y) override;

	void DrawPrintText(const wchar_t *text, int textLen) override;

	void DrawUnicodeChar(wchar_t wch) override;
	void DrawUnicodeCharAdd(wchar_t wch) override;

	void DrawFlushText() override;

	IHTML *CreateHTMLWindow(IHTMLEvents *events, VPANEL context) override;
	void PaintHTMLWindow(IHTML *htmlwin) override;
	void DeleteHTMLWindow(IHTML *htmlwin) override;

	void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) override;
	void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload) override;
	void DrawSetTexture(int id) override;
	void DrawGetTextureSize(int id, int &wide, int &tall) override;

	void DrawTexturedRect(int x0, int y0, int x1, int y1) override;

	bool IsTextureIDValid(int id) override;
	int CreateNewTextureID(bool procedural = false) override;

	void GetScreenSize(int &wide, int &tall) override;

	void SetAsTopMost(VPANEL panel, bool state) override;

	void BringToFront(VPANEL panel) override;

	void SetForegroundWindow(VPANEL panel) override;

	void SetPanelVisible(VPANEL panel, bool state) override;

	void SetMinimized(VPANEL panel, bool state) override;
	bool IsMinimized(VPANEL panel) override;

	void FlashWindow(VPANEL panel, bool state) override;

	void SetTitle(VPANEL panel, const wchar_t *title) override;

	void SetAsToolBar(VPANEL panel, bool state) override;

	void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) override;

	void SwapBuffers(VPANEL panel) override;

	void Invalidate(VPANEL panel) override;

	void SetCursor(HCursor cursor) override;

	bool IsCursorVisible() override;

	void ApplyChanges() override;

	bool IsWithin(int x, int y) override;

	bool HasFocus() override;

	bool SupportsFeature(SurfaceFeature_e feature) override;

	void RestrictPaintToSinglePanel(VPANEL panel) override;

	void SetModalPanel(VPANEL panel) override;
	VPANEL GetModalPanel() override;

	void UnlockCursor() override;
	void LockCursor() override;

	void SetTranslateExtendedKeys(bool state) override;

	VPANEL GetTopmostPopup() override;

	void SetTopLevelFocus(VPANEL panel) override;

	HFont CreateFont() override;

	bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) override;
	bool AddCustomFontFile(const char *fontFileName) override;

	int GetFontTall(HFont font) override;
	void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c) override;
	int GetCharacterWidth(HFont font, int ch) override;
	void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall) override;

	VPANEL GetNotifyPanel() override;

	void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text) override;

	void PlaySound(const char *fileName) override;

	int GetPopupCount() override;
	VPANEL GetPopup(int index) override;

	bool ShouldPaintChildPanel(VPANEL childPanel) override;

	bool RecreateContext(VPANEL panel) override;

	void AddPanel(VPANEL panel) override;
	void ReleasePanel(VPANEL panel) override;

	void MovePopupToFront(VPANEL panel) override;
	void MovePopupToBack(VPANEL panel) override;

	void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) override;
	void PaintTraverse(VPANEL panel) override;

	void EnableMouseCapture(VPANEL panel, bool state) override;

	void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) override;
	void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) override;

	void GetProportionalBase(int &width, int &height) override;

	void CalculateMouseVisible() override;

	bool NeedKBInput() override;

	bool HasCursorPosFunctions() override;

	void SurfaceGetCursorPos(int &x, int &y) override;
	void SurfaceSetCursorPos(int x, int y) override;

	void DrawTexturedPolygon(int *p, int n) override;

	int GetFontAscent(HFont font, wchar_t wch) override;

	void SetAllowHTMLJavaScript(bool state) override;
};

}; // namespace vgui2