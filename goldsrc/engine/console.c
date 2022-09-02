/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021-2022 BlackPhrase
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

#ifdef NeXT
#include <libc.h>
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <fcntl.h>
#include "quakedef.h"

#include "con_nprint.h"

int con_linewidth;

float con_cursorspeed = 4;

#define CON_TEXTSIZE 16384

qboolean con_forcedup; // because no entities to refresh

int con_totallines; // total lines in console scrollback
int con_backscroll; // lines up from bottom to display
int con_current;    // where next message will be printed
int con_x;          // offset in current line for next print
char *con_text = 0;

cvar_t console = {"console", "0", FCVAR_ARCHIVE};
cvar_t con_fastmode = {"con_fastmode", "1"};
cvar_t con_notifytime = {"con_notifytime", "4"}; //seconds
cvar_t con_color = {"con_color", "255 180 30", FCVAR_ARCHIVE};
cvar_t con_shifttoggleconsole = {"con_shifttoggleconsole", "0"};
cvar_t con_mono = {"con_mono", "0", FCVAR_ARCHIVE};

#define NUM_CON_TIMES 4
float con_times[NUM_CON_TIMES]; // realtime time the line was generated
                                // for transparent notify lines

int con_vislines;

qboolean con_debuglog;

#define MAXCMDLINE 256
extern char key_lines[32][MAXCMDLINE];
extern int edit_line;
extern int key_linepos;

qboolean con_initialized;

int con_notifylines; // scan lines to clear for notify lines

extern void M_Menu_Main_f();

/*
================
Con_Debug_f
================
*/
void Con_Debug_f()
{
	if(con_debuglog)
	{
		con_debuglog = true;
		Con_Printf("condebug enabled");
	}
	else
	{
		Con_Printf("condebug disabled");
		con_debuglog = false;
	};
};


/*
================
Con_Times_f
================
*/
void Con_Times_f()
{
	// TODO
};

/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f()
{
	if(key_dest == key_console)
	{
		if(cls.state == ca_connected)
		{
			key_dest = key_game;
			key_lines[edit_line][1] = 0; // clear any typing
			key_linepos = 1;
		}
		else
			Cbuf_AddText("menu_main"); //M_Menu_Main_f(); // TODO
	}
	else
		key_dest = key_console;

	SCR_EndLoadingPlaque();
	Q_memset(con_times, 0, sizeof(con_times));
};

/*
================
Con_HideConsole_f
================
*/
void Con_HideConsole_f()
{
	if(key_dest == key_console)
	{
		if(cls.state == ca_connected)
		{
			key_dest = key_game;
			key_lines[edit_line][1] = 0; // clear any typing
			key_linepos = 1;
		}
		else
			Cbuf_AddText("menu_main"); //M_Menu_Main_f(); // TODO
	};
	
	SCR_EndLoadingPlaque();
	Q_memset(con_times, 0, sizeof(con_times));
};

/*
================
Con_Clear_f
================
*/
void Con_Clear_f()
{
	if(con_text)
		Q_memset(con_text, ' ', CON_TEXTSIZE);
}

/*
================
Con_Dump_f

Save the console contents out to a file
================
*/
void Con_Dump_f()
{
	int l, x;
	char *line;
	FileHandle_t f;
	char buffer[1024];
	char name[MAX_OSPATH];

	if(Cmd_Argc() != 2)
	{
		Con_Printf("usage: condump <filename>\n");
		return;
	};

	snprintf(name, sizeof(name), "%s/%s.txt", com_gamedir, Cmd_Argv(1)); // TODO: COM_Gamedir()

	Con_Printf("Dumped console text to %s.\n", name);
	COM_CreatePath(name);
	f = FS_Open(name, "w"/*, "GAMEFOLDER"*/); // TODO: FS_OpenPathID?
	if(!f)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	// skip empty lines
	for(l = con_current - con_totallines + 1; l <= con_current; l++)
	{
		line = con_text + (l % con_totallines) * con_linewidth;
		for(x = 0; x < con_linewidth; x++)
			if(line[x] != ' ')
				break;
		if(x != con_linewidth)
			break;
	}

	// write the remaining lines
	buffer[con_linewidth] = 0;
	for(; l <= con_current; l++)
	{
		line = con_text + (l % con_totallines) * con_linewidth;
		strncpy(buffer, line, con_linewidth);
		for(x = con_linewidth - 1; x >= 0; x--)
		{
			if(buffer[x] == ' ')
				buffer[x] = 0;
			else
				break;
		}
		for(x = 0; buffer[x]; x++)
			buffer[x] &= 0x7f;

		FS_FPrintf(f, "%s\n", buffer);
	}

	FS_Close(f);
}

