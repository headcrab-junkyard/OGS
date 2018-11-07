/*
 * This file is part of OGS Engine
 * Copyright (C) 2017-2018 BlackPhrase
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
/// @brief interface from engine to gameui module

#pragma once

#include "public/interface.h"

// TODO: temp hack
namespace vgui2
{
	enum class KeyCode : int
	{
		KEY_NONE = 0
	};
};

const char VENGINE_GAMEUIFUNCS_VERSION[] = "VENGINE_GAMEUIFUNCS_VERSION001";

struct IGameUIFuncs : public IBaseInterface
{
	///
	virtual bool IsKeyDown(const char *keyname, bool &isdown) = 0;
	
	///
	virtual const char *Key_NameForKey(int keynum) = 0;
	
	///
	virtual const char *Key_BindingForKey(int keynum) = 0;
	
	///
	virtual vgui2::KeyCode GetVGUI2KeyCodeForBind(const char *bind) = 0; // TODO: was vgui::KeyCode but looks like GS is using vgui2 namespace for VGUI2 code
	
	///
	virtual void GetVideoModes(struct vmode_s **liststart, int *count) = 0;
	
	///
	virtual void GetCurrentVideoMode(int *wide, int *tall, int *bpp) = 0;
	
	///
	virtual void GetCurrentRenderer(char *name, int namelen, int *windowed) = 0;
	
	///
	virtual bool IsConnectedToVACSecureServer() = 0;
	
	///
	virtual int Key_KeyStringToKeyNum(const char *string) = 0;
};