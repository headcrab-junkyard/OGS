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
	void Paint(VPANEL panel) override;
	void Paint(int x0, int y0, int x1, int y1) override;
	void Paint(int x0, int y0, int x1, int y1, int breakSide, int breakStart, int breakStop) override;

	void SetInset(int left, int top, int right, int bottom) override;
	void GetInset(int &left, int &top, int &right, int &bottom) override;

	void AddLine(sides_e side, Color color, int startOffset, int endOffset) override;

	void ApplySchemeSettings(IScheme *pScheme, KeyValues *inResourceData) override;

	void ParseSideSettings(int side_index, KeyValues *inResourceData, IScheme *pScheme) override;

	const char *GetName() override;
	void SetName(const char *name) override;
};

}; // namespace vgui2