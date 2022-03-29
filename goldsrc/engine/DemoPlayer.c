/*
 * This file is part of OGS Engine
 * Copyright (C) 2021 BlackPhrase
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
#include "DemoPlayer.h"

void Dem_Save_f()
{
	// TODO
};

void Dem_Start_f()
{
	// TODO
};

void Dem_Pause_f()
{
	// TODO
};

void Dem_Speed_f()
{
	// TODO
};

void Dem_Jump_f()
{
	// TODO
};

void Dem_ForceHLTV_f()
{
	// TODO
};

void DemoPlayer_Init()
{
	//CDemoPlayer::Init()
	//{
	Cmd_AddCommand("dem_save", Dem_Save_f);
	Cmd_AddCommand("dem_start", Dem_Start_f);
	Cmd_AddCommand("dem_pause", Dem_Pause_f);
	Cmd_AddCommand("dem_speed", Dem_Speed_f);
	Cmd_AddCommand("dem_jump", Dem_Jump_f);
	Cmd_AddCommand("dem_forcehltv", Dem_ForceHLTV_f);
	//};
};