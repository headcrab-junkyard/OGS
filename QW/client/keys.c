
#ifdef _WINDOWS
#include <windows.h>
#endif

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
		if (key != K_BACKSPACE 
			&& key != K_PAUSE 
			&& key != K_PGUP 
			&& key != K_PGDN
			&& key_repeats[key] > 1)
			return;	// ignore most autorepeats
	}

	if ( (key_dest == key_menu && menubound[key])
	|| (key_dest == key_console && !consolekeys[key])
	|| (key_dest == key_game && ( cls.state == ca_active || !consolekeys[key] ) ) )
}