/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018 BlackPhrase
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

#include <termios.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/vt.h>

#include <signal.h>
#include <asm/io.h>

//#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "GL/gl.h"
#include "GL/glx.h"

#include "quakedef.h"

static Display *dpy = NULL;
static Window win;
static GLXContext ctx = NULL;

static qboolean usedga = false;

#define stringify(m) { #m, m }
 
cvar_t  mouse_button_commands[3] =
{
    {"mouse1","+attack"},
    {"mouse2","+strafe"},
    {"mouse3","+forward"},
};

static int mouse_buttons=3;
static int mouse_buttonstate;
static int mouse_oldbuttonstate;
static float mouse_x, mouse_y;
static float p_mouse_x, p_mouse_y;
static float old_mouse_x, old_mouse_y;

cvar_t _windowed_mouse = {"_windowed_mouse", "1", true};
cvar_t m_filter = {"m_filter","0"};
static float old_windowed_mouse;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask)



int XLateKey(XKeyEvent *ev)
{
	switch(keysym)
	{
		
		case XK_KP_Begin: key = K_AUX30;	break;

		case XK_Insert:
		case XK_KP_Insert: key = K_INS; break;

		case XK_KP_Multiply: key = '*'; break;
		case XK_KP_Add: key = '+'; break;
		case XK_KP_Subtract: key = '-'; break;
		case XK_KP_Divide: key = '/'; break;

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

	} 
}

struct
{
	int key;
	int down;
} keyq[64];
int keyq_head=0;
int keyq_tail=0;

int config_notify=0;
int config_notify_width;
int config_notify_height;
						      
qboolean Keyboard_Update(void)
{
	XEvent x_event;
   
	if(!XCheckMaskEvent(dpy,KEY_MASK,&x_event))
		return false;

	switch(x_event.type) {
	case KeyPress:
		keyq[keyq_head].key = XLateKey(&x_event.xkey);
		keyq[keyq_head].down = true;
		keyq_head = (keyq_head + 1) & 63;
		break;
	case KeyRelease:
		keyq[keyq_head].key = XLateKey(&x_event.xkey);
		keyq[keyq_head].down = false;
		keyq_head = (keyq_head + 1) & 63;
		break;
	}

	return true;
}

qboolean Mouse_Update(void)
{
	XEvent x_event;
	int b;
   
	if(!XCheckMaskEvent(dpy,MOUSE_MASK,&x_event))
		return false;

	switch(x_event.type) {
	case MotionNotify:
		if (usedga) {
			mouse_x += x_event.xmotion.x_root;
			mouse_y += x_event.xmotion.y_root;
		} else if (_windowed_mouse.value) {
			mouse_x += (float) ((int)x_event.xmotion.x - (int)(scr_width/2));
			mouse_y += (float) ((int)x_event.xmotion.y - (int)(scr_height/2));

			/* move the mouse to the window center again */
			XSelectInput(dpy,win, (KEY_MASK | MOUSE_MASK) & ~PointerMotionMask);
			XWarpPointer(dpy,None,win,0,0,0,0, (scr_width/2),(scr_height/2));
			XSelectInput(dpy,win, KEY_MASK | MOUSE_MASK);
		} else {
			mouse_x = (float) (x_event.xmotion.x-p_mouse_x);
			mouse_y = (float) (x_event.xmotion.y-p_mouse_y);
			p_mouse_x=x_event.xmotion.x;
			p_mouse_y=x_event.xmotion.y;
		}
		break;

	case ButtonPress:
		b=-1;
		if (x_event.xbutton.button == 1)
			b = 0;
		else if (x_event.xbutton.button == 2)
			b = 2;
		else if (x_event.xbutton.button == 3)
			b = 1;
		if (b>=0)
			mouse_buttonstate |= 1<<b;
		break;

	case ButtonRelease:
		b=-1;
		if (x_event.xbutton.button == 1)
			b = 0;
		else if (x_event.xbutton.button == 2)
			b = 2;
		else if (x_event.xbutton.button == 3)
			b = 1;
		if (b>=0)
			mouse_buttonstate &= ~(1<<b);
		break;
	}
   
	if (old_windowed_mouse != _windowed_mouse.value) {
		old_windowed_mouse = _windowed_mouse.value;

		if (!_windowed_mouse.value) {
			/* ungrab the pointer */
			Con_Printf("Releasing mouse.\n");

			XUngrabPointer(dpy,CurrentTime);
			XUngrabKeyboard(dpy,CurrentTime);
		} else {
			/* grab the pointer */
			Con_Printf("Grabbing mouse.\n");

			XGrabPointer(dpy,win,False,MOUSE_MASK,GrabModeAsync,
				GrabModeAsync,win,None,CurrentTime);
			XWarpPointer(dpy,None,win, 0,0,0,0, scr_width/2, scr_height/2);
			XGrabKeyboard(dpy,win,
				False,
				GrabModeAsync,GrabModeAsync,
				CurrentTime);

			//XSync(dpy,True);
		}
	}
	return true;
}

