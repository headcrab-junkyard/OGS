/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2019 BlackPhrase
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

#include "Game.hpp"
#include "IGameSetup.hpp"

CGame *gpGame = nullptr; // TODO

CGame::CGame(IGameRules *apRules, CGameWorld *apWorld) : mpRules(apRules), mpWorld(apWorld)
{
	gpGame = this;
};

CGame::~CGame() //= default;
{
	gpGame = nullptr;
};

void CGame::Init()
{
	//gEngFuncs.pfnPrecacheModel("models/player.mdl"); // TODO: studio models are not supported yet...
};

void CGame::Shutdown()
{
	// NOTE: free resources here
};

void CGame::Frame()
{
	if(mpRules)
		mpRules->Think();

	//
	// cvars checked each frame
	//
	//timelimit = cvar("timelimit") * 60;
	//fraglimit = cvar("fraglimit");
	gpGlobals->teamplay = teamplay.value; // teamplay = cvar("teamplay");
	//deathmatch = cvar("deathmatch");

	framecount++;
};