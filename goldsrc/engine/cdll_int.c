/// @file
/// @brief this implementation handles the linking of the engine to the DLL

// NOTE: The reason why the client dll support is implemented this way is historical
// The GoldSrc is a fork of Source codebase somewhere at the beginning of November 1998
// Current Source is a single dll for both dedicated/listenserver modes (it skips client dll
// loading and does nothing inside these wrapper functions below)

// NOTE: OGS planned to be doing the same

#include "quakedef.h"

static cl_enginefunc_t gEngFuncs; // TODO: name overlap with server-side version?
cldll_func_t gClientDLL;

typedef int /*CL_DLLEXPORT*/ (*fnF)(void *pv);

void ClientDLL_Init()
{
	if(gClientDLL.pfnInit)
		gClientDLL.pfnInit();
};

int ClientDLL_VidInit()
{
	if(gClientDLL.pfnVidInit)
		return gClientDLL.pfnVidInit();
	
	return 0;
};

/*
void ClientDLL_Shutdown()
{
	if(gClientDLL.pfnShutdown)
		gClientDLL.pfnShutdown();
};
*/

void ClientDLL_Frame(double time)
{
	if(gClientDLL.pfnFrame)
		gClientDLL.pfnFrame(time);
};

int ClientDLL_Redraw(float time, int intermission)
{
	if(gClientDLL.pfnRedraw)
		return gClientDLL.pfnRedraw(time, intermission);
	
	return 0;
};

qboolean LoadClientDLLviaF()
{
	// TODO: per-export loading as a fallback
	
	pfnF fnF = NULL;
	
	//memcpy(&gEngFuncs, 0, sizeof(cl_enginefunc_t));
	//memcpy(&gClientDLL, 0, sizeof(cldll_func_t));
	
	void *pClientDLL = Sys_LoadModule("valve/cl_dll/client"); // TODO: FS_LoadLibrary or something else that will load it from the game folder instead of app folder
	
	if(!pClientDLL)
		return false;
	
	fnF = (pfnF)Sys_GetExport(pClientDLL, "F"); // [BP]: Absolutely uninformative export name
	
	if(!fnF)
		return false;
	
	fnF(&gClientDLL);
	
	if(!gClientDLL.Initialize(&gEngFuncs, CLDLL_INTERFACE_VERSION)) // TODO: So.... Are you alive?
		return false;
	
	return true;
};

void LoadClientDLL()
{
	if(!LoadClientDLLviaF()
		return;
};