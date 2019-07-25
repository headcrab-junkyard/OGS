/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
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

#include "GameWorld.hpp"
#include "engine.h"
#include "Util.hpp"
#include "BaseEntity.hpp"

CGameWorld::CGameWorld() = default;
CGameWorld::~CGameWorld() = default;

CBaseEntity *CGameWorld::GetEntity(int anIndex) const
{
	return ToBaseEntity(gpEngine->pfnPEntityOfEntIndex(anIndex));
};

CBaseEntity *CGameWorld::GetEntity(const char *asClassName) const
{
	return FindEntityByString(nullptr, "classname", asClassName);
};

int CGameWorld::GetEntityCount() const
{
	return gpEngine->pfnNumberOfEntities();
};

int CGameWorld::GetPointContents(const idVec3 &avOrigin) const
{
	return gpEngine->pfnPointContents(avOrigin);
};

CBaseEntity *CGameWorld::SpawnEntity(const char *asClassName, const idVec3 &avOrigin = idVec3::Origin, const idVec3 &avAngles = idVec3::Origin, CBaseEntity *apOwner = nullptr)
{
	auto pEntity{ToBaseEntity(gpEngine->pfnCreateNamedEntity(gpEngine->pfnMakeString(asClassName)))};
	
	if(!pEntity)
		return nullptr;
	
	//pEntity->SetClassName(asClassName); // TODO
	pEntity->SetOrigin(avOrigin);
	pEntity->SetAngles(avAngles);
	pEntity->SetOwner(apOwner);
	return pEntity;
};

void CGameWorld::DestroyEntity(CBaseEntity &aEntity)
{
	//aEntity.UpdateOnRemove(); // TODO
	aEntity.MarkForDeletion();
};

void CGameWorld::SetLightStyle(int anStyle, const char *asValue)
{
	gpEngine->pfnLightStyle(anStyle, asValue);
};

void CGameWorld::CreateStaticDecal(const idVec3 &avOrigin, int anDecalIndex, int anEntityIndex, int anModelIndex)
{
	gpEngine->pfnStaticDecal(avOrigin, anDecalIndex, anEntityIndex, anModelIndex);
};

void CGameWorld::CreateParticleEffect(const idVec3 &origin, const idVec3 &direction, float color, float count)
{
	gpEngine->pfnParticleEffect(origin, direction, color, count);
};

/*
================
SpawnBlood
================
*/
void CGameWorld::SpawnBlood(const idVec3 &avOrigin, float afDamage) // TODO: afDamage is unused
{
	gpEngine->pfnWriteByte(MSG_MULTICAST, SVC_TEMPENTITY);
	gpEngine->pfnWriteByte(MSG_MULTICAST, TE_BLOOD);
	gpEngine->pfnWriteByte(MSG_MULTICAST, 1);
	gpEngine->pfnWriteCoord(MSG_MULTICAST, avOrigin.x);
	gpEngine->pfnWriteCoord(MSG_MULTICAST, avOrigin.y);
	gpEngine->pfnWriteCoord(MSG_MULTICAST, avOrigin.z);
	gpEngine->pfnMulticast(avOrigin, MULTICAST_PVS);
};

void CGameWorld::ChangeLevel(const char *asName, const char *asStartSpot)
{
	gpEngine->pfnChangeLevel(asName, asStartSpot);
};

CBaseEntity *CGameWorld::FindEntityInSphere(CBaseEntity *pEntStartSearchAfter, const idVec3 &avOrigin, float afRadius) const
{
	return ToBaseEntity(gpEngine->pfnFindEntityInSphere(pEntStartSearchAfter->ToEdict(), avOrigin, afRadius));
};

CBaseEntity *CGameWorld::FindEntityByString(CBaseEntity *pEntStartSearchAfter, const char *sField, const char *sValue) const
{
	return ToBaseEntity(gpEngine->pfnFindEntityByString(pEntStartSearchAfter->ToEdict(), sField, sValue));
};

