/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2023 BlackPhrase
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
/// @brief client-side engine functions exports

#include "quakedef.h"
#include "spriteapi.h"
#include "usermsg.h"
#include "textmessage.h"
#include "draw.h"
#include "view.h"
#include "vgui_int.h"
#include <common/event_api.h>
#include <common/demo_api.h>
#include <common/net_api.h>
#include <common/ivoicetweak.h>
#include <common/triangleapi.h>
#include <common/r_efx.h>

#include "winquake.h"

extern triangleapi_t gTriAPI;

void FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
	// TODO
};

int GetScreenInfo(SCREENINFO *pscrinfo)
{
	if(!pscrinfo)
		return 0;
	
	// TODO
	if(pscrinfo->iSize != sizeof(*pscrinfo))
		return 0;
	
	pscrinfo->iWidth = vid.width;
	pscrinfo->iHeight = vid.height;
	
	pscrinfo->iFlags = 0; // TODO
	
	pscrinfo->iCharHeight = 8;
	
	for(int i = 0; i < 256; ++i)
		pscrinfo->charWidths[i] = 8;
	
	return 1;
};

void SetCrosshair(SpriteHandle_t ahSprite, wrect_t aRect, int r, int g, int b)
{
	// TODO
};

struct cvar_s *RegisterVariable(const char *szName, const char *szValue, int flags)
{
	return Cvar_RegisterClientVariable(szName, szValue, flags);
};

float GetCvarFloat(const char *szName)
{
	return Cvar_VariableValue(szName);
};

char *GetCvarString(const char *szName)
{
	return Cvar_VariableString(szName);
};

int AddCommand(const char *cmd_name, void (*function)())
{
	if(!cmd_name || !*cmd_name)
		return false;
	
	//if(function == NULL)
		//return false;
	
	return Cmd_AddClientCommand(cmd_name, function);
};

int ServerCmd(const char *szCmdString)
{
	if(!szCmdString || !*szCmdString)
		return false;
	
	Cmd_TokenizeString(szCmdString);
	Cmd_ForwardToServer();
	return true;
};

int ClientCmd(const char *szCmdString)
{
	if(!szCmdString || !*szCmdString)
		return false;
	
	Cbuf_AddText(szCmdString);
	return true;
};

void GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
	//bi_trace();
	
	if(ent_num < 0 || ent_num > cl.maxclients)
		return;
	
	if(!pinfo)
		return;
	
	Q_strcpy(pinfo->name, cl.players[ent_num].name); // TODO: Q_strncpy
	
	pinfo->ping = cl.players[ent_num].ping;
	
	pinfo->thisplayer = 0; // TODO: ???
	pinfo->spectator = cl.players[ent_num].spectator;
	pinfo->packetloss = cl.players[ent_num].pl; // TODO: ???
	
	Q_strcpy(pinfo->model, ""); // TODO: Q_strncpy
	
	pinfo->topcolor = cl.players[ent_num].topcolor;
	pinfo->bottomcolor = cl.players[ent_num].bottomcolor; 
	
	pinfo->m_nSteamID = 0; // TODO: userinfo?
};

void PlaySoundByName(const char *szSound, float volume)
{
	
	S_StartStaticSound(S_FindName(szSound), vec3_origin, volume, 1.0f, PITCH_NORM);
};

void PlaySoundByIndex(int iSound, float volume)
{
	// TODO
};

/*
void AngleVectors(const float *avAngles, float *avForward, float *avRight, float *avUp)
{
	// TODO
};
*/

void ConsolePrint(const char *asMsg)
{
	Con_Print(asMsg);
};

void CenterPrint(const char *asMsg)
{
	SCR_CenterPrint(/*0,*/ asMsg /*, true*/);
};

int GetWindowCenterX()
{
	return window_center_x;
};

int GetWindowCenterY()
{
	return window_center_y;
};

void GetViewAngles(float *vAngles)
{
	if(vAngles)
		VectorCopy(cl.viewangles, vAngles);
};

void SetViewAngles(float *vAngles)
{
	if(vAngles)
		VectorCopy(vAngles, cl.viewangles);
};

int GetMaxClients()
{
	return cl.maxclients;
};

const char *PhysInfo_ValueForKey(const char *asKey)
{
	//return Info_ValueForKey(cls.physinfo, asKey); // TODO
	return "";
};

const char *ServerInfo_ValueForKey(const char *asKey)
{
	return ""; // TODO: Info_ValueForKey(cl.serverinfo, asKey);
};

float GetClientMaxspeed()
{
	// TODO
	return 0.0f;
};

