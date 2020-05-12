/// @file

#pragma once

#include "tier1/interface.h"

const char VGUIMODULELOADER_INTERFACE_VERSION[] = "VGuiModuleLoader003";

//-----------------------------------------------------------------------------
// Purpose: interface to accessing all loaded modules
//-----------------------------------------------------------------------------
struct IVGuiModuleLoader : public IBaseInterface
{
	virtual int GetModuleCount() = 0;
	virtual const char *GetModuleLabel(int moduleIndex) = 0;
	virtual CreateInterfaceFn GetModuleFactory(int moduleIndex) = 0;
	virtual bool ActivateModule(int moduleIndex) = 0;
	virtual bool ActivateModule(const char *moduleName) = 0;
	virtual void SetPlatformToRestart() = 0;
};

//extern IVGuiModuleLoader* g_pIVGuiModuleLoader;