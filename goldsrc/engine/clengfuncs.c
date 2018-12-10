/// @file
/// @brief client-side engine functions exports

#include "quakedef.h"
#include "spriteapi.h"
#include "usermsg.h"
#include "textmessage.h"
#include "draw.h"
#include "view.h"
#include "vgui_int.h"
#include "event_api.h"
#include "demo_api.h"
#include "net_api.h"
#include "voicetweak.h"

void FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
	// TODO
};

int GetScreenInfo(SCREENINFO *pscrinfo)
{
	if(!pscrinfo)
		return 0;
	
	// TODO
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
	return Cmd_AddClientCommand(cmd_name, function);
};

int ServerCmd(const char *szCmdString)
{
	// TODO
	return 0;
};

int ClientCmd(const char *szCmdString)
{
	// TODO
	return 0;
};

void GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
	// TODO
};

void PlaySoundByName(const char *szSound, float volume)
{
	// TODO
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
	// TODO
};

int GetWindowCenterX()
{
	// TODO
	return 0;
};

int GetWindowCenterY()
{
	// TODO
	return 0;
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
	// TODO
	return "";
};

const char *ServerInfo_ValueForKey(const char *asKey)
{
	// TODO
	return "";
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
	// TODO
};

int IsNoClipping()
{
	// TODO
	return 0;
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
	if(anIndex < 0 || anIndex > MAX_EDICTS) // TODO
		return NULL;
	
	return &cl_entities[anIndex];
};

float GetClientTime()
{
	return cl.time;
};

struct model_s *CL_LoadModel(const char *asName, int *index)
{
	// TODO
	return NULL;
};

int CL_CreateVisibleEntity(int anType, struct cl_entity_s *apEnt)
{
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
	// TODO
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
	
	// TODO
	//for(check every hook)
	{
		//if(found an already assigned)
		{
			//Con_Printf("CL_HookEvent:  Called on existing hook, updating event hook");
			//event.hook = newhook;
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
	
	// TODO
};

void SetMousePos(int x, int y)
{
	// TODO
};

void SetMouseEnable(qboolean abEnable)
{
	// TODO
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
	// TODO
	return 0;
};

const char *LocalPlayerInfo_ValueForKey(const char *key)
{
	// TODO
	return "";
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
	// TODO
	return realtime;
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
	// TODO
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
	// TODO
	return NULL;
};

void pfnVguiWrap2_GetMouseDelta(int *x, int *y)
{
	// TODO
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
	
	HookUserMsg,
	
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
	
	PrecacheEvent,
	PlaybackEvent,
	
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

	NULL, //&triapi, // TODO
	NULL, //&efxapi, // TODO
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