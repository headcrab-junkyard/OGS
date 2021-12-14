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

void HPAK_List_f()
{
	HPAK_List();
};

void HPAK_Remove_f()
{
	HPAK_RemoveLump();
};

void HPAK_Val_f()
{
	// TODO
};

void HPAK_Extract_f()
{
	// TODO
};

void HPAK_Init()
{
	Cmd_AddCommand("hpklist", HPAK_List_f);
	Cmd_AddCommand("hpkremove", HPAK_Remove_f);
	Cmd_AddCommand("hpkval", HPAK_Val_f);
	Cmd_AddCommand("hpkextract", HPAK_Extract_f);
};

void HPAK_CreatePak()
{
	// TODO
};

void HPAK_AddLump()
{
	// TODO
};

void HPAK_RemoveLump()
{
	// TODO
};

void HPAK_List()
{
	// TODO
};