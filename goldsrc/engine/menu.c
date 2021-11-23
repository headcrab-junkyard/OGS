/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2020-2021 BlackPhrase
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
/// @brief temporary legacy menu support layer

#include "quakedef.h"

pfnM_Keydown fnM_Keydown;
pfnM_Draw fnM_Draw;

void M_Keydown(int key)
{
	if(fnM_Keydown)
		fnM_Keydown(key);
};

void M_Draw()
{
	if(fnM_Draw)
		fnM_Draw();
	
	VID_UnlockBuffer();
	S_ExtraUpdate();
	VID_LockBuffer();
};

/*
================
M_ToggleMenu_f
================
*/
void M_ToggleMenu_f()
{
	if(BaseUI_IsGameUIVisible())
		BaseUI_HideGameUI();
	else
		BaseUI_ActivateGameUI();
	
/*
	m_entersound = true;
	
	if(key_dest == key_menu)
	{
		if(m_state != m_main)
		{
			M_Menu_Main_f();
			return;
		};
		key_dest = key_game;
		m_state = m_none;
		return;
	};
	
	if(key_dest == key_console)
		Con_ToggleConsole_f();
	else
		M_Menu_Main_f();
*/
};