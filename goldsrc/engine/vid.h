/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-2001 Id Software, Inc.
 *	Copyright (C) 2018-2020 BlackPhrase
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
/// @brief video driver defs

#pragma once

#define VID_CBITS 6
#define VID_GRADES (1 << VID_CBITS)

// a pixel can be one, two, or four bytes
typedef byte pixel_t;

typedef struct vrect_s
{
	int x, y, width, height;
	struct vrect_s *pnext; // TODO: non-q2
} vrect_t;

typedef struct
{
//
	pixel_t *buffer;            // invisible buffer
	pixel_t *colormap;          // 256 * VID_GRADES size
	unsigned short *colormap16; // 256 * VID_GRADES size
	int fullbright;             // index of first fullbright color
	unsigned rowbytes;          // may be > width if displayed in a window
	unsigned width;
	unsigned height;
	float aspect; // width / height -- < 0 is taller than wide
	int numpages;
	int recalc_refdef; // if true, recalc vid-based stuff
	pixel_t *conbuffer;
	int conrowbytes;
	unsigned conwidth;
	unsigned conheight;
	int maxwarpwidth;
	int maxwarpheight;
	pixel_t *direct; // direct drawing to framebuffer, if not nullptr

//
// TODO: q2
	//int width;
	//int height;
//
} viddef_t;

extern viddef_t vid; // global video state // TODO: viddef in q2

extern unsigned short d_8to16table[256];
extern unsigned d_8to24table[256];

extern void (*vid_menudrawfn)();
extern void (*vid_menukeyfn)(int key);

void VID_SetPalette(unsigned short *palette); // TODO: non-q2
// called at startup and after any gamma correction

void VID_ShiftPalette(unsigned short *palette); // TODO: non-q2
// called for bonus and pain flashes, and for underwater color changes

// Video module initialisation etc
void VID_Init(unsigned short *palette); // TODO: no palette in args in q2
// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again

void VID_Shutdown();
// Called at shutdown

//void VID_Update(vrect_t *rects); // TODO: non-q2 // TODO: unused in gs?
// flushes the given rectangles from the view buffer to the screen

//int VID_SetMode(int modenum, unsigned short *palette); // TODO: non-q2 // TODO: unused in gs?
// sets the mode; only used by the engine for resetting to mode 0 (the
// base mode) on memory allocation failures

void VID_HandlePause(qboolean pause); // TODO: non-q2 // TODO: unused in gs?
// called only on Win32, when pause happens, so the mouse can be released

//void VID_CheckChanges(); // TODO: q2

//void VID_MenuInit(); // TODO: q2
//void VID_MenuDraw(); // TODO: q2
//const char *VID_MenuKey(int key); // TODO: q2

//void VID_LockBuffer(); // TODO: non-q2 // TODO: unused in gs?
//void VID_UnlockBuffer(); // TODO: non-q2 // TODO: unused in gs?

#ifdef GLQUAKE
qboolean VID_Is8bit(); // TODO: non-q2 // TODO: unused in gs?
#endif
