/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2019 BlackPhrase
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

EXPOSE_SINGLE_INTERFACE(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION);

CGameUI::CGameUI() = default;
CGameUI::~CGameUI() = default;

void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	// TODO
	
	// Load localization file
	//gpVGuiLocalize->AddFile("resource/gameui_%language%.txt", "GAME", true);
	
	// Load localization file for kb_act.lst
	//gpVGuiLocalize->AddFile("resource/valve_%language%.txt", "GAME", true);
	
	/*
	for(int i = 0; i < count; ++i)
	{
		mpVGUI = (vgui2::IVGUI*)*factories[i](VGUI_INTERFACE_VERSION, nullptr);
		mpVGUISurfave = (vgui2::ISurface*)*factories[i](VGUI_SURFACE_INTERFACE_VERSION, nullptr);
		mpFileSystem = (IFileSystem*)*factories[i](FILESYSTEM_INTERFACE_VERSION, nullptr);
		mpEngineVGui = (IEngineVGui*)*factories[i](VENGINE_VGUI_VERSION, nullptr);
		mpGameUIFuncs = (IGameUIFuncs*)*factories[i](GAMEUIFUNCS_INTERFACE_VERSION, nullptr);
		mpGameClientExports = (IGameClientExports*)*factories[i](GAMECLIENTEXPORTS_INTERFACE_VERSION, nullptr);
	};
	
	if(!mpGameUIFuncs)
		return; // Sys_Error("CGameUI::Initialize() failed to get necessary interfaces\n");
	
	if(!mpGameClientExports)
		mpGameClientExports = new CDefaultGameClientExports();
	*/
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
};

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
};

void CGameUI::DisconnectFromServer()
{
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
};

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
};

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
};

int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	return 0;
};

void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
};

int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	return 0;
};

void CGameUI::SetSecondaryProgressBar(float progress)
{
};

void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
};

void CGameUI::ValidateCDKey(bool force, bool inConnect)
{
	// UNUSED
};

void CGameUI::OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
};