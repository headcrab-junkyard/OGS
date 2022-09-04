/*
 * This file is part of OGS Engine
 * Copyright (C) 2022 BlackPhrase
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

#include "quakedef.h"
#include "EngineVGui.hpp"

EXPOSE_SINGLE_INTERFACE(CEngineVGui, IEngineVGui, VENGINE_VGUI_VERSION);

vgui2::VPANEL CEngineVGui::GetPanel(VGuiPanel_t aeType)
{
	switch(aeType)
	{
	case PANEL_ROOT:
		return 0; // TODO
	case PANEL_GAMEUIDLL:
		return 0; // TODO
	case PANEL_CLIENTDLL:
		return 0; // TODO
	};
	
	return 0;
};

bool CEngineVGui::IsGameUIVisible()
{
	// TODO
	return false;
};

//void CEngineVGui::ActivateGameUI()
//{
//};