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