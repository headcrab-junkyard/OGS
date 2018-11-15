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
/// @brief event arguments structure

#pragma once

/// Event was invoked with stated origin
#define FEVENT_ORIGIN (1 << 0)

/// Event was invoked with states angles
#define FEVENT_ANGLES (1 << 1)

typedef struct event_args_s
{
	int flags;
	
	// Transmitted
	int entindex;
	
	float origin[3];
	float angles[3];
	float velocity[3];
	
	int ducking;
	
	float fparam1;
	float fparam2;
	
	int iparam1;
	int iparam2;
	
	int bparam1;
	int bparam2;
} event_args_t;