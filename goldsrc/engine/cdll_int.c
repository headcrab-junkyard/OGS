/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018-2019 BlackPhrase
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
/// @brief this implementation handles the linking of the engine to the DLL

// NOTE: The reason why the client dll support is implemented this way is historical
// The GoldSrc is a fork of Source codebase somewhere at the beginning of November 1998
// Current Source is a single dll for both dedicated/listenserver modes, it skips client dll
// loading and does nothing inside these wrapper functions below (but it also contains a special 
// dll for dedicated server so this feature is mostly useless)

#include "quakedef.h"

void *gpClientDLL;

static cl_enginefunc_t gEngFuncs; // TODO: name overlap with server-side version? cl_enginefuncs in GS
cldll_func_t cl_funcs;

typedef int (*pfnGetClientDLL)(void *pv);

extern qboolean LoadClientDLLClientGame();

void UnloadClientDLL()
{
	if(gpClientDLL)
	{
		Sys_UnloadModule_Wrapper(gpClientDLL);
		gpClientDLL = NULL;
	};
};

qboolean LoadClientDLLF()
{
	pfnGetClientDLL fnGetClientDLL = NULL;
	
	fnGetClientDLL = (pfnGetClientDLL)Sys_GetExport_Wrapper(gpClientDLL, "F"); // TODO: GetClientDLL?

	if(!fnGetClientDLL)
		return false;
	
	Q_memset(&cl_funcs, 0, sizeof(cl_funcs));
	
	fnGetClientDLL(&cl_funcs);

	return true;
};

void LoadClientDLL()
{
	char sClientDLLPath[256];
	snprintf(sClientDLLPath, sizeof(sClientDLLPath) - 1, "%s/cl_dlls/client", com_gamedir);
	
	gpClientDLL = FS_LoadLibrary(sClientDLLPath);

	if(!gpClientDLL)
	{
		Sys_Error("could not load library %s", sClientDLLPath);
		//return;
	};

	if(!LoadClientDLLClientGame()) // First check for IClientGame interface
		if(!LoadClientDLLF()) // Then check there is an "F" export present
			return; // TODO: per-export loading as a fallback
	
	if(!&cl_funcs || (cl_funcs.pfnInitialize && !cl_funcs.pfnInitialize(&gEngFuncs, CLDLL_INTERFACE_VERSION)))
		Sys_Error("Can't initialize the client dll!");
};

void ClientDLL_Init()
{
	LoadClientDLL();
	
	// TODO
	ClientDLL_HudInit();
	ClientDLL_HudVidInit();
	ClientDLL_ClientMoveInit(&clpmove);
};

void ClientDLL_Shutdown()
{
	if(cl_funcs.pfnShutdown)
		cl_funcs.pfnShutdown();
	
	UnloadClientDLL();
};

void ClientDLL_HudInit()
{
	if(cl_funcs.pfnHudInit)
		cl_funcs.pfnHudInit();
};

void ClientDLL_HudVidInit()
{
	if(cl_funcs.pfnHudVidInit)
		cl_funcs.pfnHudVidInit();
};

void ClientDLL_UpdateClientData()
{
	if(cl_funcs.pfnUpdateClientData)
		cl_funcs.pfnUpdateClientData(NULL, 0.0f); // TODO
};

// TODO
/*
void ClientDLL_HudReset()
{
	if(cl_funcs.pfnHudReset)
		cl_funcs.pfnHudReset();
};
*/

void ClientDLL_Frame(double time)
{
	if(cl_funcs.pfnFrame)
		cl_funcs.pfnFrame(time);
};

void ClientDLL_HudRedraw(int intermission)
{
	if(cl_funcs.pfnHudRedraw)
		cl_funcs.pfnHudRedraw(host_frametime /*cl.time*/, intermission); // TODO
};

void ClientDLL_MoveClient(struct playermove_s *ppmove)
{
	if(cl_funcs.pfnClientMove)
		cl_funcs.pfnClientMove(ppmove, false);
};

void ClientDLL_ClientMoveInit(struct playermove_s *ppmove)
{
	if(cl_funcs.pfnClientMoveInit)
		cl_funcs.pfnClientMoveInit(ppmove);
};

char ClientDLL_ClientTextureType(const char *name)
{
	if(cl_funcs.pfnClientTextureType)
		return cl_funcs.pfnClientTextureType(name);
	
	return '\0';
};

void ClientDLL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	if(cl_funcs.pfnCreateMove)
		cl_funcs.pfnCreateMove(frametime, cmd, active);
};

void ClientDLL_ActivateMouse()
{
	if(cl_funcs.pfnIN_ActivateMouse)
		cl_funcs.pfnIN_ActivateMouse();
};

void ClientDLL_DeactivateMouse()
{
	if(cl_funcs.pfnIN_DeactivateMouse)
		cl_funcs.pfnIN_DeactivateMouse();
};

void ClientDLL_MouseEvent(int mstate)
{
	if(cl_funcs.pfnIN_MouseEvent)
		cl_funcs.pfnIN_MouseEvent(mstate);
};

void ClientDLL_ClearStates()
{
	if(cl_funcs.pfnIN_ClearStates)
		cl_funcs.pfnIN_ClearStates();
};

int ClientDLL_IsThirdPerson()
{
	if(cl_funcs.pfnIsThirdPerson)
		return cl_funcs.pfnIsThirdPerson();
	
	return 0;
};

void ClientDLL_GetCameraOffsets(float *ofs)
{
	if(cl_funcs.pfnGetCameraOffsets)
		cl_funcs.pfnGetCameraOffsets(ofs);
};

