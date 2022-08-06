/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018-2020, 2022 BlackPhrase
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
/// @brief external (non-keyboard) input devices

#pragma once

void IN_Init();

void IN_Shutdown();

//void IN_Frame(); // TODO: Q2

//void IN_Activate(qboolean active); // TODO: q2

/// Restores all button and position states to defaults
void IN_ClearStates(); // TODO: not present in Q2

void IN_Accumulate();

void IN_MouseEvent(int mstate);