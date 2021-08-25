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

#include "GameConsoleDialog.hpp"

CGameConsoleDialog::CGameConsoleDialog() : BaseClass(nullptr, "GameConsole", false)
{
	AddActionSignalTarget(this);
};

void CGameConsoleDialog::OnClosedByHittingTilde()
{
	if(!LoadingDialog())
		GameUI().HideGameUI();
	else
		vgui::GetSurface()->RestrictPaintToSinglePanel(LoadingDialog()->GetVPanel());
};

void CGameConsoleDialog::OnCommandSubmitted(const char *asCmd)
{
	gpEngine->ClientCmd_Unretricted(asCmd);
};

void CGameConsoleDialog::OnKeyCodeTyped(vgui::KeyCode aeCode)
{
	BaseClass::OnKeyCodeTyped(aeCode);
	
	// Check for processing
	if(mpConsolePanel->TextEntryHasFocus())
		if(aeCode >= KEY_F1 && aeCode <= KEY_F12)
		{
		};
};

void CGameConsoleDialog::OnCommand(const char *asCmd)
{
	if(!Q_stricmp(asCmd, "Close"))
		if(GameUI().IsInBackgroundLevel())
		{
			// Tell the engine that we've hid the console, so that it unpauses the game (even though we're still sitting at the menu)
			gpEngine->ClientCmd_Unrestricted("unpause");
		};
	
	BaseClass::OnCommand(asCmd);
};