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
/// @brief server options page of the create game server dialog

#pragma once

#include <vgui_controls/PropertyPage.h>

class CCreateMultiplayerGameServerPage : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CCreateMultiplayerGameServerPage, vgui::PropertyPage);
public:
	CCreateMultiplayerGameServerPage();
	~CCreateMultiplayerGameServerPage();
	
	void SetMap(const char *asName);
	
	bool IsRandomMapSelected() const;
	
	const char *GetMapName() const;
	
	// (CS) Bots
	void EnableBots(KeyValues *apData);
	int GetBotQuota() const;
	bool GetBotsEnabled() const;
protected:
private:
	void LoadMapList();
	void LoadMaps(const char *asPathID);
private:
};