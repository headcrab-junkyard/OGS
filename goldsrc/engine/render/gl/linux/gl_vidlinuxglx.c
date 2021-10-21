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

#include <termios.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/vt.h>

#include <signal.h>
#include <dlfcn.h>

#include <GL/glx.h>

#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>

#include "quakedef.h"

#define WARP_WIDTH 320
#define WARP_HEIGHT 200

static Display *dpy = NULL;
static int scrnum;
static Window win;
static GLXContext ctx = NULL;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
                    PointerMotionMask | ButtonMotionMask)
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask)

unsigned short d_8to16table[256];
unsigned int d_8to24table[256];
unsigned char d_15to8table[65536];

cvar_t vid_mode = { "vid_mode", "0", false };

#ifdef GLX
qboolean vidmode_ext = false;

static int win_x, win_y;
#endif

static int scr_width, scr_height;

static XF86VidModeModeInfo **vidmodes;
static int default_dotclock_vidmode;
static int num_vidmodes;
static qboolean vidmode_active = false;

/*-----------------------------------------------------------------------*/

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int texture_mode = GL_LINEAR;
//int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int texture_extension_number = 1;

float gldepthmin, gldepthmax;

cvar_t gl_ztrick = { "gl_ztrick", "1" };

const char *gl_vendor;
const char *gl_renderer;
const char *gl_version;
const char *gl_extensions;

void (*qglColorTableEXT)(int, int, int, int, int, const void *);
void (*qgl3DfxSetPaletteEXT)(GLuint *);

static float vid_gamma = 1.0;

qboolean is8bit = false;
qboolean isPermedia = false;
qboolean gl_mtexable = false;

/*-----------------------------------------------------------------------*/
void D_BeginDirectRect(int x, int y, byte *pbitmap, int width, int height)
{
}

void D_EndDirectRect(int x, int y, int width, int height)
{
}

#ifdef X11
/*
=================
VID_Gamma_f

Keybinding command
=================
*/
void VID_Gamma_f (void)
{
	float	gamma, f, inf;
	unsigned char	palette[768];
	int		i;

	if (Cmd_Argc () == 2)
	{
		gamma = Q_atof (Cmd_Argv(1));

		for (i=0 ; i<768 ; i++)
		{
			f = pow ( (host_basepal[i]+1)/256.0 , gamma );
			inf = f*255 + 0.5;
			if (inf < 0)
				inf = 0;
			if (inf > 255)
				inf = 255;
			palette[i] = inf;
		}

		VID_SetPalette (palette);

		vid.recalc_refdef = 1;				// force a surface cache flush
	}
}
#endif

