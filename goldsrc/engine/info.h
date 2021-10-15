/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018, 2021 BlackPhrase
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

//
// key / value info strings
//
#define MAX_INFO_KEY 64
#define MAX_INFO_VALUE 64
//#define	MAX_INFO_STRING		512 // TODO: com_model

char *Info_ValueForKey(const char *s, const char *key);

void Info_RemoveKey(char *s, const char *key);
void Info_RemovePrefixedKeys(char *start, char prefix);

void Info_SetValueForKey(char *s, const char *key, const char *value, int maxsize);
void Info_SetValueForStarKey(char *s, const char *key, const char *value, int maxsize);

void Info_Print(char *s);

//qboolean Info_Validate (char *s); // TODO: Q2