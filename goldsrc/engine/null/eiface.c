/// @file

#include "quakedef.h"

void GameInit()
{
};

void GameShutdown()
{
};

void EntitySpawn(edict_t *pent)
{
};

void EntityThink(edict_t *pent)
{
};

void EntityUse(edict_t *pent, edict_t *pother)
{
};

void EntityTouch(edict_t *pent, edict_t *pother)
{
};

void EntityBlocked(edict_t *pent, edict_t *pother)
{
};

void EntityKeyValue(edict_t *pent, KeyValueData *pkvd)
{
};

void EntitySave(TODO)
{
};

void EntityRestore(TODO)
{
};

void EntitySetAbsPos(edict_t *pent)
{
};

void SaveWriteFields(TODO)
{
};

void SaveReadFields(TODO)
{
};

void SaveGlobalState(SAVERESTOREDATA *pSaveRestoreData)
{
};

void RestoreGlobalState(SAVERESTOREDATA *pSaveRestoreData)
{
};

void ResetGlobalState()
{
};

qboolean ClientConnect(edict_t *pClEnt, const char *name, const char *adr, TODO)
{
	return true;
};

void ClientDisconnect(edict_t *pclent)
{
};

void ClientKill(edict_t *pclent)
{
};

void ClientPutInServer(edict_t *pclent)
{
};

void ClientCommand(edict_t *pclent)
{
};

void ClientUserInfoChanged(edict_t *pclent, char *userinfo)
{
};

void ServerActivate(edict_t *edicts, int edictcount, int maxclients)
{
};

void ServerDeactivate()
{
};

void PlayerPreThink(edict_t *pent)
{
};

void PlayerPostThink(edict_t *pent)
{
};

void StartFrame()
{
};

void ParmsNewLevel()
{
};

void ParmsChangeLevel()
{
};

const char *GetGameDescription()
{
	return "Stub (Null)";
};

void PlayerCustomization(TODO)
{
};

void SpectatorConnect(edict_t *pent)
{
};

void SpectatorDisconnect(edict_t *pent)
{
};

void SpectatorThink(edict_t *pent)
{
};

void Sys_Error_Game(const char *error)
{
};

void PM_Init_Game(struct playermove_s *ppmove)
{
};

void PM_Move_Game(struct playermove_s *ppmove, qboolean server)
{
};

char PM_FindTextureType_Game(char *name)
{
	return '\0';
};

void SetupVisibility(TODO)
{
};

void UpdateClientData(TODO)
{
};

int AddToFullPack(TODO)
{
	return 0;
};

void CreateBaseline(TODO)
{
};

void RegisterEncoders()
{
};

int GetWeaponData(TODO)
{
	return 0;
};

void CmdStart(TODO)
{
};

void CmdEnd(const edict_t *player)
{
};

int ConnectionlessPacket(TODO)
{
	return 0;
};

int GetHullBounds(TODO)
{
	return 0;
};

void CreateInstancedBaselines()
{
};

int InconsistentFile(TODO)
{
	return 0;
};

int AllowLagCompensation()
{
	return 0;
};

void OnFreeEntPrivateData(edict_t *pent)
{
};

int ShouldCollide(edict_t *pent, edict_t *pother)
{
	return 0;
};

void CvarValue(TODO)
{
};

void CvarValue2(TODO)
{
};