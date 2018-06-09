/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief wrapper functions for IGameUI interface

#pragma once

//void GameUI_Initialize(CreateInterfaceFn *factories, int count);
//void GameUI_Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system);
//void GameUI_Shutdown();

int GameUI_ActivateGameUI();
int GameUI_ActivateDemoUI();

int GameUI_HasExclusiveInput();

//void GameUI_RunFrame();

void GameUI_ConnectToServer(const char *game, int IP, int port);
void GameUI_DisconnectFromServer();

void GameUI_HideGameUI();

bool GameUI_IsGameUIActive();

void GameUI_LoadingStarted(const char *resourceType, const char *resourceName);
void GameUI_LoadingFinished(const char *resourceType, const char *resourceName);

void GameUI_StartProgressBar(const char *progressType, int progressSteps);
int GameUI_ContinueProgressBar(int progressPoint, float progressFraction);
void GameUI_StopProgressBar(bool bError, const char *failureReason, const char *extendedReason);
int GameUI_SetProgressBarStatusText(const char *statusText);

void GameUI_SetSecondaryProgressBar(float progress);
void GameUI_SetSecondaryProgressBarText(const char *statusText);

// Obsolete, does nothing
//void GameUI_ValidateCDKey(bool force, bool inConnect);

void GameUI_OnDisconnectFromServer(int eSteamLoginFailure, const char *username);