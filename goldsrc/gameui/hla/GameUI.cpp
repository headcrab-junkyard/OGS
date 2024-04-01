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

#include <cstdlib>
#include "GameUI.hpp"
#include "menu.h"

#include "IMenuExportsTemp.hpp"
IMenuExportsTemp *gpMenuExports{nullptr};

void VID_MenuDraw();
void VID_MenuKey(int key);

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
	
	gpMenuExports = (IMenuExportsTemp*)factories[0](OGS_MENUEXPORTS_INTERFACE_VERSION, nullptr);
	
	if(!gpMenuExports)
		return;
	
	M_Init(); // TODO: temp
	
#ifdef _WIN32 // TODO: ???
	vid_menudrawfn = VID_MenuDraw;
	vid_menukeyfn = VID_MenuKey;
#endif
};

void CGameUI::Start(/*cl_enginefunc_t*/ struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void /*IBaseSystem*/ *system)
{
	// TODO: interfaceVersion
	
	mpEngine = engineFuncs;
	mpBaseSystem = system;
};

void CGameUI::Shutdown()
{
	mpEngine = nullptr;
	mpBaseSystem = nullptr;
};

int CGameUI::ActivateGameUI()
{
	M_ToggleMenu_f();
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
	// TODO: call VID_MenuDraw here?
};

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
	byte vIP[4] = IP;
	mpEngine->Con_Printf("Connecting to %s (%d.%d.%d.%d:%d)...\n", game, vIP[0], vIP[1], vIP[2], vIP[3], port);
};

void CGameUI::DisconnectFromServer()
{
};

void CGameUI::HideGameUI()
{
	M_ToggleMenu_f();
	mbActive = false;
};

bool CGameUI::IsGameUIActive()
{
	return mbActive;
};

void CGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	mpEngine->Con_Printf("Loading started for %s (%s)...\n", resourceName, resourceType);
};

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	mpEngine->Con_Printf("Loading finished for %s (%s)...\n", resourceName, resourceType);
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
	const char *sReason{""};
	
	switch(eSteamLoginFailure)
	{
	//case STEAMLOGINFAILURE_NONE:
		//break;
	case STEAMLOGINFAILURE_BADTICKET:
		sReason = "Bad Ticket";
		break;
	case STEAMLOGINFAILURE_NOSTEAMLOGIN:
		sReason = "No Steam Login";
		break;
	case STEAMLOGINFAILURE_VACBANNED:
		sReason = "VAC Banned";
		break;
	case STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE:
		sReason = "Logged-In Elsewhere";
		break;
	case STEAMLOGINFAILURE_CONNECTIONLOST:
		sReason = "Lost Connection To Server";
		break;
	case STEAMLOGINFAILURE_NOCONNECTION:
		sReason = "No Connection";
		break;
	};
	
	mpEngine->Con_Printf("Disconnected from server (Reason: %s)\n", sReason);
};