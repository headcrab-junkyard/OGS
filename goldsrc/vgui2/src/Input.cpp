/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018-2019 BlackPhrase
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

#include "Input.h"

namespace vgui2
{

EXPOSE_SINGLE_INTERFACE(CInput, IInput, VGUI_INPUT_INTERFACE_VERSION)

void CInput::SetMouseFocus(VPANEL newMouseFocus)
{
};

void CInput::SetMouseCapture(VPANEL panel)
{
};

void CInput::GetKeyCodeText(KeyCode code, char *buf, int buflen)
{
};

VPANEL CInput::GetFocus()
{
	return 0;
};

VPANEL CInput::GetMouseOver()
{
	return 0;
};

void CInput::SetCursorPos(int x, int y)
{
};

void CInput::GetCursorPos(int &x, int &y)
{
};

bool CInput::WasMousePressed(MouseCode code)
{
	return false;
};

bool CInput::WasMouseDoublePressed(MouseCode code)
{
	return false;
};

bool CInput::IsMouseDown(MouseCode code)
{
	return false;
};

void CInput::SetCursorOveride(HCursor cursor)
{
};

HCursor CInput::GetCursorOveride()
{
	return 0;
};

bool CInput::WasMouseReleased(MouseCode code)
{
	return false;
};

bool CInput::WasKeyPressed(KeyCode code)
{
	return false;
};

bool CInput::IsKeyDown(KeyCode code)
{
	return false;
};

bool CInput::WasKeyTyped(KeyCode code)
{
	return false;
};

bool CInput::WasKeyReleased(KeyCode code)
{
	return false;
};

VPANEL CInput::GetAppModalSurface()
{
	return 0;
};

void CInput::SetAppModalSurface(VPANEL panel)
{
};

void CInput::ReleaseAppModalSurface()
{
};

void CInput::GetCursorPosition(int &x, int &y)
{
};

}; // namespace vgui2