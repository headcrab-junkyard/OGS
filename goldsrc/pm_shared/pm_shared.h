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

#pragma once

typedef struct playermove_s playermove_t;

void PM_Init(playermove_t *ppmove);
void PM_Move(playermove_t *ppmove, int server);
char PM_GetTextureType(const char *name);

/// Spectator Movement Modes
/// (stored in pev->iuser1, so the physics code can get at them)
enum
{
	OBS_NONE = 0,
	OBS_CHASE_LOCKED,
	OBS_CHASE_FREE,
	OBS_ROAMING,
	OBS_IN_EYE,
	OBS_MAP_FREE,
	OBS_MAP_CHASE
};