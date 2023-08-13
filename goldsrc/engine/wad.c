/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2005 A Nourai
 * Copyright (C) 2009 Crow_bar
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

#include "quakedef.h"

int wad_numlumps;
lumpinfo_t *wad_lumps;
byte *wad_base;

void SwapPic(qpic_t *pic);

/*
==================
W_CleanupName

Lowercases name and pads with spaces and a terminating 0 to the length of
lumpinfo_t->name.
Used so lumpname lookups can proceed rapidly by comparing 4 chars at a time
Space padding is so names can be printed nicely in tables.
Can safely be performed in place.
==================
*/
void W_CleanupName(const char *in, char *out)
{
	int i;
	int c;

	for(i = 0; i < 16; i++)
	{
		c = in[i];
		if(!c)
			break;

		if(c >= 'A' && c <= 'Z')
			c += ('a' - 'A');
		out[i] = c;
	}

	for(; i < 16; i++)
		out[i] = 0;
}

/*
====================
W_LoadWadFile
====================
*/
void W_LoadWadFile(const char *filename)
{
	lumpinfo_t *lump_p;
	wadinfo_t *header;
	unsigned i;
	int infotableofs;

	wad_base = COM_LoadHunkFile(filename);
	if(!wad_base)
		Sys_Error("W_LoadWadFile: couldn't load %s", filename);

	header = (wadinfo_t *)wad_base;

	if(header->identification[0] != 'W' || header->identification[1] != 'A' || header->identification[2] != 'D' || (header->identification[3] != '2' && header->identification[3] != '3'))
		Sys_Error("Wad file %s doesn't have WAD2/WAD3 id\n", filename);

	wad_numlumps = LittleLong(header->numlumps);
	infotableofs = LittleLong(header->infotableofs);
	wad_lumps = (lumpinfo_t *)(wad_base + infotableofs);

	for(i = 0, lump_p = wad_lumps; i < wad_numlumps; i++, lump_p++)
	{
		lump_p->filepos = LittleLong(lump_p->filepos);
		lump_p->size = LittleLong(lump_p->size);
		W_CleanupName(lump_p->name, lump_p->name);
		if(lump_p->type == TYP_QPIC)
			SwapPic((qpic_t *)(wad_base + lump_p->filepos));
	}
}

/*
=============
W_GetLumpinfo
=============
*/
lumpinfo_t *W_GetLumpinfo(const char *name)
{
	int i;
	lumpinfo_t *lump_p;
	char clean[16];

	W_CleanupName(name, clean);

	for(lump_p = wad_lumps, i = 0; i < wad_numlumps; i++, lump_p++)
	{
		if(!strcmp(clean, lump_p->name))
			return lump_p;
	}

	Sys_Error("W_GetLumpinfo: %s not found", name);
	return NULL;
}

void *W_GetLumpName(const char *name)
{
	lumpinfo_t *lump;

	lump = W_GetLumpinfo(name);

	return (void *)(wad_base + lump->filepos);
}

void *W_GetLumpNum(int num)
{
	lumpinfo_t *lump;

	if(num < 0 || num > wad_numlumps)
		Sys_Error("W_GetLumpNum: bad number: %i", num);

	lump = wad_lumps + num;

	return (void *)(wad_base + lump->filepos);
}

/*
=============================================================================

automatic byte swapping

=============================================================================
*/

void SwapPic(qpic_t *pic)
{
	pic->width = LittleLong(pic->width);
	pic->height = LittleLong(pic->height);
}

/*
=============================================================================
WAD3 Texture Loading for BSP 3.0 Support
=============================================================================
*/

//#ifdef GLQUAKE

#define TEXWAD_MAXIMAGES 16384

typedef struct
{
	char name[16]; // TODO: MAX_QPATH?
	FileHandle_t file;
	int position;
	int size;
} texwadlump_t;

/*static */ texwadlump_t texwadlump[TEXWAD_MAXIMAGES];
int numwadtextures;