/*
================
Con_ClearNotify
================
*/
void Con_ClearNotify()
{
	int i;

	for(i = 0; i < NUM_CON_TIMES; i++)
		con_times[i] = 0;
}

/*
================
Con_MessageMode_f
================
*/
extern qboolean team_message;

void Con_MessageMode_f()
{
	key_dest = key_message;
	team_message = false;
}

/*
================
Con_MessageMode2_f
================
*/
void Con_MessageMode2_f()
{
	key_dest = key_message;
	team_message = true;
}

/*
================
Con_CheckResize

If the line width has changed, reformat the buffer.
================
*/
void Con_CheckResize()
{
	int i, j, width, oldwidth, oldtotallines, numlines, numchars;
	char tbuf[CON_TEXTSIZE];

	width = (vid.width >> 3) - 2;

	if(width == con_linewidth)
		return;

	if(width < 1) // video hasn't been initialized yet
	{
		width = 38;
		con_linewidth = width;
		con_totallines = CON_TEXTSIZE / con_linewidth;
		Q_memset(con_text, ' ', CON_TEXTSIZE);
	}
	else
	{
		oldwidth = con_linewidth;
		con_linewidth = width;
		oldtotallines = con_totallines;
		con_totallines = CON_TEXTSIZE / con_linewidth;
		numlines = oldtotallines;

		if(con_totallines < numlines)
			numlines = con_totallines;

		numchars = oldwidth;

		if(con_linewidth < numchars)
			numchars = con_linewidth;

		Q_memcpy(tbuf, con_text, CON_TEXTSIZE);
		Q_memset(con_text, ' ', CON_TEXTSIZE);

		for(i = 0; i < numlines; i++)
		{
			for(j = 0; j < numchars; j++)
			{
				con_text[(con_totallines - 1 - i) * con_linewidth + j] =
				tbuf[((con_current - i + oldtotallines) %
				      oldtotallines) *
				     oldwidth +
				     j];
			}
		}

		Con_ClearNotify();
	}

	con_backscroll = 0;
	con_current = con_totallines - 1;
}

/*
================
Con_Init
================
*/
void Con_Init()
{
#define MAXGAMEDIRLEN 1000
	char temp[MAXGAMEDIRLEN + 1];
	char *t2 = "/qconsole.log";

	con_debuglog = COM_CheckParm("-condebug");

	if(con_debuglog)
	{
		if(strlen(com_gamedir) < (MAXGAMEDIRLEN - strlen(t2)))
		{
			sprintf(temp, "%s%s", com_gamedir, t2);
			unlink(temp);
		}
	}

	con_text = Hunk_AllocName(CON_TEXTSIZE, "context");
	Q_memset(con_text, ' ', CON_TEXTSIZE);
	con_linewidth = -1;
	Con_CheckResize();

	Con_Printf("Console initialized.\n");

	//
	// register our commands
	//
	Cvar_RegisterVariable(&console); // TODO: unused
	Cvar_RegisterVariable(&con_fastmode); // TODO: unused
	Cvar_RegisterVariable(&con_notifytime);
	Cvar_RegisterVariable(&con_color); // TODO: unused
	Cvar_RegisterVariable(&con_shifttoggleconsole); // TODO: unused
	Cvar_RegisterVariable(&con_mono); // TODO: unused
	
	Cmd_AddCommand("contimes", Con_Times_f);
	Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
	Cmd_AddCommand("hideconsole", Con_HideConsole_f);
	Cmd_AddCommand("messagemode", Con_MessageMode_f);
	Cmd_AddCommand("messagemode2", Con_MessageMode2_f);
	Cmd_AddCommand("clear", Con_Clear_f);
	Cmd_AddCommand("condebug", Con_Debug_f);
	Cmd_AddCommand("condump", Con_Dump_f); // TODO: ghost cmd?
	
	con_initialized = true;
}

