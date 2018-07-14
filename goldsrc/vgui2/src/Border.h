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

#include "vgui2/IBorder.h"

namespace vgui2
{

class CBorder : public IBorder
{
public:
	void Paint(VPANEL panel);
	void Paint(int x0, int y0, int x1, int y1);
	void Paint(int x0, int y0, int x1, int y1, int breakSide, int breakStart, int breakStop);

	void SetInset(int left, int top, int right, int bottom);
	void GetInset(int &left, int &top, int &right, int &bottom);

	void AddLine(sides_e side, Color color, int startOffset, int endOffset);

	void ApplySchemeSettings(IScheme *pScheme, KeyValues *inResourceData);

	void ParseSideSettings(int side_index, KeyValues *inResourceData, IScheme *pScheme);

	const char *GetName();
	void SetName(const char *name);
};

}; // namespace vgui2