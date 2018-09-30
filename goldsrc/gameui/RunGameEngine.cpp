/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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

#include "RunGameEngine.hpp"

EXPOSE_SINGLE_INTERFACE(CRunGameEngine, IRunGameEngine, RUNGAMEENGINE_INTERFACE_VERSION);

CRunGameEngine::CRunGameEngine() = default;
CRunGameEngine::~CRunGameEngine() = default;

bool CRunGameEngine::IsRunning()
{
	return true;
};

bool CRunGameEngine::AddTextCommand(const char *text)
{
	return true;
};

bool CRunGameEngine::RunEngine(const char *gameDir, const char *commandLineParams)
{
	return true;
};

bool CRunGameEngine::IsInGame()
{
	return true;
};

bool CRunGameEngine::GetGameInfo(char *infoBuffer, int bufferSize)
{
	return true;
};

void CRunGameEngine::SetTrackerUserID(int trackerID, const char *trackerName)
{
};

int CRunGameEngine::GetPlayerCount()
{
	return 0;
};

unsigned int CRunGameEngine::GetPlayerFriendsID(int playerIndex)
{
	return 0;
};

const char *CRunGameEngine::GetPlayerName(int friendsID)
{
	return "";
};

const char *CRunGameEngine::GetPlayerFriendsName(int friendsID)
{
	return "";
};

unsigned int CRunGameEngine::GetEngineBuildNumber()
{
	return 0;
};

const char *CRunGameEngine::GetProductVersionString()
{
	return "";
};

unsigned int CRunGameEngine::GetPlayerUserID(int friendsID)
{
	return 0;
};