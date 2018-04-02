/// @file

#pragma once

#include "interface.h"

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
		
		auto pEngine{(IEngineAPI*)pEngineFactory(VENGINE_LAUNCHER_API_VERSION, nullptr)};
		
		if(!pEngine)
			return EXIT_FAILURE;
		
		//char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory
		if(!pEngine->Run(".", "", nullptr, pFSFactory))
			return EXIT_FAILURE;
	};
};