/// @file

#include "BaseAmmo.hpp"

class CAmmo9MMClip : public CBaseAmmo
{
public:
	bool GiveTo(CBasePlayer *apPlayer) override;
private:
	void PostSpawn() override;
};

LINK_ENTITY_TO_CLASS(ammo_glockclip, CAmmo9MMClip)
LINK_ENTITY_TO_CLASS(ammo_9mmclip, CAmmo9MMClip)

void CAmmo9MMClip::PostSpawn()
{
	SetModel("models/w_9mmclip.mdl");
};

bool CAmmo9MMClip::GiveTo(CBasePlayer *apPlayer)
{
	return true;
};

class CAmmo9MMBox : public CBaseAmmo
{
public:
	bool GiveTo(CBasePlayer *apPlayer) override;
private:
	void PostSpawn() override;
};

LINK_ENTITY_TO_CLASS(ammo_9mmbox, CAmmo9MMBox)

void CAmmo9MMBox::PostSpawn()
{
	SetModel("models/w_9mmbox.mdl");
};

bool CAmmo9MMBox::GiveTo(CBasePlayer *apPlayer)
{
	return false;
};