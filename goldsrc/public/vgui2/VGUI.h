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

#pragma warning(disable:4800)
#pragma warning(disable:4786)
#pragma warning(disable:4355)
#pragma warning(disable:4097)
#pragma warning(disable:4514)
#pragma warning(disable:4100)
#pragma warning(disable:4127)

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;

#ifndef _WCHAR_T_DEFINED
#if !defined(__x86_64__) && !defined(__WCHAR_TYPE__)
using wchar_t = unsigned short;
#define _WCHAR_T_DEFINED
#endif
#endif

namespace vgui2
{

/// handle to an internal vgui panel
/// this is the only handle to a panel that is valid across dll boundaries
using VPANEL = unsigned int;

/// handles to vgui objects
/// NULL values signify an invalid value
using HContext = int;
using HScheme = unsigned long;
using HTexture = unsigned long;
using HCursor = unsigned long;
using HPanel = unsigned long;
using HFont = unsigned long;

const VPANEL NULL_PANEL = 0;
const HPanel INVALID_PANEL = 0xffffffff;
const HFont INVALID_FONT = 0; ///< the value of an invalid font handle

}; // namespace vgui2