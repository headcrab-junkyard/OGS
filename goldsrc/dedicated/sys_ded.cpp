/// @file

#include <cstdio>

#ifdef _WIN32
	#include <windows.h>
	#include "win/conproc.h"
#endif

#include "engine_hlds_api.h"

#include "interface.h"

bool InitConsole()
{
// TODO
/*
#ifdef _WIN32
	if(!AllocConsole())
		Sys_Error("Couldn't create dedicated server console");

	hinput = GetStdHandle (STD_INPUT_HANDLE);
	houtput = GetStdHandle (STD_OUTPUT_HANDLE);

	// give QHOST a chance to hook into the console
	if ((t = COM_CheckParm ("-HFILE")) > 0)
	{
		if (t < com_argc)
			hFile = (HANDLE)Q_atoi (com_argv[t+1]);
	};
		
	if ((t = COM_CheckParm ("-HPARENT")) > 0)
	{
		if (t < com_argc)
			heventParent = (HANDLE)Q_atoi (com_argv[t+1]);
	};
		
	if ((t = COM_CheckParm ("-HCHILD")) > 0)
	{
		if (t < com_argc)
			heventChild = (HANDLE)Q_atoi (com_argv[t+1]);
	};

	InitConProc (hFile, heventParent, heventChild);
#endif // _WIN32
*/
};

int RunServer() // void?
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
	
	if(!pEngineFactory)
		return EXIT_FAILURE;
	
	auto pEngine{(IDedicatedServerAPI*)pEngineFactory(VENGINE_HLDS_API_VERSION, nullptr)};
	
	if(!pEngine)
		return EXIT_FAILURE;
	
	//char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory
	if(!pEngine->Init(".", "", nullptr, pFSFactory))
		return EXIT_FAILURE;
	
	while(true)
		pEngine->RunFrame();
	
	pEngine->Shutdown();
	
	return EXIT_SUCCESS;
};

int main(int argc, char **argv)
{
	if(!InitConsole())
		return EXIT_FAILURE;
	
	if(!RunServer())
		return EXIT_FAILURE;
	
#ifdef _WIN32
	FreeConsole();
	
	// shutdown QHOST hooks if necessary
	//DeinitConProc(); // TODO
#endif

	// return success of application
	return EXIT_SUCCESS;
};