void signal_handler(int sig)
{
	printf("Received signal %d, exiting...\n", sig);
	VID_Shutdown();
	exit(0);
}

void InitSig(void)
{
	signal(SIGHUP, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
}

void VID_ShiftPalette(unsigned char *p)
{
	VID_SetPalette(p);
}

void	VID_SetPalette (unsigned char *palette)
{
	byte	*pal;
	unsigned short r,g,b;
	int     v;
	int     r1,g1,b1;
	int		k;
	unsigned short i;
	unsigned	*table;
	FILE *f;
	char s[255];
	float dist, bestdist;
	static qboolean palflag = false;

//
// 8 8 8 encoding
//
	pal = palette;
	table = d_8to24table;
	for (i=0 ; i<256 ; i++)
	{
		r = pal[0];
		g = pal[1];
		b = pal[2];
		pal += 3;
		
//		v = (255<<24) + (r<<16) + (g<<8) + (b<<0);
//		v = (255<<0) + (r<<8) + (g<<16) + (b<<24);
		v = (255<<24) + (r<<0) + (g<<8) + (b<<16);
		*table++ = v;
	}
	d_8to24table[255] &= 0xffffff;	// 255 is transparent

	// JACK: 3D distance calcs - k is last closest, l is the distance.
	// FIXME: Precalculate this and cache to disk.
	if (palflag)
		return;
	palflag = true;

	COM_FOpenFile("glquake/15to8.pal", &f);
	if (f) {
		fread(d_15to8table, 1<<15, 1, f);
		fclose(f);
	} else {
		for (i=0; i < (1<<15); i++) {
			/* Maps
 			000000000000000
 			000000000011111 = Red  = 0x1F
 			000001111100000 = Blue = 0x03E0
 			111110000000000 = Grn  = 0x7C00
 			*/
 			r = ((i & 0x1F) << 3)+4;
 			g = ((i & 0x03E0) >> 2)+4;
 			b = ((i & 0x7C00) >> 7)+4;
			pal = (unsigned char *)d_8to24table;
			for (v=0,k=0,bestdist=10000.0; v<256; v++,pal+=4) {
 				r1 = (int)r - (int)pal[0];
 				g1 = (int)g - (int)pal[1];
 				b1 = (int)b - (int)pal[2];
				dist = sqrt(((r1*r1)+(g1*g1)+(b1*b1)));
				if (dist < bestdist) {
					k=v;
					bestdist = dist;
				}
			}
			d_15to8table[i]=k;
		}
		sprintf(s, "%s/glquake", com_gamedir);
 		FS_mkdir (s);
		sprintf(s, "%s/glquake/15to8.pal", com_gamedir);
		if ((f = fopen(s, "wb")) != NULL) {
			fwrite(d_15to8table, 1<<15, 1, f);
			fclose(f);
		}
	}
}

#ifdef GL_EXT_SHARED
void VID_Init8bitPalette() 
{
	// Check for 8bit Extensions and initialize them.
	int i;
	char thePalette[256*3];
	char *oldPalette, *newPalette;

	if (strstr(gl_extensions, "GL_EXT_shared_texture_palette") == NULL)
		return;

	Con_SafePrintf("8-bit GL extensions enabled.\n");
	qglEnable( GL_SHARED_TEXTURE_PALETTE_EXT );
	oldPalette = (char *) d_8to24table; //d_8to24table3dfx;
	newPalette = thePalette;
	for (i=0;i<256;i++) {
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		oldPalette++;
	}
	qglColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, (void *) thePalette);
	is8bit = true;
}

#else
extern void qgl3DfxSetPaletteEXT(GLuint *pal);

