/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021-2023 BlackPhrase
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

//===============
//   TYPES
//===============

/// Wad compression
enum
{
	CMP_NONE = 0,
	CMP_LZSS
};

/// Wad type
enum
{
	TYP_NONE = 0,
	TYP_LABEL,

	TYP_LUMPY = 64, // 64 + grab command number
	TYP_PALETTE = 64,
	TYP_QTEX,
	TYP_QPIC,
	TYP_SOUND,
	TYP_MIPTEX = 68,
	// TODO: something type 69,
	TYP_QFONT = 70
};

typedef struct qpic_s
{
	int width, height;
	byte data[4]; // variably sized // TODO: = height * width
	
	// own palette support
	// TODO: these fields probably should be here (because they're present in a pic file)
	short palette_colors; // Number of colors in a palette (can't be more than 256)
	/*color24*/ byte *palette; // TODO: color24 /*lbm*/palette[palette_colors][3]; // 8-bit RGB palette data // filestart + width*height*4 + 2?
} qpic_t;

typedef struct
{
	char identification[4]; // should be WAD2/WAD3 or 2DAW/3DAW
	int numlumps;
	int infotableofs;
} wadinfo_t;

typedef struct
{
	int filepos;
	int disksize;
	int size; // uncompressed
	char type;
	char compression;
	char pad1, pad2;
	char name[16]; // must be null terminated
} lumpinfo_t;

extern int wad_numlumps;
extern lumpinfo_t *wad_lumps;
extern byte *wad_base;

void W_LoadWadFile(const char *filename);

void W_CleanupName(const char *in, char *out);
lumpinfo_t *W_GetLumpinfo(const char *name);
void *W_GetLumpName(const char *name);
void *W_GetLumpNum(int num);

void SwapPic(qpic_t *pic);

// WAD3

void WAD3_LoadTextureWadFile(const char *filename);
//byte WAD3_LoadTexture(miptex_t *mt);