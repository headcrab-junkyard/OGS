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

LINK_ENTITY_TO_CLASS(ammo_9mmbox, CAmmo9MM)

void CAmmo9MMClip::PostSpawn()
{
	SetModel("models/w_9mmclip.mdl");
};

bool CAmmo9MMClip::GiveTo(CBasePlayer *apPlayer)
{
	return true;
};