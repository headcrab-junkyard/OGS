/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"
#include "delta.h"

void DELTA_Init()
{
	// TODO
};

void DELTA_Load(const char *filename)
{
	FileHandle_t pFile = FS_Open(va("%s", filename), "rb");
	
	if(!pFile)
		Sys_Error("DELTA_Load:  Couldn't load file %s", filename);
	
	// TODO
	
	FS_Close(pFile);
};

void DELTA_SetField(struct delta_s *pFields, const char *fieldname)
{
	// TODO
};

void DELTA_UnsetField(struct delta_s *pFields, const char *fieldname)
{
	// TODO
};

void DELTA_AddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const byte *from, const byte *to))
{
	// TODO
};

int DELTA_FindFieldIndex(struct delta_s *pFields, const char *fieldname)
{
	// TODO
	
	//("DELTA_FindFieldIndex:  Warning, couldn't find %s", fieldname);
	
	return 0;
};

void DELTA_SetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	// TODO
};

void DELTA_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	// TODO
};

void DELTA_ParseField(const char *fieldname)
{
	if(!fieldname)
		Sys_Error("DELTA_ParseField:  Expecting fieldname");
	
	// TODO
};

void DELTA_ParseDescription()
{
	// TODO
};