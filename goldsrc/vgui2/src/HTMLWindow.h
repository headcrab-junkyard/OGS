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

#include "vgui2/IHTML.h"

namespace vgui2
{

class CHTMLWindow : public IHTML, public IHTMLEvents
{
public:
	void OpenURL(const char *url);

	bool StopLoading();

	bool Refresh();

	bool Show(bool shown);

	char *GetOpenedPage();

	void OnSize(int x, int y, int w, int h);
	void GetHTMLSize(int &wide, int &tall);

	void Clear();

	void AddText(const char *text);

	void OnMouse(MouseCode code, MOUSE_STATE s, int x, int y);
	void OnChar(wchar_t unichar);
	void OnKeyDown(KeyCode code);

	IImage *GetBitmap();

	void SetVisible(bool state);

	bool OnStartURL(const char *url, const char *target, bool first);
	void OnFinishURL(const char *url);
	void OnProgressURL(long current, long maximum);

	void OnSetStatusText(const char *text);

	void OnUpdate();

	void OnLink();
	void OffLink();
};

}; // namespace vgui2