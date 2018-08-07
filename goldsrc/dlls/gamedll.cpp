/// @file

#include "quakedef.h"
#include "custom.h" // TODO: temp?

void GameInit(){};

void GameShutdown(){};

int EntitySpawn(edict_t *pent){};

void EntityThink(edict_t *pent){};

void EntityUse(edict_t *pent, edict_t *pother){};

void EntityTouch(edict_t *pent, edict_t *pother){};

void EntityBlocked(edict_t *pent, edict_t *pother){};

void EntityKeyValue(edict_t *pent, KeyValueData *pkvd){};

void EntitySave(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData){};

int EntityRestore(edict_t *pent, SAVERESTOREDATA *pSaveRestoreData, int globalentity){};

void EntitySetAbsPos(edict_t *pent){};

void SaveWriteFields(SAVERESTOREDATA *pSaveRestoreData, const char *sName, void *pBaseData, TYPEDESCRIPTION *pFields, int nFieldCount){};

void SaveReadFields(SAVERESTOREDATA *pSaveRestoreData, const char *sName, void *pBaseData, TYPEDESCRIPTION *pFields, int nFieldCount){};

void SaveGlobalState(SAVERESTOREDATA *pSaveRestoreData){};

void RestoreGlobalState(SAVERESTOREDATA *pSaveRestoreData){};

void ResetGlobalState(){};

qboolean ClientConnect(edict_t *pEntity, const char *name, const char *adr, char *sRejectReason[128])
{
	return true;
};

void ClientDisconnect(edict_t *pclent){};

void ClientKill(edict_t *pclent){};

void ClientPutInServer(edict_t *pclent){};

void ClientCommand(edict_t *pclent){};

void ClientUserInfoChanged(edict_t *pclent, char *userinfo){};

void ServerActivate(edict_t *edicts, int edictcount, int maxclients){};

void ServerDeactivate(){};

void PlayerPreThink(edict_t *pent){};

void PlayerPostThink(edict_t *pent){};

void StartFrame(){};

void ParmsNewLevel(){};

void ParmsChangeLevel(){};

const char *GetGameDescription()
{
	return "Stub (Null)";
};

void PlayerCustomization(edict_t *pPlayer, customization_t *pCustom){};

void SpectatorConnect(edict_t *pent){};

void SpectatorDisconnect(edict_t *pent){};

void SpectatorThink(edict_t *pent){};

void Sys_Error_Game(const char *error){};

void PM_Init_Game(struct playermove_s *ppmove){};

void PM_Move_Game(struct playermove_s *ppmove, qboolean server){};

char PM_FindTextureType_Game(char *name)
{
	return '\0';
};

void SetupVisibility(edict_t *pViewEntity, edict_t *pClientEnt, unsigned char **pvs, unsigned char **pas){};

void UpdateClientData(const edict_t *pent, int sendweapons, struct clientdata_s *pcd){};

int AddToFullPack(struct entity_state_s *state, int e, edict_t *pent, edict_t *host_edict, int hostflags, int player, unsigned char *pSet)
{
	return 0;
};

void CreateBaseline(int player, int entindex, struct entity_state_s *baseline, edict_t *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs){};

void RegisterEncoders(){};

int GetWeaponData(edict_t *player, struct weapon_data_s *data)
{
	return 0;
};

void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed){};

void CmdEnd(const edict_t *player){};

int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *len)
{
	return 0;
};

int GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	return 0;
};

void CreateInstancedBaselines(){};

int InconsistentFile(const edict_t *player, const char *filename, char *disconnectmsg)
{
	return 0;
};

int AllowLagCompensation()
{
	return 0;
};

void OnFreeEntPrivateData(edict_t *pent){};

int ShouldCollide(edict_t *pent, edict_t *pother)
{
	return 0;
};

void CvarValue(const edict_t *pent, const char *value){};

void CvarValue2(const edict_t *pent, int requestid, const char *cvarname, const char *value){};