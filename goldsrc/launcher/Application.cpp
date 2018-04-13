/// @file

#include <stdexcept>
#include "Application.hpp"

CApplication::CApplication() = default;
CApplication::~CApplication() = default;

int CApplication::Run()
{
	do
	{
		if(!Init())
			return EXIT_FAILURE;
		
		if(!mpEngine->Run(nullptr, ".", "", "", nullptr, mfnFSFactory))
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
		throw std::runtime_error(std::string("Failed to load the filesystem module!"));
	
	mpEngineLib = Sys_LoadModule("engine");
	
	if(!mpEngineLib)
		throw std::runtime_error(std::string("Failed to load the engine module!"));
	
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
	if(!name || !*name)
	{
		//throw std::invalid_argument(std::string("Argument passed to CApplication::LoadFileSystemModule is invalid! (").append(name).append(")"));
		return false;
	};
	
	mpFSLib = Sys_LoadModule(name);
	
	if(!mpFSLib)
		return false;
	
	mfnFSFactory = Sys_GetFactory(mpFSLib);
	
	if(!mfnFSFactory)
		return false;
	
	return true;
};