int ClientDLL_GraphKeyDown()
{
	// TODO
	//kbutton_s *pGraphKey = ClientDLL_FindKey("in_graph");
	
	//if(pGraphKey)
		//return pGraphKey->down;
	
	return 0;
};

struct kbutton_s *ClientDLL_FindKey(const char *name)
{
	if(cl_funcs.pfnFindKey)
		return cl_funcs.pfnFindKey(name);
	
	return NULL;
};

void ClientDLL_CAM_Think()
{
	if(cl_funcs.pfnCAM_Think)
		cl_funcs.pfnCAM_Think();
};

void ClientDLL_IN_Accumulate()
{
	if(cl_funcs.pfnIN_Accumulate)
		cl_funcs.pfnIN_Accumulate();
};

void ClientDLL_CalcRefdef(struct ref_params_s *pparams)
{
	if(cl_funcs.pfnCalcRefdef)
		cl_funcs.pfnCalcRefdef(pparams);
};

int ClientDLL_AddEntity(int type, struct cl_entity_s *ent)
{
	if(cl_funcs.pfnAddEntity)
		return cl_funcs.pfnAddEntity(type, ent, ""); // TODO
	
	return 0;
};

void ClientDLL_CreateEntities()
{
	if(cl_funcs.pfnCreateEntities)
		cl_funcs.pfnCreateEntities();
};

void ClientDLL_DrawNormalTriangles()
{
	if(cl_funcs.pfnDrawNormalTriangles)
		cl_funcs.pfnDrawNormalTriangles();
};

void ClientDLL_DrawTransparentTriangles()
{
	if(cl_funcs.pfnDrawTransparentTriangles)
		cl_funcs.pfnDrawTransparentTriangles();
};

void ClientDLL_StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity)
{
	if(cl_funcs.pfnStudioEvent)
		cl_funcs.pfnStudioEvent(event, entity);
};

void ClientDLL_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
	if(cl_funcs.pfnPostRunCmd)
		cl_funcs.pfnPostRunCmd(from, to, cmd, runfuncs, time, random_seed);
};

void ClientDLL_TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client)
{
	if(cl_funcs.pfnTxferLocalOverrides)
		cl_funcs.pfnTxferLocalOverrides(state, client);
};

void ClientDLL_ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src)
{
	if(cl_funcs.pfnProcessPlayerState)
		cl_funcs.pfnProcessPlayerState(dst, src);
};

void ClientDLL_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd)
{
	if(cl_funcs.pfnTxferPredictionData)
		cl_funcs.pfnTxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
};

void ClientDLL_ReadDemoBuffer(int size, byte *buffer)
{
	if(cl_funcs.pfnReadDemoBuffer)
		cl_funcs.pfnReadDemoBuffer(size, buffer);
};

int ClientDLL_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	if(cl_funcs.pfnConnectionlessPacket)
		return cl_funcs.pfnConnectionlessPacket(net_from, args, response_buffer, response_buffer_size);
	
	return 0;
};

int ClientDLL_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	if(cl_funcs.pfnGetHullBounds)
		return cl_funcs.pfnGetHullBounds(hullnumber, mins, maxs);
	
	return 0;
};

void ClientDLL_VGui_ConsolePrint(const char *text) // TODO: why it's here????
{
	// TODO
	VGui_ConsolePrint(text);
};

int ClientDLL_Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	if(cl_funcs.pfnKey_Event)
		return cl_funcs.pfnKey_Event(down, keynum, pszCurrentBinding);

	return 0;
};

void ClientDLL_TempEntUpdate(double ft, double ct, double grav, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int (*addEntity)(struct cl_entity_s *pEntity), void (*playTESound)(struct tempent_s *pTemp, float damp))
{
	if(cl_funcs.pfnTempEntUpdate)
		cl_funcs.pfnTempEntUpdate(ft, ct, grav, ppFreeTE, ppActiveTE, addEntity, playTESound);
};

struct cl_entity_s *ClientDLL_GetUserEntity(int index)
{
	if(cl_funcs.pfnGetUserEntity)
		return cl_funcs.pfnGetUserEntity(index);
	
	return NULL;
};

void ClientDLL_VoiceStatus(int entindex, qboolean bTalking)
{
	if(cl_funcs.pfnVoiceStatus)
		cl_funcs.pfnVoiceStatus(entindex, bTalking);
};

void ClientDLL_DirectorMessage(int iSize, void *pbuf)
{
	if(cl_funcs.pfnDirectorMessage)
		cl_funcs.pfnDirectorMessage(iSize, pbuf);
};

// TODO
/*
int ClientDLL_StudioInterface(int version, struct r_studio_interface_s **ppInterface, struct engine_studio_api_s *pStudio)
{
	if(cl_funcs.pfnStudioInterface)
		return cl_funcs.pfnStudioInterface(version, ppInterface, pStudio);
	
	return 0;
};
*/

void ClientDLL_ChatInputPosition(int *x, int *y)
{
	if(cl_funcs.pfnChatInputPosition)
		cl_funcs.pfnChatInputPosition(x, y);
};

// TODO
/*
int ClientDLL_GetPlayerTeam(int playernum)
{
	if(cl_funcs.pfnGetPlayerTeam)
		return cl_funcs.pfnGetPlayerTeam(playernum);
	
	return 0;
};
*/

// TODO
/*
void *ClientDLL_GetFactory() // TODO: actually returns CreateInterfaceFn
{
	if(cl_funcs.pfnClientFactory)
		return cl_funcs.pfnClientFactory();
	
	return NULL;
};
*/