/*
============
T_RadiusDamage
============
*/
void CGameWorld::RadiusDamage(CBaseEntity *inflictor, CBaseEntity *attacker, float damage, CBaseEntity *ignore, string_t dtype)
{
	auto head{FindEntityInSphere(nullptr, inflictor->GetOrigin(), damage + 40)};
	
	while (head)
	{
		//bprint (PRINT_HIGH, head->GetClassName());
		//bprint (PRINT_HIGH, " | ");
		//bprint (PRINT_HIGH, head->netname);
		//bprint (PRINT_HIGH, "\n");
	
		if (head != ignore)
		{
			if (head->v.takedamage) // TODO: ->IsDamageable()
			{
				idVec3 org = head->GetOrigin() + (head->GetSize().mins + head->GetSize().maxs) * 0.5;
				float points = 0.5 * (inflictor->GetOrigin() - org).Length();
				if (points < 0)
					points = 0;
				points = damage - points;
				
				if (head == attacker)
					points = points * 0.5;
				if (points > 0)
				{
					if (CanDamage(head, inflictor)) // TODO: mpGameRules->CanDamage
					{
						head->v.deathtype = dtype; // TODO: SetDeathType
						head->TakeDamage(inflictor, attacker, points);
					};
				};
			};
		};
		head = FindEntityInSphere(head, inflictor->GetOrigin(), damage + 40);
	};
};

void CGameWorld::TraceLine(const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, CBaseEntity *pEntToSkip, TraceResult *pTraceResult)
{
	gpEngine->pfnTraceLine(vOriginA, vOriginB, bNoMonsters, pEntToSkip->ToEdict(), pTraceResult);
};

void CGameWorld::TraceToss(CBaseEntity *pEntity, CBaseEntity *pEntToIgnore, TraceResult *pTraceResult)
{
	gpEngine->pfnTraceToss(pEntity->ToEdict(), pEntToIgnore->ToEdict(), pTraceResult);
};

int CGameWorld::TraceMonsterHull(CBaseEntity *pEdict, const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, CBaseEntity *pEntToSkip, TraceResult *pTraceResult)
{
	return gpEngine->pfnTraceMonsterHull(pEdict->ToEdict(), vOriginA, vOriginB, bNoMonsters, pEntToSkip->ToEdict(), pTraceResult);
};

void CGameWorld::TraceHull(const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, int hullNumber, CBaseEntity *pEntToSkip, TraceResult *pTraceResult)
{
	gpEngine->pfnTraceHull(vOriginA, vOriginB, bNoMonsters, hullNumber, pEntToSkip->ToEdict(), pTraceResult);
};

void CGameWorld::TraceModel(const idVec3 &vOriginA, const idVec3 &vOriginB, int hullNumber, CBaseEntity *pent, TraceResult *pTraceResult)
{
	gpEngine->pfnTraceModel(vOriginA, vOriginB, hullNumber, pent->ToEdict(), pTraceResult);
};

const char *CGameWorld::TraceTexture(CBaseEntity *pTextureEntity, const idVec3 &vOriginA, const idVec3 &vOriginB)
{
	return gpEngine->pfnTraceTexture(pTextureEntity->ToEdict(), vOriginA, vOriginB);
};

void CGameWorld::TraceSphere(const idVec3 &vOriginA, const idVec3 &vOriginB, bool bNoMonsters, float radius, CBaseEntity *pEntToSkip, TraceResult *pTraceResult)
{
	gpEngine->pfnTraceSphere(vOriginA, vOriginB, bNoMonsters, radius, pEntToSkip->ToEdict(), pTraceResult);
};

// TODO
/*
void (*pfnEmitSound)(edict_t *ent, int channel, const char *sample, float volume, float attenuation, int nFlags, int pitch);
void (*pfnEmitAmbientSound)(edict_t *ent, vec3_t origin, const char *sample, float volume, float attenuation, int nFlags, int pitch);
*/