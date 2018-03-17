/// @file

#pragma once

#include "IApplication.hpp"

class CLinuxApplication : public IApplication
{
public:
	CLinuxApplication(int argc, char **argv);
	~CLinuxApplication();
	
	bool Init() override;
	bool LoadFileSystemModule() override;
private:
	
};