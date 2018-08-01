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

#include "Border.h"

namespace vgui2
{

void CBorder::Paint(VPANEL panel)
{
};

void CBorder::Paint(int x0, int y0, int x1, int y1)
{
};

void CBorder::Paint(int x0, int y0, int x1, int y1, int breakSide, int breakStart, int breakStop)
{
};

void CBorder::SetInset(int left, int top, int right, int bottom)
{
};

void CBorder::GetInset(int &left, int &top, int &right, int &bottom)
{
};

void CBorder::AddLine(sides_e side, Color color, int startOffset, int endOffset)
{
};

void CBorder::ApplySchemeSettings(IScheme *pScheme, KeyValues *inResourceData)
{
};

void CBorder::ParseSideSettings(int side_index, KeyValues *inResourceData, IScheme *pScheme)
{
};

const char *CBorder::GetName()
{
	return "";
};

void CBorder::SetName(const char *name)
{
};

}; // namespace vgui2