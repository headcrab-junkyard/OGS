/// @file

#pragma once

#define INTERFACE_IMPL
#include "Interface.hpp"

#include "engine_launcher_api.h"

struct IApplication;

int AppMain(IApplication *apApplication);

struct IApplication
{
	virtual bool Init() = 0;
	virtual bool LoadFileSystemModule() = 0;
	
	bool Init(const char *cmdline)
	{
		char sCmdLine[512];
		sCmdLine[0] = '\0';
		
		for(int i = 0; i < strlen(cmdline); ++i)
			strcat(sCmdLine, cmdline);
		
		return Init();
	};
	
	bool CommonInit()
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
	};
};