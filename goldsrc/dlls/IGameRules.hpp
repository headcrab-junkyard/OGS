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

class CBaseEntity;

struct IGameRules
{
	// Shared
	
	///
	virtual bool ShouldCollide(const CBaseEntity &lhs, const CBaseEntity &rhs) const = 0;
	
	///
	//virtual float GetAmmoDamage(const char *asName) const = 0;
	
	// Server
	
	///
	//virtual void EndGameFrame() = 0;
	
	///
	//virtual void LevelShutdown() = 0;
	
	///
	virtual bool HandleClientConnect(CBaseEntity *apEntity, const char *asName, const char *asAdr, char sRejectReason[128]) = 0;
	
	///
	virtual void HandleClientDisconnect(CBaseEntity *apEntity) = 0;
	
	///
	//virtual bool CanSpawn(CBaseEntity *apEntity) = 0; // TODO: CBasePlayer?
	
	///
	//virtual bool CanRespawn(CBaseEntity *apEntity) = 0; // TODO: CBasePlayer?
	
	///
	virtual void OnPlayerSpawn(CBasePlayer *apPlayer) = 0;
	
	///
	virtual void OnPlayerDeath(CBasePlayer *apPlayer) = 0;
	
	// TODO
	/*
	Difficulty levels
	The single/multiplayer status of a game
	Client damage rules
	Client kills and scoring
	Weapon Damage
	Weapon/item/ammo retrieval
	Weapon/item spawn and respawn control
	AI Definitions
	Healthcharger respawn control
	What happens to a dead player's weapons and ammo
	Teamplay rules
	Footstep sounds
	NPCs
	TraceLines
	Chat team/location prefixes
	Achievements
	*/
};