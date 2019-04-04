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
#include "Game.hpp"
#include "GameSetup_Default.hpp"

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
	auto pGameSetup{new CGameSetup_Default()}; // TODO
	
	// Initialize the game (master) class
	if(!gpGame)
		gpGame = new CGame(pGameSetup->CreateRules(), pGameSetup->CreateWorld());
	//gEngFuncs.pfnPrecacheModel("models/player.mdl"); // TODO: studio models are not supported yet...
	
	gpGame->Init();
};

int EntitySpawn(edict_t *pent)
{
	auto pBaseEntity{ToBaseEntity(pent)};
	
	if(pBaseEntity)
		pBaseEntity->Spawn();
	
	return 1;
};

void EntityThink(edict_t *pent)
{
	ToBaseEntity(pent)->Think();
};

void EntityUse(edict_t *pent, edict_t *pother)
{
	ToBaseEntity(pent)->Use(ToBaseEntity(pother));
};

void EntityTouch(edict_t *pent, edict_t *pother)
{
	ToBaseEntity(pent)->Touch(ToBaseEntity(pother));
};

void EntityBlocked(edict_t *pent, edict_t *pother)
{
	ToBaseEntity(pent)->Blocked(ToBaseEntity(pother));
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
	//ToBaseEntity(pent)->Save(); // TODO
};

int EntityRestore(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData, int globalentity)
{
	//ToBaseEntity(pent)->Restore(); // TODO
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
	gpGame->Shutdown();
};

int ShouldCollide(edict_t *pent, edict_t *pother)
{
	return gpGame->GetRules()->ShouldCollide(ToBaseEntity(pent), ToBaseEntity(pother));
};

void CvarValue(const edict_t *pent, const char *value){};

void CvarValue2(const edict_t *pent, int requestid, const char *cvarname, const char *value){};