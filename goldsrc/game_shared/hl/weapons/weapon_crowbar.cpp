/// @file

#include "BaseWeapon.hpp"

class CWeaponCrowbar : public CBaseWeapon
{
public:
	void Spawn() override;
	
	void PrimaryAttack() override;
private:
	void Swing();
};

LINK_ENTITY_TO_CLASS(weapon_crowbar, CWeaponCrowbar)

void CWeaponCrowbar::Spawn()
{
	mnID = WEAPON_CROWBAR;
	
	SetModel("models/w_crowbar.mdl");
	
	mnClip = -1;
};

void CWeaponCrowbar::PrimaryAttack()
{
	Swing();
};

void CWeaponCrowbar::Swing()
{
	mfNextPrimaryAttack = 0.5;
	
	mpOwner->SetAnimation(PLAYER_ATTACK1);
};