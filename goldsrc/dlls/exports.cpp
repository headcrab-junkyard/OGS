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

#include <cstring>

#include "exports.h"
#include "engine.h"

//
// required prog functions
//
/*

void()          PutClientInServer;              // call after setting the parm1... parms

void()          SetNewParms;                    // called when a client first connects to
									// a server. sets parms so they can be
									// saved off for restarts

void()          SetChangeParms;                 // call to set parms for self so they can
									// be saved for a level transition
*/

typedef struct globalvars_s globalvars_t;

extern void GameInit();
extern int EntitySpawn(edict_t *pent);
extern void EntityThink(edict_t *pent);
extern void EntityUse(edict_t *pent, edict_t *pother);
extern void EntityTouch(edict_t *pent, edict_t *pother);
extern void EntityBlocked(edict_t *pent, edict_t *pother);
extern void EntityKeyValue(edict_t *pent, KeyValueData *pkvd);
extern void EntitySave(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData);
extern int EntityRestore(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData, int globalentity);
extern void EntitySetAbsPos(edict_t *pent);
extern void SaveWriteFields(SAVERESTOREDATA *pSaveRestoreData, const char *sName, void *pBaseData, TYPEDESCRIPTION *pFields, int nFieldCount);
extern void SaveReadFields(SAVERESTOREDATA *pSaveRestoreData, const char *sName, void *pBaseData, TYPEDESCRIPTION *pFields, int nFieldCount);
extern void SaveGlobalState(SAVERESTOREDATA *pSaveRestoreData);
extern void RestoreGlobalState(SAVERESTOREDATA *pSaveRestoreData);
extern void ResetGlobalState();
extern qboolean ClientConnect(edict_t *pEntity, const char *name, const char *adr, char sRejectReason[128]);
extern void ClientDisconnect(edict_t *pclent);
extern void ClientKill(edict_t *pclent);
extern void ClientPutInServer(edict_t *pclent); ///< Call after setting the parm1... parms
extern void ClientCommand(edict_t *pclent);
extern void ClientUserInfoChanged(edict_t *pclent, char *userinfo);
extern void ServerActivate(edict_t *edicts, int edictcount, int maxclients);
extern void ServerDeactivate();
extern void PlayerPreThink(edict_t *pent);
extern void PlayerPostThink(edict_t *pent);
extern void StartFrame();
extern void ParmsNewLevel(); // TODO: SetNewParms? ///< Called when a client first connects to a server. Sets parms so they can be saved off for restarts
extern void ParmsChangeLevel(); // TODO: SetChangeParms? ///< Call to set parms for self so they can be saved for a level transition
extern const char *GetGameDescription();
extern void PlayerCustomization(edict_t *pPlayer, customization_t *pCustom);
extern void SpectatorConnect(edict_t *pent);
extern void SpectatorDisconnect(edict_t *pent);
extern void SpectatorThink(edict_t *pent);
extern void Sys_Error_Game(const char *error);
extern void PM_Init_Game(struct playermove_s *ppmove);
extern void PM_Move_Game(struct playermove_s *ppmove, qboolean server);
extern char PM_FindTextureType_Game(const char *name);
extern void SetupVisibility(edict_t *pViewEntity, edict_t *pClientEnt, unsigned char **pvs, unsigned char **pas);
extern void UpdateClientData(const edict_t *pent, int sendweapons, struct clientdata_s *pcd);
extern int AddToFullPack(struct entity_state_s *state, int e, edict_t *pent, edict_t *host_edict, int hostflags, int player, unsigned char *pSet);
extern void CreateBaseline(int player, int entindex, struct entity_state_s *baseline, edict_t *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
extern void RegisterEncoders();
extern int GetWeaponData(edict_t *player, struct weapon_data_s *data);
extern void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed);
extern void CmdEnd(const edict_t *player);
extern int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *len);
extern int GetHullBounds(int hullnumber, float *mins, float *maxs);
extern void CreateInstancedBaselines();
extern int InconsistentFile(const edict_t *player, const char *filename, char *disconnectmsg);
extern int AllowLagCompensation();

