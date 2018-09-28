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
#include "vgui2/IVGUI.h"

void *gpVGui2Lib{nullptr};
vgui2::IVGui *gpVGui{nullptr};

void UnloadVGui2Module()
{
	if(gpVGui2Lib)
	{
		Sys_UnloadModule(gpVGui2Lib);
		gpVGui2Lib = nullptr;
	};
};

bool LoadVGui2Module()
{
	//UnloadVGui2Module();
	
	gpVGui2Lib = Sys_LoadModule("vgui2");
	
	if(!gpVGui2Lib)
		return false;
	
	auto fnVGui2Factory{Sys_GetFactory(gpVGui2Lib)};
	
	if(!fnVGui2Factory)
		return false;
	
	gpVGui = (vgui2::IVGui*)fnVGui2Factory(VGUI_IVGUI_INTERFACE_VERSION, nullptr);
	
	if(!gpVGui)
		return false;
	
	return true;
};

void VGuiWrap2_Startup()
{
	if(!LoadVGui2Module())
		return;
	
	gpVGui->Init(nullptr, 0); // TODO
};

void VGuiWrap2_Shutdown()
{
	if(gpVGui)
		gpVGui->Shutdown();
	
	UnloadVGui2Module();
};

void VGuiWrap2_Frame()
{
	gpVGui->RunFrame();
};

void VGuiWrap2_Paint()
{
	//CBaseUI::Paint(int, int, int, int); // TODO
};

void VguiWrap2_GetMouseDelta(int *x, int *y)
{
	// TODO
};