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

// TODO: unused in gs?

#pragma once

#include "VGUI.h"
#include "interface.h"
#include "Color.h"

const char VGUI_SCHEME_INTERFACE_VERSION[] = "VGUI_Scheme009";

namespace vgui2
{

using HScheme = unsigned long;
using HTexture = unsigned long;

struct IBorder;
struct IImage;

struct IScheme : public IBaseInterface
{
	///
	virtual const char *GetResourceString(const char *sStringName) = 0;
	
	///
	virtual IBorder *GetBorder(const char *sBorderName) = 0;
	
	///
	virtual HFont GetFont(const char *sFontName, bool bProportional = false) = 0;
	
	///
	virtual Color GetColor(const char *sColorName, Color defaultColor) = 0;
};

struct ISchemeManager : public IBaseInterface
{
	///
	virtual HScheme LoadSchemeFromFile(const char *sFileName, const char *sTag) = 0;
	
	///
	virtual void ReloadSchemes() = 0;
	
	///
	virtual HScheme GetDefaultScheme() = 0;
	
	///
	virtual HScheme GetScheme(const char *sTag) = 0;
	
	///
	virtual IImage *GetImage(const char *sImageName, bool bHardwareFiltered) = 0; // TODO: return void * (no support for IImage in GS???)
	
	///
	virtual HTexture GetImageID(const char *sImageName, bool bHardwareFiltered) = 0;
	
	///
	virtual IScheme *GetIScheme(HScheme nScheme) = 0; // TODO: return void * (no support for IScheme in GS???)
	
	///
	virtual void Shutdown(bool bFull = true) = 0;
	
	///
	virtual int GetProportionalScaledValue(int nNormalizedValue) = 0;
	
	///
	virtual int GetProportionalNormalizedValue(int nScaledValue) = 0;
};

}; // namespace vgui2