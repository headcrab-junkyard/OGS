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
#include "Color.h"

class KeyValues;

namespace vgui2
{

struct IScheme;

struct IBorder
{
	enum sides_e
	{
		SIDE_LEFT = 0,
		SIDE_TOP,
		SIDE_RIGHT,
		SIDE_BOTTOM
	};

	///
	virtual void Paint(VPANEL nPanel) = 0;
	
	///
	virtual void Paint(int x0, int y0, int x1, int y1) = 0;
	
	///
	virtual void Paint(int x0, int y0, int x1, int y1, int nBreakSide, int nBreakStart, int nBreakStop) = 0;
	
	///
	virtual void SetInset(int nLeft, int nTop, int nRight, int nBottom) = 0;
	
	///
	virtual void GetInset(int &left, int &top, int &right, int &bottom) = 0;
	
	///
	virtual void AddLine(sides_e side, Color color, int nStartOffset, int nEndOffset) = 0;
	
	///
	virtual void ApplySchemeSettings(IScheme *pScheme, KeyValues *pInResourceData) = 0;
	
	///
	virtual void ParseSideSettings(int nSideIndex, KeyValues *pInResourceData, IScheme *pScheme) = 0;
	
	///
	virtual const char *GetName() = 0;
	
	///
	virtual void SetName(const char *sName) = 0;
};

}; // namespace vgui2