/*
 * This file is part of OGS Engine
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

#pragma once

typedef struct modinfo_s
{
	char game[32];
	char url_info[32];
	char url_dl[32];
	char version[32];
	
	int size;
	
	qboolean svonly;
	qboolean secure;
	
	int type;
	
	qboolean cldll;
	
	char hlversion[16];
	
	qboolean nomodels;
	qboolean nohimodel;
	
	char mpentity[16];
	
	char gamedll[32];
	
	char trainmap[16];
} modinfo_t;

modinfo_t *ModInfo_LoadFromFile(const char *filename);

const char *ModInfo_GetKeyValue(const char *key);