/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018-2022 BlackPhrase
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
/// @brief master for refresh, status bar, console, chat, notify, etc

#include "quakedef.h"
//#include "r_local.h"

/*

background clear
rendering
turtle/net/ram icons
sbar
centerprint / slow centerprint
notify lines
intermission / finale overlay
loading plaque
console
menu

required background clears
required update regions


syncronous draw mode or async
One off screen buffer, with updates either copied or xblited
Need to double buffer?


async draw will require the refresh area to be cleared, because it will be
xblited, but sync draw can just ignore it.

sync
draw

CenterPrint ()
SlowPrint ()
Screen_Update ();
Con_Printf ();

net 
turn off messages option

the refresh is allways rendered, unless the console is full screen


console is:
	notify lines
	half
	full
	

*/

#ifdef GLQUAKE
int glx, gly, glwidth, glheight;
#endif

// only the refresh window will be updated unless these variables are flagged
int scr_copytop;
int scr_copyeverything;

float scr_con_current;
float scr_conlines; // lines of console to display

float oldscreensize, oldfov;
//float oldsbar; // TODO: qw
cvar_t scr_viewsize = { "viewsize", "120", true };
cvar_t scr_fov = { "fov", "90" }; // 10 - 170 // TODO: remove?
cvar_t scr_conspeed = { "scr_conspeed", "300" }; // TODO: remove?
cvar_t scr_centertime = { "scr_centertime", "2" };
cvar_t scr_showpause = { "showpause", "1" };
cvar_t scr_printspeed = { "scr_printspeed", "8" };

cvar_t scr_graphheight = { "graphheight", "64.0", FCVAR_ARCHIVE };

cvar_t scr_downloading = { "scr_downloading", "-1.0" };

cvar_t scr_connectmsg = { "scr_connectmsg", "0" };
cvar_t scr_connectmsg1 = { "scr_connectmsg1", "0" };
cvar_t scr_connectmsg2 = { "scr_connectmsg2", "0" };

#ifdef GLQUAKE
//cvar_t scr_allowsnap = {"scr_allowsnap", "1"}; // TODO: outside of the conditional compilation check in qw
cvar_t gl_triplebuffer = { "gl_triplebuffer", "1", true }; // TODO: remove

extern cvar_t crosshair;
#endif

qboolean scr_initialized; // ready to draw

//qpic_t *scr_ram;
qpic_t *scr_net;
//qpic_t *scr_turtle;

int scr_fullupdate;

int clearconsole;
int clearnotify;

//viddef_t vid; // global video state

#ifndef GLQUAKE
vrect_t *pconupdate;
#endif
vrect_t scr_vrect;

qboolean scr_disabled_for_loading;
qboolean scr_drawloading;
float scr_disabled_time;
//qboolean scr_skipupdate; // TODO: ???

qboolean block_drawing;

void SCR_ScreenShot_f(void);
//void SCR_RSShot_f (void); // TODO

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/

char scr_centerstring[1024];
float scr_centertime_start; // for slow victory printing
float scr_centertime_off;
int scr_center_lines;
int scr_erase_lines;
int scr_erase_center;

/*
==============
SCR_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void SCR_CenterPrint(const char *str)
{
	strncpy(scr_centerstring, str, sizeof(scr_centerstring) - 1);
	scr_centertime_off = scr_centertime.value;
	scr_centertime_start = cl.time;

	// count the number of lines for centering
	scr_center_lines = 1;
	while(*str)
	{
		if(*str == '\n')
			scr_center_lines++;
		str++;
	}
}

void SCR_DrawCenterString(void)
{
	char *start;
	int l;
	int j;
	int x, y;
	int remaining;

	// the finale prints the characters one at a time
	if(cl.intermission)
		remaining = scr_printspeed.value * (cl.time - scr_centertime_start);
	else
		remaining = 9999;

	scr_erase_center = 0;
	start = scr_centerstring;

	if(scr_center_lines <= 4)
		y = vid.height * 0.35;
	else
		y = 48;

	do
	{
		// scan the width of the line
		for(l = 0; l < 40; l++)
			if(start[l] == '\n' || !start[l])
				break;
		x = (vid.width - l * 8) / 2;
		for(j = 0; j < l; j++, x += 8)
		{
			Draw_Character(x, y, start[j]);
			if(!remaining--)
				return;
		}

		y += 8;

		while(*start && *start != '\n')
			start++;

		if(!*start)
			break;
		start++; // skip the \n
	} while(1);
}

#ifndef GLQUAKE
void SCR_EraseCenterString(void)
{
	int y;

	if(scr_erase_center++ > vid.numpages)
	{
		scr_erase_lines = 0;
		return;
	}

	if(scr_center_lines <= 4)
		y = vid.height * 0.35;
	else
		y = 48;

	scr_copytop = 1;
	Draw_TileClear(0, y, vid.width, 8 * scr_erase_lines);
}
#endif // GLQUAKE

void SCR_CheckDrawCenterString(void)
{
	scr_copytop = 1;
	if(scr_center_lines > scr_erase_lines)
		scr_erase_lines = scr_center_lines;

	scr_centertime_off -= host_frametime;

	if(scr_centertime_off <= 0 && !cl.intermission)
		return;
	if(key_dest != key_game)
		return;

	SCR_DrawCenterString();
}

//=============================================================================

/*
====================
CalcFov
====================
*/
float CalcFov(float fov_x, float width, float height)
{
	float a;
	float x;

	if(fov_x < 1 || fov_x > 179)
		Sys_Error("Bad fov: %f", fov_x);

	x = width / tan(fov_x / 360 * M_PI);

	a = atan(height / x);

	a = a * 360 / M_PI;

	return a;
}

