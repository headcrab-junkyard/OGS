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

SpriteHandle_t SPR_Load(const char *asName)
{
	// TODO
	return 0;
};

int SPR_Frames(SpriteHandle_t ahSprite)
{
	// TODO
	return 0;
};

int SPR_Height(SpriteHandle_t ahSprite, int anFrame)
{
	// TODO
	return 0;
};

int SPR_Width(SpriteHandle_t ahSprite, int anFrame)
{
	// TODO
	return 0;
};

void SPR_Set(SpriteHandle_t ahSprite, int r, int g, int b)
{
	// TODO
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
	// TODO
};

void SPR_DisableScissor()
{
	// TODO
};

client_sprite_t *SPR_GetList(char *psz, int *apnCount)
{
	// TODO
	return NULL;
};