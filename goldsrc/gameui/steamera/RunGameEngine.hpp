/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2020 BlackPhrase
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

#include "GameUI/IRunGameEngine.h"

class CRunGameEngine final : public IRunGameEngine
{
public:
	CRunGameEngine();
	~CRunGameEngine();
	
	bool IsRunning() override;

	bool AddTextCommand(const char *text) override;

	bool RunEngine(const char *gameDir, const char *commandLineParams) override;

	bool IsInGame() override;

	bool GetGameInfo(char *infoBuffer, int bufferSize) override;

	void SetTrackerUserID(int trackerID, const char *trackerName) override;

	int GetPlayerCount() override;

	unsigned int GetPlayerFriendsID(int playerIndex) override;

	const char *GetPlayerName(int friendsID) override;

	const char *GetPlayerFriendsName(int friendsID) override;

	unsigned int GetEngineBuildNumber() override;
	const char *GetProductVersionString() override;
	
	unsigned int GetPlayerUserID(int friendsID) override;
};