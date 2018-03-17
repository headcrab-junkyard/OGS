/// @file

#include <memory>

#include <windows.h>

#include "WinApplication.hpp"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::unique_ptr<IApplication> pApp = std::make_unique<CWinApplication>(lpCmdLine);
	
	return AppMain(pApp.get());
};