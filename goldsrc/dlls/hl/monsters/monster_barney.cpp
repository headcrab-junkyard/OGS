/// @file

#include "BaseMonster.hpp" // TODO: #include "TalkingMonster.hpp"

class CMonsterBarney : public CBaseMonster // TODO: CTalkingMonster or component
{
public:
	void Spawn() override;
	
	int TakeDamage(CBaseEntity *apInflictor, CBaseEntity *apAttacker, float afDamage, int anDmgBitSum) override;
};

LINK_ENTITY_TO_CLASS(monster_barney, CMonsterBarney)

void CMonsterBarney::Spawn()
{
	//Precache()
	//
	gpEngine->pfnPrecacheModel("models/barney.mdl");
	//
	
	SetModel("models/barney.mdl");
	SetSize(VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	SetSolidity(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_STEP);
	SetBloodColor(BloodColor::Red); // TODO: Color::Red?
	SetHealth(gSkillData.barneyHealth);
};

int CMonsterBarney::TakeDamage(CBaseEntity *apInflictor, CBaseEntity *apAttacker, float afDamage, int anDmgBitSum)
{
	return 0;
};