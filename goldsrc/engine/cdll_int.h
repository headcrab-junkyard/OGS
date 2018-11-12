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
/// @brief engine <-> client dll interaction interface declarations

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "const.h"
//#include "in_buttons.h" // TODO
#include "wrect.h" // TODO

#define CLDLL_INTERFACE_VERSION 1 // TODO: 7

#define MAX_ALIAS_NAME 32

typedef struct cmdalias_s
{
	struct cmdalias_s *next;
	char name[MAX_ALIAS_NAME];
	char *value;
} cmdalias_t;

typedef int SpriteHandle_t;

typedef int (*pfnUserMsgHook)(const char *asName, int anSize, void *apBuffer);

/// Screen Info Flags
enum
{
	SCRINFO_SCREENFLASH = 1,
	SCRINFO_STRETCHED
};

/// Screen Info
typedef struct SCREENINFO_s
{
	int iSize;
	
	int iWidth;
	int iHeight;
	
	int iFlags;
	
	int iCharHeight;
	
	short charWidths[256];
} SCREENINFO;

typedef struct client_data_s
{
	// TODO
} client_data_t;

typedef struct client_sprite_s
{
	// TODO
} client_sprite_t;

typedef struct hud_player_info_s
{
	// TODO
} hud_player_info_t;

typedef struct module_s
{
	// TODO
} module_t;

typedef struct demo_api_s demo_api_t;
typedef struct client_textmessage_s client_textmessage_t; // TODO
typedef struct con_nprint_s con_nprint_t; // TODO
typedef struct sequenceEntry_ sequenceEntry_s;
typedef struct sentenceEntry_ sentenceEntry_s;

struct screenfade_s;
struct playermove_s;
struct ref_params_s;
struct cl_entity_s;
struct mstudioevent_s;
struct clientdata_s;
struct weapon_data_s;
struct tempent_s;
struct local_state_s;
struct event_args_s;
struct tagPOINT;
struct r_studio_interface_s;
struct engine_studio_api_s;

