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
#include "vox.h"

void VOX_Init()
{
	VOX_ReadSentenceFile("sound/sentences.txt");
};

void VOX_LoadSound(const char *sentence)
{
	// TODO
};

void VOX_ReadSentenceFile(const char *filename)
{
	FileHandle_t pFile = FS_Open(filename, "rb");
	
	if(!pFile)
	{
		Con_Printf("VOX_ReadSentenceFile: Couldn't load %s\n", filename);
		return;
	};
	
	char *pPos{nullptr};
	
	while(!FS_EndOfFile(pFile))
	{
		//FS_Seek(pFile, 0, FILESYSTEM_SEEK_HEAD);
		
		// TODO: check for comments (//) and skip the whole line
		//if(pPos[0] == '/' && pPos[1] == '/')
			//pPos = fgetc();
		
		// TODO: read until a whitespace char, mark as a key
		// TODO: read the rest of a line and mark as a value
	};
	
	FS_Close(pFile);
};