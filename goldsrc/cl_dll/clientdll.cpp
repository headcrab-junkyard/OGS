/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "const.h"
#include "engine.h"
#include "interface.h"
#include "input.h"

extern "C"
{
#include "pm_shared.h"
};

cl_enginefunc_t *gpEngine{nullptr};

// TODO
/*
int UserMsgHook_Test(const char *sName, int nSize, void *pBuf)
{
	return 0;
};
*/

int Initialize(cl_enginefunc_t *apEngFuncs, int anVersion)
{
	if(anVersion != CLDLL_INTERFACE_VERSION)
		return 0;
	
	gpEngine = apEngFuncs;
	
	//gpEngine->pfnHookUserMsg("Test", UserMsgHook_Test);
	return 1;
};

void HUD_Init()
{
	InitInput();
};

int HUD_VidInit()
{
	return 0;
};

int HUD_Redraw(float time, int intermission)
{
	return 0;
};

int HUD_UpdateClientData(struct clientdata_s *pData, float time)
{
	IN_Commands();
	return 0;
};

void HUD_Reset()
{
};

void HUD_ClientMove(struct playermove_s *ppmove, qboolean server)
{
	PM_Move(ppmove, server);
};

void HUD_ClientMoveInit(struct playermove_s *ppmove)
{
	PM_Init(ppmove);
};

char HUD_ClientTextureType(const char *name)
{
	return PM_GetTextureType(name);
};

int CL_IsThirdPerson()
{
	return 0;
};

void CL_GetCameraOffsets(float *ofs)
{
};

struct kbutton_s *KB_FindKey(const char *name)
{
	return nullptr;
};

void CAM_Think()
{
};

int HUD_AddEntity(int type, struct cl_entity_s *ent, const char *modelname)
{
	return 0;
};

void HUD_CreateEntities()
{
};

void HUD_DrawNormalTriangles()
{
};

void HUD_DrawTransparentTriangles()
{
};

void HUD_StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity)
{
};

void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
};

void HUD_Shutdown()
{
};

void HUD_TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client)
{
};

void HUD_ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src)
{
};

void HUD_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd)
{
};

void CL_ReadDemoBuffer(int size, byte *buffer)
{
};

int HUD_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	return 0;
};

int HUD_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	return 0;
};

void HUD_Frame(double time)
{
};

int HUD_Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	return 0;
};

void HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int (*addEntity)(struct cl_entity_s *pEntity), void (*playTESound)(struct tempent_s *pTemp, float damp))
{
};

struct cl_entity_s *HUD_GetUserEntity(int index)
{
	return nullptr;
};

void HUD_VoiceStatus(int entindex, qboolean bTalking)
{
};

void HUD_DirectorMessage(int iSize, void *pbuf)
{
};

void HUD_ChatInputPosition(int *x, int *y)
{
};

int HUD_GetPlayerTeam(int playernum)
{
	return 0;
};

void *HUD_ClientFactory()
{
	return (void*)Sys_GetFactoryThis();
};