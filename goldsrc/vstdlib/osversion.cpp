/*
 * This file is part of OGS Engine
 * Copyright (C) 2024 BlackPhrase
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

#include <vstdlib/osversion.h>

VSTDLIB_INTERFACE const char *GetNameFromOSType(eOSType aeType)
{
	switch(aeType)
	{
	case eOSType::Windows311:
		return "Windows 3.11";
	case eOSType::Windows95:
		return "Windows 95";
	case eOSType::Windows98:
		return "Windows 98";
	case eOSType::WindowsME:
		return "Windows ME";
	case eOSType::Windows98:
		return "Windows NT";	
	case eOSType::Windows2000:
		return "Windows 2000";
	case eOSType::WindowsXP:
		return "Windows XP";
	case eOSType::Windows2003:
		return "Windows 2003";
	case eOSType::WindowsVista:
		return "Windows Vista";
	case eOSType::Windows7:
		return "Windows 7";
	case eOSType::Windows8:
		return "Windows 8";
	case eOSType::Windows81:
		return "Windows 8.1";
	case eOSType::Windows10:
		return "Windows 10";
	case eOSType::Windows11:
		return "Windows 11";
	default:
		return "Unknown";
	};
};

VSTDLIB_INTERFACE eOSType GetOSType()
{
};

winVista    win2003 winXP   win2000 winNT   winME   win98  unknown


VSTDLIB_INTERFACE eOSType GetOSTypeFromString(const char *asString)
{
	if(!strcmp(asString, "win311"))
		return eOSType::Windows311;
	else if(!strcmp(asString, "win95"))
		return eOSType::Windows95;
	else if(!strcmp(asString, "win98"))
		return eOSType::Windows98;
	else if(!strcmp(asString, "win95"))
		return eOSType::Windows95;
	else
		return eOSType::Unknown;
};