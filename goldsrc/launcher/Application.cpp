/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
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
	constexpr auto FILESYSTEM_MODULE_NAME{"filesystem_stdio"};
	
	if(!LoadFileSystemModule(FILESYSTEM_MODULE_NAME))
		throw std::runtime_error(std::string("Failed to load the filesystem module (") + FILESYSTEM_MODULE_NAME + ")!");
	
	const char *ENGINE_MODULE_NAME{"hw"};
	
	// TODO: we're already in hardware by default, need to check the previous mode the engine was launched in
	//if(strstr(msCmdLine, "-hw"))
		//ENGINE_MODULE_NAME = "hw";
	
	if(strstr(msCmdLine, "-sw") && !strstr(msCmdLine, "-hw"))
		ENGINE_MODULE_NAME = "sw";
	
	mpEngineLib = Sys_LoadModule(ENGINE_MODULE_NAME);
	
	if(!mpEngineLib)
		throw std::runtime_error(std::string("Failed to load the engine module (") + ENGINE_MODULE_NAME + ")!");
	
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

bool CApplication::LoadFileSystemModule(const char *asName)
{
	if(!asName || !*asName)
	{
		//throw std::invalid_argument(std::string("Argument passed to CApplication::LoadFileSystemModule is invalid! (").append(asName).append(")"));
		return false;
	};
	
	mpFSLib = Sys_LoadModule(asName);
	
	if(!mpFSLib)
		return false;
	
	mfnFSFactory = Sys_GetFactory(mpFSLib);
	
	if(!mfnFSFactory)
		return false;
	
	return true;
};