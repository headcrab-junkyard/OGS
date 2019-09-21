/// @file

#include "BaseAmmo.hpp"

class CAmmo9MM : public CBaseAmmo
{
public:
	void PostSpawn() override;
	
	bool GiveTo(CBasePlayer *apPlayer) override;
};

LINK_ENTITY_TO_CLASS(ammo_glockclip, CAmmo9MM)
LINK_ENTITY_TO_CLASS(ammo_9mmclip, CAmmo9MM)

LINK_ENTITY_TO_CLASS(ammo_9mmbox, CAmmo9MM)

void CAmmo9MM::PostSpawn()
{
	SetModel("models/w_9mmclip.mdl");
};

bool CAmmo9MM::GiveTo(CBasePlayer *apPlayer)
{
	return true;
};