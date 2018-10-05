/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "quakedef.h"
#ifdef _WINDOWS
#include <windows.h>
#endif
/*

key up events are sent even if in console mode

*/


#define		MAXCMDLINE	256
char	key_lines[32][MAXCMDLINE];
int		key_linepos;
int		shift_down=false;
int		key_lastpress;


qboolean CheckForCommand ()
{
	char	command[128];
	char	*cmd, *s;
	int		i;

	s = key_lines[edit_line]+1;

	for (i=0 ; i<127 ; i++)
		if (s[i] <= ' ')
			break;
		else
			command[i] = s[i];
	command[i] = 0;

	cmd = Cmd_CompleteCommand (command);
	if (!cmd || strcmp (cmd, command))
		cmd = Cvar_CompleteVariable (command);
	if (!cmd  || strcmp (cmd, command) )
		return false;		// just a chat message
	return true;
}

void CompleteCommand ()
{
	char	*cmd, *s;

	s = key_lines[edit_line]+1;
	if (*s == '\\' || *s == '/')
		s++;

	cmd = Cmd_CompleteCommand (s);
	if (!cmd)
		cmd = Cvar_CompleteVariable (s);
	if (cmd)
	{
		key_lines[edit_line][1] = '/';
		Q_strcpy (key_lines[edit_line]+2, cmd);
		key_linepos = Q_strlen(cmd)+2;
		key_lines[edit_line][key_linepos] = ' ';
		key_linepos++;
		key_lines[edit_line][key_linepos] = 0;
		return;
	}
}


void Key_Console (int key)
{
#ifdef _WIN32
	char	*cmd, *s;
	int		i;
	HANDLE	th;
	char	*clipText, *textCopied;
#endif
	
	if (key == K_ENTER)
	{	// backslash text are commands, else chat
		if (key_lines[edit_line][1] == '\\' || key_lines[edit_line][1] == '/')
			Cbuf_AddText (key_lines[edit_line]+2);	// skip the >
		else if (CheckForCommand())
			Cbuf_AddText (key_lines[edit_line]+1);	// valid command
		else
		{	// convert to a chat message
			if (cls.state >= ca_connected)
				Cbuf_AddText ("say ");
			Cbuf_AddText (key_lines[edit_line]+1);	// skip the >
		}

		Cbuf_AddText ("\n");
		Con_Printf ("%s\n",key_lines[edit_line]);
		edit_line = (edit_line + 1) & 31;
		history_line = edit_line;
		key_lines[edit_line][0] = ']';
		key_linepos = 1;
		if (cls.state == ca_disconnected)
			SCR_UpdateScreen ();	// force an update, because the command
									// may take some time
		return;
	}

	if (key == K_TAB)
	{	// command completion
		CompleteCommand ();
		return;
	}

	if (key == K_PGUP || key==K_MWHEELUP)
	{
		con->display -= 2;
		return;
	}

	if (key == K_PGDN || key==K_MWHEELDOWN)
	{
		con->display += 2;
		if (con->display > con->current)
			con->display = con->current;
		return;
	}

	if (key == K_HOME)
	{
		con->display = con->current - con_totallines + 10;
		return;
	}

	if (key == K_END)
	{
		con->display = con->current;
		return;
	}
	
#ifdef _WIN32
	if ((key=='V' || key=='v') && GetKeyState(VK_CONTROL)<0) {
		if (OpenClipboard(NULL)) {
			th = GetClipboardData(CF_TEXT);
			if (th) {
				clipText = GlobalLock(th);
				if (clipText) {
					textCopied = malloc(GlobalSize(th)+1);
					strcpy(textCopied, clipText);
	/* Substitutes a NULL for every token */strtok(textCopied, "\n\r\b");
					i = strlen(textCopied);
					if (i+key_linepos>=MAXCMDLINE)
						i=MAXCMDLINE-key_linepos;
					if (i>0) {
						textCopied[i]=0;
						strcat(key_lines[edit_line], textCopied);
						key_linepos+=i;;
					}
					free(textCopied);
				}
				GlobalUnlock(th);
			}
			CloseClipboard();
		return;
		}
	}
#endif

}

//============================================================================

qboolean	chat_team;
char		chat_buffer[MAXCMDLINE];
int			chat_bufferlen = 0;




void Key_Bind_f ()
{
	
	for (i=2 ; i< c ; i++)
	{
		strcat (cmd, Cmd_Argv(i));
		if (i != (c-1))
			strcat (cmd, " ");
	}
}

void Key_WriteBindings (FILE *f)
{
	int		i;

	for (i=0 ; i<256 ; i++)
		if (keybindings[i])
			fprintf (f, "bind %s \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
}

void Key_Init ()
{

	consolekeys[K_ENTER] = true;
	consolekeys[K_TAB] = true;
	consolekeys[K_LEFTARROW] = true;
	consolekeys[K_RIGHTARROW] = true;
	consolekeys[K_UPARROW] = true;
	consolekeys[K_DOWNARROW] = true;
	consolekeys[K_BACKSPACE] = true;
	consolekeys[K_HOME] = true;
	consolekeys[K_END] = true;
	consolekeys[K_PGUP] = true;
	consolekeys[K_PGDN] = true;

	keyshift['1'] = '!';
	keyshift['2'] = '@';
	keyshift['3'] = '#';
	keyshift['4'] = '$';
	keyshift['5'] = '%';
	keyshift['6'] = '^';
	keyshift['7'] = '&';
	keyshift['8'] = '*';
	keyshift['9'] = '(';
	keyshift['0'] = ')';
	keyshift['-'] = '_';
	keyshift['='] = '+';
	keyshift[','] = '<';
	keyshift['.'] = '>';
	keyshift['/'] = '?';
	keyshift[';'] = ':';
	keyshift['\''] = '"';
	keyshift['['] = '{';
	keyshift[']'] = '}';
	keyshift['`'] = '~';
	keyshift['\\'] = '|';

}

void Key_Event (int key, qboolean down)
{


// update auto-repeat status
	if (down)
	{
		key_repeats[key]++;
		if (key != K_BACKSPACE 
			&& key != K_PAUSE 
			&& key != K_PGUP 
			&& key != K_PGDN
			&& key_repeats[key] > 1)
			return;	// ignore most autorepeats
			
		if (key >= 200 && !keybindings[key])
			Con_Printf ("%s is unbound, hit F4 to set.\n", Key_KeynumToString (key) );
	}

//
// if not a consolekey, send to the interpreter no matter what mode is
//
	if ( (key_dest == key_menu && menubound[key])
	|| (key_dest == key_console && !consolekeys[key])
	|| (key_dest == key_game && ( cls.state == ca_active || !consolekeys[key] ) ) )
	{
		kb = keybindings[key];
		if (kb)
		{
			if (kb[0] == '+')
			{	// button commands add keynum as a parm
				sprintf (cmd, "%s %i\n", kb, key);
				Cbuf_AddText (cmd);
			}
			else
			{
				Cbuf_AddText (kb);
				Cbuf_AddText ("\n");
			}
		}
		return;
	}

	if (!down)
		return;		// other systems only care about key down events

	if (shift_down)
		key = keyshift[key];

	switch (key_dest)
	{
	case key_message:
		Key_Message (key);
		break;
	case key_menu:
		M_Keydown (key);
		break;

	case key_game:
	case key_console:
		Key_Console (key);
		break;
	default:
		Sys_Error ("Bad key_dest");
	}
}

void Key_ClearStates ()
{
	int		i;

	for (i=0 ; i<256 ; i++)
	{
		keydown[i] = false;
		key_repeats[i] = false;
	}
}