/*
=================
SCR_CalcRefdef

Must be called whenever vid changes
Internal use only
=================
*/
static void SCR_CalcRefdef(void)
{
	float size;
#ifdef GLQUAKE
	int h;
	qboolean full = false;
#else
	vrect_t vrect;
#endif

	scr_fullupdate = 0; // force a background redraw
	vid.recalc_refdef = 0;

	// force the status bar to redraw
	Sbar_Changed();

	//========================================

	// bound viewsize
	if(scr_viewsize.value < 30)
		Cvar_Set("viewsize", "30");
	if(scr_viewsize.value > 120)
		Cvar_Set("viewsize", "120");

	// bound field of view
	if(scr_fov.value < 10)
		Cvar_Set("fov", "10");
	if(scr_fov.value > 170)
		Cvar_Set("fov", "170");

#ifndef GLQUAKE
	r_refdef.fov_x = scr_fov.value;
	r_refdef.fov_y = CalcFov(r_refdef.fov_x, r_refdef.vrect.width, r_refdef.vrect.height);
#endif

	// intermission is always full screen
	if(cl.intermission)
		size = 120;
	else
		size = scr_viewsize.value;

	if(size >= 120)
		sb_lines = 0; // no status bar at all
	else if(size >= 110)
		sb_lines = 24; // no inventory
	else
		sb_lines = 24 + 16 + 8;

#ifdef GLQUAKE
	if(scr_viewsize.value >= 100.0)
	{
		full = true;
		size = 100.0;
	}
	else
		size = scr_viewsize.value;
	
	if(cl.intermission)
	{
		full = true;
		size = 100.0;
		sb_lines = 0;
	}
	size /= 100.0;

	// TODO: qw
	//if (!cl_sbar.value && full)
		//h = vid.height;
	//else
		h = vid.height - sb_lines;

	r_refdef.vrect.width = vid.width * size;
	if(r_refdef.vrect.width < 96)
	{
		size = 96.0 / r_refdef.vrect.width;
		r_refdef.vrect.width = 96; // min for icons
	}

	r_refdef.vrect.height = vid.height * size;
	//if (cl_sbar.value || !full) // TODO: qw
	//{
		if(r_refdef.vrect.height > vid.height - sb_lines)
			r_refdef.vrect.height = vid.height - sb_lines;
	//}
	/*else*/ if(r_refdef.vrect.height > vid.height)
		r_refdef.vrect.height = vid.height;
	r_refdef.vrect.x = (vid.width - r_refdef.vrect.width) / 2;
	if(full)
		r_refdef.vrect.y = 0;
	else
		r_refdef.vrect.y = (h - r_refdef.vrect.height) / 2;

	r_refdef.fov_x = scr_fov.value;
	r_refdef.fov_y = CalcFov(r_refdef.fov_x, r_refdef.vrect.width, r_refdef.vrect.height);

	scr_vrect = r_refdef.vrect;
#else // if not GLQUAKE
	// these calculations mirror those in R_Init() for r_refdef, but take no
	// account of water warping
	vrect.x = 0;
	vrect.y = 0;
	vrect.width = vid.width;
	vrect.height = vid.height;

	R_SetVrect(&vrect, &scr_vrect, sb_lines);

	// guard against going from one mode to another that's less than half the
	// vertical resolution
	if(scr_con_current > vid.height)
		scr_con_current = vid.height;

	// notify the refresh of the change
	R_ViewChanged(&vrect, sb_lines, vid.aspect);
#endif // GLQUAKE
}

/*
=================
SCR_SizeUp_f

Keybinding command
=================
*/
void SCR_SizeUp_f(void)
{
	Cvar_SetValue("viewsize", scr_viewsize.value + 10);
	vid.recalc_refdef = 1;
}

/*
=================
SCR_SizeDown_f

Keybinding command
=================
*/
void SCR_SizeDown_f(void)
{
	Cvar_SetValue("viewsize", scr_viewsize.value - 10);
	vid.recalc_refdef = 1;
}

//============================================================================

/*
==================
SCR_Init
==================
*/
void SCR_Init(void)
{
	Cvar_RegisterVariable(&scr_fov);
	Cvar_RegisterVariable(&scr_viewsize);
	Cvar_RegisterVariable(&scr_conspeed);
	Cvar_RegisterVariable(&scr_showpause);
	Cvar_RegisterVariable(&scr_centertime);
	Cvar_RegisterVariable(&scr_printspeed);
	
#ifdef GLQUAKE
	//Cvar_RegisterVariable (&scr_allowsnap); // TODO: is this glquake only? looks like not in qw
	Cvar_RegisterVariable(&gl_triplebuffer);
#endif

	//
	// register our commands
	//
	Cmd_AddCommand("screenshot", SCR_ScreenShot_f);
	//Cmd_AddCommand("snap", SCR_RSShot_f); // TODO: GLQUAKE only or QW
	Cmd_AddCommand("sizeup", SCR_SizeUp_f);
	Cmd_AddCommand("sizedown", SCR_SizeDown_f);
	//Cmd_AddCommand("timerefresh", SCR_TimeRefresh_f); // TODO: q2
	//Cmd_AddCommand("loading", SCR_Loading_f); // TODO: q2
	//Cmd_AddCommand("sky", SCR_Sky_f); // TODO: q2

	//scr_ram = Draw_PicFromWad("ram"); // TODO: W_GetLumpName in qw
	//scr_net = Draw_PicFromWad("net"); // TODO: W_GetLumpName in qw
	//scr_turtle = Draw_PicFromWad("turtle"); // TODO: W_GetLumpName in qw

	scr_initialized = true;
}

