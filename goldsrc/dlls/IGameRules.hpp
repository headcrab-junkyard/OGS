/// @file

#pragma once

class CBaseEntity;

struct IGameRules
{
	// Shared
	
	///
	virtual bool ShouldCollide(const CBaseEntity &lhs, const CBaseEntity &rhs) const = 0;
	
	///
	virtual float GetAmmoDamage(const char *asName) const = 0;
	
	// Server
	
	///
	virtual void EndGameFrame() = 0;
	
	///
	virtual void LevelShutdown() = 0;
	
	// TODO
	/*
	Difficulty levels
	The single/multiplayer status of a game
	Client connection & disconnection
	Client damage rules
	Client kills and scoring
	Spawning and respawning control
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