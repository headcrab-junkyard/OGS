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
/// @brief scripted sequences api

#pragma once

#include "const.h"

typedef struct sequenceEntry_
{
} sequenceEntry_s;

typedef struct sentenceEntry_
{
} sentenceEntry_s;

sequenceEntry_s *SequenceGet(const char *sFileName, const char *sEntryName);
void Sequence_ParseFile(const char *sFileName, qboolean bIsGlobal);
void Sequence_OnLevelLoad(const char *sMapName);
sentenceEntry_s *SequencePickSentence(const char *sGroupName, int nPickMethod, int *pPicked);