/*
==============
SCR_DrawRam
==============
*/
// TODO: unused?
/*
void SCR_DrawRam(void)
{
	if(!r_cache_thrash)
		return;

	Draw_Pic(scr_vrect.x + 32, scr_vrect.y, scr_ram);
}
*/

/*
==============
SCR_DrawTurtle
==============
*/
// TODO: unused?
/*
void SCR_DrawTurtle(void)
{
	static int count;

	if(host_frametime < 0.1)
	{
		count = 0;
		return;
	}

	count++;
	if(count < 3)
		return;

	Draw_Pic(scr_vrect.x, scr_vrect.y, scr_turtle);
}
*/

/*
==============
SCR_DrawNet
==============
*/
void SCR_DrawNet(void)
{
	if(realtime - cl.last_received_message < 0.3)
	//if (cls.netchan.outgoing_sequence - cls.netchan.incoming_acknowledged < UPDATE_BACKUP-1) // TODO: qw
		return;
	if(cls.demoplayback)
		return;

	//Draw_Pic(scr_vrect.x + 64, scr_vrect.y, scr_net); // TODO
}

void SCR_DrawFPS (void)
{
	extern cvar_t show_fps;
	static double lastframetime;
	double t;
	extern int fps_count;
	static int lastfps;
	int x, y;
	char st[80];

	if (!show_fps.value)
		return;

	t = Sys_FloatTime();
	if ((t - lastframetime) >= 1.0) {
		lastfps = fps_count;
		fps_count = 0;
		lastframetime = t;
	}

	sprintf(st, "%3d fps", lastfps);
	x = Q_strlen(st) + 8;
	y = 8;
	//x = vid.width - Q_strlen(st) * 8 - 8;
	//y = vid.height - sb_lines - 8;
//	Draw_TileClear(x, y, Q_strlen(st) * 8, 8);
	Draw_String(x, y, st);
}

/*
==============
SCR_DrawPause
==============
*/
void SCR_DrawPause(void)
{
	if(!scr_showpause.value) // turn off for screenshots
		return;

	if(!cl.paused) // TODO: !cl_paused->value (cvar) in q2
		return;

	//
	qpic_t *pic = Draw_CachePic("gfx/pause.lmp");
	Draw_Pic((vid.width - pic->width) / 2,
	         (vid.height - 48 - pic->height) / 2, pic);
	//
	// TODO: q2
	// int w, h;
	//
	//re.DrawGetPicSize (&w, &h, "pause");
	//re.DrawPic ((viddef.width-w)/2, viddef.height/2 + 8, "pause");
	//
}

/*
==============
SCR_DrawLoading
==============
*/
void SCR_DrawLoading(void)
{
	if(!scr_drawloading)
		return;

	//
	qpic_t *pic = Draw_CachePic("gfx/loading.lmp");
	Draw_Pic((vid.width - pic->width) / 2,
	         (vid.height - 48 - pic->height) / 2, pic);
	//
	// TODO: q2
	//int w, h;

	//scr_draw_loading = false;
	//re.DrawGetPicSize (&w, &h, "loading");
	//re.DrawPic ((viddef.width-w)/2, (viddef.height-h)/2, "loading");
	//
}

//=============================================================================

/*
==================
SCR_SetUpToDrawConsole
==================
*/
void SCR_SetUpToDrawConsole(void)
{
	Con_CheckResize();

	if(scr_drawloading)
		return; // never show a console with loading plaque

	// decide on the height of the console
	con_forcedup = !cl.worldmodel || cls.signon != SIGNONS; // TODO: unused in qw

	//if (cls.state != ca_active) // TODO: qw
	if(con_forcedup)
	{
		scr_conlines = vid.height; // full screen
		scr_con_current = scr_conlines;
	}
	else if(key_dest == key_console)
		scr_conlines = vid.height / 2; // half screen
	else
		scr_conlines = 0; // none visible

	if(scr_conlines < scr_con_current)
	{
		scr_con_current -= scr_conspeed.value * host_frametime;
		if(scr_conlines > scr_con_current)
			scr_con_current = scr_conlines;
	}
	else if(scr_conlines > scr_con_current)
	{
		scr_con_current += scr_conspeed.value * host_frametime;
		if(scr_conlines < scr_con_current)
			scr_con_current = scr_conlines;
	}

	if(clearconsole++ < vid.numpages)
	{
#ifndef GLQUAKE
		scr_copytop = 1;
		Draw_TileClear(0, (int)scr_con_current, vid.width, vid.height - (int)scr_con_current);
#endif
		Sbar_Changed();
	}
	else if(clearnotify++ < vid.numpages)
	{
#ifndef GLQUAKE
		scr_copytop = 1;
		Draw_TileClear(0, 0, vid.width, con_notifylines);
#endif
	}
	else
		con_notifylines = 0;
}

