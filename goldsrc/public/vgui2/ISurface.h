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

#pragma once

#include "VGUI.h"
#include "IHTML.h"
#include "interface.h"

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef PlaySound
#undef PlaySound
#endif

const char VGUI_SURFACE_INTERFACE_VERSION[] = "VGUI_Surface026";

class Color;

namespace vgui2
{

struct IImage;
class Image;
class Point;

using HCursor = unsigned long;
using HTexture = unsigned long;
using HFont = unsigned long;

struct ISurface : public IBaseInterface
{
	enum EFontFlags
	{
		FONTFLAG_NONE,

		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
	};

	enum SurfaceFeature_e
	{
		ANTIALIASED_FONTS = 1,
		DROPSHADOW_FONTS = 2,
		ESCAPE_KEY = 3,
		OPENING_NEW_HTML_WINDOWS = 4,
		FRAME_MINIMIZE_MAXIMIZE = 5,
		DIRECT_HWND_RENDER = 6,
	};

	///
	virtual void Shutdown() = 0;
	
	///
	virtual void RunFrame() = 0;
	
	///
	virtual VPANEL GetEmbeddedPanel() = 0;
	
	///
	virtual void SetEmbeddedPanel(VPANEL nPanel) = 0;
	
	///
	virtual void PushMakeCurrent(VPANEL nPanel, bool bUseInsets) = 0;
	
	///
	virtual void PopMakeCurrent(VPANEL nPanel) = 0;
	
	///
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	
	///
	virtual void DrawSetColor(Color col) = 0;
	
	///
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	
	///
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	
	///
	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	
	///
	virtual void DrawPolyLine(int *px, int *py, int numPoints) = 0;
	
	///
	virtual void DrawSetTextFont(HFont nFont) = 0;
	
	///
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	
	///
	virtual void DrawSetTextColor(Color col) = 0;
	
	///
	virtual void DrawSetTextPos(int x, int y) = 0;
	
	///
	virtual void DrawGetTextPos(int &x, int &y) = 0;
	
	///
	virtual void DrawPrintText(const wchar_t *sText, int nTextLen) = 0;
	
	///
	virtual void DrawUnicodeChar(wchar_t wch) = 0;
	
	///
	virtual void DrawUnicodeCharAdd(wchar_t wch) = 0;
	
	///
	virtual void DrawFlushText() = 0;
	
	///
	virtual IHTML *CreateHTMLWindow(IHTMLEvents *pEvents, VPANEL nContext) = 0;
	
	///
	virtual void PaintHTMLWindow(IHTML *pWindow) = 0;
	
	///
	virtual void DeleteHTMLWindow(IHTML *pWindow) = 0;
	
	///
	virtual void DrawSetTextureFile(int nId, const char *sFileName, int nHardwareFilter, bool bForceReload) = 0;
	
	///
	virtual void DrawSetTextureRGBA(int nId, const unsigned char *rgba, int wide, int tall, int nHardwareFilter, bool bForceReload) = 0;
	
	///
	virtual void DrawSetTexture(int nId) = 0;
	
	///
	virtual void DrawGetTextureSize(int nId, int &wide, int &tall) = 0;
	
	///
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	
	///
	virtual bool IsTextureIDValid(int nId) = 0;
	
	///
	virtual int CreateNewTextureID(bool bProcedural = false) = 0;
	
	///
	virtual void GetScreenSize(int &wide, int &tall) = 0;
	
	///
	virtual void SetAsTopMost(VPANEL nPanel, bool bState) = 0;
	
	///
	virtual void BringToFront(VPANEL nPanel) = 0;
	
	///
	virtual void SetForegroundWindow(VPANEL nPanel) = 0;
	
	///
	virtual void SetPanelVisible(VPANEL nPanel, bool bState) = 0;
	
	///
	virtual void SetMinimized(VPANEL nPanel, bool bState) = 0;
	
	///
	virtual bool IsMinimized(VPANEL nPanel) = 0;
	
	///
	virtual void FlashWindow(VPANEL nPanel, bool bState) = 0;
	
	///
	virtual void SetTitle(VPANEL nPanel, const wchar_t *sTitle) = 0;
	
