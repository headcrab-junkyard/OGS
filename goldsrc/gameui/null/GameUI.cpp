/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include "GameUINull.hpp"

static CGameUI gGameUI;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION, gGameUI);

void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	TRACE("CGameUI::Initialize");
};

void CGameUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system)
{
	TRACE("CGameUI::Start");
};

void CGameUI::Shutdown()
{
	TRACE("CGameUI::Shutdown");
};

int CGameUI::ActivateGameUI()
{
	TRACE("CGameUI::ActivateGameUI");
	mbActive = true;
	return 1;
};

int CGameUI::ActivateDemoUI()
{
	TRACE("CGameUI::ActivateDemoUI");
	return 1;
};

int CGameUI::HasExclusiveInput()
{
	TRACE("CGameUI::HasExclusiveInput");
	return 0;
};

void CGameUI::RunFrame()
{
	TRACE("CGameUI::RunFrame");
};

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
	TRACE("CGameUI::ConnectToServer(%s, %d, %d)", game, IP, port);
};

void CGameUI::DisconnectFromServer()
{
	TRACE("CGameUI::DisconnectFromServer");
};

void CGameUI::HideGameUI()
{
	TRACE("CGameUI::HideGameUI");
	mbActive = false;
};

bool CGameUI::IsGameUIActive()
{
	TRACE("CGameUI::IsGameUIActive");
	return mbActive;
};

void CGameUI::LoadingStarted(const char *resourceType,
                             const char *resourceName)
{
	TRACE("CGameUI::LoadingStarted");
};

void CGameUI::LoadingFinished(const char *resourceType,
                              const char *resourceName)
{
	TRACE("CGameUI::LoadingFinished");
};

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	TRACE("CGameUI::StartProgressBar");
};

int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	TRACE("CGameUI::ContinueProgressBar");
	return 1;
};

void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	TRACE("CGameUI::StopProgressBar");
};

int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	TRACE("CGameUI::SetProgressBarStatusText");
};

void CGameUI::SetSecondaryProgressBar(float progress)
{
	TRACE("CGameUI::SetSecondaryProgressBar");
};

void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	TRACE("CGameUI::SetSecondaryProgressBarText");
};