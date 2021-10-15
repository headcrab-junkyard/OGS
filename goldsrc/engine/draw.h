/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
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
/// @brief these are the only functions outside the render allowed to touch the vid buffer

#pragma once

extern qpic_t *draw_disc; // also used on sbar

void Draw_Init();

int Draw_Character(int x, int y, int num/*, int r, int g, int b*/); // TODO
void Draw_DebugChar(char num);

void Draw_Pic(int x, int y, qpic_t *pic);
//void Draw_SubPic(int x, int y, qpic_t *pic, int srcx, int srcy, int width, int height); // TODO: qw

void Draw_TransPic(int x, int y, qpic_t *pic);
void Draw_TransPicTranslate(int x, int y, qpic_t *pic, byte *translation);

void Draw_ConsoleBackground(int lines);

void Draw_BeginDisc();
void Draw_EndDisc();

void Draw_TileClear(int x, int y, int w, int h);

void Draw_Fill(int x, int y, int w, int h, int c);

void Draw_FadeScreen();

void Draw_String(int x, int y, const char *str);
//void Draw_Alt_String(int x, int y, const char *str); // TODO: qw
int Draw_ConsoleString(int x, int y, const char *string);
void Draw_ConsoleStringLen(const char *string, int *length, int *height);

qpic_t *Draw_PicFromWad(const char *name);
qpic_t *Draw_CachePic(const char *path);

//void Draw_Crosshair(); // TODO: qw

void Draw_SetTextColor(float r, float g, float b);