	///
	virtual void SetAsToolBar(VPANEL nPanel, bool bState) = 0;
	
	///
	virtual void CreatePopup(VPANEL nPanel, bool bMinimised, bool bShowTaskbarIcon = true, bool bDisabled = false, bool bMouseInput = true, bool bKbInput = true) = 0;
	
	///
	virtual void SwapBuffers(VPANEL nPanel) = 0;
	
	///
	virtual void Invalidate(VPANEL nPanel) = 0;
	
	///
	virtual void SetCursor(HCursor nCursor) = 0;
	
	///
	virtual bool IsCursorVisible() = 0;
	
	///
	virtual void ApplyChanges() = 0;
	
	///
	virtual bool IsWithin(int x, int y) = 0;
	
	///
	virtual bool HasFocus() = 0;
	
	///
	virtual bool SupportsFeature(SurfaceFeature_e feature) = 0;
	
	///
	virtual void RestrictPaintToSinglePanel(VPANEL nPanel) = 0;
	
	///
	virtual void SetModalPanel(VPANEL nPanel) = 0;
	
	///
	virtual VPANEL GetModalPanel() = 0;
	
	///
	virtual void UnlockCursor() = 0;
	
	///
	virtual void LockCursor() = 0;
	
	///
	virtual void SetTranslateExtendedKeys(bool bState) = 0;
	
	///
	virtual VPANEL GetTopmostPopup() = 0;
	
	///
	virtual void SetTopLevelFocus(VPANEL nPanel) = 0;
	
	///
	virtual HFont CreateFont() = 0;
	
	///
	virtual bool AddGlyphSetToFont(HFont nFont, const char *sWindowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) = 0;
	
	///
	virtual bool AddCustomFontFile(const char *sFontFileName) = 0;
	
	///
	virtual int GetFontTall(HFont nFont) = 0;
	
	///
	virtual void GetCharABCwide(HFont nFont, int ch, int &a, int &b, int &c) = 0;
	
	///
	virtual int GetCharacterWidth(HFont nFont, int ch) = 0;
	
	///
	virtual void GetTextSize(HFont nFont, const wchar_t *sText, int &wide, int &tall) = 0;
	
	///
	virtual VPANEL GetNotifyPanel() = 0;
	
	///
	virtual void SetNotifyIcon(VPANEL nContext, HTexture icon, VPANEL panelToReceiveMessages, const char *sText) = 0;
	
	///
	virtual void PlaySound(const char *sFileName) = 0;
	
	///
	virtual int GetPopupCount() = 0;
	
	///
	virtual VPANEL GetPopup(int nIndex) = 0;
	
	///
	virtual bool ShouldPaintChildPanel(VPANEL nChildPanel) = 0;
	
	///
	virtual bool RecreateContext(VPANEL nPanel) = 0;
	
	///
	virtual void AddPanel(VPANEL nPanel) = 0;
	
	///
	virtual void ReleasePanel(VPANEL nPanel) = 0;
	
	///
	virtual void MovePopupToFront(VPANEL nPanel) = 0;
	
	///
	virtual void MovePopupToBack(VPANEL nPanel) = 0;
	
	///
	virtual void SolveTraverse(VPANEL nPanel, bool bForceApplySchemeSettings = false) = 0;
	
	///
	virtual void PaintTraverse(VPANEL nPanel) = 0;
	
	///
	virtual void EnableMouseCapture(VPANEL nPanel, bool bState) = 0;
	
	///
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;
	
	///
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;
	
	///
	virtual void GetProportionalBase(int &width, int &height) = 0;
	
	///
	virtual void CalculateMouseVisible() = 0;
	
	///
	virtual bool NeedKBInput() = 0;
	
	///
	virtual bool HasCursorPosFunctions() = 0;
	
	///
	virtual void SurfaceGetCursorPos(int &x, int &y) = 0;
	
	///
	virtual void SurfaceSetCursorPos(int x, int y) = 0;
	
	///
	virtual void DrawTexturedPolygon(int *p, int n) = 0;
	
	///
	virtual int GetFontAscent(HFont nFont, wchar_t wch) = 0;
	
	///
	virtual void SetAllowHTMLJavaScript(bool bState) = 0;
};

}; // namespace vgui2