/*
==================
SCR_DrawConsole
==================
*/
void SCR_DrawConsole(void)
{
	if(scr_con_current)
	{
		scr_copyeverything = 1;
		Con_DrawConsole(scr_con_current, true); // TODO: Con_DrawConsole (scr_con_current); in QW
		clearconsole = 0;
	}
	else
	{
		if(key_dest == key_game || key_dest == key_message)
			Con_DrawNotify(); // only draw notify in game
	}
}

/* 
============================================================================== 
 
						SCREEN SHOTS 
 
============================================================================== 
*/

typedef struct _TargaHeader
{
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
} TargaHeader;

/* 
================== 
SCR_ScreenShot_f
================== 
*/
#ifdef GLQUAKE
void SCR_ScreenShot_f(void)
{
	byte *buffer;
	char pcxname[80];
	char checkname[MAX_OSPATH];
	int i, c, temp;
	//
	// find a file name to save it to
	//
	strcpy(pcxname, "quake00.tga");

	for(i = 0; i <= 99; i++)
	{
		pcxname[5] = i / 10 + '0';
		pcxname[6] = i % 10 + '0';
		sprintf(checkname, "%s/%s", com_gamedir, pcxname);
		if(FS_GetFileTime(checkname) == -1)
			break; // file doesn't exist
	}
	if(i == 100)
	{
		Con_Printf("SCR_ScreenShot_f: Couldn't create a PCX file\n");
		return;
	}

	buffer = malloc(glwidth * glheight * 3 + 18);
	memset(buffer, 0, 18);
	buffer[2] = 2; // uncompressed type
	buffer[12] = glwidth & 255;
	buffer[13] = glwidth >> 8;
	buffer[14] = glheight & 255;
	buffer[15] = glheight >> 8;
	buffer[16] = 24; // pixel size

	qglReadPixels(glx, gly, glwidth, glheight, GL_RGB, GL_UNSIGNED_BYTE, buffer + 18);

	// swap rgb to bgr
	c = 18 + glwidth * glheight * 3;
	for(i = 18; i < c; i += 3)
	{
		temp = buffer[i];
		buffer[i] = buffer[i + 2];
		buffer[i + 2] = temp;
	}
	COM_WriteFile(pcxname, buffer, glwidth * glheight * 3 + 18);

	free(buffer);
	Con_Printf("Wrote %s\n", pcxname);
}

/* 
============== 
WritePCXfile 
============== 
*/
/* 
void WritePCXfile (char *filename, byte *data, int width, int height,
	int rowbytes, byte *palette, qboolean upload) 
{
	int		i, j, length;
	pcx_t	*pcx;
	byte		*pack;
	  
	pcx = Hunk_TempAlloc (width*height*2+1000);
	if (pcx == NULL)
	{
		Con_Printf("SCR_ScreenShot_f: not enough memory\n");
		return;
	} 
 
	pcx->manufacturer = 0x0a;	// PCX id
	pcx->version = 5;			// 256 color
 	pcx->encoding = 1;		// uncompressed
	pcx->bits_per_pixel = 8;		// 256 color
	pcx->xmin = 0;
	pcx->ymin = 0;
	pcx->xmax = LittleShort((short)(width-1));
	pcx->ymax = LittleShort((short)(height-1));
	pcx->hres = LittleShort((short)width);
	pcx->vres = LittleShort((short)height);
	Q_memset (pcx->palette,0,sizeof(pcx->palette));
	pcx->color_planes = 1;		// chunky image
	pcx->bytes_per_line = LittleShort((short)width);
	pcx->palette_type = LittleShort(2);		// not a grey scale
	Q_memset (pcx->filler,0,sizeof(pcx->filler));

// pack the image
	pack = &pcx->data;

	data += rowbytes * (height - 1);

	for (i=0 ; i<height ; i++)
	{
		for (j=0 ; j<width ; j++)
		{
			if ( (*data & 0xc0) != 0xc0)
				*pack++ = *data++;
			else
			{
				*pack++ = 0xc1;
				*pack++ = *data++;
			}
		}

		data += rowbytes - width;
		data -= rowbytes * 2;
	}
			
// write the palette
	*pack++ = 0x0c;	// palette ID byte
	for (i=0 ; i<768 ; i++)
		*pack++ = *palette++;
		
// write output file 
	length = pack - (byte *)pcx;

	if (upload)
		CL_StartUpload((void *)pcx, length);
	else
		COM_WriteFile (filename, pcx, length);
}

// Find closest color in the palette for named color
int MipColor(int r, int g, int b)
{
	int i;
	float dist;
	int best;
	float bestdist;
	int r1, g1, b1;
	static int lr = -1, lg = -1, lb = -1;
	static int lastbest;

	if (r == lr && g == lg && b == lb)
		return lastbest;

	bestdist = 256*256*3;

	for (i = 0; i < 256; i++) {
		r1 = host_basepal[i*3] - r;
		g1 = host_basepal[i*3+1] - g;
		b1 = host_basepal[i*3+2] - b;
		dist = r1*r1 + g1*g1 + b1*b1;
		if (dist < bestdist) {
			bestdist = dist;
			best = i;
		}
	}
	lr = r; lg = g; lb = b;
	lastbest = best;
	return best;
}

// TODO
// from gl_draw.c
byte *draw_chars; // 8*8 graphic characters

void SCR_DrawCharToSnap (int num, byte *dest, int width)
{
	int		row, col;
	byte	*source;
	int		drawline;
	int		x;

	row = num>>4;
	col = num&15;
	source = draw_chars + (row<<10) + (col<<3);

	drawline = 8;

	while (drawline--)
	{
		for (x=0 ; x<8 ; x++)
			if (source[x])
				dest[x] = source[x];
			else
				dest[x] = 98;
		source += 128;
		dest -= width;
	}

}

void SCR_DrawStringToSnap (const char *s, byte *buf, int x, int y, int width)
{
	byte *dest;
	const unsigned char *p;

	dest = buf + ((y * width) + x);

	p = (const unsigned char *)s;
	while (*p) {
		SCR_DrawCharToSnap(*p++, dest, width);
		dest += 8;
	}
}
*/