static int XLateKey(XKeyEvent *ev)
{
	int key;
	char buf[64];
	KeySym keysym;

	key = 0;

	XLookupString(ev, buf, sizeof(buf), &keysym, 0);

	switch(keysym)
	{
	case XK_KP_Page_Up:
	case XK_Page_Up:
		key = K_PGUP;
		break;

	case XK_KP_Page_Down:
	case XK_Page_Down:
		key = K_PGDN;
		break;

	case XK_KP_Home:
	case XK_Home:
		key = K_HOME;
		break;

	case XK_KP_End:
	case XK_End:
		key = K_END;
		break;

	case XK_KP_Left:
	case XK_Left:
		key = K_LEFTARROW;
		break;

	case XK_KP_Right:
	case XK_Right:
		key = K_RIGHTARROW;
		break;

	case XK_KP_Down:
	case XK_Down:
		key = K_DOWNARROW;
		break;

	case XK_KP_Up:
	case XK_Up:
		key = K_UPARROW;
		break;

	case XK_Escape:
		key = K_ESCAPE;
		break;

	case XK_KP_Enter:
	case XK_Return:
		key = K_ENTER;
		break;

	case XK_Tab:
		key = K_TAB;
		break;

	case XK_F1:
		key = K_F1;
		break;

	case XK_F2:
		key = K_F2;
		break;

	case XK_F3:
		key = K_F3;
		break;

	case XK_F4:
		key = K_F4;
		break;

	case XK_F5:
		key = K_F5;
		break;

	case XK_F6:
		key = K_F6;
		break;

	case XK_F7:
		key = K_F7;
		break;

	case XK_F8:
		key = K_F8;
		break;

	case XK_F9:
		key = K_F9;
		break;

	case XK_F10:
		key = K_F10;
		break;

	case XK_F11:
		key = K_F11;
		break;

	case XK_F12:
		key = K_F12;
		break;

	case XK_BackSpace:
		key = K_BACKSPACE;
		break;

	case XK_KP_Delete:
	case XK_Delete:
		key = K_DEL;
		break;

	case XK_Pause:
		key = K_PAUSE;
		break;

	case XK_Shift_L:
	case XK_Shift_R:
		key = K_SHIFT;
		break;

	case XK_Execute:
	case XK_Control_L:
	case XK_Control_R:
		key = K_CTRL;
		break;

	case XK_Alt_L:
	case XK_Meta_L:
	case XK_Alt_R:
	case XK_Meta_R:
		key = K_ALT;
		break;

	case XK_KP_Begin:
		key = '5';
		break;

	case XK_KP_Insert:
	case XK_Insert:
		key = K_INS;
		break;

	case XK_KP_Multiply:
		key = '*';
		break;
	case XK_KP_Add:
		key = '+';
		break;
	case XK_KP_Subtract:
		key = '-';
		break;
	case XK_KP_Divide:
		key = '/';
		break;

#if 0
		case 0x021: key = '1';break;/* [!] */
		case 0x040: key = '2';break;/* [@] */
		case 0x023: key = '3';break;/* [#] */
		case 0x024: key = '4';break;/* [$] */
		case 0x025: key = '5';break;/* [%] */
		case 0x05e: key = '6';break;/* [^] */
		case 0x026: key = '7';break;/* [&] */
		case 0x02a: key = '8';break;/* [*] */
		case 0x028: key = '9';;break;/* [(] */
		case 0x029: key = '0';break;/* [)] */
		case 0x05f: key = '-';break;/* [_] */
		case 0x02b: key = '=';break;/* [+] */
		case 0x07c: key = '\'';break;/* [|] */
		case 0x07d: key = '[';break;/* [}] */
		case 0x07b: key = ']';break;/* [{] */
		case 0x022: key = '\'';break;/* ["] */
		case 0x03a: key = ';';break;/* [:] */
		case 0x03f: key = '/';break;/* [?] */
		case 0x03e: key = '.';break;/* [>] */
		case 0x03c: key = ',';break;/* [<] */
#endif

	default:
		key = *(unsigned char *)buf;
		if(key >= 'A' && key <= 'Z')
			key = key - 'A' + 'a';
		//fprintf(stdout, "case 0x0%x: key = ___;break;/* [%c] */\n", keysym);
		break;
	}

	return key;
}

#ifdef GLX
static Cursor CreateNullCursor(Display *display, Window root)
{
	Pixmap cursormask;
	XGCValues xgc;
	GC gc;
	XColor dummycolour;
	Cursor cursor;

	cursormask = XCreatePixmap(display, root, 1, 1, 1 /*depth*/);
	xgc.function = GXclear;
	gc = XCreateGC(display, cursormask, GCFunction, &xgc);
	XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
	dummycolour.pixel = 0;
	dummycolour.red = 0;
	dummycolour.flags = 04;
	cursor = XCreatePixmapCursor(display, cursormask, cursormask,
	                             &dummycolour, &dummycolour, 0, 0);
	XFreePixmap(display, cursormask);
	XFreeGC(display, gc);
	return cursor;
}
#endif

#ifdef GLX
static void install_grabs()
{
	// inviso cursor
	XDefineCursor(dpy, win, CreateNullCursor(dpy, win));

	XGrabPointer(dpy, win,
	             True,
	             0,
	             GrabModeAsync, GrabModeAsync,
	             win,
	             None,
	             CurrentTime);

	if(in_dgamouse.value)
	{
		int MajorVersion, MinorVersion;

		if(!XF86DGAQueryVersion(dpy, &MajorVersion, &MinorVersion))
		{
			// unable to query, probalby not supported
			Con_Printf("Failed to detect XF86DGA Mouse\n");
			in_dgamouse.value = 0;
		}
		else
		{
			dgamouse = true;
			XF86DGADirectVideo(dpy, DefaultScreen(dpy), XF86DGADirectMouse);
			XWarpPointer(dpy, None, win, 0, 0, 0, 0, 0, 0);
		}
	}
	else
	{
		XWarpPointer(dpy, None, win,
		             0, 0, 0, 0,
		             vid.width / 2, vid.height / 2);
	}

	XGrabKeyboard(dpy, win,
	              False,
	              GrabModeAsync, GrabModeAsync,
	              CurrentTime);

	mouse_active = true;

	//	XSync(dpy, True);
}

