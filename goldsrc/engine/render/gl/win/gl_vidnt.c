/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019, 2021 BlackPhrase
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
/// @brief NT GL vid component

#include "quakedef.h"
#include "winquake.h"
//#include "resource.h"
#include <commctrl.h>

#include "glw_win.h"

#define MAX_MODE_LIST 30
#define VID_ROW_SIZE 3
#define WARP_WIDTH 320
#define WARP_HEIGHT 200
#define MAXWIDTH 10000
#define MAXHEIGHT 10000
#define BASEWIDTH 320
#define BASEHEIGHT 200

#define MODE_WINDOWED 0
#define NO_MODE (MODE_WINDOWED - 1)
#define MODE_FULLSCREEN_DEFAULT (MODE_WINDOWED + 1)

typedef struct
{
	modestate_t type;
	int width;
	int height;
	int modenum;
	int dib;
	int fullscreen;
	int bpp;
	int halfscreen;
	char modedesc[17];
} vmode_t;

typedef struct
{
	int width;
	int height;
} lmode_t;

lmode_t lowresmodes[] = {
	{ 320, 200 },
	{ 320, 240 },
	{ 400, 300 },
	{ 512, 384 },
};

const char *gl_vendor;
const char *gl_renderer;
const char *gl_version;
const char *gl_extensions;

qboolean DDActive;
qboolean scr_skipupdate;

static vmode_t modelist[MAX_MODE_LIST];
static int nummodes;
static vmode_t *pcurrentmode;
static vmode_t badmode;

static DEVMODE gdevmode;
static qboolean vid_initialized = false;
static qboolean windowed, leavecurrentmode;
static qboolean vid_canalttab = false;
static qboolean vid_wassuspended = false;
static int windowed_mouse;
extern qboolean mouseactive; // from in_win.c
static HICON hIcon;

HINSTANCE global_hInstance; // TODO: temp

int DIBWidth, DIBHeight;
RECT WindowRect;
DWORD WindowStyle, ExWindowStyle;

HWND mainwindow;

int vid_modenum = NO_MODE;
int vid_realmode;
int vid_default = MODE_WINDOWED;
static int windowed_default;
unsigned char vid_curpal[256 * 3];
static qboolean fullsbardraw = false;

static float vid_gamma = 1.0;

HGLRC baseRC;
HDC maindc;

glvert_t glv;

cvar_t gl_ztrick = { "gl_ztrick", "1" };

viddef_t vid; // global video state

unsigned short d_8to16table[256];
unsigned d_8to24table[256];
unsigned char d_15to8table[65536];

float gldepthmin, gldepthmax;

modestate_t modestate = MS_UNINIT;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AppActivate(BOOL fActive, BOOL minimize);
const char *VID_GetModeDescription(int mode);
void ClearAllStates();
void VID_UpdateWindowStatus();
void GL_Init();

PROC glArrayElementEXT;
PROC glColorPointerEXT;
PROC glTexCoordPointerEXT;
PROC glVertexPointerEXT;

typedef void(APIENTRY *lp3DFXFUNC)(int, int, int, int, int, const void *);
lp3DFXFUNC glColorTableEXT;
qboolean is8bit = false;
qboolean isPermedia = false;
qboolean gl_mtexable = false;

//====================================

/*
cvar_t _windowed_mouse = { "_windowed_mouse", "1", true };
*/

int window_center_x, window_center_y, window_x, window_y, window_width, window_height;
RECT window_rect;

glwstate_t glw_state; // TODO: glw_imp.c

// direct draw software compatability stuff

void VID_HandlePause(qboolean pause)
{
}

void VID_ForceLockState(int lk)
{
}

void VID_LockBuffer()
{
}

void VID_UnlockBuffer()
{
}

int VID_ForceUnlockedAndReturnState()
{
	return 0;
}

void D_BeginDirectRect(int x, int y, byte *pbitmap, int width, int height)
{
}

void D_EndDirectRect(int x, int y, int width, int height)
{
}

