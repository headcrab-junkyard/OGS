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
/// @brief dialog for launching a listen server

#pragma once

#include <vgui_controls/PropertyDialog.h>

class CCreateMultiplayerGameServerPage;
class CCreateMultiplayerGameGameplayPage;
class CCreateMultiplayerGameBotPage;

class CCreateMultiplayerGameDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(CCreateMultiplayerGameDialog, vgui::PropertyDialog);
public:
	CCreateMultiplayerGameDialog(vgui::Panel *apParent);
	~CCreateMultiplayerGameDialog();
protected:
	/*virtual*/ bool OnOK(bool abApplyOnly);
private:
	CCreateMultiplayerGameServerPage *mpServerPage{nullptr};
	CCreateMultiplayerGameGameplayPage *mpGameplayPage{nullptr};
	CCreateMultiplayerGameBotPage *mpBotPage{nullptr};
	
	// For loading/saving the game config
	KeyValues *mpSavedData{nullptr};
	
	bool mbBotsEnabled{false};
};