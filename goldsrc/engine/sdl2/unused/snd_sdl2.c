/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019, 2021 BlackPhrase
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
/// @brief SDL2 sound driver implementation

#include "quakedef.h"

qboolean SNDDMA_Init()
{
	if(SDL_WasInit(SDL_INIT_AUDIO) != 0)
		return true;
	
	if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
	{
		Con_DPrintf("Failed to init SDL2 audio subsystem, sound initialization skipped...\n");
		return false;
	};
	
	return true;
}

void SNDDMA_Shutdown()
{
	if(SDL_WasInit(SDL_INIT_AUDIO) != 0)
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SNDDMA_Submit()
{
	if(SDL_WasInit(SDL_INIT_AUDIO) == 0)
		return;
	
	// TODO
};

int SNDDMA_GetDMAPos()
{
	if(SDL_WasInit(SDL_INIT_AUDIO) == 0)
		return;
	
	// TODO
};