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

#include "vgui2/IInput.hpp"

namespace vgui2
{

class CInput : public IInput
{
public:
	void SetMouseFocus(VPANEL newMouseFocus);
	void SetMouseCapture(VPANEL panel);

	void GetKeyCodeText(KeyCode code, char *buf, int buflen);

	VPANEL GetFocus();
	VPANEL GetMouseOver();

	void SetCursorPos(int x, int y);
	void GetCursorPos(int &x, int &y);

	bool WasMousePressed(MouseCode code);
	bool WasMouseDoublePressed(MouseCode code);

	bool IsMouseDown(MouseCode code);

	void SetCursorOveride(HCursor cursor);
	HCursor GetCursorOveride();

	bool WasMouseReleased(MouseCode code);
	bool WasKeyPressed(KeyCode code);

	bool IsKeyDown(KeyCode code);

	bool WasKeyTyped(KeyCode code);
	bool WasKeyReleased(KeyCode code);

	VPANEL GetAppModalSurface();
	void SetAppModalSurface(VPANEL panel);
	void ReleaseAppModalSurface();

	void GetCursorPosition(int &x, int &y);
};

}; // namespace vgui2