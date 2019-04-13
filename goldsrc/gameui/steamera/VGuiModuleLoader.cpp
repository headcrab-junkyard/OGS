/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

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