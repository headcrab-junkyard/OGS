/// @file

#include "BaseAmmo.hpp"

class CAmmoBuckshot : public CBaseAmmo
{
public:
	void PostSpawn() override;
	
	bool GiveTo(CBasePlayer *apPlayer) override;
};

LINK_ENTITY_TO_CLASS(ammo_buckshot, CAmmoBuckshot)

void CAmmoBuckshot::PostSpawn()
{
	gpEngine->pfnPrecacheModel("models/w_shotbox.mdl");
	
	SetModel("models/w_shotbox.mdl");
};

bool CAmmoBuckshot::GiveTo(CBasePlayer *apPlayer)
{
	apPlayer->GiveAmmo("buckshot");
	return true;
};