int CheckParm(const char *parm, char **ppnext)
{
	// TODO
	return 0;
};

void GetMousePosition(int *apnX, int *apnY)
{
#ifdef OGS_USE_SDL
	SDL_GetMouseState(apnX, apnY);
#else
#	if defined(_WIN32)
	POINT current_pos;
	GetCursorPos(&current_pos);
	
	if(apnX)
		*apnX = current_pos.x;
	
	if(apnY)
		*apnY = current_pos.y;
#	endif
#endif
};

int IsNoClipping()
{
	return noclip_anglehack;
};

struct cl_entity_s *GetLocalPlayer()
{
	return &cl_entities[cl.playernum];
};

struct cl_entity_s *GetViewModel()
{
	return &cl.viewent;
};

struct cl_entity_s *GetEntityByIndex(int anIndex)
{
	if(anIndex < 0 || anIndex > MAX_EDICTS) // TODO: MAX_EDICTS -> cl_maxentities or something received from server
		return NULL;
	
	return &cl_entities[anIndex];
};

float GetClientTime()
{
	return cl.time;
};

struct model_s *CL_LoadModel(const char *asName, int *index)
{
	if(!asName || !*asName)
		return NULL;
	
	if(index)
		*index = -1;
	
	// TODO
	return NULL;
};

int CL_CreateVisibleEntity(int anType, struct cl_entity_s *apEnt)
{
	switch(anType)
	{
	};
	
	// TODO
	return 0;
};

const struct model_s *GetSpritePointer(SpriteHandle_t ahSprite)
{
	// TODO
	return NULL;
};

void PlaySoundByNameAtLocation(const char *asSound, float afVolume, float *avOrigin)
{
	S_StartStaticSound(S_FindName(asSound), avOrigin, afVolume, 1.0f, PITCH_NORM);
};

unsigned short PrecacheEvent(int anType, const char *asName)
{
	// TODO
	return 0;
};

void PlaybackEvent(int anFlags, const struct edict_s *apInvoker, unsigned short nIndex, float afDelay, 
						float *avOrigin, float *avAngles, 
						float afParam1, float afParam2,
						int anParam1, int anParam2,
						int abParam1, int abParam2)
{
	// TODO
};

extern float RandomFloat(float afLow, float afHigh); // TODO: already defined in server-side exports
/*
{
	// TODO
	return 0.0f;
};
*/

extern int32_t RandomLong(int32_t anLow, int32_t anHigh); // TODO: already defined in server-side exports
/*
{
	// TODO
	return 0;
};
*/

void CL_HookEvent(const char *name, void (*pfnEvent)(struct event_args_s *apArgs))
{
	if(!name || !*name)
		Sys_Error("CL_HookEvent:  Must provide a valid event name");
	
	if(!pfnEvent)
		Sys_Error("CL_HookEvent:  Must provide an event hook callback");
	
	for(int i = 0; i < MAX_EVENTS; ++i)
	{
		if(!Q_strcmp(cl.event_precache[i]->name, name))
		{
			if(cl.event_precache[i]->fnHook)
				Con_Printf("CL_HookEvent:  Called on existing hook, updating event hook");
			
			cl.event_precache[i]->fnHook = pfnEvent;
		};
	};
};

const char *GetGameDirectory()
{
	return com_gamedir;
};

struct cvar_s *GetCvarPointer(const char *asName)
{
	return Cvar_FindVar(asName);
};

const char *GetLevelName()
{
	return cl.levelname;
};

void GetScreenFade(struct screenfade_s *fade)
{
	if(!fade)
		return;
	
	// TODO
};

void SetScreenFade(struct screenfade_s *fade)
{
	if(!fade)
		return;
	
	// TODO
};

int IsSpectateOnly()
{
	// TODO
	return 0;
};

struct model_s *LoadMapSprite(const char *asFileName)
{
	// TODO
	return NULL;
};

const char *PlayerInfo_ValueForKey(int anPlayer, const char *asKey)
{
	return Info_ValueForKey(cl.players[anPlayer].userinfo, asKey);
};

void PlayerInfo_SetValueForKey(const char *asKey, const char *asValue)
{
	Info_SetValueForKey(cls.userinfo, asKey, asValue, MAX_INFO_STRING);
};

qboolean GetPlayerUniqueID(int anPlayer, char asPlayerID[16])
{
	if(anPlayer < 0 || anPlayer > cl.maxclients)
		return false;
	
	// TODO
	return false;
};

int GetTrackerIDForPlayer(int anPlayerSlot)
{
	if(anPlayerSlot < 0 || anPlayerSlot > cl.maxclients)
		return 0;
	
	// TODO
	return 0;
};

