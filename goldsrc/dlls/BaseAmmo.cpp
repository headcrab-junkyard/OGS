/*
	items.qc

	item functions

	Copyright (C) 1996-1997  Id Software, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

*/

/// @file

#include "BaseAmmo.hpp"

void CBaseAmmo::Spawn()
{
	//SetMoveType();
	SetSolidity(SOLID_TRIGGER);
	SetSize(idVec3(0), idVec3(32, 32, 56));
	//SetOrigin();
	SetTouchCallback(CBaseAmmo::Touch);
	
	PostSpawn();
};

void CBaseAmmo::Respawn()
{
	// remove it in single player, or setup for respawning in deathmatch
	SetModel(string_null);
	SetSolidity(SOLID_NOT);

	if (deathmatch != 2)
		SetNextThink(gpGlobals->time + 30);

// Xian -- If playing in DM 3.0 mode, halve the time ammo respawns        

	if (deathmatch == 3 || deathmatch == 5)        
		SetNextThink(gpGlobals->time + 15);

	SetThinkCallback(SUB_regen);
};

void CBaseAmmo::Touch(CBaseEntity *other)
{
	if (other->GetClassName() != "player")
		return;
	if (other->GetHealth() <= 0)
		return;

	if(GiveTo(other)) // TODO: OnPickup?
	{
		SetTouchCallback(nullptr);
		SetThinkCallback(CBaseAmmo::SUB_Remove);
		SetNextThink(gpGlobals->time + 0.1);
	};

/*
	// if the player was using his best weapon, change up to the new one if better          
	float best = other->GetBestWeapon();

	bound_other_ammo ();
	
	sprint (other, PRINT_LOW, "You got the ");
	sprint (other, PRINT_LOW, self->netname);
	sprint (other, PRINT_LOW, "\n");

	stuffcmd (other, "bf\n");

// change to a better weapon if appropriate

	if ( other->weapon == best )
	{
		self->weapon = other->GetBestWeapon();
		other->SetCurrentAmmo ();
	}

// if changed current ammo, update it
	other->SetCurrentAmmo();

	activator = other;
	SUB_UseTargets(); // fire all targets / killtargets
*/
};