extern void OnFreeEntPrivateData(edict_t *pent);
extern void GameShutdown();
extern int ShouldCollide(edict_t *pent, edict_t *pother);
extern void CvarValue(const edict_t *pent, const char *value);
extern void CvarValue2(const edict_t *pent, int requestid, const char *cvarname, const char *value);

enginefuncs_t gEngFuncs{};
globalvars_t *gpGlobals{nullptr};

static DLL_FUNCTIONS gGameFuncs =
{
	GameInit,
	
	EntitySpawn,
	EntityThink,
	
	EntityUse,
	EntityTouch,
	EntityBlocked,
	
	EntityKeyValue,
	
	EntitySave,
	EntityRestore,
	
	EntitySetAbsPos,
	
	SaveWriteFields,
	SaveReadFields,
	
	SaveGlobalState,
	RestoreGlobalState,
	ResetGlobalState,
	
	ClientConnect,
	ClientDisconnect,
	ClientKill,
	ClientPutInServer,
	ClientCommand,
	ClientUserInfoChanged,
	
	ServerActivate,
	ServerDeactivate,
	
	PlayerPreThink,
	PlayerPostThink,
	
	StartFrame,
	
	ParmsNewLevel,
	ParmsChangeLevel,
	
	GetGameDescription,
	
	PlayerCustomization,
	
	SpectatorConnect,
	SpectatorDisconnect,
	SpectatorThink,
	
	Sys_Error_Game,
	
	PM_Move_Game,
	PM_Init_Game,
	PM_FindTextureType_Game,
	
	SetupVisibility,
	UpdateClientData,
	AddToFullPack,
	CreateBaseline,
	RegisterEncoders,
	GetWeaponData,
	
	CmdStart,
	CmdEnd,
	
	ConnectionlessPacket,
	
	GetHullBounds,
	
	CreateInstancedBaselines,
	
	InconsistentFile,
	
	AllowLagCompensation
};

static NEW_DLL_FUNCTIONS gNewGameFuncs =
{
	OnFreeEntPrivateData,
	
	GameShutdown,
	
	ShouldCollide,
	
	CvarValue,
	CvarValue2
};

C_EXPORT void GiveFnptrsToDll(enginefuncs_t *pEngFuncs, globalvars_t *pGlobals)
{
	if(!pEngFuncs || !pGlobals)
	{
		// TODO
		return;
	};
	
	memcpy(&gEngFuncs, pEngFuncs, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;
};

C_EXPORT int GetEntityAPI(DLL_FUNCTIONS *pGameFuncs, int nInterfaceVersion)
{
	if(!pGameFuncs || nInterfaceVersion != INTERFACE_VERSION)
	{
		// TODO
		return 0;
	};
	
	memcpy(pGameFuncs, &gGameFuncs, sizeof(DLL_FUNCTIONS));
	return 1;
};

C_EXPORT int GetEntityAPI2(DLL_FUNCTIONS *pGameFuncs, int *pInterfaceVersion)
{
	if(!pGameFuncs || !pInterfaceVersion)
	{
		// TODO
		return 0;
	};
	
	if(*pInterfaceVersion != INTERFACE_VERSION)
	{
		*pInterfaceVersion = INTERFACE_VERSION;
		return 0;
	};
	
	memcpy(pGameFuncs, &gGameFuncs, sizeof(DLL_FUNCTIONS));
	return 1;
};

int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pFuncs, int *pInterfaceVersion)
{
	if(!pFuncs || !pInterfaceVersion)
		return 0;
	
	if(*pInterfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
	{
		*pInterfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return 0;
	};
	
	memcpy(pFuncs, &gNewGameFuncs, sizeof(NEW_DLL_FUNCTIONS));
	return 1;
};