/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
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

#include <stdint.h>
#include "const.h"
#include "crc.h"
#include "Sequence.h"

#define	INTERFACE_VERSION 140
#define NEW_DLL_FUNCTIONS_VERSION 1

typedef enum
{
	at_notice,
	at_console,
	at_aiconsole,
	at_warning,
	at_error,
	at_logged
} ALERT_TYPE;

typedef enum
{
	print_console,
	print_center,
	print_chat
} PRINT_TYPE;

typedef enum
{
	force_exactfile,
	force_model_samebounds,
	force_model_specifybounds,
	force_model_specifybounds_if_avail
} FORCE_TYPE;

typedef struct
{
} TraceResult;

typedef struct cvar_s cvar_t;
typedef struct edict_s edict_t;
typedef struct usercmd_s usercmd_t;
typedef struct delta_s delta_t;

typedef struct customization_s customization_t;
typedef struct playermove_s playermove_t;
typedef struct clientdata_s clientdata_t;
typedef struct weapon_data_s weapon_data_t;

//
// functions provided by the main engine
//
typedef struct enginefuncs_s
{
	int (*pfnPrecacheModel)(const char *name);
	int (*pfnPrecacheSound)(const char *name);
	
	void (*pfnSetModel)(edict_t *ent, const char *name);
	
	int (*pfnModelIndex)(const char *name);
	int (*pfnModeFrames)(int modelindex);
	
	void (*pfnSetSize)(edict_t *ent, const float *pfMin, const float *pfMax);
	
	void (*pfnChangeLevel)(const char *name, const char *startspot);
	
	void (*pfnGetSpawnParms)(edict_t *ent);
	void (*pfnSaveSpawnParms)(edict_t *ent);
	
	float (*pfnVecToYaw)(const float *vVector);
	void (*pfnVecToAngles)(const float *vVectorIn, float *vVectorOut);
	
	void (*pfnMoveToOrigin)(edict_t *pEnt, const float *vGoal, float fDist, int nMoveType);
	
	void (*pfnChangeYaw)(edict_t *pent);
	void (*pfnChangePitch)(edict_t *pent);
	
	edict_t *(*pfnFindEntityByString)(edict_t *pEntStartSearchAfter, const char *sField, const char *sValue);
	
	int (*pfnGetEntityIllum)(edict_t *pent);
	
	edict_t *(*pfnFindEntityInSphere)(edict_t *pEntStartSearchAfter, const float *origin, float radius);
	edict_t *(*pfnFindClientInPVS)(edict_t *pEdict);
	edict_t *(*pfnEntitiesInPVS)(edict_t *pPlayer);
	
	void (*pfnMakeVectors)(const float *pfVector);
	void (*pfnAngleVectors)(const float *pfVector, float *forward, float *right, float *up);
	
	edict_t *(*pfnCreateEntity)();
	void (*pfnRemoveEntity)(edict_t *ent);
	edict_t *(*pfnCreateNamedEntity)(int nClassName);
	
	void (*pfnMakeStatic)(edict_t *ent);
	int (*pfnEntIsOnFloor)(edict_t *ent);
	int (*pfnDropToFloor)(edict_t *ent);
	int (*pfnWalkMove)(edict_t *ent, float yaw, float dist, int nMode);
	void (*pfnSetOrigin)(edict_t *ent, const vec3_t origin); // const float *
	
	void (*pfnEmitSound)(edict_t *ent, int channel, const char *sample, float volume, float attenuation, int nFlags, int pitch);
	void (*pfnEmitAmbientSound)(edict_t *ent, vec3_t origin, const char *sample, float volume, float attenuation, int nFlags, int pitch);
	
	void (*pfnTraceLine)(const float *vOriginA, const float *vOriginB, int nNoMonsters, edict_t *pEntToSkip, TraceResult *pTraceResult);
	void (*pfnTraceToss)(edict_t *pEntity, edict_t *pEntToIgnore, TraceResult *pTraceResult);
	int (*pfnTraceMonsterHull)(edict_t *pEdict, const float *vOriginA, const float *vOriginB, int nNoMonsters, edict_t *pEntToSkip, TraceResult *pTraceResult);
	void (*pfnTraceHull)(const float *vOriginA, const float *vOriginB, int nNoMonsters, int hullNumber, edict_t *pEntToSkip, TraceResult *pTraceResult);
	void (*pfnTraceModel)(const float *vOriginA, const float *vOriginB, int hullNumber, edict_t *pent, TraceResult *pTraceResult);
	const char *(*pfnTraceTexture)(edict_t *pTextureEntity, const float *vOriginA, const float *vOriginB);
	void (*pfnTraceSphere)(const float *vOriginA, const float *vOriginB, int nNoMonsters, float radius, edict_t *pEntToSkip, TraceResult *pTraceResult);
	
	void (*pfnGetAimVector)(edict_t *ent, float speed, float *pfReturn);
	
	void (*pfnServerCommand)(const char *cmd);
	void (*pfnServerExecute)();
	
	void (*pfnClientCommand)(edict_t *pEdict, const char *cmd, ...);
	
	void (*pfnParticleEffect)(const float *origin, const float *direction, float color, float count);
	
	void (*pfnLightStyle)(int style, const char *val);
	
	int (*pfnDecalIndex)(const char *name);
	
	int (*pfnPointContents)(const vec3_t point); // const float *pfVector
	
	//
	// direct client message generation
	//
	
	void (*pfnMessageBegin)(int msg_dest, int msg_type, const float *vOrigin, edict_t *ed);
	void (*pfnMessageEnd)();
	
	// network messaging
	void	(*pfnWriteByte) (int c);
	void	(*pfnWriteChar) (int c);
	void	(*pfnWriteShort) (int c);
	void	(*pfnWriteLong) (int c);
	void	(*pfnWriteAngle) (float f);
	void	(*pfnWriteCoord) (float f);
	void	(*pfnWriteString) (const char *s);
	void	(*pfnWriteEntity) (int val);
	
	// console variable interaction
	void (*pfnCVarRegister)(cvar_t *pCvar);
	float (*pfnCVarGetFloat) (const char *var_name);
	const char *(*pfnCVarGetString) (const char *var_name);
	void (*pfnCVarSetFloat) (const char *var_name, float fValue);
	void (*pfnCVarSetString) (const char *var_name, const char *value);
	
	void (*pfnAlertMessage)(ALERT_TYPE aType, const char *sMsg, ...);
	
	void (*pfnEngineFprintf)(void *pfile, const char *text, ...);
	
	void *(*pfnPvAllocEntPrivateData)(edict_t *pEnt, int32_t cb);
	void *(*pfnPvEntPrivateData)(edict_t *pEnt);
	void (*pfnFreeEntPrivateData)(edict_t *pEnt);
	
	const char *(*pfnSzFromIndex)(int nString);
	int (*pfnAllocString)(const char *sValue);
	
	struct entvars_s *(*pfnGetVarsOfEnt)(edict_t *pEnt);
	
	edict_t *(*pfnPEntityOfEntOffset)(int nOffset);
	int (*pfnEntOffssetOfPEntity)(const edict_t *pEntity);
	
	int (*pfnIndexOfEdict)(const edict_t *pEdict);
	edict_t *(*pfnPEntityOfEntIndex)(int nIndex);
	
	edict_t *(*pfnFindEntityByVars)(struct entvars_s *pvars);
	
	void *(*GetModelPtr)(edict_t *pEdict);
	
	int (*pfnRegUserMsg)(const char *sName, int nSize);
	
	void (*pfnAnimationAutomove)(const edict_t *pEdict, float fTime);
	void (*pfnGetBonePosition)(const edict_t *pEdict, int nBone, float *pfOrigin, float *pfAngles);
	
	uint32_t (*pfnFunctionFromName)(const char *sName);
	const char *(*pfnNameForFunction)(uint32_t nFunction);
	
	void (*pfnClientPrintf)(edict_t *pEdict, PRINT_TYPE type, const char *asMsg);
	void (*pfnServerPrint)(const char *asMsg);
	
	// ClientCommand and ServerCommand parameter access
	const char	*(*pfnCmd_Args) ();	// concatenation of all argv >= 1
	const char	*(*pfnCmd_Argv) (int n);
	int		(*pfnCmd_Argc) ();
	
	void (*pfnGetAttachment)(const edict_t *pEdict, int nAttachment, float *pfOrigin, float *pfAngles);
	
	void (*pfnCRC32_Init)(CRC32_t *pulCRC);
	void (*pfnCRC32_ProcessBuffer)(CRC32_t *pulCRC, void *p, int len);
	void (*pfnCRC32_ProcessByte)(CRC32_t *pulCRC, byte ch);
	CRC32_t (*pfnCRC32_Final)(CRC32_t pulCRC);
	
	int32_t (*pfnRandomLong)(int32_t nLow, int32_t nHigh);
	float (*pfnRandomFloat)(float fLow, float fHigh);
	
	void (*pfnSetView)(const edict_t *pClient, const edict_t *pViewEnt);
	
	float (*pfnTime)();
	
	void (*pfnCrosshairAngle)(const edict_t *pClient, float pitch, float yaw);
	
	byte *(*pfnLoadFileForMe)(char *filename, int *pLength);
	void (*pfnFreeFile)(void *buffer);
	
	void (*pfnEndSection)(const char *sSectionName);
	
	int (*pfnCompareFileTime)(char *filename1, char *filename2, int *pCompare);
	
	void (*pfnGetGameDir)(char *sGameDir);
	
	void (*pfnCvar_RegisterVariable)(cvar_t *variable);
	
	void (*pfnFadeClientVolume)(const edict_t *pClient, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds);
	
	void (*pfnSetClientMaxspeed)(const edict_t *pClient, float fSpeed);
	
	edict_t *(*pfnCreateFakeClient)(const char *name);
	
	void (*pfnRunPlayerMove)(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec);
	
	int (*pfnNumberOfEntities)();
	
	char *(*pfnGetInfoKeyBuffer)(edict_t *pent);
	
	char *(*pfnInfoKeyValue)(char *infobuffer, char *key);
	
	void (*pfnSetKeyValue)(char *infobuffer, char *key, char *value);
	void (*pfnSetClientKeyValue)(int clientindex, char *infobuffer, char *key, char *value);
	
	int (*pfnIsMapValid)(char *filename);
	
	void (*pfnStaticDecal)(const float *origin, int decalIndex, int entityIndex, int modelIndex);
	
	int (*pfnPrecacheGeneric)(char *s);
	
	int (*pfnGetPlayerUserId)(edict_t *player);
	
	void (*pfnBuildSoundMsg)(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int nFlags, int pitch, int msg_dest, int msg_type, const float *vOrigin, edict_t *ed);
	
	int (*pfnIsDedicatedServer)();
	
	cvar_t *(*pfnCVarGetPointer)(const char *sVarName);
	
	uint (*pfnGetPlayerWONId)(edict_t *player);
	
	void (*pfnInfo_RemoveKey)(char *s, const char *key);
	
	const char *(*pfnGetPhysicsKeyValue)(const edict_t *pClient, const char *key);
	void (*pfnSetPhysicsKeyValue)(const edict_t *pClient, const char *key, const char *value);
	
	const char *(*pfnGetPhysicsInfoString)(const edict_t *pClient);
	
	unsigned short (*pfnPrecacheEvent)(int type, const char *name);
	void (*pfnPlaybackEvent)(int flags, const edict_t *pInvoker, unsigned short eventIndex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	
	byte *(*pfnSetFatPVS)(float *org);
	byte *(*pfnSetFatPAS)(float *org);
	
	int (*pfnCheckVisibility)(const edict_t *entity, byte *pset);
	
	void (*pfnDeltaSetField)(struct delta_s *pFields, const char *fieldname);
	void (*pfnDeltaUnsetField)(struct delta_s *pFields, const char *fieldname);
	
	void (*pfnDeltaAddEncoder)(char *name, void (*conditionalencode)(struct delta_s *pFields, const byte *from, const byte *to));
	
	int (*pfnGetCurrentPlayer)();
	
	int (*pfnCanSkipPlayer)(const edict_t *player);
	
	int (*pfnDeltaFindField)(struct delta_s *pFields, const char *fieldname);
	
	void (*pfnDeltaSetFieldByIndex)(struct delta_s *pFields, int fieldNumber);
	void (*pfnDeltaUnsetFieldByIndex)(struct delta_s *pFields, int fieldNumber);
	
	void (*pfnSetGroupMask)(int mask, int op);
	
	int (*pfnCreateInstancedBaseline)(int classname, struct entity_state_s *baseline);
	
	void (*pfnCvar_DirectSet)(struct cvar_s *var, char *value);
	
	void (*pfnForceUnmodified)(FORCE_TYPE type, float *mins, float *maxs, const char *filename);
	
	void (*pfnGetPlayerStats)(const edict_t *pPlayer, int *ping, int *packet_loss);
	
	void (*pfnAddServerCommand)(char *cmd_name, void (*function)());
	
	qboolean (*pfnVoice_GetClientListening)(int nReceiver, int nSender);
	qboolean (*pfnVoice_SetClientListening)(int nReceiver, int nSender, qboolean bListen);
	
	const char *(*pfnGetPlayerAuthId)(edict_t *player);
	
	sequenceEntry_s *(*pfnSequenceGet)(const char *fileName, const char *entryName);
	sentenceEntry_s *(*pfnSequencePickSentence)(const char *groupName, int pickMethod, int *picked);
	
	int (*pfnGetFileSize)(char *filename);
	
	uint (*pfnGetApproxWavePlayLen)(const char *filepath);
	
	int (*pfnIsCareerMatch)();
	
	int (*pfnGetLocalizedStringLength)(const char *label);
	
	void (*pfnRegisterTutorMessageShown)(int mid);
	int (*pfnGetTimesTutorMessageShown)(int mid);
	
	void (*pfnProcessTutorMessageDecayBuffer)(int *buffer, int bufferLength);
	void (*pfnConstructTutorMessageDecayBuffer)(int *buffer, int bufferLength);
	
	void (*pfnResetTutorMessageDecayData)();
	
	void (*pfnQueryClientCvarValue)(const edict_t *player, const char *cvarName);
	void (*pfnQueryClientCvarValue2)(const edict_t *player, const char *cvarName, int requestID);
	
	int (*pfnCheckParm)(const char *sCmdLineToken, char **ppnext);
} enginefuncs_t;

// TODO
typedef struct KeyValueData_s
{
} KeyValueData;

// TODO
typedef struct saverestore_s
{
	char *pBaseData;
	char *pCurrentData;
	
	int size;
	int bufferSize;
	
	int tokenSize;
	int tokenCount;
	char **pTokens;
	
	int currentIndex;
	int tableCount;
	int connectionCount;
	
	//ENTITYTABLE *pTable; // TODO
	//LEVELLIST levelList[MAX_LEVELCONNECTIONS];
	
	int fUseLandmark;
	char szLandmarkName[20];
	vec3_t vecLandmarkOffset;
	float time;
	char szCurrentMapName[32];
} SAVERESTOREDATA;

typedef enum
{
	FIELD_FLOAT = 0,
	FIELD_STRING,
	FIELD_ENTITY,
	FIELD_CLASSPTR,
	// TODO
} FIELDTYPE;

typedef struct
{
	FIELDTYPE fieldType;
	char *fieldName;
	int fieldOffset;
	short fieldSize;
	short flags;
} TYPEDESCRIPTION;

//
// functions exported by the game subsystem
//
typedef struct DLL_FUNCTIONS
{
	/// The init function will only be called when a game starts, not each time a level is loaded
	/// (one-time call after loading of game dll)
	/// Persistant data for clients and the server can be allocated in init
	void		(*pfnGameInit) ();

	int (*pfnSpawn)(edict_t *ent);
	void (*pfnThink)(edict_t *ent);
	
	void (*pfnUse)(edict_t *ent, edict_t *other);
	void (*pfnTouch)(edict_t *ent, edict_t *other);
	void (*pfnBlocked)(edict_t *ent, edict_t *other);
	
	void (*pfnKeyValue)(edict_t *ent, KeyValueData *pKeyValueData);
	
	void (*pfnSave)(edict_t *ent, SAVERESTOREDATA *pSaveRestoreData);
	int (*pfnRestore)(edict_t *ent, SAVERESTOREDATA *pSaveRestoreData, int nGlobalEntity);
	
	void (*pfnSetAbsBox)(edict_t *ent);
	
	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void (*pfnSaveWriteFields)(SAVERESTOREDATA *pSaveRestoreData, const char *, void *, TYPEDESCRIPTION *, int);
	void (*pfnSaveReadFields)(SAVERESTOREDATA *pSaveRestoreData, const char *, void *, TYPEDESCRIPTION *, int);
	
	void (*pfnSaveGlobalState)(SAVERESTOREDATA *pSaveRestoreData);
	void (*pfnRestoreGlobalState)(SAVERESTOREDATA *pSaveRestoreData);
	void (*pfnResetGlobalState)();
	
	qboolean	(*pfnClientConnect) (edict_t *ent, const char *name, const char *adr, char sRejectReason[128]);
	
	void		(*pfnClientDisconnect) (edict_t *ent);
	void (*pfnClientKill)(edict_t *ent);
	void (*pfnClientPutInServer)(edict_t *ent); // PutClientInServer
	void		(*pfnClientCommand) (edict_t *ent);
	void		(*pfnClientUserinfoChanged) (edict_t *ent, char *userinfo); // TODO: infobuffer
	
	// @param num_edicts - current number, <= max_edicts
	void (*pfnServerActivate)(struct edict_s *edicts, int num_edicts, int max_clients);
	void (*pfnServerDeactivate)();
	
	void (*pfnPlayerPreThink)(edict_t *player);
	void (*pfnPlayerPostThink)(edict_t *player);
	
	void		(*pfnStartFrame) ();
	
	void (*pfnParmsNewLevel)();
	void (*pfnParmsChangeLevel)();
	
	/// @return string describing current game/mod
	const char *(*pfnGetGameDescription)();
	
	/// Notify dll about a player customization
	void (*pfnPlayerCustomization)(edict_t *player, customization_t *pCustom);
	
	// Spectator funcs
	void (*pfnSpectatorConnect)(edict_t *self);
	void (*pfnSpectatorDisconnect)(edict_t *self);
	void (*pfnSpectatorThink)(edict_t *self);
	
	/// Notify game dll that engine is going to shut down. Allows developers to set a breakpoint
	void (*pfnSys_Error)(const char *error_string);
	
	void (*pfnPM_Move)(struct playermove_s *pmove, qboolean server);
	void (*pfnPM_Init)(struct playermove_s *pmove);
	char (*pfnPM_FindTextureType)(const char *name);
	
	void (*pfnSetupVisibility)(struct edict_s *pViewEntity, struct edict_s *pClient, unsigned char **pvs, unsigned char **pas);
	void (*pfnUpdateClientData)(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
	int (*pfnAddToFullPack)(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
	void (*pfnCreateBaseline)(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
	void (*pfnRegisterEncoders)();
	int (*pfnGetWeaponData)(struct edict_s *player, struct weapon_data_s *info);
	
	void (*pfnCmdStart)(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed);
	void (*pfnCmdEnd)(const edict_t *player);
	
	/// @return 1 if the packet is valid. Set response_buffer_size if you want to send a response packet. Incoming, it holds the max
	/// size of the response_buffer, so you must zero it out if you choose not to respond
	int (*pfnConnectionlessPacket)(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	
	/// Enumerates player hulls
	/// @return 0 if the hull number doesn't exist, 1 otherwise
	int (*pfnGetHullBounds)(int hullnumber, float *mins, float *maxs);
	
	/// Create baselines for certain "unplaced" items
	void (*pfnCreateInstancedBaselines)();
	
	/// One of the pfnForceUnmodified files failed the consistency check for the specified player
	/// @return 0 to allow the client to continue, 1 to force immediate disconnection (with a optional disconnect message of up to 256 chars)
	int (*pfnInconsistentFile)(const struct edict_s *player, const char *filename, char *disconnect_message);
	
	/// The game dll should return 1 if lag compensation should be allowed (could also just set the sv_unlag cvar)
	/// Most games right now should return 0, until client-side weapon prediction code is written and tested for them
	int (*pfnAllowLagCompensation)();
} DLL_FUNCTIONS;

extern DLL_FUNCTIONS gEntityInterface;

typedef int (*APIFUNCTION)(DLL_FUNCTIONS *pFunctions, int nInterfaceVersion);
typedef int (*APIFUNCTION2)(DLL_FUNCTIONS *pFunctions, int *pInterfaceVersion);

typedef struct
{
	/// Caled right before the object's memory if freed
	/// Calls its destructor
	void (*pfnOnFreeEntPrivateData)(edict_t *pEnt);
	
	void (*pfnGameShutdown)();
	
	int (*pfnShouldCollide)(edict_t *ent, edict_t *other);
	
	void (*pfnCvarValue)(const edict_t *pEnt, const char *value);
	void (*pfnCvarValue2)(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
} NEW_DLL_FUNCTIONS;

/// Pointers will be null if the game dll doesn't support this API
extern NEW_DLL_FUNCTIONS gNewDLLFunctions;

typedef int (*NEW_DLL_FUNCTIONS_FN)(NEW_DLL_FUNCTIONS *pFunctions, int *pInterfaceVersion);