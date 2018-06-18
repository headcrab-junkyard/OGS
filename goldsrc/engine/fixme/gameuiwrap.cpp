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

void *gpGameUILib{ nullptr };
IGameUI *gpGameUI{ nullptr };

extern IGameConsole *gpGameConsole;

ICareerUI *gpCareerUI{ nullptr };

void UnloadGameUIModule()
{
	if(gpGameUILib)
	{
		Sys_UnloadModule(gpGameUILib);
		gpGameUILib = nullptr;
	};
};

void LoadGameUIModule()
{
	UnloadGameUIModule();

	gpGameUILib = Sys_LoadModule("gameui");

	if(!gpGameUILib)
		return;

	auto fnGameUIFactory{ Sys_GetFactory(gpGameUILib) };

	if(!fnGameUIFactory)
		return;

	gpGameUI = fnGameUIFactory(GAMEUI_INTERFACE_VERSION, nullptr);
	gpGameConsole = fnGameUIFactory(GAMECONSOLE_INTERFACE_VERSION, nullptr);
	gpCareerUI = fnGameUIFactory(CAREERUI_INTERFACE_VERSION, nullptr);

	if(!gpGameUI || !gpGameConsole || !gpCareerUI)
		return;
};

//void GameUI_Initialize(CreateInterfaceFn *factories, int count);
//void GameUI_Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system);
//void GameUI_Shutdown();

int GameUI_ActivateGameUI()
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

void GameUI_HideGameUI()
{
	if(gpGameUI)
		gpGameUI->HideGameUI();
};

bool GameUI_IsGameUIActive()
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

void GameUI_StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
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
//void GameUI_ValidateCDKey(bool force, bool inConnect);

void GameUI_OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
	if(gpGameUI)
		gpGameUI->OnDisconnectFromServer(eSteamLoginFailure, username);
};