static void uninstall_grabs()
{
	if(!dpy || !win)
		return;

	if(dgamouse)
	{
		dgamouse = false;
		XF86DGADirectVideo(dpy, DefaultScreen(dpy), 0);
	}

	XUngrabPointer(dpy, CurrentTime);
	XUngrabKeyboard(dpy, CurrentTime);

	// inviso cursor
	XUndefineCursor(dpy, win);

	mouse_active = false;
}

static void HandleEvents()
{
	XEvent event;
	KeySym ks;
	int b;
	qboolean dowarp = false;
	int mwx = vid.width / 2;
	int mwy = vid.height / 2;

	if(!dpy)
		return;

	while(XPending(dpy))
	{
		XNextEvent(dpy, &event);

		switch(event.type)
		{
		case KeyPress:
		case KeyRelease:
			Key_Event(XLateKey(&event.xkey), event.type == KeyPress);
			break;

		case MotionNotify:
			if(mouse_active)
			{
				if(dgamouse)
				{
					mx += (event.xmotion.x + win_x) * 2;
					my += (event.xmotion.y + win_y) * 2;
				}
				else
				{
					mx += ((int)event.xmotion.x - mwx) * 2;
					my += ((int)event.xmotion.y - mwy) * 2;
					mwx = event.xmotion.x;
					mwy = event.xmotion.y;

					if(mx || my)
						dowarp = true;
				}
			}
			break;

			break;

		case ButtonPress:
			b = -1;
			if(event.xbutton.button == 1)
				b = 0;
			else if(event.xbutton.button == 2)
				b = 2;
			else if(event.xbutton.button == 3)
				b = 1;
			if(b >= 0)
				Key_Event(K_MOUSE1 + b, true);
			break;

		case ButtonRelease:
			b = -1;
			if(event.xbutton.button == 1)
				b = 0;
			else if(event.xbutton.button == 2)
				b = 2;
			else if(event.xbutton.button == 3)
				b = 1;
			if(b >= 0)
				Key_Event(K_MOUSE1 + b, false);
			break;

		case CreateNotify:
			win_x = event.xcreatewindow.x;
			win_y = event.xcreatewindow.y;
			break;

		case ConfigureNotify:
			win_x = event.xconfigure.x;
			win_y = event.xconfigure.y;
			break;
		}
	}

	if(dowarp)
	{
		/* move the mouse to the window center again */
		XWarpPointer(dpy, None, win, 0, 0, 0, 0, vid.width / 2, vid.height / 2);
	}
}
#endif // GLX

void VID_Shutdown()
{
#ifdef GLX
	if(!ctx || !dpy)
		return;
	IN_DeactivateMouse();
	if(dpy)
	{
		if(ctx)
			qglXDestroyContext(dpy, ctx);
		if(win)
			XDestroyWindow(dpy, win);
		if(vidmode_active)
			XF86VidModeSwitchToMode(dpy, scrnum, vidmodes[0]);
		XCloseDisplay(dpy);
	}
	vidmode_active = false;
	dpy = NULL;
	win = 0;
	ctx = NULL;
#endif
}

void signal_handler(int sig)
{
	printf("Received signal %d, exiting...\n", sig);
	Sys_Quit();
	exit(0);
}

void InitSig()
{
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}

void VID_ShiftPalette(unsigned char *p)
{
	//	VID_SetPalette(p);
}

void VID_SetPalette(unsigned char *palette)
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
	int dist, bestdist;

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

		v = (255 << 24) + (r << 0) + (g << 8) + (b << 16);
		*table++ = v;
	}
	d_8to24table[255] &= 0xffffff; // 255 is transparent

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
		for(v = 0, k = 0, bestdist = 10000 * 10000; v < 256; v++, pal += 4)
		{
			r1 = (int)r - (int)pal[0];
			g1 = (int)g - (int)pal[1];
			b1 = (int)b - (int)pal[2];
			dist = (r1 * r1) + (g1 * g1) + (b1 * b1);
			if(dist < bestdist)
			{
				k = v;
				bestdist = dist;
			}
		}
		d_15to8table[i] = k;
	}
}