void VID_Init8bitPalette(void) 
{
	// Check for 8bit Extensions and initialize them.
	int i;
	GLubyte table[256][4];
	char *oldpal;

	if (strstr(gl_extensions, "3DFX_set_global_palette") == NULL)
		return;

	Con_SafePrintf("8-bit GL extensions enabled.\n");
	qglEnable( GL_SHARED_TEXTURE_PALETTE_EXT );
	oldpal = (char *) d_8to24table; //d_8to24table3dfx;
	for (i=0;i<256;i++) {
		table[i][2] = *oldpal++;
		table[i][1] = *oldpal++;
		table[i][0] = *oldpal++;
		table[i][3] = 255;
		oldpal++;
	}
	qgl3DfxSetPaletteEXT((GLuint *)table);
	is8bit = true;
}
#endif

void VID_Init(unsigned char *palette)
{
	int i;
	char	gldir[MAX_OSPATH];
	int width = 640, height = 480;
	int attrib[] = { 
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None };
	int scrnum;
	XSetWindowAttributes attr;
	unsigned long mask;
	Window root;
	XVisualInfo *visinfo;

	S_Init();

	Cvar_RegisterVariable (&vid_mode);
	Cvar_RegisterVariable (&gl_ztrick);
	
	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));

// interpret command-line params

// set vid parameters

	if ((i = COM_CheckParm("-width")) != 0)
		width = atoi(com_argv[i+1]);
	if ((i = COM_CheckParm("-height")) != 0)
		height = atoi(com_argv[i+1]);

	if ((i = COM_CheckParm("-conwidth")) != 0)
		vid.conwidth = Q_atoi(com_argv[i+1]);
	else
		vid.conwidth = 640;

	vid.conwidth &= 0xfff8; // make it a multiple of eight

	if (vid.conwidth < 320)
		vid.conwidth = 320;

	// pick a conheight that matches with correct aspect
	vid.conheight = vid.conwidth*3 / 4;

	if ((i = COM_CheckParm("-conheight")) != 0)
		vid.conheight = Q_atoi(com_argv[i+1]);
	if (vid.conheight < 200)
		vid.conheight = 200;

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Error couldn't open the X display\n");
		exit(1);
	}

	scrnum = DefaultScreen(dpy);
	root = RootWindow(dpy, scrnum);

	visinfo=qglXChooseVisual(dpy,scrnum,attrib);
	if (!visinfo) {
		fprintf(stderr, "Error couldn't get an RGB, Double-buffered, Depth visual\n");
		exit(1);
	}

	/* window attributes */
	attr.background_pixel=0;
	attr.border_pixel=0;
	attr.colormap=XCreateColormap(dpy,root,visinfo->visual,AllocNone);
	attr.event_mask=KEY_MASK|MOUSE_MASK|VisibilityChangeMask;
	mask=CWBackPixel|CWBorderPixel|CWColormap|CWEventMask;

	win=XCreateWindow(dpy,root,0,0,width,height,
			0,visinfo->depth,InputOutput,
			visinfo->visual,mask,&attr);
	XMapWindow(dpy,win);

	XMoveWindow(dpy,win,0,0);

	XFlush(dpy);

	if (COM_CheckParm("-window"))
		putenv("MESA_GLX_FX=window");
	else
		putenv("MESA_GLX_FX=fullscreen");

	ctx = qglXCreateContext(dpy,visinfo,NULL,True);

	if (!ctx) {
		fprintf(stderr, "Unable to create glX context.\n");
		exit(1);
	}

	qglXMakeCurrent(dpy,win,ctx);

	scr_width = width;
	scr_height = height;

	if (vid.conheight > height)
		vid.conheight = height;
	if (vid.conwidth > width)
		vid.conwidth = width;
	vid.width = vid.conwidth;
	vid.height = vid.conheight;

	vid.aspect = ((float)vid.height / (float)vid.width) *
				(320.0 / 240.0);
	vid.numpages = 2;

	InitSig(); // trap evil signals

	GL_Init();

	sprintf (gldir, "%s/glquake", com_gamedir);
	FS_mkdir (gldir);

	VID_SetPalette(palette);

	// Check for 3DFX Extensions and initialize them.
	VID_Init8bitPalette();

	Con_SafePrintf ("Video mode %dx%d initialized.\n", width, height);

	vid.recalc_refdef = 1;				// force a surface cache flush
}

void	VID_LockBuffer (void) {}
void	VID_UnlockBuffer (void) {}