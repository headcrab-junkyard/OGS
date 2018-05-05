/// @file

#include <memory>

#include <windows.h>

#include "WinApplication.hpp"

/*
==================
WinMain
==================
*/

//HINSTANCE global_hInstance;
//int global_nCmdShow;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// previous instances do not exist in Win32
	//if(hPrevInstance)
		//return 0;

	//global_hInstance = hInstance;
	//global_nCmdShow = nCmdShow;
	
	std::unique_ptr<CApplication> pApp = std::make_unique<CWinApplication>(lpCmdLine);
	
	return pApp->Run();
};