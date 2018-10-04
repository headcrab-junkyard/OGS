/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"
#include "ivguiwrap.hpp"

void *gpVGuiWrapLib{nullptr};
IVGuiWrap *gpVGuiWrap{nullptr};

void UnloadVGuiWrapModule()
{
	if(gpVGuiWrapLib)
	{
		Sys_UnloadModule(gpVGuiWrapLib);
		gpVGuiWrapLib = nullptr;
	};
};

bool LoadVGuiWrapModule()
{
	//UnloadVGuiWrapModule();
	
	gpVGuiWrapLib = Sys_LoadModule("vguiwrap");
	
	if(!gpVGuiWrapLib)
		return false;
	
	auto fnVGuiWrapFactory{Sys_GetFactory(gpVGuiWrapLib)};
	
	if(!fnVGuiWrapFactory)
		return false;
	
	gpVGuiWrap = (IVGuiWrap*)fnVGuiWrapFactory(OGS_VGUIWRAP_INTERFACE_VERSION, nullptr);
	
	if(!gpVGuiWrap)
		return false;
	
	return true;
};

void VGuiWrap_Startup()
{
	if(!LoadVGuiWrapModule())
		return;
	
	gpVGuiWrap->Startup();
};

void VGuiWrap_Shutdown()
{
	gpVGuiWrap->Shutdown();
	
	UnloadVGuiWrapModule();
};

void *VGuiWrap_GetPanel()
{
	if(gpVGuiWrap)
		return gpVGuiWrap->GetPanel();
	
	return nullptr;
};