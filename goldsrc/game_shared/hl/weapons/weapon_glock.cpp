/// @file

//#include "exports.h"
//#include "engine.h"
#include "BaseWeapon.hpp"

class CWeaponGlock : public CBaseWeapon
{
public:
	void Spawn() override;
	
	void PrimaryAttack() override;
	void SecondaryAttack() override;
private:
	void Fire();
};

LINK_ENTITY_TO_CLASS(weapon_glock, CWeaponGlock)
LINK_ENTITY_TO_CLASS(weapon_9mmhandgun, CWeaponGlock)

void CWeaponGlock::Spawn()
{
	SetClassName("weapon_9mmhandgun"); // TODO: gpEngine->pfnMakeString(classname)
	
	//
	// TODO: Precache();
	//
	gpEngine->pfnPrecacheModel("models/v_9mmhandgun.mdl");
	gpEngine->pfnPrecacheModel("models/p_9mmhandgun.mdl");
	gpEngine->pfnPrecacheModel("models/w_9mmhandgun.mdl");
	//
	
	mnID = WEAPON_GLOCK;
	
	SetModel("models/w_9mmhandgun.mdl");
};

void CWeaponGlock::PrimaryAttack()
{
	Fire();
};

void CWeaponGlock::SecondaryAttack()
{
	Fire();
};

void CWeaponGlock::Fire()
{
	if(mnClip <= 0)
	{
		return;
	};
	
	--mnClip;
	
	mpOwner->SetAnim(PLAYER_ATTACK1);
};