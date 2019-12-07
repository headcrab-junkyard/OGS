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

#include <stdexcept>
#include "Application.hpp"
#include "AppConfig.hpp"

//const char *gsPostRestartCmdLine{""}; // TODO

int setenv(const char *name, const char *value, int overwrite);

CApplication::CApplication() = default;
CApplication::~CApplication() = default;

int CApplication::Run()
{
	do
	{
		if(!Init())
			return EXIT_FAILURE;
		
		if(!mpEngine->Run(nullptr, ".", msCmdLine, nullptr, Sys_GetFactoryThis(), mfnFSFactory))
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
	// File system module name to load
	const char *sFSModuleName{Config::Defaults::FSModuleName};
	
	LoadFileSystemModule(sFSModuleName);
	
	// Engine module name to load
	const char *sEngineModuleName{Config::Defaults::EngineModuleName};

	// Do we have an engine module name config var stored on the system?
	char *sPrevEngineModule{getenv("OGS_ENGINE_MODULE")};
	
	// Yes? Nice, then use it!
	if(sPrevEngineModule && *sPrevEngineModule)
		sEngineModuleName = sPrevEngineModule;
	
	// Any overrides?
	if(strstr(msCmdLine, "-hw"))
		// Skip checking for sw
		sEngineModuleName = "hw";
	else
		if(strstr(msCmdLine, "-sw"))
			// Skip checking for hw
			sEngineModuleName = "sw";

	// Try to load it
	LoadEngineModule(sEngineModuleName);
	
	// Engine module was successfully loaded, so store its name
	setenv("OGS_ENGINE_MODULE", sEngineModuleName, 1);
	
	mpEngine = static_cast<IEngineAPI*>(mfnEngineFactory(VENGINE_LAUNCHER_API_VERSION, nullptr));
	
	if(!mpEngine)
		throw std::runtime_error(std::string("Failed to get the engine launcher interface (") + sEngineModuleName + ")!");
	
	return PostInit();
};

void CApplication::Shutdown()
{
	// TODO
};

void CApplication::LoadFileSystemModule(const char *asName)
{
	if(!asName || !*asName)
	{
		//throw std::invalid_argument(std::string("Argument passed to CApplication::LoadFileSystemModule is invalid! (").append(asName).append(")"));
		return;
	};
	
	auto pFSLib{Sys_LoadModule(asName)};
	
	if(!pFSLib)
		throw std::runtime_error(std::string("Failed to load the filesystem module (") + asName + ")!");
	
	mpFSLib = pFSLib;
	
	auto fnFSFactory{Sys_GetFactory(mpFSLib)};
	
	if(!fnFSFactory)
		throw std::runtime_error(std::string("Failed to get the filesystem module factory (") + asName + ")!");
	
	mfnFSFactory = fnFSFactory;
};

void CApplication::LoadEngineModule(const char *asName)
{
	if(!asName || !*asName)
	{
		//throw std::invalid_argument(std::string("Argument passed to CApplication::LoadEngineModule is invalid! (").append(asName).append(")"));
		return;
	};
	
	auto pEngineLib{Sys_LoadModule(asName)};
	
	// Rekt
	if(!pEngineLib)
		throw std::runtime_error(std::string("Failed to load the engine module (") + asName + ")!");
	
	mpEngineLib = pEngineLib;
	
	auto fnEngineFactory{Sys_GetFactory(mpEngineLib)};
	
	if(!fnEngineFactory)
		throw std::runtime_error(std::string("Failed to get the engine module factory (") + asName + ")!");
	
	mfnEngineFactory = fnEngineFactory;
};