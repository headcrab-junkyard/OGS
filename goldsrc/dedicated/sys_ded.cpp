/*
 *	This file is part of Open GoldSrc Project
 *	Copyright (C) 2018 Headcrab Garage
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of this program with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve"). You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve. If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so. If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include <cstdio>
#include <stdexcept>

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
		throw std::runtime_error(std::string("Failed to load the filesystem module!"));
	
	auto pFSFactory{Sys_GetFactory(pFSLib)};
	
	if(!pFSFactory)
		return EXIT_FAILURE;
	
	auto pEngineLib{Sys_LoadModule("engine")};
	
	if(!pEngineLib)
		throw std::runtime_error(std::string("Failed to load the engine module!"));
	
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
	//CCmdLine CmdLine(argc, argv); // TODO
	
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