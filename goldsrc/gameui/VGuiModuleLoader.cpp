/// @file

#include "VGuiModuleLoader.hpp"

EXPOSE_SINGLE_INTERFACE(CVGuiModuleLoader, IVGuiModuleLoader, VGUIMODULELOADER_INTERFACE_VERSION);

CVGuiModuleLoader::CVGuiModuleLoader() = default;
CVGuiModuleLoader::~CVGuiModuleLoader() = default;

int CVGuiModuleLoader::GetModuleCount()
{
	return 0;
};

const char *CVGuiModuleLoader::GetModuleLabel(int moduleIndex)
{
	return "";
};

CreateInterfaceFn CVGuiModuleLoader::GetModuleFactory(int moduleIndex)
{
	return nullptr;
};

bool CVGuiModuleLoader::ActivateModule(int moduleIndex)
{
	return false;
};

bool CVGuiModuleLoader::ActivateModule(const char *moduleName)
{
	return false;
};

void CVGuiModuleLoader::SetPlatformToRestart()
{
};