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

#include "HTMLWindow.h"

namespace vgui2
{

void CHTMLWindow::OpenURL(const char *url)
{
};

bool CHTMLWindow::StopLoading()
{
	return false;
};

bool CHTMLWindow::Refresh()
{
	return false;
};

bool CHTMLWindow::Show(bool shown)
{
	return false;
};

char *CHTMLWindow::GetOpenedPage()
{
	return nullptr;
};

void CHTMLWindow::OnSize(int x, int y, int w, int h)
{
};

void CHTMLWindow::GetHTMLSize(int &wide, int &tall)
{
};

void CHTMLWindow::Clear()
{
};

void CHTMLWindow::AddText(const char *text)
{
};

void CHTMLWindow::OnMouse(MouseCode code, MOUSE_STATE s, int x, int y)
{
};

void CHTMLWindow::OnChar(wchar_t unichar)
{
};

void CHTMLWindow::OnKeyDown(KeyCode code)
{
};

IImage *CHTMLWindow::GetBitmap()
{
	return nullptr;
};

void CHTMLWindow::SetVisible(bool state)
{
};

bool CHTMLWindow::OnStartURL(const char *url, const char *target, bool first)
{
	return false;
};

void CHTMLWindow::OnFinishURL(const char *url)
{
};

void CHTMLWindow::OnProgressURL(long current, long maximum)
{
};

void CHTMLWindow::OnSetStatusText(const char *text)
{
};

void CHTMLWindow::OnUpdate()
{
};

void CHTMLWindow::OnLink()
{
};

void CHTMLWindow::OffLink()
{
};

}; // namespace vgui2