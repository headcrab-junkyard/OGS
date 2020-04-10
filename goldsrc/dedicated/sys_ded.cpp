/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include <cstdio>
#include <stdexcept>

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
	#include "win/conproc.h"
#endif

#include "engine_hlds_api.h"

#include "interface.h"

#ifdef _WIN32
static bool sc_return_on_enter{false};
HANDLE hinput, houtput;
#endif

IDedicatedServerAPI *gpEngine{nullptr}; // TODO: hacky way to access the command buffer...

int CreateConsoleWindow()
{
#ifdef _WIN32
	if(!AllocConsole())
		return 0; // TODO: was Sys_Error("Couldn't create dedicated server console");

	hinput = GetStdHandle(STD_INPUT_HANDLE);
	houtput = GetStdHandle(STD_OUTPUT_HANDLE);

	InitConProc();
#endif // _WIN32

	return 1;
};

void DestroyConsoleWindow()
{
#ifdef _WIN32
	FreeConsole();
	
	// shutdown QHOST hooks if necessary
	DeinitConProc();
#endif
};

char *Sys_ConsoleInput()
{
#ifdef _WIN32

//#ifdef SWDS
	static char text[256];
	static int len;
	int c;

	// read a line out
	while(_kbhit())
	{
		c = _getch();
		putch(c);
		if(c == '\r')
		{
			text[len] = 0;
			putch('\n');
			len = 0;
			return text;
		}
		if(c == 8)
		{
			//if (len) // TODO
			{
				putch(' ');
				putch(c);
				len--;
				text[len] = 0;
			}
			continue;
		}
		text[len] = c;
		len++;
		text[len] = 0;
		if(len == sizeof(text))
			len = 0;
	}

	return NULL;
/*
#else // if not SWDS
	static char text[256];
	static int len;
	INPUT_RECORD recs[1024];
	int count;
	int i, dummy;
	int ch, numread, numevents;

	for(;;)
	{
		if(!GetNumberOfConsoleInputEvents(hinput, &numevents))
			Sys_Error("Error getting # of console events");

		if(numevents <= 0)
			break;

		if(!ReadConsoleInput(hinput, recs, 1, &numread))
			Sys_Error("Error reading console input");

		if(numread != 1)
			Sys_Error("Couldn't read console input");

		if(recs[0].EventType == KEY_EVENT)
		{
			if(!recs[0].Event.KeyEvent.bKeyDown)
			{
				ch = recs[0].Event.KeyEvent.uChar.AsciiChar;

				switch(ch)
				{
				case '\r':
					WriteFile(houtput, "\r\n", 2, &dummy, NULL);

					if(len)
					{
						text[len] = 0;
						len = 0;
						return text;
					}
					else if(sc_return_on_enter)
					{
						// special case to allow exiting from the error handler on Enter
						text[0] = '\r';
						len = 0;
						return text;
					}

					break;

				case '\b':
					WriteFile(houtput, "\b \b", 3, &dummy, NULL);
					if(len)
					{
						len--;
					}
					break;

				default:
					if(ch >= ' ')
					{
						WriteFile(houtput, &ch, 1, &dummy, NULL);
						text[len] = ch;
						len = (len + 1) & 0xff;
					}

					break;
				}
			}
		}
	}

	return NULL;
#endif // SWDS
*/

#elif __linux__
	static char text[256];
	int len;
	fd_set fdset;
	struct timeval timeout;

	FD_ZERO(&fdset);
	FD_SET(0, &fdset); // stdin
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	if(select(1, &fdset, NULL, NULL, &timeout) == -1 || !FD_ISSET(0, &fdset))
		return NULL;

	len = read(0, text, sizeof(text));
	if(len < 1)
		return NULL;
	text[len - 1] = 0; // rip off the /n and terminate

	return text;
#elif sun
	static char text[256];
	int len;
	fd_set readfds;
	int ready;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	ready = select(1, &readfds, 0, 0, &timeout);

	if(ready > 0)
	{
		len = read(0, text, sizeof(text));
		if(len >= 1)
		{
			text[len - 1] = 0; // rip off the /n and terminate
			return text;
		};
	};

	return 0;
#endif // _WIN32
};

/*
===================
Host_GetConsoleCommands

Add them exactly as if they had been typed at the console
===================
*/
void Host_GetConsoleCommands()
{
	char *cmd{nullptr};

	while(1)
	{
		cmd = Sys_ConsoleInput();
		if(!cmd)
			break;
		gpEngine->AddConsoleText(cmd);
	};
};

int RunServer() // void?
{
	constexpr auto FILESYSTEM_MODULE_NAME{"filesystem_stdio"};
	
	auto pFSLib{Sys_LoadModule(FILESYSTEM_MODULE_NAME)};
	
	if(!pFSLib)
		throw std::runtime_error(std::string("Failed to load the filesystem module (") + FILESYSTEM_MODULE_NAME + ")!");
	
	auto pFSFactory{Sys_GetFactory(pFSLib)};
	
	if(!pFSFactory)
		return EXIT_FAILURE;
	
	constexpr auto ENGINE_MODULE_NAME{"swds"};
	
	auto pEngineLib{Sys_LoadModule(ENGINE_MODULE_NAME)};
	
	if(!pEngineLib)
		throw std::runtime_error(std::string("Failed to load the engine module (") + ENGINE_MODULE_NAME + ")!");
	
	auto pEngineFactory{Sys_GetFactory(pEngineLib)};
	
	if(!pEngineFactory)
		return EXIT_FAILURE;
	
	auto pEngine{(IDedicatedServerAPI*)pEngineFactory(VENGINE_HLDS_API_VERSION, nullptr)};
	
	if(!pEngine)
		return EXIT_FAILURE;
	
	gpEngine = pEngine;
	
	//char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory
	if(!pEngine->Init(".", "TODO", Sys_GetFactoryThis(), pFSFactory))
		return EXIT_FAILURE;
	
	// main loop
	bool bRunning{true};
	while(bRunning)
	{
		// check for commands typed to the host
		Host_GetConsoleCommands();
		
		bRunning = pEngine->RunFrame();
	};
	
	pEngine->Shutdown();
	
	return EXIT_SUCCESS;
};

/*
==================
main

==================
*/
//char *newargv[256]; // TODO: unused?
int main(int argc, char **argv)
{
	//CCmdLine CmdLine(argc, argv); // TODO
	
	if(!CreateConsoleWindow())
		return EXIT_FAILURE;
	
	if(!RunServer())
		return EXIT_FAILURE;
	
	DestroyConsoleWindow();

	// return success of application
	return EXIT_SUCCESS;
};