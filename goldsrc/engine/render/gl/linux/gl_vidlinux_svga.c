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

#include <asm/io.h>

/*#include "vga.h" */
#include "vgakeyboard.h"
#include "vgamouse.h"

#include "quakedef.h"
#include "GL/fxmesa.h"

#define WARP_WIDTH              320
#define WARP_HEIGHT             200

static fxMesaContext fc = NULL;
#define stringify(m) { #m, m }

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];
unsigned char d_15to8table[65536];

int num_shades=32;

struct
{
	char *name;
	int num;
} mice[] =
{
	stringify(MOUSE_MICROSOFT),
	stringify(MOUSE_MOUSESYSTEMS),
	stringify(MOUSE_MMSERIES),
	stringify(MOUSE_LOGITECH),
	stringify(MOUSE_BUSMOUSE),
	stringify(MOUSE_PS2),
};

static unsigned char scantokey[128];

int num_mice = sizeof (mice) / sizeof(mice[0]);

int	d_con_indirect = 0;

int		svgalib_inited=0;
int		UseMouse = 1;
int		UseKeyboard = 1;

int		mouserate = MOUSE_DEFAULTSAMPLERATE;

cvar_t		vid_mode = {"vid_mode","5",false};
cvar_t		vid_redrawfull = {"vid_redrawfull","0",false};
cvar_t		vid_waitforrefresh = {"vid_waitforrefresh","0",true};
 
char	*framebuffer_ptr;

cvar_t  mouse_button_commands[3] =
{
    {"mouse1","+attack"},
    {"mouse2","+strafe"},
    {"mouse3","+forward"},
};

int     mouse_buttons;
int     mouse_buttonstate;
int     mouse_oldbuttonstate;
float   mouse_x, mouse_y;
float	old_mouse_x, old_mouse_y;
int		mx, my;

cvar_t _windowed_mouse = {"_windowed_mouse", "1", true};
cvar_t	m_filter = {"m_filter","0"};

int scr_width, scr_height;

/*
=================
VID_Gamma_f

Keybinding command
=================
*/
void VID_Gamma_f ()
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

int matchmouse(int mouse, char *name)
{
	int i;
	for (i=0 ; i<num_mice ; i++)
		if (!strcmp(mice[i].name, name))
			return i;
	return mouse;
}

#if 0

void vtswitch(int newconsole)
{

	int fd;
	struct vt_stat x;

// switch consoles and wait until reactivated
	fd = open("/dev/console", O_RDONLY);
	ioctl(fd, VT_GETSTATE, &x);
	ioctl(fd, VT_ACTIVATE, newconsole);
	ioctl(fd, VT_WAITACTIVE, x.v_active);
	close(fd);

}

#endif

void keyhandler(int scancode, int state)
{
	
	int sc;

	sc = scancode & 0x7f;
//	Con_Printf("scancode=%x (%d%s)\n", scancode, sc, scancode&0x80?"+128":"");
	Key_Event(scantokey[sc], state == KEY_EVENTPRESS);

}

void signal_handler(int sig)
{
	printf("Received signal %d, exiting...\n", sig);
	VID_Shutdown();
	exit(0);
}

void InitSig()
{
	signal(SIGHUP, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}

void VID_ShiftPalette(unsigned short *p)
{
	VID_SetPalette(p);
}

void	VID_SetPalette (unsigned short *palette)
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
 		Sys_mkdir (s);
		sprintf(s, "%s/glquake/15to8.pal", com_gamedir);
		if ((f = fopen(s, "wb")) != NULL) {
			fwrite(d_15to8table, 1<<15, 1, f);
			fclose(f);
		}
	}
}