/* 
================== 
SCR_RSShot_f
================== 
*/
/* 
void SCR_RSShot_f (void) 
{ 
	int     i, x, y;
	unsigned char		*src, *dest;
	char		pcxname[80]; 
	char		checkname[MAX_OSPATH];
	unsigned char		*newbuf, *srcbuf;
	int srcrowbytes;
	int w, h;
	int dx, dy, dex, dey, nx;
	int r, b, g;
	int count;
	float fracw, frach;
	char st[80];
	time_t now;

	if (CL_IsUploading())
		return; // already one pending

	if (cls.state < ca_onserver)
		return; // gotta be connected

	Con_Printf("Remote screen shot requested.\n");

#if 0
// 
// find a file name to save it to 
// 
	strcpy(pcxname,"mquake00.pcx");
		
	for (i=0 ; i<=99 ; i++) 
	{ 
		pcxname[6] = i/10 + '0'; 
		pcxname[7] = i%10 + '0'; 
		sprintf (checkname, "%s/%s", com_gamedir, pcxname);
		if (Sys_FileTime(checkname) == -1)
			break;	// file doesn't exist
	} 
	if (i==100) 
	{
		Con_Printf ("SCR_ScreenShot_f: Couldn't create a PCX"); 
		return;
	}
#endif
 
// 
// save the pcx file 
// 
	newbuf = malloc(glheight * glwidth * 3);

	qglReadPixels (glx, gly, glwidth, glheight, GL_RGB, GL_UNSIGNED_BYTE, newbuf ); 

	w = (vid.width < RSSHOT_WIDTH) ? glwidth : RSSHOT_WIDTH;
	h = (vid.height < RSSHOT_HEIGHT) ? glheight : RSSHOT_HEIGHT;

	fracw = (float)glwidth / (float)w;
	frach = (float)glheight / (float)h;

	for (y = 0; y < h; y++) {
		dest = newbuf + (w*3 * y);

		for (x = 0; x < w; x++) {
			r = g = b = 0;

			dx = x * fracw;
			dex = (x + 1) * fracw;
			if (dex == dx) dex++; // at least one
			dy = y * frach;
			dey = (y + 1) * frach;
			if (dey == dy) dey++; // at least one

			count = 0;
			for (; dy < dey; dy++) {
				src = newbuf + (glwidth * 3 * dy) + dx * 3;
				for (nx = dx; nx < dex; nx++) {
					r += *src++;
					g += *src++;
					b += *src++;
					count++;
				}
			}
			r /= count;
			g /= count;
			b /= count;
			*dest++ = r;
			*dest++ = b;
			*dest++ = g;
		}
	}

	// convert to eight bit
	for (y = 0; y < h; y++) {
		src = newbuf + (w * 3 * y);
		dest = newbuf + (w * y);

		for (x = 0; x < w; x++) {
			*dest++ = MipColor(src[0], src[1], src[2]);
			src += 3;
		}
	}

	time(&now);
	strcpy(st, ctime(&now));
	st[strlen(st) - 1] = 0;
	SCR_DrawStringToSnap (st, newbuf, w - strlen(st)*8, h - 1, w);

	strncpy(st, cls.servername, sizeof(st));
	st[sizeof(st) - 1] = 0;
	SCR_DrawStringToSnap (st, newbuf, w - strlen(st)*8, h - 11, w);

	strncpy(st, name.string, sizeof(st));
	st[sizeof(st) - 1] = 0;
	SCR_DrawStringToSnap (st, newbuf, w - strlen(st)*8, h - 21, w);

	WritePCXfile (pcxname, newbuf, w, h, w, host_basepal, true);

	free(newbuf);

	Con_Printf ("Wrote %s\n", pcxname);
}
*/
#else // if not GLQUAKE
void SCR_ScreenShot_f(void)
{
	int i;
	char pcxname[80];
	char checkname[MAX_OSPATH];

	//
	// find a file name to save it to
	//
	strcpy(pcxname, "quake00.pcx");

	for(i = 0; i <= 99; i++)
	{
		pcxname[5] = i / 10 + '0';
		pcxname[6] = i % 10 + '0';
		sprintf(checkname, "%s/%s", com_gamedir, pcxname);
		if(FS_FileTime(checkname) == -1)
			break; // file doesn't exist
	}
	if(i == 100)
	{
		Con_Printf("SCR_ScreenShot_f: Couldn't create a PCX file\n");
		return;
	}

	//
	// save the pcx file
	//
	D_EnableBackBufferAccess(); // enable direct drawing of console to back
	                            //  buffer

	WritePCXfile(pcxname, vid.buffer, vid.width, vid.height, vid.rowbytes,
	             host_basepal);

	D_DisableBackBufferAccess(); // for adapters that can't stay mapped in
	                             //  for linear writes all the time

	Con_Printf("Wrote %s\n", pcxname);
}

