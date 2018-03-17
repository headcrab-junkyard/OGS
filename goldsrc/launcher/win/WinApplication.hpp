/// @file

#pragma once

#include "IApplication.hpp"

class CWinApplication : public IApplication
{
public:
	CWinApplication(const char *cmdline);
	~CWinApplication();
	
	bool Init() override;
	bool LoadFileSystemModule() override;
private:
	
};