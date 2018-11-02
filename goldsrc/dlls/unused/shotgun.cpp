/// @file

#include "Weapon.hpp"

class CShotgun : public CWeapon
{
public:
	void Fire() override;
};

class CSuperShotgun : public CWeapon
{
public:
	void Fire() override;
};

/*
================
W_FireShotgun
================
*/
void CShotgun::Fire()
{
	vector dir;

	gpEngine->pfnEmitSound(self, CHAN_WEAPON, "weapons/guncock.wav", 1, ATTN_NORM); 

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
void CSuperShotgun::Fire()
{
	vector dir;

	if(self.currentammo == 1)
	{
		W_FireShotgun();
		return;
	};
		
	gpEngine->pfnEmitSound(self, CHAN_WEAPON, "weapons/shotgn2.wav", 1, ATTN_NORM); 

	msg_entity = self;
	WriteByte(MSG_ONE, SVC_BIGKICK);
	
	if(deathmatch != 4)
		self.currentammo = self.ammo_shells = self.ammo_shells - 2;
	
	dir = aim(self, 100000);
	FireBullets(14, dir, '0.14 0.08 0');
};