/*
===============
Con_Linefeed
===============
*/
void Con_Linefeed()
{
	con_x = 0;
	con_current++;
	Q_memset(&con_text[(con_current % con_totallines) * con_linewidth], ' ', con_linewidth);
}

/*
================
Con_Print

Handles cursor positioning, line wrapping, etc
All console printing must go through this in order to be logged to disk
If no console is visible, the notify window will pop up.
================
*/
void Con_Print(const char *txt)
{
	int y;
	int c, l;
	static int cr;
	int mask;

	con_backscroll = 0;

	if(txt[0] == 1)
	{
		mask = 128; // go to colored text
		S_LocalSound("misc/talk.wav");
		// play talk wav
		txt++;
	}
	else if(txt[0] == 2)
	{
		mask = 128; // go to colored text
		txt++;
	}
	else
		mask = 0;

	while((c = *txt))
	{
		// count word length
		for(l = 0; l < con_linewidth; l++)
			if(txt[l] <= ' ')
				break;

		// word wrap
		if(l != con_linewidth && (con_x + l > con_linewidth))
			con_x = 0;

		txt++;

		if(cr)
		{
			con_current--;
			cr = false;
		}

		if(!con_x)
		{
			Con_Linefeed();
			// mark time for transparent overlay
			if(con_current >= 0)
				con_times[con_current % NUM_CON_TIMES] = realtime;
		}

		switch(c)
		{
		case '\n':
			con_x = 0;
			break;

		case '\r':
			con_x = 0;
			cr = 1;
			break;

		default: // display character and advance
			y = con_current % con_totallines;
			con_text[y * con_linewidth + con_x] = c | mask;
			con_x++;
			if(con_x >= con_linewidth)
				con_x = 0;
			break;
		}
	}
}

/*
================
Con_DebugLog
================
*/
void Con_DebugLog(const char *file, const char *fmt, ...)
{
	va_list argptr;
	static char data[1024];
	int fd;

	va_start(argptr, fmt);
	vsprintf(data, fmt, argptr);
	va_end(argptr);
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	write(fd, data, strlen(data));
	close(fd);
}

/*
================
Con_Printf

Handles cursor positioning, line wrapping, etc
================
*/
#define MAXPRINTMSG 4096
// FIXME: make a buffer size safe vsprintf?
void Con_Printf(const char *fmt, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];
	static qboolean inupdate;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	// add to redirected message
	if(sv_redirected)
	{
		if(Q_strlen(msg) + Q_strlen(outputbuf) > sizeof(outputbuf) - 1)
			SV_FlushRedirect();
		Q_strcat(outputbuf, msg);
		return;
	};
	
	// also echo to debugging console
	Sys_Printf("%s", msg);

	// log all messages to file
	if(con_debuglog)
		Con_DebugLog("qconsole.log", "%s", msg);

	// TODO
	//if(sv_logfile)
		//fprintf(sv_logfile, "%s", msg);
	
	if(!con_initialized)
		return;

	if(cls.state == ca_dedicated)
		return; // no graphics mode

	GameConsole_Printf(msg); // TODO: VguiWrap2_ConPrintf(msg);

	// write it to the scrollable buffer
	Con_Print(msg);

	// update the screen if the console is displayed
	if(cls.signon != SIGNONS && !scr_disabled_for_loading)
	{
		// protect against infinite loop if something in SCR_UpdateScreen calls
		// Con_Printd
		if(!inupdate)
		{
			inupdate = true;
			SCR_UpdateScreen();
			inupdate = false;
		}
	}
}

/*
================
Con_DPrintf

A Con_Printf that only shows up if the "developer" cvar is set
================
*/
void Con_DPrintf(const char *fmt, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];

	if(!developer.value)
		return; // don't confuse non-developers with techie stuff...

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	Con_Printf("%s", msg);
}

void Con_NPrintf(int anPos, const char *fmt, ...)
{
	// TODO
	
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	Con_Printf("%s", msg);
};

void Con_NXPrintf(struct con_nprint_s *apInfo, const char *fmt, ...)
{
	// TODO
	
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	Con_Printf("%s", msg);
};

/*
==================
Con_SafePrintf

Okay to call even when the screen can't be updated
==================
*/
void Con_SafePrintf(const char *fmt, ...)
{
	va_list argptr;
	char msg[1024];
	int temp;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	temp = scr_disabled_for_loading;
	scr_disabled_for_loading = true;
	Con_Printf("%s", msg);
	scr_disabled_for_loading = temp;
}

