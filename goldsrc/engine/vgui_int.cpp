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
#include "vgui_int.h"

void VGui_Startup()
{
	// TODO: VGuiWrap2_Startup();
};

void VGui_Shutdown()
{
	// TODO: VGuiWrap2_Shutdown();
};

/*
void VGui_Frame()
{
	CEGUI::System::renderAllGUIContexts();
};
*/

void VGui_ViewportPaintBackground(int extents[4])
{
	// TODO
};

void VGui_ConsolePrint(const char *text)
{
	// TODO: something else?

	ClientDLL_VGui_ConsolePrint(text);

	Con_Print(text);

	//VGuiWrap_ConsolePrint(text);
	//VGuiWrap2_ConsolePrint(text);
};

void *VGui_GetPanel()
{
	return nullptr; // TODO: VGuiWrap_GetPanel();
};