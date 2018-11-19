/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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

typedef struct edict_s edict_t;

class CBaseEntity
{
public:
	using pfnThinkCallback = void (CBaseEntity::*)();
	using pfnTouchCallback = void (CBaseEntity::*)(CBaseEntity *apOther);
	using pfnUseCallback = void (CBaseEntity::*)(CBaseEntity *apOther);
	using pfnBlockedCallback = void (CBaseEntity::*)(CBaseEntity *apOther);
public:
	virtual ~CBaseEntity() = default;
	
	virtual void Think()
	{
		if(mfnThinkCallback)
			mfnThinkCallback();
	};
	virtual void Touch(CBaseEntity *other)
	{
		if(mfnTouchCallback)
			mfnTouchCallback(other);
	};
	virtual void Use(CBaseEntity *other)
	{
		if(mfnUseCallback)
			mfnUseCallback(other);
	};
	virtual void Blocked(CBaseEntity *other)
	{
		if(mfnBlockedCallback)
			mfnBlockedCallback(other);
	};
	
	virtual void Spawn(){}
	
	void SetName(const char *asName);
	const char *GetName() const;
	
	const char *GetClassName() const;
	
	void SetThinkCallback(pfnThinkCallback afnCallback); // TODO: IEntityThinkCallback?
	void SetTouchCallback(pfnTouchCallback afnCallback); // TODO: IEntityTouchCallback?
	void SetUseCallback(pfnUseCallback afnCallback); // TODO: IEntityUseCallback?
	void SetBlockedCallback(pfnBlockedCallback afnCallback); // TODO: IEntityBlockedCallback?
	
	void SetNextThink(float afTime){self->v.nextthink = afTime;}
	float GetNextThink() const {return self->v.nextthink;}
	
	void SetKeyValue(const char *asKey, const char *asValue);
	const char *GetKeyValue(const char *asKey) const;
	
	void SetHealth(float afHealth);
	float GetHealth() const;
	
	//void SetMaxHealth(float afValue);
	//float GetMaxHealth() const;
	
	void SetVelocity(const Vector3 &avVelocity);
	const Vector3 &GetVelocity() const;
	
	void SetAngles(const vec3_t avAngles);
	vec3_t GetAngles();
	
	void SetModel(const char *asName);
	const char *GetModel() const;
	
	void SetOrigin(const Vec3 &avOrigin);
	const Vec3 &GetOrigin() const;
	
	void SetSize(const CVec3 &avSize);
	const Vec3 &GetSize() const;
	
	void SetMoveType(int anType){self->v.movetype = anType;}
	int GetMoveType() const {return self->v.movetype;}
	
	void SetFlags(int anFlags){self->v.flags = anFlags;}
	int GetFlags() const {return self->v.flags;}
	
	void SetEffects(int anEffects){self->v.effects = anEffects;}
	//void AddEffects();
	int GetEffects() const {return self->v.effects;}
	
	void EmitSound(int anChannel, const char *asSample, float afVolume, float afAttenuation, int anFlags, int anPitch)
	{
		gpEngine->pfnEmitSound(self, anChannel, asSample, afVolume, afAttenuation, anFlags, anPitch;
	};
	
	int GetWaterType() const {return self->v.watertype;}
	int GetWaterLevel() const {return self->v.waterlevel;}
	
	void SetEnemy(CBaseEntity *apEnemy);
	CBaseEntity *GetEnemy() const;
private:
	edict_t *self{nullptr};
	
	pfnThinkCallback mfnThinkCallback{nullptr};
	pfnTouchCallback mfnTouchCallback{nullptr};
	pfnUseCallback mfnUseCallback{nullptr};
	pfnBlockedCallback mfnBlockedCallback{nullptr};
};