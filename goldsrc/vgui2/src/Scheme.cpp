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

#include "Scheme.h"

namespace vgui2
{

const char *CScheme::GetResourceString(const char *stringName)
{
	return "";
};

IBorder *CScheme::GetBorder(const char *borderName)
{
	return nullptr;
};

HFont CScheme::GetFont(const char *fontName, bool proportional)
{
	return 0;
};

Color CScheme::GetColor(const char *colorName, Color defaultColor)
{
	return {};
};

}; // namespace vgui2