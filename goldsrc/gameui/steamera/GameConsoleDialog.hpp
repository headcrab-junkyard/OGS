/*
 * This file is part of OGS Engine
 * Copyright (C) 2020-2021 BlackPhrase
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
/// @brief Game/dev console dialog

#pragma once

#include <vgui_controls/ConsoleDialog.h>

class CGameConsoleDialog : public vgui::CConsoleDialog
{
	DECLARE_CLASS_SIMPLE(CGameConsoleDialog, vgui::CConsoleDialog);
public:
	CGameConsoleDialog();
	
	//void Activate();
	
	//void Hide();
	
	//void Clear();
	
	//void SetParent(vgui2::VPANEL anParent);
	
	//bool IsVisible() const;
private:
	/// Submits a command
	MESSAGE_FUNC(OnClosedByHittingTilde, "ClosedByHittingTilde");
	
	/// Submits a command
	MESSAGE_FUNC_CHARPTR(OnCommandSubmitted, "CommandSubmitted", asCmd);
	
	/*virtual*/ void OnKeyCodeTyped(vgui::KeyCode aeCode) override;
	
	/// Generic VGUI command handler
	/*virtual*/ void OnCommand(const char *asCmd) override;
};