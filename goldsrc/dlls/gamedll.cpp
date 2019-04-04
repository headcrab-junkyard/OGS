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

#include "exports.h"
#include "engine.h"
#include "BaseEntity.hpp"

inline CBaseEntity *ToBaseEntity(edict_t *apEdict)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(apEdict)};
	
	if(!pPrivateData)
		return nullptr;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	if(!pBaseEntity)
		return nullptr;
	
	return pBaseEntity;
};

void GameInit()
{
	//gEngFuncs.pfnPrecacheModel("models/player.mdl"); // TODO: studio models are not supported yet...
};

int EntitySpawn(edict_t *pent)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	
	if(!pPrivateData)
		return 0;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	pBaseEntity->Spawn();
	return 1;
};

void EntityThink(edict_t *pent)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	
	if(!pPrivateData)
		return;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	pBaseEntity->Think();
};

void EntityUse(edict_t *pent, edict_t *pother)
{
	auto pEntPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	auto pOtherPrivateData{gpEngine->pfnPvEntPrivateData(pother)};
	
	if(!pEntPrivateData || !pOtherPrivateData)
		return;
	
	auto pEntity{reinterpret_cast<CBaseEntity*>(pEntPrivateData)};
	auto pOtherEntity{reinterpret_cast<CBaseEntity*>(pOtherPrivateData)};
	
	pEntity->Use(pOtherEntity);
};

void EntityTouch(edict_t *pent, edict_t *pother)
{
	auto pEntPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	auto pOtherPrivateData{gpEngine->pfnPvEntPrivateData(pother)};
	
	if(!pEntPrivateData || !pOtherPrivateData)
		return;
	
	auto pEntity{reinterpret_cast<CBaseEntity*>(pEntPrivateData)};
	auto pOtherEntity{reinterpret_cast<CBaseEntity*>(pOtherPrivateData)};
	
	pEntity->Touch(pOtherEntity);
};

void EntityBlocked(edict_t *pent, edict_t *pother)
{
	auto pEntPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	auto pOtherPrivateData{gpEngine->pfnPvEntPrivateData(pother)};
	
	if(!pEntPrivateData || !pOtherPrivateData)
		return;
	
	auto pEntity{reinterpret_cast<CBaseEntity*>(pEntPrivateData)};
	auto pOtherEntity{reinterpret_cast<CBaseEntity*>(pOtherPrivateData)};
	
	pEntity->Blocked(pOtherEntity);
};

void EntityKeyValue(edict_t *pent, KeyValueData *pkvd)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	
	if(!pPrivateData)
		return;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	//pkvd->bHandled = 0;
	//pBaseEntity->HandleKeyValue(pkvd); // TODO
	
	//auto bHandled{pBaseEntity->HandleKeyValue(pkvd)};
	//if(bHandled)
		//pkvd->bHandled = 1;
};

void EntitySave(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	
	if(!pPrivateData)
		return;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	//pBaseEntity->Save(); // TODO
};

int EntityRestore(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData, int globalentity)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(pent)};
	
	if(!pPrivateData)
		return 0;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	//pBaseEntity->Restore(); // TODO
	return 1;
};

void EntitySetAbsPos(edict_t *pent){};

void SaveWriteFields(SAVERESTOREDATA *pSaveRestoreData, const char *sName, void *pBaseData, TYPEDESCRIPTION *pFields, int nFieldCount){};

void SaveReadFields(SAVERESTOREDATA *pSaveRestoreData, const char *sName, void *pBaseData, TYPEDESCRIPTION *pFields, int nFieldCount){};

void SaveGlobalState(SAVERESTOREDATA *pSaveRestoreData){};

void RestoreGlobalState(SAVERESTOREDATA *pSaveRestoreData){};

void ResetGlobalState(){};

void PM_Init_Game(struct playermove_s *ppmove){};

void PM_Move_Game(struct playermove_s *ppmove, qboolean server){};

char PM_FindTextureType_Game(const char *name)
{
	return '\0';
};

//

void OnFreeEntPrivateData(edict_t *pent){};

void GameShutdown()
{
	// NOTE: free resources here
};

int ShouldCollide(edict_t *pent, edict_t *pother)
{
	return 0;
};

void CvarValue(const edict_t *pent, const char *value){};

void CvarValue2(const edict_t *pent, int requestid, const char *cvarname, const char *value){};