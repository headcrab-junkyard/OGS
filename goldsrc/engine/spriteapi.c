/*
 * This file is part of OGS Engine
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

#include "quakedef.h"
#include "spriteapi.h"

typedef float vec2_t[2];

SpriteHandle_t ghCurrentSprite = NULL;
vec3_t gvSpriteColor;

qboolean gbScissorActive = false;
vec2_t gvScissorPos;
vec2_t gvScissorSize;

mspriteframe_t *SPR_GetFrame(SpriteHandle_t ahSprite, int anFrame)
{
	// TODO
	return NULL;
};

SpriteHandle_t SPR_Load(const char *asName)
{
	// TODO
	/*
	FileHandle_t pSpriteFile = FS_Open("asName", "rb");
	
	if(!pSpriteFile)
		return 0;
	
	dsprite_t *pin = (sprite_t*)Z_Malloc(sizeof(dsprite_t));
	
	int version = LittleLong(pin->version);
	
	if(version != SPRITE_VERSION)
		return 0;
	
	numframes = LittleLong(pin->numframes);
	
	int size = sizeof(msprite_t) + (numframes - 1) * sizeof(psprite->frames);
	
	msprite_t *psprite = Hunk_AllocName(size, asName);
	
	psprite->type = LittleLong(pin->type);
	psprite->maxwidth = LittleLong(pin->width);
	psprite->maxheight = LittleLong(pin->height);
	psprite->beamlength = LittleFloat(pin->beamlength);
	psprite->numframes = numframes;
	
	Z_Free(pin);
	
	FS_Close(pSpriteFile);
	*/
	return 0;
};

int SPR_Frames(SpriteHandle_t ahSprite)
{
	// TODO
	return 0;
};

int SPR_Height(SpriteHandle_t ahSprite, int anFrame)
{
	//bi_trace();
	
	mspriteframe_t *pFrame = SPR_GetFrame(ahSprite, anFrame);
	if(pFrame)
		return pFrame->height;
	
	return 0;
};

int SPR_Width(SpriteHandle_t ahSprite, int anFrame)
{
	mspriteframe_t *pFrame = SPR_GetFrame(ahSprite, anFrame);
	if(pFrame)
		return pFrame->width;
	
	return 0;
};

void SPR_Set(SpriteHandle_t ahSprite, int r, int g, int b)
{
	ghCurrentSprite = ahSprite;
	
	gvSpriteColor[0] = r;
	gvSpriteColor[1] = g;
	gvSpriteColor[2] = b;
};

void SPR_Draw(int frame, int x, int y, const wrect_t *pRect)
{
	// TODO
};

void SPR_DrawHoles(int frame, int x, int y, const wrect_t *pRect)
{
	// TODO
};

void SPR_DrawAdditive(int frame, int x, int y, const wrect_t *pRect)
{
	// TODO
};

void SPR_EnableScissor(int x, int y, int width, int height)
{
	gbScissorActive = true;
	
	gvScissorPos[0] = x;
	gvScissorPos[1] = y;
	
	gvScissorSize[0] = width;
	gvScissorSize[1] = height;
};

void SPR_DisableScissor()
{
	gbScissorActive = false;
};

client_sprite_t *SPR_GetList(char *psz, int *apnCount)
{
	// TODO
	return NULL;
};