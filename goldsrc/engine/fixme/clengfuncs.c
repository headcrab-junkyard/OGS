/// @file
/// @brief client-side engine functions exports

#include "quakedef.h"
#include "spriteapi.h"

void FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
};

int GetScreenInfo(SCREENINFO *pscrinfo)
{
	return 0;
};

void SetCrosshair(SpriteHandle_t ahSprite, wrect_t aRect, int r, int g, int b)
{
};

struct cvar_s *RegisterVariable(char *szName, char *szValue, int flags)
{
	return NULL;
};

float GetCvarFloat(char *szName)
{
	return 0.0f;
};

char *GetCvarString(char *szName)
{
	return "";
};

int AddCommand(char *cmd_name, void (*function)())
{
	return 0;
};

int HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	return 0;
};

int ServerCmd(char *szCmdString)
{
	return 0;
};

int ClientCmd(char *szCmdString)
{
	return 0;
};

void GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
};

void PlaySoundByName(char *szSound, float volume)
{
};

void PlaySoundByIndex(int iSound, float volume)
{
};

void AngleVectors(const float *avAngles, float *avForward, float *avRight, float *avUp)
{
};

client_textmessage_t *TextMessageGet(const char *asName)
{
	return NULL;
};

int DrawCharacter(int x, int y, int number, int r, int g, int b)
{
	return 0;
};

int DrawConsoleString(int x, int y, char *string)
{
	return 0;
};

void DrawSetTextColor(float r, float g, float b)
{
};

void DrawConsoleStringLen(const char *string, int *length, int *height)
{
};

void ConsolePrint(const char *asMsg)
{
};

void CenterPrint(const char *asMsg)
{
};

int GetWindowCenterX()
{
	return 0;
};

int GetWindowCenterY()
{
	return 0;
};

void GetViewAngles(float *)
{
};

void SetViewAngles(float *)
{
};

int GetMaxClients()
{
	return 1;
};

void Cvar_SetValue(char *cvar, float afValue)
{
};

int Cmd_Argc()
{
	return 0;
};

char *Cmd_Argv(int anArg)
{
	return "";
};

void Con_Printf(char *fmt, ...)
{
};

void Con_DPrintf(char *fmt, ...)
{
};

void Con_NPrintf(int anPos, char *fmt, ...)
{
};

void Con_NXPrintf(struct con_nprint_s *apInfo, char *fmt, ...)
{
};

const char *PhysInfo_ValueForKey(const char *asKey)
{
	return "";
};

const char *ServerInfo_ValueForKey(const char *asKey)
{
	return "";
};

float GetClientMaxspeed()
{
	return 0.0f;
};

int CheckParm(char *parm, char **ppnext)
{
	return 0;
};

void Key_Event(int anKey, int anDown)
{
};

void GetMousePosition(int *apnX, int *apnY)
{
};

int IsNoClipping()
{
	return 0;
};

struct cl_entity_s *GetLocalPlayer()
{
	return NULL;
};

struct cl_entity_s *GetViewModel()
{
	return NULL;
};

struct cl_entity_s *GetEntityByIndex(int anIndex)
{
	return NULL;
};

float GetClientTime()
{
	return 0.0f;
};

void V_CalcShake()
{
};

void V_ApplyShake(float *avOrigin, float *avAngles, float afFactor)
{
};

int PM_PointContents(float *point, int *truecontents)
{
	return 0;
};

int PM_WaterEntity(float *p)
{
	return 0;
};

struct pmtrace_s *PM_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe)
{
	return NULL;
};

struct model_s *CL_LoadModel(const char *asName, int *index)
{
	return NULL;
};

int CL_CreateVisibleEntity(int anType, struct cl_entity_s *apEnt)
{
	return 0;
};

const struct model_s *GetSpritePointer(SpriteHandle_t ahSprite)
{
	return NULL;
};

void PlaySoundByNameAtLocation(char *asSound, float afVolume, float *avOrigin)
{
};

unsigned short PrecacheEvent(int anType, const char *asName)
{
	return 0;
};

void PlaybackEvent(int anFlags, const struct edict_s *apInvoker, unsigned short nIndex, float afDelay, 
						float *avOrigin, float *avAngles, 
						float afParam1, float afParam2,
						int anParam1, int anParam2,
						int abParam1, int abParam2)
{
};

void WeaponAnim(int anAnim, int anBody)
{
};

float RandomFloat(float afLow, float afHigh)
{
	return 0.0f;
};

long RandomLong(long anLow, long anHigh)
{
	return 0;
};

void HookEvent(char *name, void (*pfnEvent)(struct event_args_s *apArgs))
{
};

int Con_IsVisible()
{
	return 0;
};

const char *GetGameDirectory()
{
	return "";
};

struct cvar_s *GetCvarPointer(const char *asName)
{
	return NULL;
};

const char *Key_LookupBinding(const char *asBinding)
{
	return "";
};

const char *GetLevelName()
{
	return "";
};

void GetScreenFade(struct screenfade_s *fade)
{
};

void SetScreenFade(struct screenfade_s *fade)
{
};

void *VGui_GetPanel()
{
	return NULL;
};

void VGui_ViewportPaintBackground(int extents[4])
{
};

byte *COM_LoadFile(char *path, int usehunk, int *pLength)
{
	return NULL;
};

char *COM_ParseFile(char *data, char *token)
{
	return NULL;
};

void COM_FreeFile(void *buffer)
{
};

int IsSpectateOnly()
{
	return 0;
};

struct model_s *LoadMapSprite(const char *asFileName)
{
	return NULL;
};

void COM_AddAppDirectoryToSearchPath(const char *asBaseDir, const char *asAppName)
{
};

int COM_ExpandFilename(const char *asFileName, char *asNameOutBuffer, int anOutBufferSize)
{
	return 0;
};

const char *PlayerInfo_ValueForKey(int anPlayer, const char *asKey)
{
	return "";
};

void PlayerInfo_SetValueForKey(const char *asKey, const char *asValue)
{
};

qboolean GetPlayerUniqueID(int anPlayer, char asPlayerID[16])
{
	return false;
};

int GetTrackerIDForPlayer(int anPlayerSlot)
{
	return 0;
};

int GetPlayerForTrackerID(int anTrackerID)
{
	return 0;
};

int ServerCmdUnreliable(char *asCmd)
{
	return 0;
};

void GetMousePos(struct tagPOINT *ppt)
{
};

void SetMousePos(int x, int y)
{
};

void SetMouseEnable(qboolean abEnable)
{
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
	
	DrawCharacter,
	DrawConsoleString,
	DrawSetTextColor,
	DrawConsoleStringLen,
	
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
	
	WeaponAnim,
	
	RandomFloat,
	RandomLong,
	
	HookEvent,
	
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
	NULL, //&event_api, // TODO
	NULL, //&demoapi, // TODO
	NULL, //&netapi, // TODO
	NULL, //&gpVoiceTweak, // TODO

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
	SetMouseEnable
};