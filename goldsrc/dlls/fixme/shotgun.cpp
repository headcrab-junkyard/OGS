/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018 BlackPhrase
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

#include "BaseWeapon.hpp"

class CWeaponShotgun : public CBaseWeapon
{
public:
	void PrimaryAttack() override;
	void SecondaryAttack() override;
};

/*
================
W_FireShotgun
================
*/
void CWeaponShotgun::PrimaryAttack()
{
	vec3_t dir;
	
	self->EmitSound(CHAN_WEAPON, "weapons/guncock.wav", 1, ATTN_NORM); 

	msg_entity = self;
	WriteByte(MSG_ONE, SVC_SMALLKICK);
	
	if(deathmatch != 4 )
		self.currentammo = self.ammo_shells = self.ammo_shells - 1;

	dir = aim(self, 100000);
	FireBullets(6, dir, '0.04 0.04 0');
};

/*
================
W_FireSuperShotgun
================
*/
void CWeaponShotgun::SecondaryAttack()
{
	vec3_t dir;

	if(self.currentammo == 1)
	{
		PrimaryAttack();
		return;
	};
		
	self->EmitSound(CHAN_WEAPON, "weapons/shotgn2.wav", 1, ATTN_NORM); 

	msg_entity = self;
	WriteByte(MSG_ONE, SVC_BIGKICK);
	
	if(deathmatch != 4)
		self.currentammo = self.ammo_shells = self.ammo_shells - 2;
	
	dir = aim(self, 100000);
	FireBullets(14, dir, '0.14 0.08 0');
};