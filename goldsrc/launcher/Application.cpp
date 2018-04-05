/// @file

#include "Application.hpp"

CApplication::CApplication() = default;
CApplication::~CApplication() = default;

int CApplication::Run()
{
	do
	{
		if(!Init())
			return EXIT_FAILURE;
		
		//char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory
		if(!mpEngine->Run(nullptr, ".", "", "", nullptr, mpFSFactory))
			return EXIT_FAILURE;
		
		mbRestart = false; // TODO
		
		Shutdown();
	}
	while(mbRestart);
	
	// return success of application
	return EXIT_SUCCESS;
};

bool CApplication::Init()
{
	if(!LoadFileSystemModule("filesystem_stdio"))
		return false;
	
	mpEngineLib = Sys_LoadModule("engine");
	
	if(!mpEngineLib)
		return false;
	
	auto pEngineFactory{Sys_GetFactory(mpEngineLib)};
	
	if(!pEngineFactory)
		return false;
	
	mpEngine = (IEngineAPI*)pEngineFactory(VENGINE_LAUNCHER_API_VERSION, nullptr);
	
	if(!mpEngine)
		return false;
	
	return PostInit();
};

void CApplication::Shutdown()
{
	// TODO
};

bool CApplication::LoadFileSystemModule(const char *name)
{
	mpFSLib = Sys_LoadModule(name);
	
	if(!mpFSLib)
		return false;
	
	mpFSFactory = Sys_GetFactory(mpFSLib);
	
	if(!mpFSFactory)
		return false;
	
	return true;
};