/*
==============================================================================

DRAWING

==============================================================================
*/

/*
================
Con_DrawInput

The input line scrolls horizontally if typing goes beyond the right edge
================
*/
void Con_DrawInput()
{
	int y;
	int i;
	char *text;

	if(key_dest != key_console && !con_forcedup)
		return; // don't draw anything

	text = key_lines[edit_line];

	// add the cursor frame
	text[key_linepos] = 10 + ((int)(realtime * con_cursorspeed) & 1);

	// fill out remainder with spaces
	for(i = key_linepos + 1; i < con_linewidth; i++)
		text[i] = ' ';

	//	prestep if horizontally scrolling
	if(key_linepos >= con_linewidth)
		text += 1 + key_linepos - con_linewidth;

	// draw it
	y = con_vislines - 16;

	for(i = 0; i < con_linewidth; i++)
		Draw_Character((i + 1) << 3, con_vislines - 16, text[i]);

	// remove cursor
	key_lines[edit_line][key_linepos] = 0;
}

/*
================
Con_DrawNotify

Draws the last few lines of output transparently over the game top
================
*/
void Con_DrawNotify()
{
	int x, v;
	char *text;
	int i;
	float time;
	extern char chat_buffer[];

	v = 0;
	for(i = con_current - NUM_CON_TIMES + 1; i <= con_current; i++)
	{
		if(i < 0)
			continue;
		time = con_times[i % NUM_CON_TIMES];
		if(time == 0)
			continue;
		time = realtime - time;
		if(time > con_notifytime.value)
			continue;
		text = con_text + (i % con_totallines) * con_linewidth;

		clearnotify = 0;
		scr_copytop = 1;

		for(x = 0; x < con_linewidth; x++)
			Draw_Character((x + 1) << 3, v, text[x]);

		v += 8;
	}

	if(key_dest == key_message)
	{
		clearnotify = 0;
		scr_copytop = 1;

		x = 0;

		Draw_String(8, v, "say:");
		while(chat_buffer[x])
		{
			Draw_Character((x + 5) << 3, v, chat_buffer[x]);
			x++;
		}
		Draw_Character((x + 5) << 3, v, 10 + ((int)(realtime * con_cursorspeed) & 1));
		v += 8;
	}

	if(v > con_notifylines)
		con_notifylines = v;
}

/*
================
Con_DrawConsole

Draws the console with the solid background
The typing input line at the bottom should only be drawn if typing is allowed
================
*/
void Con_DrawConsole(int lines, qboolean drawinput)
{
	int i, x, y;
	int rows;
	char *text;
	int j;

	if(lines <= 0)
		return;

	// draw the background
	Draw_ConsoleBackground(lines);

	// draw the text
	con_vislines = lines;

	rows = (lines - 16) >> 3;     // rows of text to draw
	y = lines - 16 - (rows << 3); // may start slightly negative

	for(i = con_current - rows + 1; i <= con_current; i++, y += 8)
	{
		j = i - con_backscroll;
		if(j < 0)
			j = 0;
		text = con_text + (j % con_totallines) * con_linewidth;

		for(x = 0; x < con_linewidth; x++)
			Draw_Character((x + 1) << 3, y, text[x]);
	}

	// draw the input prompt, user text, and cursor if desired
	if(drawinput)
		Con_DrawInput();
}

/*
==================
Con_NotifyBox
==================
*/
void Con_NotifyBox(const char *text)
{
	double t1, t2;

	// during startup for sound / cd warnings
	Con_Printf("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n");

	Con_Printf(text);

	Con_Printf("Press a key.\n");
	Con_Printf("\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n");

	key_count = -2; // wait for a key down and up
	key_dest = key_console;

	do
	{
		t1 = Sys_FloatTime();
		SCR_UpdateScreen();
		Sys_SendKeyEvents();
		t2 = Sys_FloatTime();
		realtime += t2 - t1; // make the cursor blink
	} while(key_count < 0);

	Con_Printf("\n");
	key_dest = key_game;
	realtime = 0; // put the cursor back to invisible
}

int Con_IsVisible()
{
	// TODO
	return 0;
};