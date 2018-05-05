/// @file

#pragma once

#include "Application.hpp"

class CWinApplication : public CApplication
{
public:
	CWinApplication(const char *cmdline);
	~CWinApplication();
	
	bool PostInit() override;
private:
	char msCmdLine[512]{};
};