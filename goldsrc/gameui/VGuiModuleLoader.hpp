/// @file

#pragma once

#include "GameUI/IVGuiModuleLoader.h"

class CVGuiModuleLoader final : public IVGuiModuleLoader
{
public:
	CVGuiModuleLoader();
	~CVGuiModuleLoader();
	
	int GetModuleCount() override;
	const char *GetModuleLabel(int moduleIndex) override;
	CreateInterfaceFn GetModuleFactory(int moduleIndex) override;
	
	bool ActivateModule(int moduleIndex) override;
	bool ActivateModule(const char *moduleName) override;
	
	void SetPlatformToRestart() override;
};