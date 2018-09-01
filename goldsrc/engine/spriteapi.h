/// @file

#pragma once

SpriteHandle_t SPR_Load(const char *asName);

int SPR_Frames(SpriteHandle_t ahSprite);

int SPR_Height(SpriteHandle_t ahSprite, int anFrame);
int SPR_Width(SpriteHandle_t ahSprite, int anFrame);

void SPR_Set(SpriteHandle_t ahSprite, int r, int g, int b);

void SPR_Draw(int frame, int x, int y, const wrect_t *pRect);
void SPR_DrawHoles(int frame, int x, int y, const wrect_t *pRect);
void SPR_DrawAdditive(int frame, int x, int y, const wrect_t *pRect);

void SPR_EnableScissor(int x, int y, int width, int height);
void SPR_DisableScissor();

client_sprite_t *SPR_GetList(char *psz, int *apnCount);