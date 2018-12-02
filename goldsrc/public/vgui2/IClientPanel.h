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

#ifdef GetClassName
#undef GetClassName
#endif

class KeyValues;

namespace vgui2
{

class Panel;
class SurfaceBase;

enum EInterfaceID
{
	ICLIENTPANEL_STANDARD_INTERFACE = 0
};

struct IClientPanel
{
	///
	virtual VPANEL GetVPanel() = 0;
	
	///
	virtual void Think() = 0;
	
	///
	virtual void PerformApplySchemeSettings() = 0;
	
	///
	virtual void PaintTraverse(bool bForceRepaint, bool bAllowForce) = 0;
	
	///
	virtual void Repaint() = 0;
	
	///
	virtual VPANEL IsWithinTraverse(int x, int y, bool bTraversePopups) = 0;
	
	///
	virtual void GetInset(int &top, int &left, int &right, int &bottom) = 0;
	
	///
	virtual void GetClipRect(int &x0, int &y0, int &x1, int &y1) = 0;
	
	///
	virtual void OnChildAdded(VPANEL nChild) = 0;
	
	///
	virtual void OnSizeChanged(int nNewWide, int nNewTall) = 0;
	
	///
	virtual void InternalFocusChanged(bool bLost) = 0;
	
	///
	virtual bool RequestInfo(KeyValues *pOutputData) = 0;
	
	///
	virtual void RequestFocus(int nDirection = 0) = 0;
	
	///
	virtual bool RequestFocusPrev(VPANEL nExistingPanel) = 0;
	
	///
	virtual bool RequestFocusNext(VPANEL nExistingPanel) = 0;
	
	///
	virtual void OnMessage(const KeyValues *pParams, VPANEL nFromPanel) = 0;
	
	///
	virtual VPANEL GetCurrentKeyFocus() = 0;
	
	///
	virtual int GetTabPosition() = 0;
	
	///
	virtual const char *GetName() = 0;
	
	///
	virtual const char *GetClassName() = 0;
	
	///
	virtual HScheme GetScheme() = 0;
	
	///
	virtual bool IsProportional() = 0;
	
	///
	virtual bool IsAutoDeleteSet() = 0;
	
	///
	virtual void DeletePanel() = 0;
	
	///
	virtual void *QueryInterface(EInterfaceID nId) = 0;
	
	///
	virtual Panel *GetPanel() = 0;
	
	///
	virtual const char *GetModuleName() = 0;
};

}; // namespace vgui2