/*
===============
GL_Init
===============
*/
void GL_Init ()
{
	gl_vendor = glGetString (GL_VENDOR);
	Con_Printf ("GL_VENDOR: %s\n", gl_vendor);
	gl_renderer = glGetString (GL_RENDERER);
	Con_Printf ("GL_RENDERER: %s\n", gl_renderer);

	gl_version = glGetString (GL_VERSION);
	Con_Printf ("GL_VERSION: %s\n", gl_version);
	gl_extensions = glGetString (GL_EXTENSIONS);
	Con_Printf ("GL_EXTENSIONS: %s\n", gl_extensions);

//	Con_Printf ("%s %s\n", gl_renderer, gl_version);

	glClearColor (1,0,0,0);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.666);

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel (GL_FLAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

/*
=================
GL_BeginRendering

=================
*/
void GL_BeginRendering (int *x, int *y, int *width, int *height)
{
	extern cvar_t gl_clear;

	*x = *y = 0;
	*width = scr_width;
	*height = scr_height;

//    if (!wglMakeCurrent( maindc, baseRC ))
//		Sys_Error ("wglMakeCurrent failed");

//	glViewport (*x, *y, *width, *height);
}

void Init_KBD()
{
	int i;

	if (COM_CheckParm("-nokbd")) UseKeyboard = 0;

	if (UseKeyboard)
	{
		for (i=0 ; i<128 ; i++)
			scantokey[i] = ' ';

		scantokey[  1] = K_ESCAPE;
		scantokey[  2] = '1';
		scantokey[  3] = '2';
		scantokey[  4] = '3';
		scantokey[  5] = '4';
		scantokey[  6] = '5';
		scantokey[  7] = '6';
		scantokey[  8] = '7';
		scantokey[  9] = '8';
		scantokey[ 10] = '9';
		scantokey[ 11] = '0';
		scantokey[ 12] = '-';
		scantokey[ 13] = '=';
		scantokey[ 14] = K_BACKSPACE;
		scantokey[ 15] = K_TAB;
		scantokey[ 16] = 'q';       
		scantokey[ 17] = 'w';       
		scantokey[ 18] = 'e';       
		scantokey[ 19] = 'r';       
		scantokey[ 20] = 't';       
		scantokey[ 21] = 'y';       
		scantokey[ 22] = 'u';       
		scantokey[ 23] = 'i';       
		scantokey[ 24] = 'o';       
		scantokey[ 25] = 'p';       
		scantokey[ 26] = '[';
		scantokey[ 27] = ']';
		scantokey[ 28] = K_ENTER;
		scantokey[ 29] = K_CTRL; //left
		scantokey[ 30] = 'a';
		scantokey[ 31] = 's';       
		scantokey[ 32] = 'd';       
		scantokey[ 33] = 'f';       
		scantokey[ 34] = 'g';       
		scantokey[ 35] = 'h';       
		scantokey[ 36] = 'j';       
		scantokey[ 37] = 'k';       
		scantokey[ 38] = 'l';       
		scantokey[ 39] = ';';
		scantokey[ 40] = '\'';
		scantokey[ 41] = '`';
		scantokey[ 42] = K_SHIFT; //left
		scantokey[ 43] = '\\';
		scantokey[ 44] = 'z';       
		scantokey[ 45] = 'x';  
		scantokey[ 46] = 'c';
		scantokey[ 47] = 'v';       
		scantokey[ 48] = 'b';
		scantokey[ 49] = 'n';       
		scantokey[ 50] = 'm';       
		scantokey[ 51] = ',';
		scantokey[ 52] = '.';
		scantokey[ 53] = '/';
		scantokey[ 54] = K_SHIFT; //right
		scantokey[ 55] = '*'; //keypad
		scantokey[ 56] = K_ALT; //left
		scantokey[ 57] = ' ';
		// 58 caps lock
		scantokey[ 59] = K_F1;
		scantokey[ 60] = K_F2;
		scantokey[ 61] = K_F3;
		scantokey[ 62] = K_F4;
		scantokey[ 63] = K_F5;
		scantokey[ 64] = K_F6;
		scantokey[ 65] = K_F7;
		scantokey[ 66] = K_F8;
		scantokey[ 67] = K_F9;
		scantokey[ 68] = K_F10;
		// 69 numlock
		// 70 scrollock
		scantokey[ 71] = K_HOME;
		scantokey[ 72] = K_UPARROW;
		scantokey[ 73] = K_PGUP;
		scantokey[ 74] = '-';
		scantokey[ 75] = K_LEFTARROW;
		scantokey[ 76] = '5';
		scantokey[ 77] = K_RIGHTARROW;
		scantokey[ 79] = K_END;
		scantokey[ 78] = '+';
		scantokey[ 80] = K_DOWNARROW;
		scantokey[ 81] = K_PGDN;
		scantokey[ 82] = K_INS;
		scantokey[ 83] = K_DEL;
		// 84 to 86 not used
		scantokey[ 87] = K_F11;
		scantokey[ 88] = K_F12;
		// 89 to 95 not used
		scantokey[ 96] = K_ENTER; //keypad enter
		scantokey[ 97] = K_CTRL; //right
		scantokey[ 98] = '/';
		scantokey[ 99] = K_F12; // print screen, bind to screenshot by default
		scantokey[100] = K_ALT; // right


		scantokey[101] = K_PAUSE; // break
		scantokey[102] = K_HOME;
		scantokey[103] = K_UPARROW;
		scantokey[104] = K_PGUP;
		scantokey[105] = K_LEFTARROW;
		scantokey[106] = K_RIGHTARROW;
		scantokey[107] = K_END;
		scantokey[108] = K_DOWNARROW;
		scantokey[109] = K_PGDN;
		scantokey[110] = K_INS;
		scantokey[111] = K_DEL;

		scantokey[119] = K_PAUSE;

		if (keyboard_init())
			Sys_Error("keyboard_init() failed");
		keyboard_seteventhandler(keyhandler);
	}
}

int findres(int *width, int *height)
{
	int i;

	for(i=0;i<NUM_RESOLUTIONS;i++)
		if((*width<=resolutions[i][0]) && (*height<=resolutions[i][1])) {
			*width = resolutions[i][0];
			*height = resolutions[i][1];
			return resolutions[i][2];
		}
        
	*width = 640;
	*height = 480;
	return GR_RESOLUTION_640x480;
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
	glEnable( GL_SHARED_TEXTURE_PALETTE_EXT );
	oldPalette = (char *) d_8to24table; //d_8to24table3dfx;
	newPalette = thePalette;
	for (i=0;i<256;i++) {
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		oldPalette++;
	}
	glColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, (void *) thePalette);
	is8bit = true;
}

