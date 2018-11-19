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

#include "quakedef.h"
#include "GameUI/IGameUI.h"
#include "GameUI/IGameConsole.h"
//#include "GameUI/ICareerUI.h" // TODO

IGameUI *gpGameUI{ nullptr };

extern IGameConsole *gpGameConsole;

//ICareerUI *gpCareerUI{ nullptr };

//void GameUI_Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system);

// TODO
extern "C" int GameUI_ActivateGameUI()
{
	if(gpGameUI)
		return gpGameUI->ActivateGameUI();

	return 0;
};

int GameUI_ActivateDemoUI()
{
	if(gpGameUI)
		return gpGameUI->ActivateDemoUI();

	return 0;
};

int GameUI_HasExclusiveInput()
{
	if(gpGameUI)
		return gpGameUI->HasExclusiveInput();

	return 0;
};

//void GameUI_RunFrame();

void GameUI_ConnectToServer(const char *game, int IP, int port)
{
	if(gpGameUI)
		gpGameUI->ConnectToServer(game, IP, port);
};

void GameUI_DisconnectFromServer()
{
	if(gpGameUI)
		gpGameUI->DisconnectFromServer();
};

// TODO
extern "C" void GameUI_HideGameUI()
{
	if(gpGameUI)
		gpGameUI->HideGameUI();
};

// TODO
extern "C" qboolean GameUI_IsGameUIActive()
{
	if(gpGameUI)
		return gpGameUI->IsGameUIActive();

	return false;
};

void GameUI_LoadingStarted(const char *resourceType, const char *resourceName)
{
	if(gpGameUI)
		gpGameUI->LoadingStarted(resourceType, resourceName);
};

void GameUI_LoadingFinished(const char *resourceType, const char *resourceName)
{
	if(gpGameUI)
		gpGameUI->LoadingFinished(resourceType, resourceName);
};

void GameUI_StartProgressBar(const char *progressType, int progressSteps)
{
	if(gpGameUI)
		gpGameUI->StartProgressBar(progressType, progressSteps);
};

int GameUI_ContinueProgressBar(int progressPoint, float progressFraction)
{
	if(gpGameUI)
		return gpGameUI->ContinueProgressBar(progressPoint, progressFraction);

	return 0;
};

void GameUI_StopProgressBar(qboolean bError, const char *failureReason, const char *extendedReason)
{
	if(gpGameUI)
		gpGameUI->StopProgressBar(bError, failureReason, extendedReason);
};

int GameUI_SetProgressBarStatusText(const char *statusText)
{
	if(gpGameUI)
		return gpGameUI->SetProgressBarStatusText(statusText);

	return 0;
};

void GameUI_SetSecondaryProgressBar(float progress)
{
	if(gpGameUI)
		gpGameUI->SetSecondaryProgressBar(progress);
};

void GameUI_SetSecondaryProgressBarText(const char *statusText)
{
	if(gpGameUI)
		gpGameUI->SetSecondaryProgressBarText(statusText);
};

// Obsolete, does nothing
//void GameUI_ValidateCDKey(qboolean force, qboolean inConnect);

void GameUI_OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
	if(gpGameUI)
		gpGameUI->OnDisconnectFromServer(eSteamLoginFailure, username);
};