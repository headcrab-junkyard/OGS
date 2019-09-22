/// @file

//#include "exports.h"
//#include "engine.h"
#include "BaseWeapon.hpp"

class CWeaponSMG : public CBaseWeapon
{
public:
	void Spawn() override;
	
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	
	void Reload() override;
};

LINK_ENTITY_TO_CLASS(weapon_9mmAR, CWeaponSMG)
LINK_ENTITY_TO_CLASS(weapon_mp5, CWeaponSMG)

void CWeaponSMG::Spawn()
{
	SetClassName("weapon_9mmAR"); // TODO: gpEngine->pfnMakeString(classname)
	
	//
	// TODO: Precache();
	//
	gpEngine->pfnPrecacheModel("models/v_9mmAR.mdl");
	gpEngine->pfnPrecacheModel("models/p_9mmAR.mdl");
	gpEngine->pfnPrecacheModel("models/w_9mmAR.mdl");
	
	gpEngine->pfnPrecacheModel("models/grenade.mdl");
	//
	
	mnID = WEAPON_SMG; // TODO: WEAPON_MP5
	
	SetModel("models/w_9mmAR.mdl");
};

void CWeaponSMG::PrimaryAttack()
{
	if(mnClip <= 0)
	{
		PlayEmptySound();
		mfNextPrimaryAttack = 0.15;
		return;
	};
	
	--mnClip;
	
	mpOwner->SetAnim(PLAYER_ATTACK1);
};

void CWeaponSMG::SecondaryAttack()
{
	mpOwner->SetAnim(PLAYER_ATTACK1);
};

void CWeaponSMG::Reload()
{
	if(mpOwner->GetAmmoCount("9mm") <= 0)
		return;
};