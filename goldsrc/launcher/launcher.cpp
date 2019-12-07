/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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

#ifdef _WIN32
	#include <windows.h>

	#include "win/WinApplication.hpp"
#else
	#include "linux/LinuxApplication.hpp"
#endif

#ifdef _WIN32
//HINSTANCE global_hInstance{0};
//int global_nCmdShow{0};
#endif

#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
#ifdef _WIN32
	// previous instances do not exist in Win32
	if(hPrevInstance)
		return 0;

	//global_hInstance = hInstance; // TODO: move to engine space
	//global_nCmdShow = nCmdShow;
	
	CWinApplication App(lpCmdLine);
#else
	CLinuxApplication App(argc, argv);
#endif

	return App.Run();
};