#ifndef X11
void CheckMultiTextureExtensions()
{
	void *prjobj;

	if(strstr(gl_extensions, "GL_SGIS_multitexture ") && !COM_CheckParm("-nomtex"))
	{
		Con_Printf("Found GL_SGIS_multitexture...\n");

		if((prjobj = dlopen(NULL, RTLD_LAZY)) == NULL)
		{
			Con_Printf("Unable to open symbol list for main program.\n");
			return;
		}

		qglMTexCoord2fSGIS = (void *)dlsym(prjobj, "glMTexCoord2fSGIS");
		qglSelectTextureSGIS = (void *)dlsym(prjobj, "glSelectTextureSGIS");

		if(qglMTexCoord2fSGIS && qglSelectTextureSGIS)
		{
			Con_Printf("Multitexture extensions found.\n");
			gl_mtexable = true;
		}
		else
			Con_Printf("Symbol not found, disabled.\n");

		dlclose(prjobj);
	}
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
	Con_Printf("GL_EXTENSIONS: %s\n", gl_extensions);

	//	Con_Printf ("%s %s\n", gl_renderer, gl_version);

#ifndef X11
	CheckMultiTextureExtensions();
#endif

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

	//	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
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
	*width = scr_width;
	*height = scr_height;

	//    if (!qwglMakeCurrent( maindc, baseRC ))
	//		Sys_Error ("wglMakeCurrent failed");

	//	qglViewport (*x, *y, *width, *height);
}

void VID_Init8bitPalette()
{
	// Check for 8bit Extensions and initialize them.
	int i;
	void *prjobj;

	if((prjobj = dlopen(NULL, RTLD_LAZY)) == NULL)
	{
		Con_Printf("Unable to open symbol list for main program.\n");
		return;
	}

	if(strstr(gl_extensions, "3DFX_set_global_palette") &&
	   (qgl3DfxSetPaletteEXT = dlsym(prjobj, "gl3DfxSetPaletteEXT")) != NULL)
	{
		GLubyte table[256][4];
		char *oldpal;

		Con_SafePrintf("8-bit GL extensions enabled.\n");
		qglEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
		oldpal = (char *)d_8to24table; //d_8to24table3dfx;
		for(i = 0; i < 256; i++)
		{
			table[i][2] = *oldpal++;
			table[i][1] = *oldpal++;
			table[i][0] = *oldpal++;
			table[i][3] = 255;
			oldpal++;
		}
		qgl3DfxSetPaletteEXT((GLuint *)table);
		is8bit = true;
	}
	else if(strstr(gl_extensions, "GL_EXT_shared_texture_palette") &&
	        (qglColorTableEXT = dlsym(prjobj, "glColorTableEXT")) != NULL)
	{
		char thePalette[256 * 3];
		char *oldPalette, *newPalette;

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
		qglColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, (void *)thePalette);
		is8bit = true;
	}

	dlclose(prjobj);
}

static void Check_Gamma(unsigned char *pal)
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

	memcpy(pal, palette, sizeof(palette));
}

void VID_Init(unsigned char *palette)
{
	int i;
	int attrib[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None
	};
	char gldir[MAX_OSPATH];

	Cvar_RegisterVariable(&vid_mode);
	
	Cvar_RegisterVariable(&in_mouse);
	Cvar_RegisterVariable(&in_dgamouse);
	
	Cvar_RegisterVariable(&m_filter);
	Cvar_RegisterVariable(&gl_ztrick);

	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong(*((int *)vid.colormap + 2048));

	// interpret command-line params

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

	ctx = qglXCreateContext(dpy, visinfo, NULL, True);

	qglXMakeCurrent(dpy, win, ctx);

	scr_width = width;
	scr_height = height;

	if(vid.conheight > height)
		vid.conheight = height;
	if(vid.conwidth > width)
		vid.conwidth = width;
	vid.width = vid.conwidth;
	vid.height = vid.conheight;

	vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);
	vid.numpages = 2;

	InitSig(); // trap evil signals

	GL_Init();

	sprintf(gldir, "%s/glquake", com_gamedir);
	FS_mkdir(gldir);

	VID_SetPalette(palette);

	// Check for 3DFX Extensions and initialize them.
	VID_Init8bitPalette();

	Con_SafePrintf("Video mode %dx%d initialized.\n", width, height);

	vid.recalc_refdef = 1; // force a surface cache flush
}