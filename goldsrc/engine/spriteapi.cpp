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
	return nullptr;
};