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

#pragma once

struct delta_s;

void DELTA_Init();

void DELTA_Load(const char *filename);

void DELTA_SetField(struct delta_s *pFields, const char *fieldname);
void DELTA_UnsetField(struct delta_s *pFields, const char *fieldname);

void DELTA_AddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const byte *from, const byte *to));

int DELTA_FindFieldIndex(struct delta_s *pFields, const char *fieldname);

void DELTA_SetFieldByIndex(struct delta_s *pFields, int fieldNumber);
void DELTA_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber);

void DELTA_ParseField(const char *fieldname);

void DELTA_ParseDescription();