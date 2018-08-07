/// @file

#include "eiface.h"

#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

#define C_EXPORT extern "C" EXPORT

//
// required prog functions
//
/*
void()          StartFrame;

void()          PlayerPreThink;
void()          PlayerPostThink;

void()          ClientKill;
void()          ClientConnect;
void()          PutClientInServer;              // call after setting the parm1... parms
void()          ClientDisconnect;

void()          SetNewParms;                    // called when a client first connects to
									// a server. sets parms so they can be
									// saved off for restarts

void()          SetChangeParms;                 // call to set parms for self so they can
									// be saved for a level transition
*/

enginefuncs_t gEngFuncs;
globalvars_t *gpGlobals{nullptr};

static DLL_FUNCTIONS gGameFuncs =
{
	// TODO
};

static NEW_DLL_FUNCTIONS gNewGameFuncs =
{
	// TODO
};

C_EXPORT void GiveFnPtrsToDll(enginefuncs_t *pEngFuncs, globalvars_t *pGlobals)
{
	if(!pEngFuncs || !pGlobals)
	{
		// TODO
		return;
	};
	
	memcpy(&gEngFuncs, pEngFuncs, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;
};

C_EXPORT int GetEntityAPI(DLL_FUNCTIONS *pGameFuncs, int nInterfaceVersion)
{
	if(!pGameFuncs || nInterfaceVersion != INTERFACE_VERSION)
	{
		// TODO
		return 0;
	};
	
	memcpy(pGameFuncs, &gGameFuncs, sizeof(DLL_FUNCTIONS));
	return 1;
};

C_EXPORT int GetEntityAPI2(DLL_FUNCTIONS *pGameFuncs, int *pInterfaceVersion)
{
	if(!pGameFuncs || !pInterfaceVersion)
	{
		// TODO
		return 0;
	};
	
	if(*pInterfaceVersion != INTERFACE_VERSION)
	{
		*pInterfaceVersion = INTERFACE_VERSION;
		return 0;
	};
	
	memcpy(pGameFuncs, &gGameFuncs, sizeof(DLL_FUNCTIONS));
	return 1;
};

int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pFuncs, int *pInterfaceVersion)
{
	if(!pFuncs || !pInterfaceVersion)
		return 0;
	
	if(*pInterfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
	{
		*pInterfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return 0;
	};
	
	memcpy(pFuncs, &gNewGameFuncs, sizeof(NEW_DLL_FUNCTIONS));
	return 1;
};