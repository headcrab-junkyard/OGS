/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2019 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include <string>

#include "edict.h"
#include "engine.h"
#include "mathlib/vec3.h"
#include "mathlib/bounds.h"

//using edict_t = struct edict_s;

class Bounds;
class CGameWorld;

class CBaseEntity
{
public:
	using pfnThinkCallback = void (CBaseEntity::*)();
	using pfnTouchCallback = void (CBaseEntity::*)(CBaseEntity *apOther);
	using pfnUseCallback = void (CBaseEntity::*)(CBaseEntity *apOther);
	using pfnBlockedCallback = void (CBaseEntity::*)(CBaseEntity *apOther);
public:
	//CBaseEntity(entvars_t *apData);
	virtual ~CBaseEntity() = default;
	
	edict_t *ToEdict() const {return gpEngine->pfnFindEntityByVars(self);}
	
	virtual void Think()
	{
		if(mfnThinkCallback)
			(this->*mfnThinkCallback)();
	};
	virtual void Touch(CBaseEntity *other)
	{
		if(mfnTouchCallback)
			(this->*mfnTouchCallback)(other);
	};
	virtual void Use(CBaseEntity *other)
	{
		if(mfnUseCallback)
			(this->*mfnUseCallback)(other);
	};
	virtual void Blocked(CBaseEntity *other)
	{
		if(mfnBlockedCallback)
			(this->*mfnBlockedCallback)(other);
	};
	
	/// @return false if the entity shouldn't be spawn at all (instead of deleting itself on spawn)
	virtual bool PreSpawn() const {return true;}
	
	virtual void Spawn(){}
	
	virtual void TraceAttack(CBaseEntity *apAttacker, float damage, const idVec3 &dir, const TraceResult &aTraceResult, int anDmgBitSum);
	virtual void TakeDamage(CBaseEntity *inflictor, CBaseEntity *attacker, float damage, int anDmgBitSum); // TODO: was T_Damage
	virtual void Killed(CBaseEntity *attacker);
	
	void FireBullets(float shotcount, const idVec3 &dir, const idVec3 &spread);
	
	void SetName(const std::string &asName);
	const std::string &GetName() const;
	
	const std::string &GetClassName() const;
	
	void SetThinkCallback(pfnThinkCallback afnCallback); // TODO: IEntityThinkCallback?
	void SetTouchCallback(pfnTouchCallback afnCallback); // TODO: IEntityTouchCallback?
	void SetUseCallback(pfnUseCallback afnCallback); // TODO: IEntityUseCallback?
	void SetBlockedCallback(pfnBlockedCallback afnCallback); // TODO: IEntityBlockedCallback?
	
	void SetNextThink(float afTime){self->nextthink = afTime;}
	float GetNextThink() const {return self->nextthink;}
	
	//void SetKeyValue(const std::string &asKey, const std::string &asValue);
	//const std::string &GetKeyValue(const std::string &asKey) const;
	bool HandleKeyValue(const std::string &asKey, const std::string &asValue)
	{
		return false;
	};
	
	bool IsValid() const {return (!ToEdict() || ToEdict()->free || self->flags & FL_KILLME) ? false : true;}
	
	int GetIndex() const {return gpEngine->pfnIndexOfEdict(ToEdict());}
	
	void SetHealth(float afHealth){self->health = afHealth;}
	float GetHealth() const {return self->health;}
	
	// TODO: should these be here?
	void SetMaxHealth(float afValue){self->max_health = afValue;}
	float GetMaxHealth() const {return self->max_health;}
	
	void SetArmorType(int anType){self->armortype = anType;}
	int GetArmorType() const {return self->armortype;}
	
	void SetArmorValue(int anValue){self->armorvalue = anValue;}
	int GetArmorValue() const {return self->armorvalue;}
	