void WAD3_LoadTextureWadFile(const char *filename)
{
	lumpinfo_t *lumps, *lump_p;
	wadinfo_t header;
	int i, j, infotableofs, numlumps, lowmark;
	FileHandle_t file;

	/*
	if(!FS_FOpenFile (va("textures/wad3/%s", filename), &file) != -1)
		if(!FS_FOpenFile (va("textures/halflife/%s", filename), &file) != -1)
			if(!FS_FOpenFile (va("textures/%s", filename), &file) != -1)
	*/
	file = FS_Open(filename, "rb");
	
	if(!file)
		Host_Error("Couldn't load wad \"%s\"\n", filename);

	if(FS_Read(&header, sizeof(wadinfo_t), file) != sizeof(wadinfo_t))
	{
		Con_Printf("WAD3_LoadTextureWadFile: unable to read wad header");
        FS_Close(file); // TODO: check if it's closed later on
		return;
	};

	if(memcmp(header.identification, "WAD3", 4))
	{
		Con_Printf("WAD3_LoadTextureWadFile: Wad file %s doesn't have WAD3 id\n", filename);
        FS_Close(file); // TODO: check if it's closed later on
		return;
	};

	numlumps = LittleLong(header.numlumps);

	if(numlumps < 1 || numlumps > TEXWAD_MAXIMAGES)
	{
		Con_Printf("WAD3_LoadTextureWadFile: invalid number of lumps (%i)\n", numlumps);
        FS_Close(file); // TODO: check if it's closed later on
		return;
	};

	infotableofs = LittleLong(header.infotableofs);

	FS_Seek(file, infotableofs, FILESYSTEM_SEEK_HEAD); // TODO: seek_set
	//if(FS_Seek(file, infotableofs, FILESYSTEM_SEEK_HEAD)) // TODO: seek_set
	{
		//Con_Printf("WAD3_LoadTextureWadFile: unable to seek to lump table");
        //FS_Close(file); // TODO: check if it's closed later on
		//return;
	};

	lowmark = Hunk_LowMark();

	if(!(lumps = (lumpinfo_t*)Hunk_Alloc(sizeof(lumpinfo_t) * numlumps)))
	{
		Con_Printf("WAD3_LoadTextureWadFile: unable to allocate temporary memory for lump table");
        FS_Close(file); // TODO: check if it's closed later on
		return;
	};

	if(FS_Read(lumps, sizeof(lumpinfo_t) * numlumps, file) != sizeof(lumpinfo_t) * numlumps)
	{
		Con_Printf("WAD3_LoadTextureWadFile: unable to read lump table");
        FS_Close(file); // TODO: check if it's closed later on
		Hunk_FreeToLowMark(lowmark);
		return;
	};

	//UnloadFileList.push_back(file); //Crow_bar. UnloadWads code

	for(i = 0, lump_p = lumps; i < numlumps; i++,lump_p++)
	{
        W_CleanupName(lump_p->name, lump_p->name);
		
		for(j = 0; j < numwadtextures; j++)
			if(!Q_strcmp(lump_p->name, texwadlump[j].name)) // name match, replace old one
				break;
		
		if(j >= TEXWAD_MAXIMAGES)
			break; // we are full, abort loading
		
		if(j == numwadtextures)
		{
			W_CleanupName(lump_p->name, texwadlump[j].name);
			texwadlump[j].file = file;
			texwadlump[j].position = LittleLong(lump_p->filepos);
			texwadlump[j].size = LittleLong(lump_p->disksize);
			numwadtextures++;
		};
	};

	Hunk_FreeToLowMark(lowmark);
	
	// Leaves the file open
};

// Converts paletted to RGBA
static byte *ConvertWad3ToRGBA(miptex_t *tex)
{
	byte *in, *data, *pal;
	int i, p, image_size;

	if(!tex->offsets[0])
		Sys_Error("ConvertWad3ToRGBA: tex->offsets[0] == 0");

	image_size = tex->width * tex->height;
	in = (byte *) ((byte *) tex + tex->offsets[0]);
	data = Q_Malloc(image_size * 4);

	pal = in + ((image_size * 85) >> 6) + 2;
	
	for(i = 0; i < image_size; i++)
	{
		p = *in++;
		
		if(tex->name[0] == '{' && p == 255)
			((int *) data)[i] = 0;
		else
		{
			p *= 3;
			data[i * 4 + 0] = pal[p];
			data[i * 4 + 1] = pal[p + 1];
			data[i * 4 + 2] = pal[p + 2];
			data[i * 4 + 3] = 255;
		};
	};
	return data;
};

byte *WAD3_LoadTexture(miptex_t *mt)
{
	char texname[MAX_QPATH];
	int i, j, lowmark = 0;
	FILE *file;
	miptex_t *tex;
	byte *data;

	if(mt->offsets[0])
		return ConvertWad3ToRGBA(mt);

	texname[sizeof(texname) - 1] = 0;
	W_CleanupName(mt->name, texname);
	
	for(i = 0; i < TEXWAD_MAXIMAGES; i++)
	{
		if(!texwadlump[i].name[0])
			break;
		
		if(strcmp(texname, texwadlump[i].name))
			continue;

		file = texwadlump[i].file;
		
		if(fseek(file, texwadlump[i].position, SEEK_SET))
		{
			Con_Printf("WAD3_LoadTexture: corrupt WAD3 file");
			return NULL;
		};
		
		lowmark = Hunk_LowMark();
		tex = Hunk_Alloc(texwadlump[i].size);
		
		if(fread(tex, 1, texwadlump[i].size, file) < texwadlump[i].size)
		{
			Con_Printf("WAD3_LoadTexture: corrupt WAD3 file");
			Hunk_FreeToLowMark(lowmark);
			return NULL;
		};
		
		tex->width = LittleLong(tex->width);
		tex->height = LittleLong(tex->height);
		
		if(tex->width != mt->width || tex->height != mt->height)
		{
			Hunk_FreeToLowMark(lowmark);
			return NULL;
		};
		
		for (j = 0;j < MIPLEVELS;j++)
			tex->offsets[j] = LittleLong(tex->offsets[j]);
		
		data = ConvertWad3ToRGBA(tex);
		Hunk_FreeToLowMark(lowmark);
		return data;
	};
	
	return NULL;
};

//#endif // GLQUAKE