/// Pointers to the exported engine functions
typedef struct cl_enginefuncs_s
{
	// Sprite Handlers
	
	SpriteHandle_t (*pfnSPR_Load)(const char *asName);
	
	int (*pfnSPR_Frames)(SpriteHandle_t ahSprite);
	
	int (*pfnSPR_Height)(SpriteHandle_t ahSprite, int anFrame);
	int (*pfnSPR_Width)(SpriteHandle_t ahSprite, int anFrame);
	
	void (*pfnSPR_Set)(SpriteHandle_t ahSprite, int r, int g, int b);
	
	void (*pfnSPR_Draw)(int frame, int x, int y, const wrect_t *pRect); // TODO: const struct rect_s * ?
	void (*pfnSPR_DrawHoles)(int frame, int x, int y, const wrect_t *pRect); // TODO: const struct rect_s * ?
	void (*pfnSPR_DrawAdditive)(int frame, int x, int y, const wrect_t *pRect); // TODO: const struct rect_s * ?
	
	void (*pfnSPR_EnableScissor)(int x, int y, int width, int height);
	void (*pfnSPR_DisableScissor)();
	
	struct client_sprite_s *(*pfnSPR_GetList)(char *psz, int *apnCount);
	
	// Screen Handlers
	
	void (*pfnFillRGBA)(int x, int y, int width, int height, int r, int g, int b, int a);
	
	int (*pfnGetScreenInfo)(struct SCREENINFO_s *pscrinfo);
	
	void (*pfnSetCrosshair)(SpriteHandle_t ahSprite, wrect_t aRect, int r, int g, int b);
	
	// Cvar Handlers
	
	struct cvar_s *(*pfnRegisterVariable)(const char *szName, const char *szValue, int flags);
	
	float (*pfnGetCvarFloat)(const char *szName);
	char *(*pfnGetCvarString)(const char *szName);
	
	// Command Handlers
	
	int (*pfnAddCommand)(const char *cmd_name, void (*function)());
	
	int (*pfnHookUserMsg)(const char *szMsgName, pfnUserMsgHook pfn);
	
	int (*pfnServerCmd)(const char *szCmdString);
	int (*pfnClientCmd)(const char *szCmdString);
	
	// Player Info
	
	void (*pfnGetPlayerInfo)(int ent_num, hud_player_info_t *pinfo);
	
	// Sound Handlers
	
	void (*pfnPlaySoundByName)(const char *szSound, float volume);
	void (*pfnPlaySoundByIndex)(int iSound, float volume);
	
	// Vector Helpers
	
	void (*pfnAngleVectors)(const float *avAngles, float *avForward, float *avRight, float *avUp);
	
	// Text Message System

	struct client_textmessage_s *(*pfnTextMessageGet)(const char *asName);
	
	int (*pfnDrawCharacter)(int x, int y, int number, int r, int g, int b);
	int (*pfnDrawConsoleString)(int x, int y, char *string);
	
	void (*pfnDrawSetTextColor)(float r, float g, float b);
	
	void (*pfnDrawConsoleStringLen)(const char *string, int *length, int *height);
	
	void (*pfnConsolePrint)(const char *asMsg);
	void (*pfnCenterPrint)(const char *asMsg);
	
	int (*GetWindowCenterX)();
	int (*GetWindowCenterY)();
	
	void (*GetViewAngles)(float *);
	void (*SetViewAngles)(float *);
	
	int (*GetMaxClients)();
	
	void (*Cvar_SetValue)(const char *cvar, float afValue);
	
	int (*Cmd_Argc)();
	char *(*Cmd_Argv)(int anArg);
	
	void (*Con_Printf)(const char *fmt, ...);
	void (*Con_DPrintf)(const char *fmt, ...);
	void (*Con_NPrintf)(int anPos, const char *fmt, ...);
	void (*Con_NXPrintf)(struct con_nprint_s *apInfo, const char *fmt, ...);
	
	const char *(*PhysInfo_ValueForKey)(const char *asKey);
	const char *(*ServerInfo_ValueForKey)(const char *asKey);
	
	float (*GetClientMaxspeed)();
	
	int (*CheckParm)(const char *parm, char **ppnext);
	
	void (*Key_Event)(int anKey, int anDown);
	
	void (*GetMousePosition)(int *apnX, int *apnY);
	
	int (*IsNoClipping)();
	
	struct cl_entity_s *(*GetLocalPlayer)();
	struct cl_entity_s *(*GetViewModel)();
	struct cl_entity_s *(*GetEntityByIndex)(int anIndex);
	
	float (*GetClientTime)();
	
	void (*V_CalcShake)();
	void (*V_ApplyShake)(float *avOrigin, float *avAngles, float afFactor);
	
	int (*PM_PointContents)(float *point, int *truecontents);
	int (*PM_WaterEntity)(float *p);
	struct pmtrace_s *(*PM_TraceLine)(float *start, float *end, int flags, int usehull, int ignore_pe);
	
	struct model_s *(*CL_LoadModel)(const char *asName, int *index);
	int (*CL_CreateVisibleEntity)(int anType, struct cl_entity_s *apEnt);
	
	const struct model_s *(*GetSpritePointer)(SpriteHandle_t ahSprite);
	
	void (*pfnPlaySoundByNameAtLocation)(const char *asSound, float afVolume, float *avOrigin);
	
	unsigned short (*pfnPrecacheEvent)(int anType, const char *asName);
	void (*pfnPlaybackEvent)(int anFlags, const struct edict_s *apInvoker, unsigned short nIndex, float afDelay, 
							float *avOrigin, float *avAngles, 
							float afParam1, float afParam2,
							int anParam1, int anParam2,
							int abParam1, int abParam2);
	
	void (*pfnWeaponAnim)(int anAnim, int anBody);
	
	float (*pfnRandomFloat)(float afLow, float afHigh);
	int32_t (*pfnRandomLong)(int32_t anLow, int32_t anHigh);
	
	void (*pfnHookEvent)(const char *name, void (*pfnEvent)(struct event_args_s *apArgs));
	
	int (*Con_IsVisible)();
	
	const char *(*pfnGetGameDirectory)();
	
	struct cvar_s *(*pfnGetCvarPointer)(const char *asName);
	
	const char *(*Key_LookupBinding)(const char *asBinding);
	
	const char *(*pfnGetLevelName)();

	void (*pfnGetScreenFade)(struct screenfade_s *fade);
	void (*pfnSetScreenFade)(struct screenfade_s *fade);
	
	void *(*VGui_GetPanel)();
	void (*VGui_ViewportPaintBackground)(int extents[4]);
	
	byte *(*COM_LoadFile)(const char *path, int usehunk, int *pLength);
	char *(*COM_ParseFile)(char *data, char *token);
	void (*COM_FreeFile)(void *buffer);
	
	struct triangleapi_s *pTriAPI;
	struct efx_api_s *pEfxAPI;
	struct event_api_s *pEventAPI;
	struct demo_api_s *pDemoAPI;
	struct net_api_s *pNetAPI;
	struct IVoiceTweak_s *pVoiceTweak;

	/// @return 1 if the client is a spectator only (connected to a proxy), 0 otherwise or 2 if in dev_overview mode
	int (*IsSpectateOnly)();
	
	struct model_s *(*LoadMapSprite)(const char *asFileName);
	
	void (*COM_AddAppDirectoryToSearchPath)(const char *asBaseDir, const char *asAppName);
	int (*COM_ExpandFilename)(const char *asFileName, char *asNameOutBuffer, int anOutBufferSize);
	
	/// anPlayer is in the range (1, MaxClients)
	/// @return NULL is player doesn't exist or "" if no value is set
	const char *(*PlayerInfo_ValueForKey)(int anPlayer, const char *asKey);
	
	void (*PlayerInfo_SetValueForKey)(const char *asKey, const char *asValue);
	
	/// Gets a unique ID for the specified player
	/// This is the same even if you see the player on a different server
	/// @param anPlayer - an entity index, so client 0 would use anPlayer = 1
	/// @return false if there is no player on the server in the specified slot
	qboolean (*GetPlayerUniqueID)(int anPlayer, char asPlayerID[16]);
	
	/// TrackerID access
	int (*GetTrackerIDForPlayer)(int anPlayerSlot);
	int (*GetPlayerForTrackerID)(int anTrackerID);
	
	/// Same as pfnServerCmd, but the message goes in the unreliable stream 
	/// so it can't clog the net stream (but it might not get there)
	int (*pfnServerCmdUnreliable)(const char *asCmd);
	
	void (*pfnGetMousePos)(struct tagPOINT *ppt);
	void (*pfnSetMousePos)(int x, int y);
	void (*pfnSetMouseEnable)(qboolean abEnable);
	
	struct cvar_s *(*GetFirstCvarPtr)();
	
	uint (*GetFirstCmdFunctionHandle)();
	uint (*GetNextCmdFunctionHandle)(uint cmdhandle);
	const char *(*GetCmdFunctionName)(uint cmdhandle);
	
	float (*hudGetClientOldTime)();
	
	float (*hudGetServerGravityValue)();
	
	struct model_s *(*hudGetModelByIndex)(int index);
	
	void (*pfnSetFilterMode)(int mode);
	void (*pfnSetFilterColor)(float r, float g, float b);
	void (*pfnSetFilterBrightness)(float brightness);
	
	sequenceEntry_s *(*pfnSequenceGet)(const char *fileName, const char *entryName);
	
	void (*pfnSPR_DrawGeneric)(int frame, int x, int y, const struct rect_s *pRect, int src, int dest, int width, int height);
	
	sentenceEntry_s *(*pfnSequencePickSentence)(const char *sentenceName, int pickMethod, int *picked);
	
	/// draw a complete string
	int (*pfnDrawString)(int x, int y, const char *str, int r, int g, int b);
	int (*pfnDrawStringReverse)(int x, int y, const char *str, int r, int g, int b);
	
	const char *(*LocalPlayerInfo_ValueForKey)(const char *key);
	
	int (*pfnVGUI2DrawCharacter)(int x, int y, int ch, uint font);
	int (*pfnVGUI2DrawCharacterAdd)(int x, int y, int ch, int r, int g, int b, uint font);
	
	uint (*COM_GetApproxWavePlayLength)(const char *filename);
	
	void *(*pfnGetCareerUI)();
	
	void (*Cvar_Set)(const char *name, const char *value);
	
	int (*pfnIsCareerMatch)();
	
	void (*pfnPlaySoundVoiceByName)(const char *szSound, float volume, int pitch);
	
	void (*pfnPrimeMusicStream)(const char *szFilename, int looping);
	
	double (*GetAbsoluteTime)();
	
	void (*pfnProcessTutorMessageDecayBuffer)(int *buffer, int bufferLength);
	void (*pfnConstructTutorMessageDecayBuffer)(int *buffer, int bufferLength);
	
	void (*pfnResetTutorMessageDecayData)();
	
	void (*pfnPlaySoundByNameAtPitch)(const char *szSound, float volume, int pitch);
	
	void (*pfnFillRGBABlend)(int x, int y, int width, int height, int r, int g, int b, int a);
	
	int (*pfnGetAppID)();
	
	cmdalias_t *(*pfnGetAliasList)();
	
	void (*pfnVguiWrap2_GetMouseDelta)(int *x, int *y);
} cl_enginefunc_t;