typedef struct
{
	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;
	unsigned short xmin, ymin, xmax, ymax;
	unsigned short hres, vres;
	unsigned char palette[48];
	char reserved;
	char color_planes;
	unsigned short bytes_per_line;
	unsigned short palette_type;
	char filler[58];
	unsigned char data; // unbounded
} pcx_t;

/* 
============== 
WritePCXfile 
============== 
*/
void WritePCXfile(char *filename, byte *data, int width, int height,
                  int rowbytes, byte *palette)
{
	int i, j, length;
	pcx_t *pcx;
	byte *pack;

	pcx = Hunk_TempAlloc(width * height * 2 + 1000);
	if(pcx == NULL)
	{
		Con_Printf("SCR_ScreenShot_f: not enough memory\n");
		return;
	}

	pcx->manufacturer = 0x0a; // PCX id
	pcx->version = 5;         // 256 color
	pcx->encoding = 1;        // uncompressed
	pcx->bits_per_pixel = 8;  // 256 color
	pcx->xmin = 0;
	pcx->ymin = 0;
	pcx->xmax = LittleShort((short)(width - 1));
	pcx->ymax = LittleShort((short)(height - 1));
	pcx->hres = LittleShort((short)width);
	pcx->vres = LittleShort((short)height);
	Q_memset(pcx->palette, 0, sizeof(pcx->palette));
	pcx->color_planes = 1; // chunky image
	pcx->bytes_per_line = LittleShort((short)width);
	pcx->palette_type = LittleShort(2); // not a grey scale
	Q_memset(pcx->filler, 0, sizeof(pcx->filler));

	// pack the image
	pack = &pcx->data;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			if((*data & 0xc0) != 0xc0)
				*pack++ = *data++;
			else
			{
				*pack++ = 0xc1;
				*pack++ = *data++;
			}
		}

		data += rowbytes - width;
	}

	// write the palette
	*pack++ = 0x0c; // palette ID byte
	for(i = 0; i < 768; i++)
		*pack++ = *palette++;

	// write output file
	length = pack - (byte *)pcx;
	COM_WriteFile(filename, pcx, length);
}
#endif // GLQUAKE

//=============================================================================

/*
===============
SCR_BeginLoadingPlaque

================
*/
void SCR_BeginLoadingPlaque(void)
{
	S_StopAllSounds(true);

	if(cls.state != ca_connected)
		return;
	if(cls.signon != SIGNONS)
		return;

	// redraw with no console and the loading plaque
	Con_ClearNotify();
	scr_centertime_off = 0;
	scr_con_current = 0;

	scr_drawloading = true;
	scr_fullupdate = 0;
	Sbar_Changed();
	SCR_UpdateScreen();
	scr_drawloading = false;

	scr_disabled_for_loading = true;
	scr_disabled_time = realtime;
	scr_fullupdate = 0;
}

/*
===============
SCR_EndLoadingPlaque

================
*/
void SCR_EndLoadingPlaque(void)
{
	scr_disabled_for_loading = false;
	scr_fullupdate = 0;
	Con_ClearNotify();
}

//=============================================================================

const char *scr_notifystring;
qboolean scr_drawdialog;

void SCR_DrawNotifyString(void)
{
	const char *start;
	int l;
	int j;
	int x, y;

	start = scr_notifystring;

	y = vid.height * 0.35;

	do
	{
		// scan the width of the line
		for(l = 0; l < 40; l++)
			if(start[l] == '\n' || !start[l])
				break;
		x = (vid.width - l * 8) / 2;
		for(j = 0; j < l; j++, x += 8)
			Draw_Character(x, y, start[j]);

		y += 8;

		while(*start && *start != '\n')
			start++;

		if(!*start)
			break;
		start++; // skip the \n
	} while(1);
}

/*
==================
SCR_ModalMessage

Displays a text string in the center of the screen and waits for a Y or N
keypress.  
==================
*/
int SCR_ModalMessage(const char *text)
{
	if(cls.state == ca_dedicated) // TODO: non-qw
		return true; // TODO: non-qw

	scr_notifystring = text;

	// draw a fresh screen
	scr_fullupdate = 0;
	scr_drawdialog = true;
	SCR_UpdateScreen();
	scr_drawdialog = false;

	S_ClearBuffer(); // so dma doesn't loop current sound

	do
	{
		key_count = -1; // wait for a key down and up
		Sys_SendKeyEvents();
	} while(key_lastpress != 'y' && key_lastpress != 'n' && key_lastpress != K_ESCAPE);

	scr_fullupdate = 0;
	SCR_UpdateScreen();

	return key_lastpress == 'y';
}

