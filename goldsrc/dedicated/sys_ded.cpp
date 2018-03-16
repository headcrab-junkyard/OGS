/// @file

#include <cstdio>

#include "engine_hlds_api.h"

#define INTERFACE_IMPL
#include "Interface.hpp"

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
	if(!RunServer())
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
};