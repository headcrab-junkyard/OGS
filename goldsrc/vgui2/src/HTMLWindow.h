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
	void OpenURL(const char *url) override;

	bool StopLoading() override;

	bool Refresh() override;

	bool Show(bool shown) override;

	char *GetOpenedPage() override;

	void OnSize(int x, int y, int w, int h) override;
	void GetHTMLSize(int &wide, int &tall) override;

	void Clear() override;

	void AddText(const char *text) override;

	void OnMouse(MouseCode code, MOUSE_STATE s, int x, int y) override;
	void OnChar(wchar_t unichar) override;
	void OnKeyDown(KeyCode code) override;

	IImage *GetBitmap() override;

	void SetVisible(bool state) override;

	bool OnStartURL(const char *url, const char *target, bool first) override;
	void OnFinishURL(const char *url) override;
	void OnProgressURL(long current, long maximum) override;

	void OnSetStatusText(const char *text) override;

	void OnUpdate() override;

	void OnLink() override;
	void OffLink() override;
};

}; // namespace vgui2