//=============================================================================

/*
===============
SCR_BringDownConsole

Brings the console down and fades the palettes back to normal
================
*/
void SCR_BringDownConsole(void)
{
	int i;

	scr_centertime_off = 0;

	for(i = 0; i < 20 && scr_conlines != scr_con_current; i++)
		SCR_UpdateScreen();

	cl.cshifts[0].percent = 0; // no area contents palette on next frame
	VID_SetPalette(host_basepal);
}

#ifdef GLQUAKE
void SCR_TileClear(void)
{
	if(r_refdef.vrect.x > 0)
	{
		// left
		Draw_TileClear(0, 0, r_refdef.vrect.x, vid.height - sb_lines);
		// right
		Draw_TileClear(r_refdef.vrect.x + r_refdef.vrect.width, 0,
		               vid.width - r_refdef.vrect.x + r_refdef.vrect.width,
		               vid.height - sb_lines);
	}
	if(r_refdef.vrect.y > 0)
	{
		// top
		Draw_TileClear(r_refdef.vrect.x, 0,
		               r_refdef.vrect.x + r_refdef.vrect.width,
		               r_refdef.vrect.y);
		// bottom
		Draw_TileClear(r_refdef.vrect.x,
		               r_refdef.vrect.y + r_refdef.vrect.height,
		               r_refdef.vrect.width,
		               vid.height - sb_lines -
		               (r_refdef.vrect.height + r_refdef.vrect.y));
	}
}
#endif

//float oldsbar = 0; // TODO: qw

/*
// TODO: qw
void SCR_UpdateScreen (void)
{
	vrect_t		vrect;

	if (!scr_initialized || !con_initialized)
		return;				// not initialized yet

	if (scr_viewsize.value != oldscr_viewsize)
	{
		oldscr_viewsize = scr_viewsize.value;
		vid.recalc_refdef = 1;
	}
	
//
// check for vid changes
//
	if (oldfov != scr_fov.value)
	{
		oldfov = scr_fov.value;
		vid.recalc_refdef = true;
	}
	
	if (oldscreensize != scr_viewsize.value)
	{
		oldscreensize = scr_viewsize.value;
		vid.recalc_refdef = true;
	}

	if (oldsbar != cl_sbar.value)
	{
		oldsbar = cl_sbar.value;
		vid.recalc_refdef = true;
	}
	
	if (vid.recalc_refdef)
	{
		// something changed, so reorder the screen
		SCR_CalcRefdef ();
	}

//
// do 3D refresh drawing, and then update the screen
//
	D_EnableBackBufferAccess ();	// of all overlay stuff if drawing directly

	if (scr_fullupdate++ < vid.numpages)
	{	// clear the entire screen
		scr_copyeverything = 1;
		Draw_TileClear (0,0,vid.width,vid.height);
		Sbar_Changed ();
	}

	pconupdate = NULL;

	SCR_SetUpToDrawConsole ();
	SCR_EraseCenterString ();
	
	D_DisableBackBufferAccess ();	// for adapters that can't stay mapped in
									//  for linear writes all the time

	VID_LockBuffer ();

	V_RenderView ();

	VID_UnlockBuffer ();

	D_EnableBackBufferAccess ();	// of all overlay stuff if drawing directly

	if (scr_drawdialog)
	{
		Sbar_Draw ();
		Draw_FadeScreen ();
		SCR_DrawNotifyString ();
		scr_copyeverything = true;
	}
	else if (cl.intermission == 1 && key_dest == key_game)
	{
		Sbar_IntermissionOverlay ();
	}
	else if (cl.intermission == 2 && key_dest == key_game)
	{
		Sbar_FinaleOverlay ();
		SCR_CheckDrawCenterString ();
	}
	else
	{
		SCR_DrawRam ();
		SCR_DrawNet ();
		SCR_DrawTurtle ();
		SCR_DrawPause ();
		SCR_DrawFPS ();
		SCR_CheckDrawCenterString ();
		Sbar_Draw ();
		SCR_DrawConsole ();	
		M_Draw ();
	}

	D_DisableBackBufferAccess ();	// for adapters that can't stay mapped in
									//  for linear writes all the time
	if (pconupdate)
	{
		D_UpdateRects (pconupdate);
	}

	V_UpdatePalette ();

//
// update one of three areas
//
	if (scr_copyeverything)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height;
		vrect.pnext = 0;
	
		VID_Update (&vrect);
	}
	else if (scr_copytop)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height - sb_lines;
		vrect.pnext = 0;
	
		VID_Update (&vrect);
	}	
	else
	{
		vrect.x = scr_vrect.x;
		vrect.y = scr_vrect.y;
		vrect.width = scr_vrect.width;
		vrect.height = scr_vrect.height;
		vrect.pnext = 0;
	
		VID_Update (&vrect);
	}	
}
*/

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.

