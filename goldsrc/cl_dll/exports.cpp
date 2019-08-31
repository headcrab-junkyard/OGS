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

#include <cstring>

#include "cdll_int.h"

#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

#define C_EXPORT extern "C" EXPORT

extern int Initialize(cl_enginefunc_t *apEngFuncs, int anVersion);
extern void HUD_Init();
extern int HUD_VidInit();
extern int HUD_Redraw(float time, int intermission);
extern int HUD_UpdateClientData(struct clientdata_s *pData, float time);
extern void HUD_Reset();
extern void HUD_ClientMove(struct playermove_s *ppmove, qboolean server);
extern void HUD_ClientMoveInit(struct playermove_s *ppmove);
extern char HUD_ClientTextureType(const char *name);
extern void IN_ActivateMouse();
extern void IN_DeactivateMouse();
extern void IN_MouseEvent(int mstate);
extern void IN_ClearStates();
extern void IN_Accumulate();
extern void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);
extern int CL_IsThirdPerson();
extern void CL_GetCameraOffsets(float *ofs);
extern struct kbutton_s *KB_FindKey(const char *name);
extern void CAM_Think();
extern void V_CalcRefdef(struct ref_params_s *pparams);
extern int HUD_AddEntity(int type, struct cl_entity_s *ent, const char *modelname);
extern void HUD_CreateEntities();
extern void HUD_DrawNormalTriangles();
extern void HUD_DrawTransparentTriangles();
extern void HUD_StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity);
extern void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
extern void HUD_Shutdown();
extern void HUD_TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client);
extern void HUD_ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src);
extern void HUD_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
extern void CL_ReadDemoBuffer(int size, byte *buffer);
extern int HUD_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
extern int HUD_GetHullBounds(int hullnumber, float *mins, float *maxs);
extern void HUD_Frame(double time);
extern int HUD_Key_Event(int down, int keynum, const char *pszCurrentBinding);
extern void HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int (*addEntity)(struct cl_entity_s *pEntity), void (*playTESound)(struct tempent_s *pTemp, float damp));
extern struct cl_entity_s *HUD_GetUserEntity(int index);
extern void HUD_VoiceStatus(int entindex, qboolean bTalking);
extern void HUD_DirectorMessage(int iSize, void *pbuf);
extern int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppInterface, struct engine_studio_api_s *pStudio);
extern void HUD_ChatInputPosition(int *x, int *y);
extern int HUD_GetPlayerTeam(int playernum);
extern void *HUD_ClientFactory();

cldll_func_t gClientFuncs =
{
	Initialize,
	HUD_Init,
	HUD_VidInit,
	HUD_Redraw,
	HUD_UpdateClientData,
	HUD_Reset,
	HUD_ClientMove,
	HUD_ClientMoveInit,
	HUD_ClientTextureType,
	IN_ActivateMouse,
	IN_DeactivateMouse,
	IN_MouseEvent,
	IN_ClearStates,
	IN_Accumulate,
	CL_CreateMove,
	CL_IsThirdPerson,
	CL_GetCameraOffsets,
	KB_FindKey,
	CAM_Think,
	V_CalcRefdef,
	HUD_AddEntity,
	HUD_CreateEntities,
	HUD_DrawNormalTriangles,
	HUD_DrawTransparentTriangles,
	HUD_StudioEvent,
	HUD_PostRunCmd,
	HUD_Shutdown,
	HUD_TxferLocalOverrides,
	HUD_ProcessPlayerState,
	HUD_TxferPredictionData,
	CL_ReadDemoBuffer,
	HUD_ConnectionlessPacket,
	HUD_GetHullBounds,
	HUD_Frame,
	HUD_Key_Event,
	HUD_TempEntUpdate,
	HUD_GetUserEntity,
	HUD_VoiceStatus,
	HUD_DirectorMessage,
	HUD_GetStudioModelInterface,
	HUD_ChatInputPosition,
	HUD_GetPlayerTeam,
	HUD_ClientFactory
};

C_EXPORT void F(void /*cldll_func_t*/ *pv)
{
	if(!pv)
		return;
	
	cldll_func_t *pcl_funcs = (cldll_func_t*)pv;
	*pcl_funcs = gClientFuncs;
};