qboolean VID_SetWindowedMode(int modenum)
{
	HDC hdc;
	int lastmodestate, width, height;

	lastmodestate = modestate;

	WindowRect.top = WindowRect.left = 0;

	WindowRect.right = modelist[modenum].width;
	WindowRect.bottom = modelist[modenum].height;

	DIBWidth = modelist[modenum].width;
	DIBHeight = modelist[modenum].height;

	WindowStyle = WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	ExWindowStyle = 0;

	//rect = WindowRect;

	modestate = MS_WINDOWED;

	// because we have set the background brush for the window to NULL
	// (to avoid flickering when re-sizing the window on the desktop),
	// we clear the window to black when created, otherwise it will be
	// empty while Quake starts up.
	hdc = GetDC(mainwindow);
	PatBlt(hdc, 0, 0, WindowRect.right, WindowRect.bottom, BLACKNESS);
	ReleaseDC(mainwindow, hdc);

	if(vid.conheight > modelist[modenum].height)
		vid.conheight = modelist[modenum].height;
	if(vid.conwidth > modelist[modenum].width)
		vid.conwidth = modelist[modenum].width;
	vid.width = vid.conwidth;
	vid.height = vid.conheight;

	vid.numpages = 2;

	SendMessage(mainwindow, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
	SendMessage(mainwindow, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);

	return true;
}

qboolean VID_SetFullDIBMode(int modenum)
{
	HDC hdc;
	int lastmodestate, width, height;
	RECT rect;

	if(!leavecurrentmode)
	{
		gdevmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		gdevmode.dmBitsPerPel = modelist[modenum].bpp;
		gdevmode.dmPelsWidth = modelist[modenum].width << modelist[modenum].halfscreen;
		gdevmode.dmPelsHeight = modelist[modenum].height;
		gdevmode.dmSize = sizeof(gdevmode);

		if(ChangeDisplaySettings(&gdevmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			Sys_Error("Couldn't set fullscreen DIB mode");
	}

	lastmodestate = modestate;
	modestate = MS_FULLDIB;

	WindowRect.top = WindowRect.left = 0;

	WindowRect.right = modelist[modenum].width;
	WindowRect.bottom = modelist[modenum].height;

	DIBWidth = modelist[modenum].width;
	DIBHeight = modelist[modenum].height;

	WindowStyle = WS_POPUP;
	ExWindowStyle = 0;

	rect = WindowRect;

	// Because we have set the background brush for the window to NULL
	// (to avoid flickering when re-sizing the window on the desktop), we
	// clear the window to black when created, otherwise it will be
	// empty while Quake starts up.
	hdc = GetDC(mainwindow);
	PatBlt(hdc, 0, 0, WindowRect.right, WindowRect.bottom, BLACKNESS);
	ReleaseDC(mainwindow, hdc);

	if(vid.conheight > modelist[modenum].height)
		vid.conheight = modelist[modenum].height;
	if(vid.conwidth > modelist[modenum].width)
		vid.conwidth = modelist[modenum].width;
	vid.width = vid.conwidth;
	vid.height = vid.conheight;

	vid.numpages = 2;

	// needed because we're not getting WM_MOVE messages fullscreen on NT
	window_x = 0;
	window_y = 0;

	SendMessage(mainwindow, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
	SendMessage(mainwindow, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);

	return true;
}

int VID_SetMode(int modenum, unsigned short *palette)
{
	int original_mode, temp;
	qboolean stat;
	MSG msg;
	HDC hdc;

	if((windowed && (modenum != 0)) ||
	   (!windowed && (modenum < 1)) ||
	   (!windowed && (modenum >= nummodes)))
	{
		Sys_Error("Bad video mode\n");
	}

	// so Con_Printfs don't mess us up by forcing vid and snd updates
	temp = scr_disabled_for_loading;
	scr_disabled_for_loading = true;

	CDAudio_Pause();

	if(vid_modenum == NO_MODE)
		original_mode = windowed_default;
	else
		original_mode = vid_modenum;

	// Set either the fullscreen or windowed mode
	if(modelist[modenum].type == MS_WINDOWED)
	{
		// TODO
		/*
		if(_windowed_mouse.value && key_dest == key_game)
		{
			stat = VID_SetWindowedMode(modenum);
			IN_ActivateMouse();
			IN_HideMouse();
		}
		else
		*/
		{
			IN_DeactivateMouse();
			IN_ShowMouse();
			stat = VID_SetWindowedMode(modenum);
		}
	}
	else if(modelist[modenum].type == MS_FULLDIB)
	{
		stat = VID_SetFullDIBMode(modenum);
		IN_ActivateMouse();
		IN_HideMouse();
	}
	else
	{
		Sys_Error("VID_SetMode: Bad mode type in modelist");
	}

	window_width = DIBWidth;
	window_height = DIBHeight;
	VID_UpdateWindowStatus();

	CDAudio_Resume();
	scr_disabled_for_loading = temp;

	if(!stat)
		Sys_Error("Couldn't set video mode");

	// now we try to make sure we get the focus on the mode switch, because
	// sometimes in some systems we don't.  We grab the foreground, then
	// finish setting up, pump all our messages, and sleep for a little while
	// to let messages finish bouncing around the system, then we put
	// ourselves at the top of the z order, then grab the foreground again,
	// Who knows if it helps, but it probably doesn't hurt
	SetForegroundWindow(mainwindow);
	VID_SetPalette(palette);
	vid_modenum = modenum;
	//Cvar_SetValue("vid_mode", (float)vid_modenum);

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Sleep(100);

	SetWindowPos(mainwindow, HWND_TOP, 0, 0, 0, 0,
	             SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
	             SWP_NOCOPYBITS);

	SetForegroundWindow(mainwindow);

	// fix the leftover Alt from any Alt-Tab or the like that switched us away
	ClearAllStates();

	if(!msg_suppress_1)
		Con_SafePrintf("Video mode %s initialized.\n", VID_GetModeDescription(vid_modenum));

	VID_SetPalette(palette);

	vid.recalc_refdef = 1;

	return true;
}

/*
================
VID_UpdateWindowStatus
================
*/
void VID_UpdateWindowStatus()
{
	window_rect.left = window_x;
	window_rect.top = window_y;
	window_rect.right = window_x + window_width;
	window_rect.bottom = window_y + window_height;
	window_center_x = (window_rect.left + window_rect.right) / 2;
	window_center_y = (window_rect.top + window_rect.bottom) / 2;

	IN_UpdateClipCursor();
}

//====================================

BINDTEXFUNCPTR bindTexFunc;

#define TEXTURE_EXT_STRING "GL_EXT_texture_object"

void CheckTextureExtensions()
{
	char *tmp;
	qboolean texture_ext;
	HINSTANCE hInstGL;

	texture_ext = false;
	/* check for texture extension */
	tmp = (unsigned char *)qglGetString(GL_EXTENSIONS);
	while(*tmp)
	{
		if(strncmp((const char *)tmp, TEXTURE_EXT_STRING, strlen(TEXTURE_EXT_STRING)) == 0)
			texture_ext = true;
		tmp++;
	}

	if(!texture_ext || COM_CheckParm("-gl11"))
	{
		hInstGL = LoadLibrary("opengl32.dll");

		if(hInstGL == NULL)
			Sys_Error("Couldn't load opengl32.dll\n");

		bindTexFunc = (void *)GetProcAddress(hInstGL, "glBindTexture");

		if(!bindTexFunc)
			Sys_Error("No texture objects!");
		return;
	}

	/* load library and get procedure adresses for texture extension API */
	if((bindTexFunc = (BINDTEXFUNCPTR)
	    qwglGetProcAddress((LPCSTR) "glBindTextureEXT")) == NULL)
	{
		Sys_Error("GetProcAddress for BindTextureEXT failed");
		return;
	}
}

void CheckArrayExtensions()
{
	char *tmp;

	/* check for texture extension */
	tmp = (unsigned char *)qglGetString(GL_EXTENSIONS);
	while(*tmp)
	{
		if(strncmp((const char *)tmp, "GL_EXT_vertex_array", strlen("GL_EXT_vertex_array")) == 0)
		{
			if(
			((glArrayElementEXT = qwglGetProcAddress("glArrayElementEXT")) == NULL) ||
			((glColorPointerEXT = qwglGetProcAddress("glColorPointerEXT")) == NULL) ||
			((glTexCoordPointerEXT = qwglGetProcAddress("glTexCoordPointerEXT")) == NULL) ||
			((glVertexPointerEXT = qwglGetProcAddress("glVertexPointerEXT")) == NULL))
			{
				Sys_Error("GetProcAddress for vertex extension failed");
				return;
			}
			return;
		}
		tmp++;
	}

	Sys_Error("Vertex array extension not present");
}

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int texture_mode = GL_LINEAR;
//int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int texture_extension_number = 1;

#ifdef _WIN32
void CheckMultiTextureExtensions()
{
	if(strstr(gl_extensions, "GL_SGIS_multitexture ") && !COM_CheckParm("-nomtex"))
	{
		Con_Printf("Multitexture extensions found.\n");
		qglMTexCoord2fSGIS = (void *)qwglGetProcAddress("glMTexCoord2fSGIS");
		qglSelectTextureSGIS = (void *)qwglGetProcAddress("glSelectTextureSGIS");
		gl_mtexable = true;
	}
}
#else
void CheckMultiTextureExtensions()
{
	gl_mtexable = true;
}
#endif

/*
===============
GL_Init
===============
*/
void GL_Init()
{
	gl_vendor = qglGetString(GL_VENDOR);
	Con_Printf("GL_VENDOR: %s\n", gl_vendor);
	gl_renderer = qglGetString(GL_RENDERER);
	Con_Printf("GL_RENDERER: %s\n", gl_renderer);

	gl_version = qglGetString(GL_VERSION);
	Con_Printf("GL_VERSION: %s\n", gl_version);
	gl_extensions = qglGetString(GL_EXTENSIONS);
	//Con_Printf("GL_EXTENSIONS: %s\n", gl_extensions); // TODO: buffer overflow!?

	//	Con_Printf ("%s %s\n", gl_renderer, gl_version);

	if(strnicmp(gl_renderer, "PowerVR", 7) == 0)
		fullsbardraw = true;

	if(strnicmp(gl_renderer, "Permedia", 8) == 0)
		isPermedia = true;

	CheckTextureExtensions();
	CheckMultiTextureExtensions();

	qglClearColor(1, 0, 0, 0);
	qglCullFace(GL_FRONT);
	qglEnable(GL_TEXTURE_2D);

	qglEnable(GL_ALPHA_TEST);
	qglAlphaFunc(GL_GREATER, 0.666);

	qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	qglShadeModel(GL_FLAT);

	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

#if 0
	CheckArrayExtensions ();

	qglEnable (GL_VERTEX_ARRAY_EXT);
	qglEnable (GL_TEXTURE_COORD_ARRAY_EXT);
	qglVertexPointerEXT (3, GL_FLOAT, 0, 0, &glv.x);
	qglTexCoordPointerEXT (2, GL_FLOAT, 0, 0, &glv.s);
	qglColorPointerEXT (3, GL_FLOAT, 0, 0, &glv.r);
#endif
}

/*
=================
GL_BeginRendering

=================
*/
void GL_BeginRendering(int *x, int *y, int *width, int *height)
{
	extern cvar_t gl_clear;

	*x = *y = 0;
	*width = WindowRect.right - WindowRect.left;
	*height = WindowRect.bottom - WindowRect.top;

	//    if (!wglMakeCurrent( maindc, baseRC ))
	//		Sys_Error ("wglMakeCurrent failed");

	//	glViewport (*x, *y, *width, *height);
}

void GL_EndRendering()
{
	if(!scr_skipupdate || block_drawing)
		SwapBuffers(maindc);

	// handle the mouse state when windowed if that's changed
	if(modestate == MS_WINDOWED)
	{
		if(false /* !_windowed_mouse.value*/)
		{
			if(windowed_mouse)
			{
				IN_DeactivateMouse();
				IN_ShowMouse();
				windowed_mouse = false;
			}
		}
		else
		{
			windowed_mouse = true;
			if(key_dest == key_game /*&& !mouseactive*/ && ActiveApp) // TODO
			{
				IN_ActivateMouse();
				IN_HideMouse();
			}
			else if(/*mouseactive &&*/ key_dest != key_game) // TODO
			{
				IN_DeactivateMouse();
				IN_ShowMouse();
			}
		}
	}
	if(fullsbardraw)
		Sbar_Changed();
}

void VID_SetPalette(unsigned short *palette)
{
	byte *pal;
	unsigned r, g, b;
	unsigned v;
	int r1, g1, b1;
	int j, k, l, m;
	unsigned short i;
	unsigned *table;
	FILE *f;
	char s[255];
	HWND hDlg, hProgress;
	float gamma;

	//
	// 8 8 8 encoding
	//
	pal = palette;
	table = d_8to24table;
	for(i = 0; i < 256; i++)
	{
		r = pal[0];
		g = pal[1];
		b = pal[2];
		pal += 3;

		//		v = (255<<24) + (r<<16) + (g<<8) + (b<<0);
		//		v = (255<<0) + (r<<8) + (g<<16) + (b<<24);
		v = (255 << 24) + (r << 0) + (g << 8) + (b << 16);
		*table++ = v;
	}
	d_8to24table[255] &= 0xffffff; // 255 is transparent

	// JACK: 3D distance calcs - k is last closest, l is the distance.
	// FIXME: Precalculate this and cache to disk.
	for(i = 0; i < (1 << 15); i++)
	{
		/* Maps
			000000000000000
			000000000011111 = Red  = 0x1F
			000001111100000 = Blue = 0x03E0
			111110000000000 = Grn  = 0x7C00
		*/
		r = ((i & 0x1F) << 3) + 4;
		g = ((i & 0x03E0) >> 2) + 4;
		b = ((i & 0x7C00) >> 7) + 4;
		pal = (unsigned char *)d_8to24table;
		for(v = 0, k = 0, l = 10000 * 10000; v < 256; v++, pal += 4)
		{
			r1 = r - pal[0];
			g1 = g - pal[1];
			b1 = b - pal[2];
			j = (r1 * r1) + (g1 * g1) + (b1 * b1);
			if(j < l)
			{
				k = v;
				l = j;
			}
		}
		d_15to8table[i] = k;
	}
}

BOOL gammaworks;

void VID_ShiftPalette(unsigned short *palette)
{
	extern byte ramps[3][256];

	//	VID_SetPalette (palette);

	//	gammaworks = SetDeviceGammaRamp (maindc, ramps);
}

void VID_SetDefaultMode()
{
	IN_DeactivateMouse();
}

void VID_Shutdown()
{
	HGLRC hRC;
	HDC hDC;

	if(vid_initialized)
	{
		vid_canalttab = false;
		hRC = qwglGetCurrentContext();
		hDC = qwglGetCurrentDC();

		qwglMakeCurrent(NULL, NULL);

		if(hRC)
			qwglDeleteContext(hRC);

		if(hDC && mainwindow)
			ReleaseDC(mainwindow, hDC);

		if(modestate == MS_FULLDIB)
			ChangeDisplaySettings(NULL, 0);

		if(maindc && mainwindow)
			ReleaseDC(mainwindow, maindc);

		AppActivate(false, false);
		
		QGL_Shutdown();
	}
}

//==========================================================================

BOOL bSetupPixelFormat(HDC hDC)
{
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
		1,                             // version number
		PFD_DRAW_TO_WINDOW             // support window
		| PFD_SUPPORT_OPENGL           // support OpenGL
		| PFD_DOUBLEBUFFER,            // double buffered
		PFD_TYPE_RGBA,                 // RGBA type
		24,                            // 24-bit color depth
		0, 0, 0, 0, 0, 0,  // color bits ignored
		0,              // no alpha buffer
		0,              // shift bit ignored
		0,              // no accumulation buffer
		0, 0, 0, 0,     // accum bits ignored
		32,             // 32-bit z-buffer
		0,              // no stencil buffer
		0,              // no auxiliary buffer
		PFD_MAIN_PLANE, // main layer
		0,              // reserved
		0, 0, 0         // layer masks ignored
	};
	int pixelformat;

	if((pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0)
	{
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
		return FALSE;
	}

	if(SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
	{
		MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
		return FALSE;
	}

	return TRUE;
}

byte scantokey[128] =
{
  //  0           1       2       3       4       5       6       7
  //  8           9       A       B       C       D       E       F
  0, 27, '1', '2', '3', '4', '5', '6',
  '7', '8', '9', '0', '-', '=', K_BACKSPACE, 9, // 0
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
  'o', 'p', '[', ']', 13, K_CTRL, 'a', 's', // 1
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', K_SHIFT, '\\', 'z', 'x', 'c', 'v', // 2
  'b', 'n', 'm', ',', '.', '/', K_SHIFT, '*',
  K_ALT, ' ', 0, K_F1, K_F2, K_F3, K_F4, K_F5, // 3
  K_F6, K_F7, K_F8, K_F9, K_F10, K_PAUSE, 0, K_HOME,
  K_UPARROW, K_PGUP, '-', K_LEFTARROW, '5', K_RIGHTARROW, '+', K_END, //4
  K_DOWNARROW, K_PGDN, K_INS, K_DEL, 0, 0, 0, K_F11,
  K_F12, 0, 0, 0, 0, 0, 0, 0, // 5
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, // 6
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0 // 7
};

byte shiftscantokey[128] =
{
  //  0           1       2       3       4       5       6       7
  //  8           9       A       B       C       D       E       F
  0, 27, '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', K_BACKSPACE, 9, // 0
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', 13, K_CTRL, 'A', 'S', // 1
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~', K_SHIFT, '|', 'Z', 'X', 'C', 'V', // 2
  'B', 'N', 'M', '<', '>', '?', K_SHIFT, '*',
  K_ALT, ' ', 0, K_F1, K_F2, K_F3, K_F4, K_F5, // 3
  K_F6, K_F7, K_F8, K_F9, K_F10, K_PAUSE, 0, K_HOME,
  K_UPARROW, K_PGUP, '_', K_LEFTARROW, '%', K_RIGHTARROW, '+', K_END, //4
  K_DOWNARROW, K_PGDN, K_INS, K_DEL, 0, 0, 0, K_F11,
  K_F12, 0, 0, 0, 0, 0, 0, 0, // 5
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, // 6
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0 // 7
};

/*
=======
MapKey

Map from windows to quake keynums
=======
*/
int MapKey(int key)
{
	key = (key >> 16) & 255;
	if(key > 127)
		return 0;
	if(scantokey[key] == 0)
		Con_DPrintf("key 0x%02x has no translation\n", key);
	return scantokey[key];
}

/*
===================================================================

MAIN WINDOW

===================================================================
*/

/*
================
ClearAllStates
================
*/
void ClearAllStates()
{
	int i;

	// send an up event for each key, to make sure the server clears them all
	for(i = 0; i < 256; i++)
	{
		Key_Event(i, false);
	}

	Key_ClearStates();
	IN_ClearStates();
}

void AppActivate(BOOL fActive, BOOL minimize)
/****************************************************************************
*
* Function:     AppActivate
* Parameters:   fActive - True if app is activating
*
* Description:  If the application is activating, then swap the system
*               into SYSPAL_NOSTATIC mode so that our palettes will display
*               correctly.
*
****************************************************************************/
{
	MSG msg;
	HDC hdc;
	int i, t;
	static BOOL sound_active;

	ActiveApp = fActive;
	Minimized = minimize;

	// enable/disable sound on focus gain/loss
	if(!ActiveApp && sound_active)
	{
		S_BlockSound();
		sound_active = false;
	}
	else if(ActiveApp && !sound_active)
	{
		S_UnblockSound();
		sound_active = true;
	}

	if(fActive)
	{
		if(modestate == MS_FULLDIB)
		{
			IN_ActivateMouse();
			IN_HideMouse();
			if(vid_canalttab && vid_wassuspended)
			{
				vid_wassuspended = false;
				ChangeDisplaySettings(&gdevmode, CDS_FULLSCREEN);
				ShowWindow(mainwindow, SW_SHOWNORMAL);
			}
		}
		else if((modestate == MS_WINDOWED) /*&& _windowed_mouse.value*/ && key_dest == key_game)
		{
			IN_ActivateMouse();
			IN_HideMouse();
		}
	}

	if(!fActive)
	{
		if(modestate == MS_FULLDIB)
		{
			IN_DeactivateMouse();
			IN_ShowMouse();
			if(vid_canalttab)
			{
				ChangeDisplaySettings(NULL, 0);
				vid_wassuspended = true;
			}
		}
		else if((modestate == MS_WINDOWED) /*&& _windowed_mouse.value*/)
		{
			IN_DeactivateMouse();
			IN_ShowMouse();
		}
	}
}

// main window procedure
LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lRet = 1;
	int fwKeys, xPos, yPos, fActive, fMinimized, temp;
	extern unsigned int uiWheelMessage;

	if(uMsg == uiWheelMessage)
		uMsg = WM_MOUSEWHEEL;

#ifdef OGS_USE_IMGUI
	if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
#endif
	
	switch(uMsg)
	{
	case WM_KILLFOCUS:
		if(modestate == MS_FULLDIB)
			ShowWindow(mainwindow, SW_SHOWMINNOACTIVE);
		break;

	case WM_CREATE:
		break;

	case WM_MOVE:
		window_x = (int)LOWORD(lParam);
		window_y = (int)HIWORD(lParam);
		VID_UpdateWindowStatus();
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		Key_Event(MapKey(lParam), true);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		Key_Event(MapKey(lParam), false);
		break;

	case WM_SYSCHAR:
		// keep Alt-Space from happening
		break;

	// this is complicated because Win32 seems to pack multiple mouse events into
	// one update sometimes, so we always check all states and look for events
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		temp = 0;

		if(wParam & MK_LBUTTON)
			temp |= 1;

		if(wParam & MK_RBUTTON)
			temp |= 2;

		if(wParam & MK_MBUTTON)
			temp |= 4;

		IN_MouseEvent(temp);

		break;

	// JACK: This is the mouse wheel with the Intellimouse
	// Its delta is either positive or neg, and we generate the proper
	// Event.
	case WM_MOUSEWHEEL:
		if((short)HIWORD(wParam) > 0)
		{
			Key_Event(K_MWHEELUP, true);
			Key_Event(K_MWHEELUP, false);
		}
		else
		{
			Key_Event(K_MWHEELDOWN, true);
			Key_Event(K_MWHEELDOWN, false);
		}
		break;

	case WM_SIZE:
		break;

	case WM_CLOSE:
		if(MessageBox(mainwindow, "Are you sure you want to quit?", "Confirm Exit",
		              MB_YESNO | MB_SETFOREGROUND | MB_ICONQUESTION) == IDYES)
		{
			Sys_Quit();
		}

		break;

	case WM_ACTIVATE:
		fActive = LOWORD(wParam);
		fMinimized = (BOOL)HIWORD(wParam);
		AppActivate(!(fActive == WA_INACTIVE), fMinimized);

		// fix the leftover Alt from any Alt-Tab or the like that switched us away
		ClearAllStates();

		break;

	case WM_DESTROY:
	{
		if(mainwindow)
			DestroyWindow(mainwindow);

		PostQuitMessage(0);
	}
	break;

	case MM_MCINOTIFY:
		lRet = CDAudio_MessageHandler(hWnd, uMsg, wParam, lParam);
		break;

	default:
		/* pass all unhandled messages to DefWindowProc */
		lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	/* return 1 if handled message, 0 if not */
	return lRet;
}

/*
=================
VID_NumModes
=================
*/
int VID_NumModes()
{
	return nummodes;
}

/*
=================
VID_GetModePtr
=================
*/
vmode_t *VID_GetModePtr(int modenum)
{
	if((modenum >= 0) && (modenum < nummodes))
		return &modelist[modenum];
	else
		return &badmode;
}

/*
=================
VID_GetModeDescription
=================
*/
const char *VID_GetModeDescription(int mode)
{
	const char *pinfo;
	vmode_t *pv;
	static char temp[100];

	if((mode < 0) || (mode >= nummodes))
		return "";

	if(!leavecurrentmode)
	{
		pv = VID_GetModePtr(mode);
		pinfo = pv->modedesc;
	}
	else
	{
		sprintf(temp, "Desktop resolution (%dx%d)",
		        modelist[MODE_FULLSCREEN_DEFAULT].width,
		        modelist[MODE_FULLSCREEN_DEFAULT].height);
		pinfo = temp;
	}

	return pinfo;
}

// KJB: Added this to return the mode driver name in description for console

const char *VID_GetExtModeDescription(int mode)
{
	static char pinfo[40];
	vmode_t *pv;

	if((mode < 0) || (mode >= nummodes))
		return NULL;

	pv = VID_GetModePtr(mode);
	if(modelist[mode].type == MS_FULLDIB)
	{
		if(!leavecurrentmode)
		{
			sprintf(pinfo, "%s fullscreen", pv->modedesc);
		}
		else
		{
			sprintf(pinfo, "Desktop resolution (%dx%d)",
			        modelist[MODE_FULLSCREEN_DEFAULT].width,
			        modelist[MODE_FULLSCREEN_DEFAULT].height);
		}
	}
	else
	{
		if(modestate == MS_WINDOWED)
			sprintf(pinfo, "%s windowed", pv->modedesc);
		else
			sprintf(pinfo, "windowed");
	}

	return pinfo;
}

/*
=================
VID_DescribeCurrentMode_f
=================
*/
/*
void VID_DescribeCurrentMode_f()
{
	Con_Printf("%s\n", VID_GetExtModeDescription(vid_modenum));
}
*/

/*
=================
VID_NumModes_f
=================
*/
/*
void VID_NumModes_f()
{
	if(nummodes == 1)
		Con_Printf("%d video mode is available\n", nummodes);
	else
		Con_Printf("%d video modes are available\n", nummodes);
}
*/

/*
=================
VID_DescribeMode_f
=================
*/
/*
void VID_DescribeMode_f()
{
	int t, modenum;

	modenum = Q_atoi(Cmd_Argv(1));

	t = leavecurrentmode;
	leavecurrentmode = 0;

	Con_Printf("%s\n", VID_GetExtModeDescription(modenum));

	leavecurrentmode = t;
}
*/

/*
=================
VID_DescribeModes_f
=================
*/
/*
void VID_DescribeModes_f()
{
	int i, lnummodes, t;
	char *pinfo;
	vmode_t *pv;

	lnummodes = VID_NumModes();

	t = leavecurrentmode;
	leavecurrentmode = 0;

	for(i = 1; i < lnummodes; i++)
	{
		pv = VID_GetModePtr(i);
		pinfo = VID_GetExtModeDescription(i);
		Con_Printf("%2d: %s\n", i, pinfo);
	}

	leavecurrentmode = t;
}
*/

void VID_InitDIB(HINSTANCE hInstance)
{
	HDC hdc;
	int i;

	modelist[0].type = MS_WINDOWED;

	if(COM_CheckParm("-width"))
		modelist[0].width = Q_atoi(com_argv[COM_CheckParm("-width") + 1]);
	else
		modelist[0].width = 640;

	if(modelist[0].width < 320)
		modelist[0].width = 320;

	if(COM_CheckParm("-height"))
		modelist[0].height = Q_atoi(com_argv[COM_CheckParm("-height") + 1]);
	else
		modelist[0].height = modelist[0].width * 240 / 320;

	if(modelist[0].height < 240)
		modelist[0].height = 240;

	sprintf(modelist[0].modedesc, "%dx%d",
	        modelist[0].width, modelist[0].height);

	modelist[0].modenum = MODE_WINDOWED;
	modelist[0].dib = 1;
	modelist[0].fullscreen = 0;
	modelist[0].halfscreen = 0;
	modelist[0].bpp = 0;

	nummodes = 1;
}

/*
=================
VID_InitFullDIB
=================
*/
void VID_InitFullDIB(HINSTANCE hInstance)
{
	DEVMODE devmode;
	int i, modenum, cmodes, originalnummodes, existingmode, numlowresmodes;
	int j, bpp, done;
	BOOL stat;

	// enumerate >8 bpp modes
	originalnummodes = nummodes;
	modenum = 0;

	do
	{
		stat = EnumDisplaySettings(NULL, modenum, &devmode);

		if((devmode.dmBitsPerPel >= 15) &&
		   (devmode.dmPelsWidth <= MAXWIDTH) &&
		   (devmode.dmPelsHeight <= MAXHEIGHT) &&
		   (nummodes < MAX_MODE_LIST))
		{
			devmode.dmFields = DM_BITSPERPEL |
			DM_PELSWIDTH |
			DM_PELSHEIGHT;

			if(ChangeDisplaySettings(&devmode, CDS_TEST | CDS_FULLSCREEN) ==
			   DISP_CHANGE_SUCCESSFUL)
			{
				modelist[nummodes].type = MS_FULLDIB;
				modelist[nummodes].width = devmode.dmPelsWidth;
				modelist[nummodes].height = devmode.dmPelsHeight;
				modelist[nummodes].modenum = 0;
				modelist[nummodes].halfscreen = 0;
				modelist[nummodes].dib = 1;
				modelist[nummodes].fullscreen = 1;
				modelist[nummodes].bpp = devmode.dmBitsPerPel;
				sprintf(modelist[nummodes].modedesc, "%dx%dx%d",
				        devmode.dmPelsWidth, devmode.dmPelsHeight,
				        devmode.dmBitsPerPel);

				// if the width is more than twice the height, reduce it by half because this
				// is probably a dual-screen monitor
				if(!COM_CheckParm("-noadjustaspect"))
				{
					if(modelist[nummodes].width > (modelist[nummodes].height << 1))
					{
						modelist[nummodes].width >>= 1;
						modelist[nummodes].halfscreen = 1;
						sprintf(modelist[nummodes].modedesc, "%dx%dx%d",
						        modelist[nummodes].width,
						        modelist[nummodes].height,
						        modelist[nummodes].bpp);
					}
				}

				for(i = originalnummodes, existingmode = 0; i < nummodes; i++)
				{
					if((modelist[nummodes].width == modelist[i].width) &&
					   (modelist[nummodes].height == modelist[i].height) &&
					   (modelist[nummodes].bpp == modelist[i].bpp))
					{
						existingmode = 1;
						break;
					}
				}

				if(!existingmode)
				{
					nummodes++;
				}
			}
		}

		modenum++;
	} while(stat);

	// see if there are any low-res modes that aren't being reported
	numlowresmodes = sizeof(lowresmodes) / sizeof(lowresmodes[0]);
	bpp = 16;
	done = 0;

	do
	{
		for(j = 0; (j < numlowresmodes) && (nummodes < MAX_MODE_LIST); j++)
		{
			devmode.dmBitsPerPel = bpp;
			devmode.dmPelsWidth = lowresmodes[j].width;
			devmode.dmPelsHeight = lowresmodes[j].height;
			devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if(ChangeDisplaySettings(&devmode, CDS_TEST | CDS_FULLSCREEN) ==
			   DISP_CHANGE_SUCCESSFUL)
			{
				modelist[nummodes].type = MS_FULLDIB;
				modelist[nummodes].width = devmode.dmPelsWidth;
				modelist[nummodes].height = devmode.dmPelsHeight;
				modelist[nummodes].modenum = 0;
				modelist[nummodes].halfscreen = 0;
				modelist[nummodes].dib = 1;
				modelist[nummodes].fullscreen = 1;
				modelist[nummodes].bpp = devmode.dmBitsPerPel;
				sprintf(modelist[nummodes].modedesc, "%dx%dx%d",
				        devmode.dmPelsWidth, devmode.dmPelsHeight,
				        devmode.dmBitsPerPel);

				for(i = originalnummodes, existingmode = 0; i < nummodes; i++)
				{
					if((modelist[nummodes].width == modelist[i].width) &&
					   (modelist[nummodes].height == modelist[i].height) &&
					   (modelist[nummodes].bpp == modelist[i].bpp))
					{
						existingmode = 1;
						break;
					}
				}

				if(!existingmode)
				{
					nummodes++;
				}
			}
		}
		switch(bpp)
		{
		case 16:
			bpp = 32;
			break;

		case 32:
			bpp = 24;
			break;

		case 24:
			done = 1;
			break;
		}
	} while(!done);

	if(nummodes == originalnummodes)
		Con_SafePrintf("No fullscreen DIB modes found\n");
}

qboolean VID_Is8bit()
{
	return is8bit;
}

#define GL_SHARED_TEXTURE_PALETTE_EXT 0x81FB

// TODO: unused in gs
/*
void VID_Init8bitPalette()
{
	// Check for 8bit Extensions and initialize them.
	int i;
	char thePalette[256 * 3];
	char *oldPalette, *newPalette;

	glColorTableEXT = (void *)qwglGetProcAddress("glColorTableEXT");
	if(!glColorTableEXT || strstr(gl_extensions, "GL_EXT_shared_texture_palette") ||
	   COM_CheckParm("-no8bit"))
		return;

	Con_SafePrintf("8-bit GL extensions enabled.\n");
	qglEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
	oldPalette = (char *)d_8to24table; //d_8to24table3dfx;
	newPalette = thePalette;
	for(i = 0; i < 256; i++)
	{
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		oldPalette++;
	}
	qglColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE,
	                (void *)thePalette);
	is8bit = TRUE;
}
*/

void VID_CheckPaletteExtensions()
{
	// TODO
	if(strstr(gl_extensions, "GL_EXT_paletted_texture") || strstr(gl_extensions, "GL_EXT_shared_texture_palette") || strstr(gl_extensions, "glColorTableEXT"))
		Con_SafePrintf("Found paletted texture extension.\n");
};

static void Check_Gamma(unsigned short *pal)
{
	float f, inf;
	unsigned char palette[768];
	int i;

	if((i = COM_CheckParm("-gamma")) == 0)
	{
		if((gl_renderer && strstr(gl_renderer, "Voodoo")) ||
		   (gl_vendor && strstr(gl_vendor, "3Dfx")))
			vid_gamma = 1;
		else
			vid_gamma = 0.7; // default to 0.7 on non-3dfx hardware
	}
	else
		vid_gamma = Q_atof(com_argv[i + 1]);

	for(i = 0; i < 768; i++)
	{
		f = pow((pal[i] + 1) / 256.0, vid_gamma);
		inf = f * 255 + 0.5;
		if(inf < 0)
			inf = 0;
		if(inf > 255)
			inf = 255;
		palette[i] = inf;
	}

	Q_memcpy(pal, palette, sizeof(palette));
}

/*
==================
GL_CheckErrors
==================
*/
void GL_CheckErrors( void )
{
    int		err;
    char	s[64];

    err = qglGetError();
    if ( err == GL_NO_ERROR )
        return;

    //if ( r_ignoreGLErrors->integer )
        //return;

    switch( err )
	{
        case GL_INVALID_ENUM:
            Q_strcpy( s, "GL_INVALID_ENUM" );
            break;
        case GL_INVALID_VALUE:
            Q_strcpy( s, "GL_INVALID_VALUE" );
            break;
        case GL_INVALID_OPERATION:
            Q_strcpy( s, "GL_INVALID_OPERATION" );
            break;
        case GL_STACK_OVERFLOW:
            Q_strcpy( s, "GL_STACK_OVERFLOW" );
            break;
        case GL_STACK_UNDERFLOW:
            Q_strcpy( s, "GL_STACK_UNDERFLOW" );
            break;
        case GL_OUT_OF_MEMORY:
            Q_strcpy( s, "GL_OUT_OF_MEMORY" );
            break;
        default:
            snprintf( s, sizeof(s), "%i", err);
            break;
    }

    Sys_Error( /*ERR_FATAL,*/ "GL_CheckErrors: %s", s ); // TODO: was ri.Error
}

/*
===================
VID_Init
===================
*/
void VID_Init(unsigned short *palette)
{
	int i, existingmode;
	int basenummodes, width, height, bpp, findbpp, done;
	byte *ptmp;
	char gldir[MAX_OSPATH];
	HDC hdc;
	DEVMODE devmode;

	memset(&devmode, 0, sizeof(devmode));
	
	global_hInstance = GetModuleHandle(""); // TODO: temp

	//Cvar_RegisterVariable(&_windowed_mouse);
	Cvar_RegisterVariable(&gl_ztrick);

	//Cmd_AddCommand("vid_nummodes", VID_NumModes_f);
	//Cmd_AddCommand("vid_describecurrentmode", VID_DescribeCurrentMode_f);
	//Cmd_AddCommand("vid_describemode", VID_DescribeMode_f);
	//Cmd_AddCommand("vid_describemodes", VID_DescribeModes_f);

	//hIcon = LoadIcon(global_hInstance, MAKEINTRESOURCE(IDI_ICON2)); // TODO

	//InitCommonControls(); // TODO

	VID_InitDIB(global_hInstance);
	basenummodes = nummodes = 1;

	VID_InitFullDIB(global_hInstance);

	if(COM_CheckParm("-window"))
	{
		hdc = GetDC(NULL);

		if(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE)
		{
			Sys_Error("Can't run in non-RGB mode");
		}

		ReleaseDC(NULL, hdc);

		windowed = true;

		vid_default = MODE_WINDOWED;
	}
	else
	{
		if(nummodes == 1)
			Sys_Error("No RGB fullscreen modes available");

		windowed = false;

		if(COM_CheckParm("-mode"))
		{
			vid_default = Q_atoi(com_argv[COM_CheckParm("-mode") + 1]);
		}
		else
		{
			if(COM_CheckParm("-current"))
			{
				modelist[MODE_FULLSCREEN_DEFAULT].width =
				GetSystemMetrics(SM_CXSCREEN);
				modelist[MODE_FULLSCREEN_DEFAULT].height =
				GetSystemMetrics(SM_CYSCREEN);
				vid_default = MODE_FULLSCREEN_DEFAULT;
				leavecurrentmode = 1;
			}
			else
			{
				if(COM_CheckParm("-width"))
				{
					width = Q_atoi(com_argv[COM_CheckParm("-width") + 1]);
				}
				else
				{
					width = 640;
				}

				if(COM_CheckParm("-bpp"))
				{
					bpp = Q_atoi(com_argv[COM_CheckParm("-bpp") + 1]);
					findbpp = 0;
				}
				else
				{
					bpp = 15;
					findbpp = 1;
				}

				if(COM_CheckParm("-height"))
					height = Q_atoi(com_argv[COM_CheckParm("-height") + 1]);

				// if they want to force it, add the specified mode to the list
				if(COM_CheckParm("-force") && (nummodes < MAX_MODE_LIST))
				{
					modelist[nummodes].type = MS_FULLDIB;
					modelist[nummodes].width = width;
					modelist[nummodes].height = height;
					modelist[nummodes].modenum = 0;
					modelist[nummodes].halfscreen = 0;
					modelist[nummodes].dib = 1;
					modelist[nummodes].fullscreen = 1;
					modelist[nummodes].bpp = bpp;
					sprintf(modelist[nummodes].modedesc, "%dx%dx%d",
					        devmode.dmPelsWidth, devmode.dmPelsHeight,
					        devmode.dmBitsPerPel);

					for(i = nummodes, existingmode = 0; i < nummodes; i++)
					{
						if((modelist[nummodes].width == modelist[i].width) &&
						   (modelist[nummodes].height == modelist[i].height) &&
						   (modelist[nummodes].bpp == modelist[i].bpp))
						{
							existingmode = 1;
							break;
						}
					}

					if(!existingmode)
					{
						nummodes++;
					}
				}

				done = 0;

				do
				{
					if(COM_CheckParm("-height"))
					{
						height = Q_atoi(com_argv[COM_CheckParm("-height") + 1]);

						for(i = 1, vid_default = 0; i < nummodes; i++)
						{
							if((modelist[i].width == width) &&
							   (modelist[i].height == height) &&
							   (modelist[i].bpp == bpp))
							{
								vid_default = i;
								done = 1;
								break;
							}
						}
					}
					else
					{
						for(i = 1, vid_default = 0; i < nummodes; i++)
						{
							if((modelist[i].width == width) && (modelist[i].bpp == bpp))
							{
								vid_default = i;
								done = 1;
								break;
							}
						}
					}

					if(!done)
					{
						if(findbpp)
						{
							switch(bpp)
							{
							case 15:
								bpp = 16;
								break;
							case 16:
								bpp = 32;
								break;
							case 32:
								bpp = 24;
								break;
							case 24:
								done = 1;
								break;
							}
						}
						else
						{
							done = 1;
						}
					}
				} while(!done);

				if(!vid_default)
				{
					Sys_Error("Specified video mode not available");
				}
			}
		}
	}

	vid_initialized = true;

	if((i = COM_CheckParm("-conwidth")) != 0)
		vid.conwidth = Q_atoi(com_argv[i + 1]);
	else
		vid.conwidth = 640;

	vid.conwidth &= 0xfff8; // make it a multiple of eight

	if(vid.conwidth < 320)
		vid.conwidth = 320;

	// pick a conheight that matches with correct aspect
	vid.conheight = vid.conwidth * 3 / 4;

	if((i = COM_CheckParm("-conheight")) != 0)
		vid.conheight = Q_atoi(com_argv[i + 1]);
	if(vid.conheight < 200)
		vid.conheight = 200;

	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong(*((int *)vid.colormap + 2048));

	//DestroyWindow(hwnd_dialog); // TODO

	Check_Gamma(palette);
	VID_SetPalette(palette);

	VID_SetMode(vid_default, palette);

	maindc = GetDC(mainwindow);
	bSetupPixelFormat(maindc);

	if(!QGL_Init("opengl32"))
	{
		QGL_Shutdown();
		Sys_Error("QGL Initialization failed!");
	};
	
	//GL_CheckErrors(); // TODO: temp
	
	baseRC = qwglCreateContext(maindc);
	if(!baseRC)
		Sys_Error("Could not initialize GL (wglCreateContext failed).\n\nMake sure you in are 65535 color mode, and try running -window.");
	if(!qwglMakeCurrent(maindc, baseRC))
		Sys_Error("wglMakeCurrent failed");

	GL_Init();

	sprintf(gldir, "%s/glquake", com_gamedir);
	FS_mkdir(gldir);

	vid_realmode = vid_modenum;

	// Check for 3DFX Extensions and initialize them.
	//VID_Init8bitPalette();
	VID_CheckPaletteExtensions();

	strcpy(badmode.modedesc, "Bad mode");
	vid_canalttab = true;

	if(COM_CheckParm("-fullsbar"))
		fullsbardraw = true;
}