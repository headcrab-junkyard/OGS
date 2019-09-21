/// @file

#include "BaseAmmo.hpp"

class CAmmo9MM : public CBaseAmmo
{
public:
	bool GiveTo(CBasePlayer *apPlayer) override;
private:
	void PostSpawn() override;
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