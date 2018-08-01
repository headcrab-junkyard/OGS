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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "Surface.h"

namespace vgui2
{

void CSurface::Shutdown()
{
};

void CSurface::RunFrame()
{
};

VPANEL CSurface::GetEmbeddedPanel()
{
	return 0;
};

void CSurface::SetEmbeddedPanel(VPANEL pPanel)
{
};

void CSurface::PushMakeCurrent(VPANEL panel, bool useInsets)
{
};

void CSurface::PopMakeCurrent(VPANEL panel)
{
};

void CSurface::DrawSetColor(int r, int g, int b, int a)
{
};

void CSurface::DrawSetColor(Color col)
{
};

void CSurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
};

void CSurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
};

void CSurface::DrawLine(int x0, int y0, int x1, int y1)
{
};

void CSurface::DrawPolyLine(int *px, int *py, int numPoints)
{
};

void CSurface::DrawSetTextFont(HFont font)
{
};

void CSurface::DrawSetTextColor(int r, int g, int b, int a)
{
};

void CSurface::DrawSetTextColor(Color col)
{
};

void CSurface::DrawSetTextPos(int x, int y)
{
};

void CSurface::DrawGetTextPos(int &x, int &y)
{
};

void CSurface::DrawPrintText(const wchar_t *text, int textLen)
{
};

void CSurface::DrawUnicodeChar(wchar_t wch)
{
};

void CSurface::DrawUnicodeCharAdd(wchar_t wch)
{
};

void CSurface::DrawFlushText()
{
};

IHTML *CSurface::CreateHTMLWindow(IHTMLEvents *events, VPANEL context)
{
	return nullptr;
};

void CSurface::PaintHTMLWindow(IHTML *htmlwin)
{
};

void CSurface::DeleteHTMLWindow(IHTML *htmlwin)
{
};

void CSurface::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
};

void CSurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
};

void CSurface::DrawSetTexture(int id)
{
};

void CSurface::DrawGetTextureSize(int id, int &wide, int &tall)
{
};

void CSurface::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
};

bool CSurface::IsTextureIDValid(int id)
{
	return false;
};

int CSurface::CreateNewTextureID(bool procedural)
{
	return 0;
};

void CSurface::GetScreenSize(int &wide, int &tall)
{
};

void CSurface::SetAsTopMost(VPANEL panel, bool state)
{
};

void CSurface::BringToFront(VPANEL panel)
{
};

void CSurface::SetForegroundWindow(VPANEL panel)
{
};

void CSurface::SetPanelVisible(VPANEL panel, bool state)
{
};

void CSurface::SetMinimized(VPANEL panel, bool state)
{
};

bool CSurface::IsMinimized(VPANEL panel)
{
	return false;
};

void CSurface::FlashWindow(VPANEL panel, bool state)
{
};

void CSurface::SetTitle(VPANEL panel, const wchar_t *title)
{
};

void CSurface::SetAsToolBar(VPANEL panel, bool state)
{
};

void CSurface::CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput)
{
};

void CSurface::SwapBuffers(VPANEL panel)
{
};

void CSurface::Invalidate(VPANEL panel)
{
};

void CSurface::SetCursor(HCursor cursor)
{
};

bool CSurface::IsCursorVisible()
{
	return false;
};

void CSurface::ApplyChanges()
{
};

bool CSurface::IsWithin(int x, int y)
{
	return false;
};

bool CSurface::HasFocus()
{
	return false;
};

bool CSurface::SupportsFeature(SurfaceFeature_e feature)
{
	return false;
};

void CSurface::RestrictPaintToSinglePanel(VPANEL panel)
{
};

void CSurface::SetModalPanel(VPANEL panel)
{
};

VPANEL CSurface::GetModalPanel()
{
	return 0;
};

void CSurface::UnlockCursor()
{
};

void CSurface::LockCursor()
{
};

void CSurface::SetTranslateExtendedKeys(bool state)
{
};

VPANEL CSurface::GetTopmostPopup()
{
	return 0;
};

void CSurface::SetTopLevelFocus(VPANEL panel)
{
};

HFont CSurface::CreateFont()
{
	return 0;
};

bool CSurface::AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	return false;
};

bool CSurface::AddCustomFontFile(const char *fontFileName)
{
	return false;
};

int CSurface::GetFontTall(HFont font)
{
	return 0;
};

void CSurface::GetCharABCwide(HFont font, int ch, int &a, int &b, int &c)
{
};

int CSurface::GetCharacterWidth(HFont font, int ch)
{
	return 0;
};

void CSurface::GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall)
{
};

VPANEL CSurface::GetNotifyPanel()
{
	return 0;
};

void CSurface::SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text)
{
};

void CSurface::PlaySound(const char *fileName)
{
};

int CSurface::GetPopupCount()
{
	return 0;
};

VPANEL CSurface::GetPopup(int index)
{
	return 0;
};

bool CSurface::ShouldPaintChildPanel(VPANEL childPanel)
{
	return false;
};

bool CSurface::RecreateContext(VPANEL panel)
{
	return false;
};

void CSurface::AddPanel(VPANEL panel)
{
};

void CSurface::ReleasePanel(VPANEL panel)
{
};

void CSurface::MovePopupToFront(VPANEL panel)
{
};

void CSurface::MovePopupToBack(VPANEL panel)
{
};

void CSurface::SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false)
{
};

void CSurface::PaintTraverse(VPANEL panel)
{
};

void CSurface::EnableMouseCapture(VPANEL panel, bool state)
{
};

void CSurface::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
};

void CSurface::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
};

void CSurface::GetProportionalBase(int &width, int &height)
{
};

void CSurface::CalculateMouseVisible()
{
};

bool CSurface::NeedKBInput()
{
	return false;
};

bool CSurface::HasCursorPosFunctions()
{
	return false;
};

void CSurface::SurfaceGetCursorPos(int &x, int &y)
{
};

void CSurface::SurfaceSetCursorPos(int x, int y)
{
};

void CSurface::DrawTexturedPolygon(int *p, int n)
{
};

int CSurface::GetFontAscent(HFont font, wchar_t wch)
{
	return 0;
};

void CSurface::SetAllowHTMLJavaScript(bool state)
{
};

}; // namespace vgui2