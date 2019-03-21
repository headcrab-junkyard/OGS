/// @file

#pragma once

#include "BaseEntity.hpp"

class CBaseCharacter : public CBaseEntity
{
public:
	void TakeDamage(float afAmount, CBaseEntity *weapon, CBaseEntity *attacker) override;
private:
	void DropQuad();
	void DropRing();
	void DropBackback();
	
	void DeathSound();
	void DeathBubbles(float num_bubbles);
	
	int GetBestWeapon() const;
	bool CheckNoAmmo();
	
	int currentammo;
};