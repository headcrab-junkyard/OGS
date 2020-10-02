/*
 * This file is part of OGS Engine
 * Copyright (C) 2019 BlackPhrase
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

class idVec3;
class CBaseEntity;

using string_t = unsigned int;
using TraceResult = struct TraceResult_s;

/*
Shared
int CBaseEntity::entindex()

Server
int ENTINDEX( edict_t* pEdict )
int engine->IndexOfEdict(edict_t)
*/

class CGameWorld
{
public:
	CGameWorld();
	~CGameWorld();
	
	///
	CBaseEntity *GetEntity(int anIndex) const;
	
	///
	CBaseEntity *GetEntity(const char *asClassName) const;

	///
	int GetEntityCount() const;
	
	///
	int GetPointContents(const idVec3 &avOrigin) const;
	
	///
	CBaseEntity *SpawnEntity(const char *asClassName, const idVec3 &avOrigin, const idVec3 &avAngles, CBaseEntity *apOwner);
	
	///
	void DestroyEntity(CBaseEntity &aEntity); // TODO: RemoveEntity? TODO: bool abImmediate = false / float afTime?
	
	///
	void SetLightStyle(int anStyle, const char *asValue);
	
	///
	void CreateStaticDecal(const idVec3 &avOrigin, int anDecalIndex, int anEntityIndex, int anModelIndex);
	
	///
	void CreateParticleEffect(const idVec3 &origin, const idVec3 &direction, float color, float count);
	
	///
	void SpawnBlood(const idVec3 &avOrigin, float afDamage);
	
	///
	void ChangeLevel(const char *asName, const char *asStartSpot = "");
	
	///
	CBaseEntity *FindEntityInSphere(CBaseEntity *pEntStartSearchAfter, const idVec3 &avOrigin, float afRadius) const;
	
	///
	CBaseEntity *FindEntityByString(CBaseEntity *pEntStartSearchAfter, const char *sField, const char *sValue) const;
	
	///
	void RadiusDamage(CBaseEntity *inflictor, CBaseEntity *attacker, float damage, CBaseEntity *ignore, string_t dtype);
	
	///
	void TraceLine(const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, CBaseEntity *pEntToSkip, TraceResult *pTraceResult);
	
	///
	void TraceToss(CBaseEntity *pEntity, CBaseEntity *pEntToIgnore, TraceResult *pTraceResult);
	
	///
	int TraceMonsterHull(CBaseEntity *pEdict, const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, CBaseEntity *pEntToSkip, TraceResult *pTraceResult);
	
	///
	void TraceHull(const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, int hullNumber, CBaseEntity *pEntToSkip, TraceResult *pTraceResult);
	
	///
	void TraceModel(const idVec3 &vOriginA, const idVec3 &vOriginB, int hullNumber, CBaseEntity *pent, TraceResult *pTraceResult);
	
	///
	const char *TraceTexture(CBaseEntity *pTextureEntity, const idVec3 &vOriginA, const idVec3 &vOriginB);
	
	///
	void TraceSphere(const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, float radius, CBaseEntity *pEntToSkip, TraceResult *pTraceResult);
};