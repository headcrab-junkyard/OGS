/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021-2022 BlackPhrase
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
#include "delta.h"

void DELTA_Stats_f()
{
	// TODO
};

void DELTA_Clear_f()
{
	// TODO
};

void DELTA_Init()
{
	// TODO
	
	Cmd_AddCommand("delta_stats", DELTA_Stats_f);
	Cmd_AddCommand("delta_clear", DELTA_Clear_f);
};

void DELTA_Load(const char *filename)
{
	FileHandle_t pFile = FS_Open(va("%s", filename), "r");
	
	if(!pFile)
		Sys_Error("DELTA_Load:  Couldn't load file %s", filename);
	
	// TODO
	
	FS_Close(pFile);
};

void DELTA_SetField(struct delta_s *pFields, const char *fieldname)
{
	if(!pFields)
		return;
	
	// TODO
};

void DELTA_UnsetField(struct delta_s *pFields, const char *fieldname)
{
	if(!pFields)
		return;
	
	// TODO
};

typedef void (*fnEncoderCallback)(struct delta_s *pFields, const byte *pFrom, const byte *pTo);

typedef struct encoder_s
{
	const char *name;
	fnEncoderCallback callback;
	struct encoder_s *pNext;
} encoder_t;

encoder_t *gpEncoderListHead = NULL;

void DELTA_AddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const byte *from, const byte *to))
{
	if(!name || !*name)
		return;
	
	//if(!conditionalencode)
		//return;
	
	encoder_t *pEncoderList = gpEncoderListHead;
	
	while(pEncoderList->pNext)
		pEncoderList = pEncoderList->pNext;
	
	encoder_t *pEncoder = (encoder_t*)malloc(sizeof(encoder_t)); // TODO
	Q_memset(pEncoder, 0, sizeof(encoder_t));
	
	pEncoder->name = name;
	pEncoder->callback = conditionalencode;
	pEncoder->pNext = NULL;
	
	pEncoderList->pNext = pEncoder;
};

int DELTA_FindFieldIndex(struct delta_s *pFields, const char *fieldname)
{
	if(!pFields)
		return 0;
	
	// TODO
	
	//("DELTA_FindFieldIndex:  Warning, couldn't find %s", fieldname);
	
	return 0;
};

void DELTA_SetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	if(!pFields)
		return;
	
	// TODO
};

void DELTA_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	if(!pFields)
		return;
	
	// TODO
};

void DELTA_ParseField(const char *fieldname)
{
	if(!fieldname || !*fieldname)
		Sys_Error("DELTA_ParseField:  Expecting fieldname");
	
	// TODO
};

void DELTA_ParseDescription()
{
	// TODO
};