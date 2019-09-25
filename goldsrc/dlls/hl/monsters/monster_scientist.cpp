/// @file

#include "BaseMonster.hpp" // TODO: #include "TalkingMonster.hpp"

class CMonsterScientist : public CBaseMonster // TODO: CTalkingMonster or component
{
public:
	void Spawn() override;
	
	int TakeDamage(CBaseEntity *apInflictor, CBaseEntity *apAttacker, float afDamage, int anDmgBitSum) override;
};

LINK_ENTITY_TO_CLASS(monster_scientist, CMonsterScientist)

void CMonsterScientist::Spawn()
{
	// Precache()
	//
	gpEngine->pfnPrecacheModel("models/scientist.mdl");
	//
	
	SetModel("models/scientist.mdl");
	SetSize(VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	SetSolidity(SOLID_SLIDEBOX);
	SetMoveType(MOVETYPE_STEP);
	SetBloodColor(BloodColor::Red); // TODO: Color::Red?
	SetHealth(gSkillData.scientistHealth);
};

int CMonsterScientist::TakeDamage(CBaseEntity *apInflictor, CBaseEntity *apAttacker, float afDamage, int anDmgBitSum)
{
	return 0;
};