/// Functions exported by the client game module
typedef struct cldll_func_s
{
	///
	int (*pfnInitialize)(cl_enginefunc_t *apEngFuncs, int anVersion);
	
	///
	void (*pfnHudInit)();
	
	///
	int (*pfnHudVidInit)();
	
	///
	int (*pfnHudRedraw)(float time, int intermission);
	
	///
	int (*pfnUpdateClientData)(struct clientdata_s *, float time);
	
	///
	void (*pfnHudReset)(); // TODO: should be handled from somewhere
	
	///
	void (*pfnClientMove)(struct playermove_s *ppmove, qboolean server);
	
	///
	void (*pfnClientMoveInit)(struct playermove_s *ppmove);
	
	///
	char (*pfnClientTextureType)(const char *name);
	
	// Input
	
	///
	void (*pfnIN_ActivateMouse)();
	
	///
	void (*pfnIN_DeactivateMouse)();
	
	///
	void (*pfnIN_MouseEvent)(int mstate);
	
	///
	void (*pfnIN_ClearStates)();
	
	///
	void (*pfnIN_Accumulate)();
	
	///
	void (*pfnCreateMove)(float frametime, struct usercmd_s *cmd, int active);
	
	///
	int (*pfnIsThirdPerson)();
	
	///
	void (*pfnGetCameraOffsets)(float *ofs);
	
	///
	struct kbutton_s *(*pfnFindKey)(const char *name);
	
	///
	void (*pfnCAM_Think)();
	
	///
	void (*pfnCalcRefdef)(struct ref_params_s *pparams);
	
	///
	int (*pfnAddEntity)(int type, struct cl_entity_s *ent, const char *modelname);
	
	///
	void (*pfnCreateEntities)();
	
	///
	void (*pfnDrawNormalTriangles)();
	
	///
	void (*pfnDrawTransparentTriangles)();
	
	///
	void (*pfnStudioEvent)(const struct mstudioevent_s *event, const struct cl_entity_s *entity);
	
	///
	void (*pfnPostRunCmd)(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	
	///
	void (*pfnShutdown)();
	
	///
	void (*pfnTxferLocalOverrides)(struct entity_state_s *state, const struct clientdata_s *client);
	
	///
	void (*pfnProcessPlayerState)(struct entity_state_s *dst, const struct entity_state_s *src);
	
	///
	void (*pfnTxferPredictionData)(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
	
	///
	void (*pfnReadDemoBuffer)(int size, byte *buffer);
	
	///
	int (*pfnConnectionlessPacket)(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	
	///
	int (*pfnGetHullBounds)(int hullnumber, float *mins, float *maxs);
	
	///
	void (*pfnFrame)(double time);
	
	///
	int (*pfnKey_Event)(int down, int keynum, const char *pszCurrentBinding);
	
	///
	void (*pfnTempEntUpdate)(double frametime, double client_time, double cl_gravity, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int (*addEntity)(struct cl_entity_s *pEntity), void (*playTESound)(struct tempent_s *pTemp, float damp));
	
	///
	struct cl_entity_s *(*pfnGetUserEntity)(int index);
	
	///
	void (*pfnVoiceStatus)(int entindex, qboolean bTalking);
	
	///
	void (*pfnDirectorMessage)(int iSize, void *pbuf);
	
	///
	int (*pfnStudioInterface)(int version, struct r_studio_interface_s **ppInterface, struct engine_studio_api_s *pStudio); // TODO: should be handled from somewhere
	
	///
	void (*pfnChatInputPosition)(int *x, int *y);
	
	///
	int (*pfnGetPlayerTeam)(int playernum); // TODO: should be handled from somewhere
	
	///
	void *(*pfnClientFactory)(); // TODO: should be handled from somewhere
} cldll_func_t;

extern cldll_func_t cl_funcs;

extern void ClientDLL_Init();
extern void ClientDLL_Shutdown();

extern void ClientDLL_HudInit();
extern void ClientDLL_HudVidInit();

extern void ClientDLL_UpdateClientData();

extern void ClientDLL_Frame(double time);

extern void ClientDLL_HudRedraw(int intermission);

extern void ClientDLL_MoveClient(struct playermove_s *ppmove);
extern void ClientDLL_ClientMoveInit(struct playermove_s *ppmove);
extern char ClientDLL_ClientTextureType(const char *name);

extern void ClientDLL_CreateMove(float frametime, struct usercmd_s *cmd, int active);

extern void ClientDLL_ActivateMouse();
extern void ClientDLL_DeactivateMouse();

extern void ClientDLL_MouseEvent(int mstate);

extern void ClientDLL_ClearStates();

extern int ClientDLL_IsThirdPerson();

extern void ClientDLL_GetCameraOffsets(float *ofs);

extern int ClientDLL_GraphKeyDown();

extern struct kbutton_s *ClientDLL_FindKey(const char *name);

extern void ClientDLL_CAM_Think();

extern void ClientDLL_IN_Accumulate();

extern void ClientDLL_CalcRefdef(struct ref_params_s *pparams);

extern int ClientDLL_AddEntity(int type, struct cl_entity_s *ent);
extern void ClientDLL_CreateEntities();

extern void ClientDLL_DrawNormalTriangles();
extern void ClientDLL_DrawTransparentTriangles();

extern void ClientDLL_StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity);

extern void ClientDLL_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);

extern void ClientDLL_TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client);
extern void ClientDLL_ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src);
extern void ClientDLL_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);

extern void ClientDLL_ReadDemoBuffer(int size, byte *buffer);

extern int ClientDLL_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);

extern int ClientDLL_GetHullBounds(int hullnumber, float *mins, float *maxs);

extern void ClientDLL_VGui_ConsolePrint(const char *text);

extern int ClientDLL_Key_Event(int down, int keynum, const char *pszCurrentBinding);

extern void ClientDLL_TempEntUpdate(double ft, double ct, double grav, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int (*addEntity)(struct cl_entity_s *pEntity), void (*playTESound)(struct tempent_s *pTemp, float damp));

extern struct cl_entity_s *ClientDLL_GetUserEntity(int index);

extern void ClientDLL_VoiceStatus(int entindex, qboolean bTalking);

extern void ClientDLL_DirectorMessage(int iSize, void *pbuf);

extern void ClientDLL_ChatInputPosition(int *x, int *y);

#ifdef __cplusplus
};
#endif