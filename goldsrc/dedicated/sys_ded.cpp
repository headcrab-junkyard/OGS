/// @file

#include <cstdio>

#include "IEngine.hpp"

#define INTERFACE_IMPL
#include "Interface.hpp"

// TEMP DUPLICATE
struct quakeparms_t
{
	char	*basedir;
	char	*cachedir;		// for development over ISDN lines
	
	int		argc;
	char	**argv;
	
	void	*membase;
	int		memsize;
};

using pfnHost_Init = void (*)(quakeparms_t *parms);
using pfnHost_Frame = void (*)(float frametime);
using pfnHost_Shutdown = void (*)();

using pfnGetEngine = IEngine *(*)();

void RunServer()
{
};

int main(int argc, char **argv)
{
	auto pFSLib{Sys_LoadModule("filesystem_stdio")};
	
	if(!pFSLib)
		return EXIT_FAILURE;
	
	auto pFSFactory{Sys_GetFactory(pFSLib)};
	
	if(!pFSFactory)
		return EXIT_FAILURE;
	
	auto pEngineLib{Sys_LoadModule("engine")};
	
	if(!pEngineLib)
		return EXIT_FAILURE;
	
	auto pEngineFactory{Sys_GetFactory(pEngineLib)};
	
	//if(!pEngineFactory)
		//return EXIT_FAILURE;
	
	auto fnHost_Init{(pfnHost_Init)Sys_GetExport(pEngineLib, "Host_Init")};
	auto fnHost_Frame{(pfnHost_Frame)Sys_GetExport(pEngineLib, "Host_Frame")};
	auto fnHost_Shutdown{(pfnHost_Shutdown)Sys_GetExport(pEngineLib, "Host_Shutdown")};
	
	if(!fnHost_Init || !fnHost_Frame || !fnHost_Shutdown)
		return EXIT_FAILURE;
	
	auto fnGetEngine{(pfnGetEngine)Sys_GetExport(pEngineLib, "GetEngine")};
	
	if(!fnGetEngine)
		return EXIT_FAILURE;
	
	auto pEngine{fnGetEngine()};
	
	if(!pEngine)
		return EXIT_FAILURE;
	
	//pEngineFactory();
	
	quakeparms_t parms{};
	
	parms.argc = argc;
	parms.argv = argv;
	
	fnHost_Init(&parms);
	//pEngine->Init();
	
	while(true)
		fnHost_Frame(0.1f);
		//pEngine->Frame();
	
	fnHost_Shutdown();
	//pEngine->Shutdown();
	
	return EXIT_SUCCESS;
};