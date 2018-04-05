/// @file

#include "WinApplication.hpp"

CWinApplication::CWinApplication(const char *cmdline)
{
	msCmdLine[0] = '\0';
	
	for(int i = 0; i < strlen(cmdline); ++i)
		strcat(msCmdLine, cmdline);
};

CWinApplication::~CWinApplication() = default;

bool CWinApplication::PostInit()
{
	return true;
};