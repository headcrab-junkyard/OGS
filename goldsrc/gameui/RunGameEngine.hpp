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