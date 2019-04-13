/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

#include "CareerUI.hpp"

bool CCareerUI::IsPlayingMatch()
{
	return false;
};

ITaskVec *CCareerUI::GetCurrentTaskVec()
{
	return nullptr;
};

bool CCareerUI::PlayAsCT()
{
	return false;
};

int CCareerUI::GetReputationGained()
{
	return 0;
};

int CCareerUI::GetNumMapsUnlocked()
{
	return 0;
};

bool CCareerUI::DoesWinUnlockAll()
{
	return false;
};

int CCareerUI::GetRoundTimeLength()
{
	return 0;
};

int CCareerUI::GetWinfastLength()
{
	return 0;
};

CareerDifficultyType CCareerUI::GetDifficulty()
{
	return 0;
};

int CCareerUI::GetCurrentMapTriplet(MapInfo *maps)
{
	return 0;
};

void CCareerUI::OnRoundEndMenuOpen(bool didWin){};

void CCareerUI::OnMatchEndMenuOpen(bool didWin){};

void CCareerUI::OnRoundEndMenuClose(bool stillPlaying){};

void CCareerUI::OnMatchEndMenuClose(bool stillPlaying){};