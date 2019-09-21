/// @file

#pragma once

#include "BaseEntity.hpp"

class CBaseAmmo : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Touch(CBaseEntity *other) override;
	
	virtual void GiveTo(CBasePlayer *apPlayer){}
protected:
	virtual void PostSpawn(){}
};