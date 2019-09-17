/// @file

#include "BaseEntity.hpp"

class CBaseDelay : public CBaseEntity
{
public:
	bool HandleKeyValue(KeyValueData *apKVData) override;
	
	//int Save(CSaveData &aSaveData);
	//int Restore(CRestoreData &aRestoreData);
	
	void DelayThink();
	
	void SUB_UseTargets(CBaseEntity *apActivator, USE_TYPE aeUseType, float afValue);
};