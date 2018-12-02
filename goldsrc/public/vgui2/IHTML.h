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
#include "MouseCode.h"
#include "KeyCode.h"
#include "IImage.h"

namespace vgui2
{

struct IHTML
{
	enum MOUSE_STATE
	{
		UP,
		DOWN,
		MOVE
	};

	///
	virtual void OpenURL(const char *asURL) = 0;
	
	///
	virtual bool StopLoading() = 0;
	
	///
	virtual bool Refresh() = 0;
	
	///
	virtual bool Show(bool abShown) = 0;
	
	///
	virtual char *GetOpenedPage() = 0;
	
	///
	virtual void OnSize(int x, int y, int w, int h) = 0;
	
	///
	virtual void GetHTMLSize(int &wide, int &tall) = 0;
	
	///
	virtual void Clear() = 0;
	
	///
	virtual void AddText(const char *asText) = 0;
	
	///
	virtual void OnMouse(MouseCode code, MOUSE_STATE s, int x, int y) = 0;
	
	///
	virtual void OnChar(wchar_t unichar) = 0;
	
	///
	virtual void OnKeyDown(KeyCode code) = 0;
	
	///
	virtual IImage *GetBitmap() = 0;
	
	///
	virtual void SetVisible(bool abState) = 0;
};

struct IHTMLEvents
{
	///
	virtual bool OnStartURL(const char *asURL, const char *sTarget, bool bFirst) = 0;
	
	///
	virtual void OnFinishURL(const char *asURL) = 0;
	
	///
	virtual void OnProgressURL(long nCurrent, long nMaximum) = 0;
	
	///
	virtual void OnSetStatusText(const char *asText) = 0;
	
	///
	virtual void OnUpdate() = 0;
	
	///
	virtual void OnLink() = 0;
	
	///
	virtual void OffLink() = 0;
};

}; // namespace vgui2