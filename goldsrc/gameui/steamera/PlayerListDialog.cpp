/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021 BlackPhrase
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

#include "PlayerListDialog.hpp"

CPlayerListDialog::CPlayerListDialog(vgui::Panel *apParent)
	: BaseClass(apParent, "PlayerListDialog")
{
	SetSize(320, 240);
	SetTitle("#GameUI_CurrentPlayers", true);
	
	mpMuteButton = new vgui::Button(this, "MuteButton", "");
	
	mpPlayerList = new ListPanel(this, "PlayerList");
	
	mpPlayerList->AddColumnHeader(0, "Name", "#GameUI_PlayerName", 180);
	mpPlayerList->AddColumnHeader(1, "Properties", "#GameUI_Properties", 80);
	
	mpPlayerList->SetEmptyListText("#GameUI_NoOtherPlayersInGame");
	
	LoadControlSettings("resource/PlayerListDialog.res"); // TODO: Resource
};

CPlayerListDialog::~CPlayerListDialog() = default;

void CPlayerListDialog::Activate()
{
	BaseClass::Activate();
};

void CPlayerListDialog::OnItemSelected()
{
	RefreshPlayerProperties();
	
	// Set the button state based on the selected item
	bool bMuteButtonEnabled{false};
	
	if(mpPlayerList->GetSelectedItemsCount() > 0)
	{
	}
	else
		mpMuteButton->SetText("#GameUI_MuteIngameVoice");
	
	mpMuteButton->SetEnabled(bMuteButtonEnabled);
};

void CPlayerListDialog::OnCommand(const char *asCmd)
{
	if(!stricmp(asCmd, "Mute"))
		ToggleMuteStateOfSelectedUser();
	else
		BaseClass::OnCommand(asCmd);
};

void CPlayerListDialog::ToggleMuteStateOfSelectedUser()
{
	if(!GameClientExports())
		return;
	
	for(int nSelectedItem = 0; nSelectedItem < mpPlayerList->GetSelectedItemsCount(); ++nSelectedItem)
	{
	};
	
	RefreshPlayerProperties();
	OnItemSelected();
};

void CPlayerListDialog::RefreshPlayerProperties()
{
};