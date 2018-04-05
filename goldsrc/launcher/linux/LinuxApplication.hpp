/// @file

#pragma once

#include "Application.hpp"

class CLinuxApplication : public CApplication
{
public:
	CLinuxApplication(int argc, char **argv);
	~CLinuxApplication();
	
	bool PostInit() override;
private:
	
};