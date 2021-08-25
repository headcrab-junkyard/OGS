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
/// @brief advanced bot properties page of the create game server dialog

#pragma once

#include <vgui_controls/PropertyPage.h>

class CCvarToggleCheckButton;

class CCreateMultiplayerGameBotPage : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CCreateMultiplayerGameBotPage, vgui::PropertyPage);
public:
	CCreateMultiplayerGameBotPage(vgui::Panel *apParent, const char *asName, KeyValues *apBotKeys);
	~CCreateMultiplayerGameBotPage();
protected:
	/*virtual*/ void OnResetChanges();
	/*virtual*/ void OnApplyChanges();
private:
	void SetJoinTeamCombo(const char *asTeam);
	
	void SetChatterCombo(const char *asTeam);
private:
	CCvarToggleCheckButton *mpJoinAfterPlayerCheckBox{nullptr};
	
	CCvarToggleCheckButton *mpAllowRoguesCheckBox{nullptr};
	
	CCvarToggleCheckButton *mpAllowPistolsCheckBox{nullptr};
	CCvarToggleCheckButton *mpAllowShotgunsCheckBox{nullptr};
	CCvarToggleCheckButton *mpAllowSMGsCheckBox{nullptr};
	CCvarToggleCheckButton *mpAllowMachineGunsCheckBox{nullptr};
	CCvarToggleCheckButton *mpAllowRiflesCheckBox{nullptr};
	CCvarToggleCheckButton *mpAllowGrenadesCheckBox{nullptr};
#ifdef OPENLAMBDA_CS_SHIELD_ENABLED
	CCvarToggleCheckButton *mpAllowShieldsCheckBox{nullptr};
#endif
	CCvarToggleCheckButton *mpAllowSnipersCheckBox{nullptr};
	
	CCvarToggleCheckButton *mpDeferToHumanCheckBox{nullptr};
	
	vgui::ComboBox *mpJoinTeamComboBox{nullptr};
	vgui::ComboBox *mpChatterComboBox{nullptr};
	
	vgui::TextEntry *mpPrefixEntry{nullptr};
	
	KeyValues *mpSavedData{nullptr};
};