int GetPlayerForTrackerID(int anTrackerID)
{
	// TODO
	return 0;
};

int ServerCmdUnreliable(const char *asCmd)
{
	// TODO
	return 0;
};

void GetMousePos(struct tagPOINT *ppt)
{
	if(!ppt)
		return;
	
#ifdef OGS_USE_SDL
	SDL_GetMouseState(ppt.x, ppt, y);
#else
#	if defined(_WIN32)
	GetCursorPos(ppt);
#	endif
#endif
};

void SetMousePos(int x, int y)
{
#ifdef OGS_USE_SDL
	SDL_SetMouseState(x, y);
#else
#	if defined(_WIN32)
	SetCursorPos(x, y);
#	endif
#endif
};

void SetMouseEnable(qboolean abEnable)
{
#ifdef OGS_USE_SDL
	SDL_ShowCursor(abEnable);
#else
#	if defined(_WIN32)
	ShowCursor(abEnable);
#	endif
#endif
};

struct cvar_s *GetFirstCvarPtr()
{
	return &cvar_vars[0];
};

uint GetFirstCmdFunctionHandle()
{
	// TODO
	return 0;
};

uint GetNextCmdFunctionHandle(uint cmdhandle)
{
	// TODO
	return 0;
};

const char *GetCmdFunctionName(uint cmdhandle)
{
	// TODO
	return "";
};

float hudGetClientOldTime()
{
	// TODO
	return 0.0f;
};

float hudGetServerGravityValue()
{
	// TODO
	return 0.0f;
};

struct model_s *hudGetModelByIndex(int index)
{
	if(index < 0 || index > MAX_MODELS) // TODO: r u sure?
		return NULL;
	
	// TODO
	return NULL;
};

void pfnSetFilterMode(int mode)
{
	// TODO
};

void pfnSetFilterColor(float r, float g, float b)
{
	// TODO
};

void pfnSetFilterBrightness(float brightness)
{
	// TODO
};

extern sequenceEntry_s *pfnSequenceGet(const char *fileName, const char *entryName); // TODO: already defined in server-side exports
/*
{
	// TODO
	return NULL;
};
*/

void pfnSPR_DrawGeneric(int frame, int x, int y, const struct rect_s *pRect, int src, int dest, int width, int height)
{
	// TODO
};

extern sentenceEntry_s *pfnSequencePickSentence(const char *sentenceName, int pickMethod, int *picked); // TODO: already defined in server-side exports
/*
{
	// TODO
	return NULL;
};
*/

int pfnDrawString(int x, int y, const char *str, int r, int g, int b)
{
	// TODO
	return 0;
};

int pfnDrawStringReverse(int x, int y, const char *str, int r, int g, int b)
{
	//char *str_reversed = malloc(Q_strlen(str) * sizeof(char));
	return pfnDrawString(x, y, str, r, g, b);
};

const char *LocalPlayerInfo_ValueForKey(const char *key)
{
	return Info_ValueForKey(cls.userinfo, key);
};

int pfnVGUI2DrawCharacter(int x, int y, int ch, uint font)
{
	// TODO
	return 0;
};

int pfnVGUI2DrawCharacterAdd(int x, int y, int ch, int r, int g, int b, uint font)
{
	// TODO
	return 0;
};

uint COM_GetApproxWavePlayLength(const char *filename)
{
	// TODO
	return 0;
};

void *pfnGetCareerUI()
{
	// TODO
	return NULL;
};

extern int pfnIsCareerMatch(); // TODO: already defined in server-side exports
/*
{
	// TODO
	return 0;
};
*/

void pfnPlaySoundVoiceByName(const char *szSound, float volume, int pitch)
{
	// TODO
};

void pfnPrimeMusicStream(const char *szFilename, int looping)
{
	// TODO
};

double GetAbsoluteTime()
{
	return realtime; // TODO
};

void pfnProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	// TODO
};

void pfnConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	// TODO
};

void pfnResetTutorMessageDecayData()
{
	// TODO
};

void pfnPlaySoundByNameAtPitch(const char *szSound, float volume, int pitch)
{
	S_StartStaticSound(S_FindName(szSound), vec3_origin, volume, 1.0f, pitch);
};

void pfnFillRGBABlend(int x, int y, int width, int height, int r, int g, int b, int a)
{
	// TODO
};

int pfnGetAppID()
{
	// TODO
	return 0;
};

cmdalias_t *pfnGetAliasList()
{
	return cmd_alias;
};

void pfnVguiWrap2_GetMouseDelta(int *x, int *y)
{
	// TODO: VguiWrap2_GetMouseDelta
};

