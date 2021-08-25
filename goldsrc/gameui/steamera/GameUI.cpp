/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2020 BlackPhrase
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

#include <cstdlib>
#include "GameUI.hpp"

IEngineVGui *gpEngineVGui{nullptr};

ILocalize *gpVGuiLocalize{nullptr}:

static IGameClientExports *gpGameClientExports{nullptr};

EXPOSE_SINGLE_INTERFACE(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION);

CGameUI::CGameUI() = default;
CGameUI::~CGameUI() = default;

void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	// TODO
	
	// Load localization file
	gpVGuiLocalize->AddFile("resource/gameui_%language%.txt", "GAME", true);
	
	// Load mod info
	//ModInfo().LoadCurrentGameInfo();
	
	// Load localization file for kb_act.lst
	gpVGuiLocalize->AddFile("resource/valve_%language%.txt", "GAME", true);
	
	for(int i = 0; i < count; ++i)
	{
		mpVGUI = reinterpret_cast<vgui2::IVGUI*>(*factories[i](VGUI_INTERFACE_VERSION, nullptr));
		mpVGUISurfave = (vgui2::ISurface*)*factories[i](VGUI_SURFACE_INTERFACE_VERSION, nullptr);
		mpFileSystem = (IFileSystem*)*factories[i](FILESYSTEM_INTERFACE_VERSION, nullptr);
		mpEngineVGui = (IEngineVGui*)*factories[i](VENGINE_VGUI_VERSION, nullptr);
		mpGameUIFuncs = (IGameUIFuncs*)*factories[i](GAMEUIFUNCS_INTERFACE_VERSION, nullptr);
		mpGameClientExports = (IGameClientExports*)*factories[i](GAMECLIENTEXPORTS_INTERFACE_VERSION, nullptr);
	};
	
	bool bFailed{false};
	
	bFailed = !mpGameUIFuncs || !gpEngineVGui;
	
	if(bFailed)
		Sys_Error("CGameUI::Initialize() failed to get necessary interfaces\n");
	
	if(!mpGameClientExports)
		mpGameClientExports = new CDefaultGameClientExports();
};

void CGameUI::Start(/*cl_enginefunc_t*/ struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void /*IBaseSystem*/ *system)
{
};

void CGameUI::Shutdown()
{
};

int CGameUI::ActivateGameUI()
{
	mbActive = true;
	return true;
};

int CGameUI::ActivateDemoUI()
{
	return false;
};

int CGameUI::HasExclusiveInput()
{
	return false;
};

void CGameUI::RunFrame()
{
	// Play the startup music the first time we run frame
	if(mnPlayGameStartupSound > 0)
	{
		--mnPlayGameStartupSound;
		if(!mnPlayGameStartupSound)
			PlayGameStartupSound();
	};
};

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
	mnGameIP = IP;
	mnGamePort = port;
	
	SendConnectedToGameMessage();
};

void CGameUI::DisconnectFromServer()
{
	mnGameIP = 0;
	mnGamePort = 0;
	
	if(ghLoadingBackgroundDialog)
		vgui2::ivgui()->PostMessage(ghLoadingBackgroundDialog, new KeyValues("DisconnectedFromGame"), nullptr);
	
	gVModuleLoader.PostMessageToAllModules(new KeyValues("DisconnectedFromGame"));
};

void CGameUI::HideGameUI()
{
	mbActive = false;
};

bool CGameUI::IsGameUIActive()
{
	return mbActive;
};

void CGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	gVModuleLoader.PostMessageToAllModules(new KeyValues("LoadingStarted"));
	
	ShowLoadingBackgroundDialog();
	
	StartProgressBar();
	
	mnPlayGameStartupSound = 0;
};

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	StopProgressBar(false, "", "");
	
	gVModuleLoader.PostMessageToAllModules(new KeyValues("LoadingFinished"));
	
	HideLoadingBackgroundDialog();
};

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	ghLoadingDialog->Activate();
	
	// TODO
};

int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	if(!ghLoadingDialog.Get())
		return 0;
	
	ghLoadingDialog->Activate();
	return ghLoadingDialog->SetProgressPoint(progressPoint, progressFraction);
};

void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	if(!ghLoadingDialog.Get())
		return;
	
	if(bError)
	{
		ghLoadingDialog->DisplayGenericError(failureReason, extendedReason);
	}
	else
	{
		// Close the loading dialog
		ghLoadingDialog->Close();
		ghLoadingDialog = nullptr;
	};
};

int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	if(!ghLoadingDialog.Get())
		return 0;
	
	if(!statusText)
		return 0;
	
	if(!stricmp(statusText, msPrevStatusText))
		return 0;
	
	ghLoadingDialog->SetStatusText(statusText);
	Q_strncpy(msPrevStatusText, statusText, sizeof(msPrevStatusText));
	return 1;
};

// NOTE: progress is in range [0...1]
void CGameUI::SetSecondaryProgressBar(float progress)
{
	if(ghLoadingDialog.Get())
		ghLoadingDialog->SetSecondaryProgress(progress);
};

void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	if(ghLoadingDialog.Get())
		ghLoadingDialog->SetSecondaryProgressText(statusText);
};

void CGameUI::ValidateCDKey(bool force, bool inConnect)
{
	// UNUSED
};

void CGameUI::OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
};

void CGameUI::PlayGameStartupSound()
{
	// TODO
};

void CGameUI::SendConnectedToGameMessage()
{
	// TODO
};