WARNING: be very careful calling this from elsewhere, because the refresh
needs almost the entire 256k of stack space!
==================
*/
void SCR_UpdateScreen(void)
{
#ifndef GLQUAKE
	static float oldscr_viewsize;
	static float oldlcd_x;
	vrect_t vrect;
#endif

#ifdef GLQUAKE
	if(block_drawing)
		return;
#else
	if(scr_skipupdate || block_drawing)
		return;
#endif

#ifdef GLQUAKE
	vid.numpages = 2 + gl_triplebuffer.value;
#endif

	scr_copytop = 0;
	scr_copyeverything = 0;

	if(scr_disabled_for_loading)
	{
		if(realtime - scr_disabled_time > 60)
		{
			scr_disabled_for_loading = false;
			Con_Printf("load failed.\n");
		}
		else
			return;
	}

	if(cls.state == ca_dedicated)
		return; // stdout only
	
	if(!scr_initialized || !con_initialized)
		return; // not initialized yet

#ifndef GLQUAKE
	if(scr_viewsize.value != oldscr_viewsize)
	{
		oldscr_viewsize = scr_viewsize.value;
		vid.recalc_refdef = 1;
	}
#endif
	
// TODO: qw
/*
	if (oldsbar != cl_sbar.value)
	{
		oldsbar = cl_sbar.value;
		vid.recalc_refdef = true;
	}
*/

#ifdef GLQUAKE
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
#endif

	//
	// check for fov changes
	//
	if(oldfov != scr_fov.value)
	{
		oldfov = scr_fov.value;
		vid.recalc_refdef = true;
	}

#ifndef GLQUAKE
	if(oldlcd_x != lcd_x.value)
	{
		oldlcd_x = lcd_x.value;
		vid.recalc_refdef = true;
	}
#endif

	// TODO: not present in qw
	if(oldscreensize != scr_viewsize.value)
	{
		oldscreensize = scr_viewsize.value;
		vid.recalc_refdef = true;
	}
	//

	// something changed, so reorder the screen
	if(vid.recalc_refdef)
		SCR_CalcRefdef();

	//
	// do 3D refresh drawing, and then update the screen
	//
#ifndef GLQUAKE
	D_EnableBackBufferAccess(); // of all overlay stuff if drawing directly

	if(scr_fullupdate++ < vid.numpages)
	{ // clear the entire screen
		scr_copyeverything = 1;
		Draw_TileClear(0, 0, vid.width, vid.height);
		Sbar_Changed();
	}

	pconupdate = NULL;
#endif

	SCR_SetUpToDrawConsole();

#ifndef GLQUAKE
	SCR_EraseCenterString();

	D_DisableBackBufferAccess(); // for adapters that can't stay mapped in
	                             //  for linear writes all the time

	VID_LockBuffer();
#endif
	
	V_RenderView();

#ifdef GLQUAKE
	GL_Set2D();
	
	//
	// draw any areas not covered by the refresh
	//
	SCR_TileClear();
	
	// TODO
	//if (r_netgraph.value)
		//R_NetGraph ();
#else
	VID_UnlockBuffer();

	D_EnableBackBufferAccess(); // of all overlay stuff if drawing directly
#endif

	if(scr_drawdialog)
	{
		//Sbar_Draw(); // TODO
		Draw_FadeScreen();
		SCR_DrawNotifyString();
		scr_copyeverything = true;
	}
	else if(scr_drawloading)
	{
		SCR_DrawLoading();
		//Sbar_Draw(); // TODO
	}
	else if(cl.intermission == 1 && key_dest == key_game)
	{
		Sbar_IntermissionOverlay();
	}
	else if(cl.intermission == 2 && key_dest == key_game)
	{
		Sbar_FinaleOverlay();
		SCR_CheckDrawCenterString();
	}
#ifndef GLQUAKE
	else if(cl.intermission == 3 && key_dest == key_game)
	{
		SCR_CheckDrawCenterString();
	}
#endif
	else
	{
#ifndef GLQUAKE
		if(crosshair.value)
			Draw_Character(scr_vrect.x + scr_vrect.width / 2, scr_vrect.y + scr_vrect.height / 2, '+');
			// TODO: Draw_Crosshair(); in QW
#endif

		//SCR_DrawRam();
		SCR_DrawNet();
		//SCR_DrawTurtle();
		SCR_DrawPause();
		SCR_CheckDrawCenterString();
		//Sbar_Draw(); // TODO
		SCR_DrawConsole();
		M_Draw();
	}

	SCR_DrawFPS();
	
#ifndef GLQUAKE
	D_DisableBackBufferAccess(); // for adapters that can't stay mapped in
	                             //  for linear writes all the time
	if(pconupdate)
		D_UpdateRects(pconupdate);
#endif

	VGui_Frame(); // TODO: looks like this should be called VGui_Paint
	
	V_UpdatePalette();

#ifdef GLQUAKE
	GL_EndRendering();
#else
	//
	// update one of three areas
	//
	if(scr_copyeverything)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height;
		vrect.pnext = 0;

		VID_Update(&vrect);
	}
	else if(scr_copytop)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height - sb_lines;
		vrect.pnext = 0;

		VID_Update(&vrect);
	}
	else
	{
		vrect.x = scr_vrect.x;
		vrect.y = scr_vrect.y;
		vrect.width = scr_vrect.width;
		vrect.height = scr_vrect.height;
		vrect.pnext = 0;

		VID_Update(&vrect);
	}
#endif
}

//=============================================================================

/*
==================
SCR_UpdateWholeScreen
==================
*/
#ifndef GLQUAKE
void SCR_UpdateWholeScreen(void)
{
	scr_fullupdate = 0;
	SCR_UpdateScreen();
}
#endif