cl_enginefunc_t gClEngFuncs =
{
	SPR_Load,
	SPR_Frames,
	SPR_Height,
	SPR_Width,
	SPR_Set,
	SPR_Draw,
	SPR_DrawHoles,
	SPR_DrawAdditive,
	SPR_EnableScissor,
	SPR_DisableScissor,
	SPR_GetList,
	
	FillRGBA,
	
	GetScreenInfo,
	
	SetCrosshair,
	
	RegisterVariable,
	
	GetCvarFloat,
	GetCvarString,
	
	AddCommand,
	
	pfnHookUserMsg,
	
	ServerCmd,
	ClientCmd,
	
	GetPlayerInfo,
	
	PlaySoundByName,
	PlaySoundByIndex,
	
	AngleVectors,
	
	TextMessageGet,
	
	Draw_Character,
	Draw_ConsoleString,
	Draw_SetTextColor,
	Draw_ConsoleStringLen,
	
	ConsolePrint,
	CenterPrint,
	
	GetWindowCenterX,
	GetWindowCenterY,
	
	GetViewAngles,
	SetViewAngles,
	
	GetMaxClients,
	
	Cvar_SetValue,
	
	Cmd_Argc,
	Cmd_Argv,
	
	Con_Printf,
	Con_DPrintf,
	Con_NPrintf,
	Con_NXPrintf,
	
	PhysInfo_ValueForKey,
	ServerInfo_ValueForKey,
	
	GetClientMaxspeed,
	
	CheckParm,
	
	Key_Event,
	
	GetMousePosition,
	
	IsNoClipping,
	
	GetLocalPlayer,
	GetViewModel,
	GetEntityByIndex,
	
	GetClientTime,
	
	V_CalcShake,
	V_ApplyShake,
	
	PM_PointContents,
	PM_WaterEntity,
	PM_TraceLine,
	
	CL_LoadModel,
	CL_CreateVisibleEntity,
	
	GetSpritePointer,
	
	PlaySoundByNameAtLocation,
	
	EV_Precache,
	EV_Playback,
	
	CL_WeaponAnim,
	
	RandomFloat,
	RandomLong,
	
	CL_HookEvent,
	
	Con_IsVisible,
	
	GetGameDirectory,
	
	GetCvarPointer,
	
	Key_LookupBinding,
	
	GetLevelName,
	
	GetScreenFade,
	SetScreenFade,
	
	VGui_GetPanel,
	VGui_ViewportPaintBackground,
	
	COM_LoadFile,
	COM_ParseFile,
	COM_FreeFile,

	&gTriAPI,
	&efx,
	&eventapi,
	&demoapi,
	&netapi,
	&gVoiceTweak,

	IsSpectateOnly,
	LoadMapSprite,
	
	COM_AddAppDirectoryToSearchPath,
	COM_ExpandFilename,
	
	PlayerInfo_ValueForKey,
	PlayerInfo_SetValueForKey,
	
	GetPlayerUniqueID,
	
	GetTrackerIDForPlayer,
	GetPlayerForTrackerID,
	
	ServerCmdUnreliable,
	
	GetMousePos,
	SetMousePos,
	SetMouseEnable,
	
	GetFirstCvarPtr,

	GetFirstCmdFunctionHandle,
	GetNextCmdFunctionHandle,
	GetCmdFunctionName,

	hudGetClientOldTime,

	hudGetServerGravityValue,

	hudGetModelByIndex,

	pfnSetFilterMode,
	pfnSetFilterColor,
	pfnSetFilterBrightness,

	pfnSequenceGet,

	pfnSPR_DrawGeneric,

	pfnSequencePickSentence,

	pfnDrawString,
	pfnDrawStringReverse,

	LocalPlayerInfo_ValueForKey,

	pfnVGUI2DrawCharacter,
	pfnVGUI2DrawCharacterAdd,

	COM_GetApproxWavePlayLength,

	pfnGetCareerUI,

	Cvar_Set,

	pfnIsCareerMatch,

	pfnPlaySoundVoiceByName,

	pfnPrimeMusicStream,

	GetAbsoluteTime,

	pfnProcessTutorMessageDecayBuffer,
	pfnConstructTutorMessageDecayBuffer,

	pfnResetTutorMessageDecayData,

	pfnPlaySoundByNameAtPitch,

	pfnFillRGBABlend,

	pfnGetAppID,

	pfnGetAliasList,

	pfnVguiWrap2_GetMouseDelta
};

// TODO: cl_enginefunc_t *pcl_enginefuncs = &gClEngFuncs;