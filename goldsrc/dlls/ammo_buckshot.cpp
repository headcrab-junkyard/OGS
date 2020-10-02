/// @file

#include "BaseAmmo.hpp"

class CAmmoBuckshot : public CBaseAmmo
{
public:
	bool GiveTo(CBasePlayer *apPlayer) override;
private:
	void PostSpawn() override;
};

LINK_ENTITY_TO_CLASS(ammo_buckshot, CAmmoBuckshot)

void CAmmoBuckshot::PostSpawn()
{
	// Precache()
	gpEngine->pfnPrecacheModel("models/w_shotbox.mdl");
	//
	
	SetModel("models/w_shotbox.mdl");
};

bool CAmmoBuckshot::GiveTo(CBasePlayer *apPlayer)
{
	if(apPlayer->GiveAmmo("buckshot"))
	{
		apPlayer->EmitSound(CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		return true;
	};
	return false;
};