	void SetVelocity(const idVec3 &avVelocity)
	{
		mvVelocity = avVelocity;
		
		self->velocity[0] = avVelocity.x;
		self->velocity[1] = avVelocity.y;
		self->velocity[2] = avVelocity.z;
	};
	const idVec3 &GetVelocity() const {return mvVelocity;}
	
	void SetAngles(const idVec3 &avAngles)
	{
		mvAngles = avAngles;
		
		self->angles[0] = avAngles.x;
		self->angles[1] = avAngles.y;
		self->angles[2] = avAngles.z;
	};
	const idVec3 &GetAngles() const {return mvAngles;}
	
	void SetGravity(float afY){self->gravity = afY;}
	//const idVec3 &GetGravity() const {return idVec3(0.0f, self->gravity, 0.0f);}
	
	void SetModel(const std::string &asName)
	{
		gpEngine->pfnSetModel(ToEdict(), asName.c_str());
	};
	const std::string &GetModel() const;
	
	void SetOrigin(const idVec3 &avOrigin)
	{
		mvOrigin = avOrigin;
		gpEngine->pfnSetOrigin(ToEdict(), avOrigin);
	};
	const idVec3 &GetOrigin() const {return mvOrigin;}
	
	void SetSize(const Bounds &aSize)
	{
		gpEngine->pfnSetSize(ToEdict(), aSize.mins, aSize.maxs);
	};
	
	void SetSize(const idVec3 &avMins, const idVec3 &avMaxs)
	{
		gpEngine->pfnSetSize(ToEdict(), avMins, avMaxs);
	};
	const Bounds &GetSize() const;
	
	void SetMoveType(int anType){self->movetype = anType;}
	int GetMoveType() const {return self->movetype;}
	
	void SetSolidity(int anSolidity);
	int GetSolidity() const;
	
	void SetFlags(int anFlags){self->flags = anFlags;}
	//void AddFlags(int anFlags){self->flags |= anFlags;}
	int GetFlags() const {return self->flags;}
	
	void SetEffects(int anEffects){self->effects = anEffects;}
	//void AddEffects(int anEffects){self->effects |= anEffects;|
	int GetEffects() const {return self->effects;}
	
	void SetSkin(int anSkin){self->skin = anSkin;}
	int GetSkin() const {return self->skin;}
	
	void EmitSound(int anChannel, const std::string &asSample, float afVolume, float afAttenuation, int anFlags, int anPitch)
	{
		gpEngine->pfnEmitSound(ToEdict(), anChannel, asSample.c_str(), afVolume, afAttenuation, anFlags, anPitch);
	};
	
	void MarkForDeletion()
	{
		//self->flags |= FL_KILLME; // TODO
	};
	
	int GetWaterType() const {return self->watertype;}
	int GetWaterLevel() const {return self->waterlevel;}
	
	void SetOwner(CBaseEntity *apOwner){mpOwner = apOwner;}
	CBaseEntity *GetOwner() const {return mpOwner;}
	
	void SetEnemy(CBaseEntity *apEnemy){mpEnemy = apEnemy;}
	CBaseEntity *GetEnemy() const {return mpEnemy;}
	
	void SetGoal(CBaseEntity *apGoal){mpGoal = apGoal;}
	CBaseEntity *GetGoal() const {return mpGoal;}
private:
	idVec3 mvOrigin{0.0f};
	idVec3 mvAngles{0.0f};
	idVec3 mvVelocity{0.0f};
public:	
	entvars_t *self{nullptr};
	CGameWorld *mpWorld{nullptr};
private:	
	pfnThinkCallback mfnThinkCallback{nullptr};
	pfnTouchCallback mfnTouchCallback{nullptr};
	pfnUseCallback mfnUseCallback{nullptr};
	pfnBlockedCallback mfnBlockedCallback{nullptr};
	
	CBaseEntity *mpOwner{nullptr};
	CBaseEntity *mpEnemy{nullptr};
	CBaseEntity *mpGoal{nullptr};
};