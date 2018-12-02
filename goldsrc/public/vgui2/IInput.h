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
#include "MouseCode.h"
#include "KeyCode.h"
#include "interface.h"

const char VGUI_INPUT_INTERFACE_VERSION[] = "VGUI_Input004";

namespace vgui2
{

using HCursor = unsigned long;

struct IInput : public IBaseInterface
{
	///
	virtual void SetMouseFocus(VPANEL nNewMouseFocus) = 0;
	
	///
	virtual void SetMouseCapture(VPANEL nPanel) = 0;
	
	///
	virtual void GetKeyCodeText(KeyCode code, char *buf, int buflen) = 0;
	
	///
	virtual VPANEL GetFocus() = 0;
	
	///
	virtual VPANEL GetMouseOver() = 0;
	
	///
	virtual void SetCursorPos(int x, int y) = 0;
	
	///
	virtual void GetCursorPos(int &x, int &y) = 0;
	
	///
	virtual bool WasMousePressed(MouseCode code) = 0;
	
	///
	virtual bool WasMouseDoublePressed(MouseCode code) = 0;
	
	///
	virtual bool IsMouseDown(MouseCode code) = 0;
	
	///
	virtual void SetCursorOveride(HCursor nCursor) = 0;
	
	///
	virtual HCursor GetCursorOveride() = 0;
	
	///
	virtual bool WasMouseReleased(MouseCode code) = 0;
	
	///
	virtual bool WasKeyPressed(KeyCode code) = 0;
	
	///
	virtual bool IsKeyDown(KeyCode code) = 0;
	
	///
	virtual bool WasKeyTyped(KeyCode code) = 0;
	
	///
	virtual bool WasKeyReleased(KeyCode code) = 0;
	
	///
	virtual VPANEL GetAppModalSurface() = 0;
	
	///
	virtual void SetAppModalSurface(VPANEL nPanel) = 0;
	
	///
	virtual void ReleaseAppModalSurface() = 0;
	
	///
	virtual void GetCursorPosition(int &x, int &y) = 0;
};

}; // namespace vgui2