#else
extern void gl3DfxSetPaletteEXT(GLuint *pal);

void VID_Init8bitPalette() 
{
	// Check for 8bit Extensions and initialize them.
	int i;
	GLubyte table[256][4];
	char *oldpal;

	if (strstr(gl_extensions, "3DFX_set_global_palette") == NULL)
		return;

	Con_SafePrintf("8-bit GL extensions enabled.\n");
	glEnable( GL_SHARED_TEXTURE_PALETTE_EXT );
	oldpal = (char *) d_8to24table; //d_8to24table3dfx;
	for (i=0;i<256;i++) {
		table[i][2] = *oldpal++;
		table[i][1] = *oldpal++;
		table[i][0] = *oldpal++;
		table[i][3] = 255;
		oldpal++;
	}
	gl3DfxSetPaletteEXT((GLuint *)table);
	is8bit = true;
}
#endif

void VID_Init(unsigned short *palette)
{
	int i;
	GLint attribs[32];
	char	gldir[MAX_OSPATH];

	S_Init();

	Init_KBD();

	Cvar_RegisterVariable (&vid_mode);
	Cvar_RegisterVariable (&vid_redrawfull);
	Cvar_RegisterVariable (&vid_waitforrefresh);
	Cvar_RegisterVariable (&gl_ztrick);
	
        vid.maxwarpwidth = WARP_WIDTH;
        vid.maxwarpheight = WARP_HEIGHT;
        vid.colormap = host_colormap;
        vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));

// interpret command-line params

// set vid parameters
	attribs[0] = FXMESA_DOUBLEBUFFER;
	attribs[1] = FXMESA_ALPHA_SIZE;
	attribs[2] = 1;
	attribs[3] = FXMESA_DEPTH_SIZE;
	attribs[4] = 1;
	attribs[5] = FXMESA_NONE;

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

	fc = fxMesaCreateContext(0, findres(&width, &height), GR_REFRESH_75Hz, 
		attribs);
	if (!fc)
		Sys_Error("Unable to create 3DFX context.\n");

	scr_width = width;
	scr_height = height;

	fxMesaMakeCurrent(fc);

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

void	VID_LockBuffer () {}
void	VID_UnlockBuffer () {}