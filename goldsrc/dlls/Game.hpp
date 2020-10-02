/*
 * This file is part of OGS Engine
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

#pragma once

struct IGameSetup;
struct IGameRules;
class CGameWorld;

class CGame;
extern CGame *gpGame;

class CGame
{
public:
	CGame(IGameRules *apRules, CGameWorld *apWorld);
	~CGame();
	
	void Init();
	void Shutdown();
	
	void Frame();
	
	IGameRules *GetRules() const {return mpRules;}
	CGameWorld *GetWorld() const {return mpWorld;}
	CBasePlayer *GetPlayer(int anIndex) const;
private:
	IGameRules *mpRules{nullptr};
	CGameWorld *mpWorld{nullptr};
	
	unsigned long framecount{0};
};