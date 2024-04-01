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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>

#include "const.h"
#include "keydefs.h"
#include "menu.h"
#include "keydefs.h"

// TODO: duplicated here from quakedef header
#define MAX_OSPATH 128 // max length of a filesystem pathname
#define SAVEGAME_COMMENT_LENGTH 39

// TODO: duplicated here from render header
#define TOP_RANGE 16 // soldier uniform colors
#define BOTTOM_RANGE 96

// TODO: duplicated here from gl_vidnt.c
#define VID_ROW_SIZE 3

typedef struct qpic_s
{
	int width, height;
	byte data[4]; // variably sized
} qpic_t;

typedef enum { key_game,
	           key_console,
	           key_message,
	           key_menu } keydest_t;

#include "IMenuExportsTemp.hpp"
extern IMenuExportsTemp *gpMenuExports;

#include "cdll_int.h"
extern cl_enginefunc_t *gpEngine;

#include "IGameUIFuncs.h"
extern IGameUIFuncs *gpGameUIFuncs;

enum menu_state_e
{
	m_none,
	m_main,
	m_singleplayer,
	m_load,
	m_save,
	m_multiplayer,
	m_setup,
	m_net,
	m_options,
	m_video,
	m_keys,
	m_help,
	m_quit,
	m_serialconfig,
	m_modemconfig,
	m_lanconfig,
	m_gameoptions,
	m_search,
	m_slist
} m_state;

void M_Menu_Main_f();
void M_Menu_SinglePlayer_f();
void M_Menu_Load_f();
void M_Menu_Save_f();
void M_Menu_MultiPlayer_f();
void M_Menu_Setup_f();
void M_Menu_Net_f();
void M_Menu_Options_f();
void M_Menu_Keys_f();
void M_Menu_Video_f();
void M_Menu_Help_f();
void M_Menu_Quit_f();
void M_Menu_SerialConfig_f();
void M_Menu_ModemConfig_f();
void M_Menu_LanConfig_f();
void M_Menu_GameOptions_f();
void M_Menu_Search_f();
void M_Menu_ServerList_f();

void (*vid_menudrawfn)();
void (*vid_menukeyfn)(int key);

/*
class CMenuMain
{
public:
	void Draw();
	void Key(int key);
};

class CMenuSinglePlayer
{
public:
	void Draw();
	void Key(int key);
};

class CMenuLoad
{
public:
	void Draw();
	void Key(int key);
};

class CMenuSave
{
public:
	void Draw();
	void Key(int key);
};

class CMenuMultiPlayer
{
public:
	void Draw();
	void Key(int key);
};

class CMenuSetup
{
public:
	void Draw();
	void Key(int key);
};

class CMenuNet
{
public:
	void Draw();
	void Key(int key);
};

class CMenuOptions
{
public:
	void Draw();
	void Key(int key);
};

class CMenuKeys
{
public:
	void Draw();
	void Key(int key);
};

class CMenuVideo
{
public:
	void Draw();
	void Key(int key);
};

class CMenuHelp
{
public:
	void Draw();
	void Key(int key);
};

class CMenuQuit
{
public:
	void Draw();
	void Key(int key);
};

class CMenuSerialConfig
{
public:
	void Draw();
	void Key(int key);
};

class CMenuModemConfig
{
public:
	void Draw();
	void Key(int key);
};

class CMenuLanConfig
{
public:
	void Draw();
	void Key(int key);
};

class CMenuGameOptions
{
public:
	void Draw();
	void Key(int key);
};

class CMenuSearch
{
public:
	void Draw();
	void Key(int key);
};

class CMenuServerList
{
public:
	void Draw();
	void Key(int key);
};
*/

void M_Main_Draw();
void M_SinglePlayer_Draw();
void M_Load_Draw();
void M_Save_Draw();
void M_MultiPlayer_Draw();
void M_Setup_Draw();
void M_Net_Draw();
void M_Options_Draw();
void M_Keys_Draw();
void M_Video_Draw();
void M_Help_Draw();
void M_Quit_Draw();
void M_SerialConfig_Draw();
void M_ModemConfig_Draw();
void M_LanConfig_Draw();
void M_GameOptions_Draw();
void M_Search_Draw();
void M_ServerList_Draw();

void M_Main_Key(int key);
void M_SinglePlayer_Key(int key);
void M_Load_Key(int key);
void M_Save_Key(int key);
void M_MultiPlayer_Key(int key);
void M_Setup_Key(int key);
void M_Net_Key(int key);
void M_Options_Key(int key);
void M_Keys_Key(int key);
void M_Video_Key(int key);
void M_Help_Key(int key);
void M_Quit_Key(int key);
void M_SerialConfig_Key(int key);
void M_ModemConfig_Key(int key);
void M_LanConfig_Key(int key);
void M_GameOptions_Key(int key);
void M_Search_Key(int key);
void M_ServerList_Key(int key);

bool m_entersound; // play after drawing a frame, so caching
                       // won't disrupt the sound
bool m_recursiveDraw;

int m_return_state;
bool m_return_onerror;
char m_return_reason[32];

#define StartingGame (m_multiplayer_cursor == 1)
#define JoiningGame (m_multiplayer_cursor == 0)
#define SerialConfig (m_net_cursor == 0)
#define DirectConfig (m_net_cursor == 1)
#define IPXConfig (m_net_cursor == 2)
#define TCPIPConfig (m_net_cursor == 3)

void M_ConfigureNetSubsystem();

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char *va(const char *format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
};

/*
================
M_DrawCharacter

Draws one solid graphics character
================
*/
void M_DrawCharacter(int cx, int line, int num)
{
	int width;
	gpGameUIFuncs->GetCurrentVideoMode(&width, nullptr, nullptr);
	gpMenuExports->Draw_Character(cx + ((width - 320) >> 1), line, num);
}

void M_Print(int cx, int cy, const char *str)
{
	while(*str)
	{
		M_DrawCharacter(cx, cy, (*str) + 128);
		str++;
		cx += 8;
	}
}

void M_PrintWhite(int cx, int cy, const char *str)
{
	while(*str)
	{
		M_DrawCharacter(cx, cy, *str);
		str++;
		cx += 8;
	}
}

void M_DrawTransPic(int x, int y, qpic_t *pic)
{
	gpMenuExports->Draw_TransPic(x + ((gpMenuExports->VID_GetWidth() - 320) >> 1), y, pic);
}

void M_DrawPic(int x, int y, qpic_t *pic)
{
	gpMenuExports->Draw_Pic(x + ((gpMenuExports->VID_GetWidth() - 320) >> 1), y, pic);
}

byte identityTable[256];
byte translationTable[256];

void M_BuildTranslationTable(int top, int bottom)
{
	int j;
	byte *dest, *source;

	for(j = 0; j < 256; j++)
		identityTable[j] = j;
	dest = translationTable;
	source = identityTable;
	memcpy(dest, source, 256);

	if(top < 128) // the artists made some backwards ranges.  sigh.
		memcpy(dest + TOP_RANGE, source + top, 16);
	else
		for(j = 0; j < 16; j++)
			dest[TOP_RANGE + j] = source[top + 15 - j];

	if(bottom < 128)
		memcpy(dest + BOTTOM_RANGE, source + bottom, 16);
	else
		for(j = 0; j < 16; j++)
			dest[BOTTOM_RANGE + j] = source[bottom + 15 - j];
}

void M_DrawTransPicTranslate(int x, int y, qpic_t *pic)
{
	gpMenuExports->Draw_TransPicTranslate(x + ((gpMenuExports->VID_GetWidth() - 320) >> 1), y, pic, translationTable);
}

void M_DrawTextBox(int x, int y, int width, int lines)
{
	qpic_t *p;
	int cx, cy;
	int n;

	// draw left side
	cx = x;
	cy = y;
	p = gpMenuExports->Draw_CachePic("gfx/box_tl.lmp");
	M_DrawTransPic(cx, cy, p);
	p = gpMenuExports->Draw_CachePic("gfx/box_ml.lmp");
	for(n = 0; n < lines; n++)
	{
		cy += 8;
		M_DrawTransPic(cx, cy, p);
	}
	p = gpMenuExports->Draw_CachePic("gfx/box_bl.lmp");
	M_DrawTransPic(cx, cy + 8, p);

	// draw middle
	cx += 8;
	while(width > 0)
	{
		cy = y;
		p = gpMenuExports->Draw_CachePic("gfx/box_tm.lmp");
		M_DrawTransPic(cx, cy, p);
		p = gpMenuExports->Draw_CachePic("gfx/box_mm.lmp");
		for(n = 0; n < lines; n++)
		{
			cy += 8;
			if(n == 1)
				p = gpMenuExports->Draw_CachePic("gfx/box_mm2.lmp");
			M_DrawTransPic(cx, cy, p);
		}
		p = gpMenuExports->Draw_CachePic("gfx/box_bm.lmp");
		M_DrawTransPic(cx, cy + 8, p);
		width -= 2;
		cx += 16;
	}

	// draw right side
	cy = y;
	p = gpMenuExports->Draw_CachePic("gfx/box_tr.lmp");
	M_DrawTransPic(cx, cy, p);
	p = gpMenuExports->Draw_CachePic("gfx/box_mr.lmp");
	for(n = 0; n < lines; n++)
	{
		cy += 8;
		M_DrawTransPic(cx, cy, p);
	}
	p = gpMenuExports->Draw_CachePic("gfx/box_br.lmp");
	M_DrawTransPic(cx, cy + 8, p);
}

//=============================================================================

int m_save_demonum;

/*
================
M_ToggleMenu_f
================
*/
void M_ToggleMenu_f()
{
	m_entersound = true;

	if(gpMenuExports->GetKeyDest() == key_menu)
	{
		if(m_state != m_main)
		{
			M_Menu_Main_f();
			return;
		};
		gpMenuExports->SetKeyDest(key_game);
		m_state = m_none;
		return;
	};
	
	if(gpMenuExports->GetKeyDest() == key_console)
		gpMenuExports->Con_ToggleConsole_f();
	else
		M_Menu_Main_f();
};

//=============================================================================
/* MAIN MENU */

int m_main_cursor;
#define MAIN_ITEMS 5

void M_Menu_Main_f()
{
	if(gpMenuExports->GetKeyDest() != key_menu)
	{
		m_save_demonum = gpMenuExports->CL_GetDemoNum();
		gpMenuExports->CL_SetDemoNum(-1);
	}
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_main;
	m_entersound = true;
}

void M_Main_Draw()
{
	int f;
	qpic_t *p;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/ttl_main.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);
	M_DrawTransPic(72, 32, gpMenuExports->Draw_CachePic("gfx/mainmenu.lmp"));

	f = (int)(gpMenuExports->Host_GetTime() * 10) % 6;

	M_DrawTransPic(54, 32 + m_main_cursor * 20, gpMenuExports->Draw_CachePic(va("gfx/menudot%i.lmp", f + 1)));
}

void M_Main_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		gpMenuExports->SetKeyDest(key_game);
		m_state = m_none;
		gpMenuExports->CL_SetDemoNum(m_save_demonum);
		if(gpMenuExports->CL_GetDemoNum() != -1 && !gpMenuExports->CL_DemoPlayback() && gpMenuExports->CL_IsConnected())
			gpMenuExports->CL_NextDemo();
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(++m_main_cursor >= MAIN_ITEMS)
			m_main_cursor = 0;
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(--m_main_cursor < 0)
			m_main_cursor = MAIN_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch(m_main_cursor)
		{
		case 0:
			M_Menu_SinglePlayer_f();
			break;

		case 1:
			M_Menu_MultiPlayer_f();
			break;

		case 2:
			M_Menu_Options_f();
			break;

		case 3:
			M_Menu_Help_f();
			break;

		case 4:
			M_Menu_Quit_f();
			break;
		}
	}
}

//=============================================================================
/* SINGLE PLAYER MENU */

int m_singleplayer_cursor;
#define SINGLEPLAYER_ITEMS 3

void M_Menu_SinglePlayer_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_singleplayer;
	m_entersound = true;
}

void M_SinglePlayer_Draw()
{
	int f;
	qpic_t *p;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/ttl_sgl.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);
	M_DrawTransPic(72, 32, gpMenuExports->Draw_CachePic("gfx/sp_menu.lmp"));

	f = (int)(gpMenuExports->Host_GetTime() * 10) % 6;

	M_DrawTransPic(54, 32 + m_singleplayer_cursor * 20, gpMenuExports->Draw_CachePic(va("gfx/menudot%i.lmp", f + 1)));
}

void M_SinglePlayer_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		M_Menu_Main_f();
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(++m_singleplayer_cursor >= SINGLEPLAYER_ITEMS)
			m_singleplayer_cursor = 0;
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(--m_singleplayer_cursor < 0)
			m_singleplayer_cursor = SINGLEPLAYER_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch(m_singleplayer_cursor)
		{
		case 0:
			if(gpMenuExports->Host_IsServerActive())
				if(!gpMenuExports->SCR_ModalMessage("Are you sure you want to\nstart a new game?\n"))
					break;
			gpMenuExports->SetKeyDest(key_game);
			if(gpMenuExports->Host_IsServerActive())
				gpMenuExports->Cbuf_AddText("disconnect\n");
			gpMenuExports->Cbuf_AddText("maxplayers 1\n");
			gpMenuExports->Cbuf_AddText("map start\n");
			break;

		case 1:
			M_Menu_Load_f();
			break;

		case 2:
			M_Menu_Save_f();
			break;
		}
	}
}

//=============================================================================
/* LOAD/SAVE MENU */

int load_cursor; // 0 < load_cursor < MAX_SAVEGAMES

#define MAX_SAVEGAMES 12
char m_filenames[MAX_SAVEGAMES][SAVEGAME_COMMENT_LENGTH + 1];
int loadable[MAX_SAVEGAMES];

void M_ScanSaves()
{
	int i, j;
	char name[MAX_OSPATH];
	FILE *f;
	int version;

	for(i = 0; i < MAX_SAVEGAMES; i++)
	{
		strcpy(m_filenames[i], "--- UNUSED SLOT ---");
		loadable[i] = false;
		sprintf(name, "%s/s%i.sav", gpEngine->pfnGetGameDirectory(), i);
		f = fopen(name, "r");
		if(!f)
			continue;
		fscanf(f, "%i\n", &version);
		fscanf(f, "%79s\n", name);
		strncpy(m_filenames[i], name, sizeof(m_filenames[i]) - 1);

		// change _ back to space
		for(j = 0; j < SAVEGAME_COMMENT_LENGTH; j++)
			if(m_filenames[i][j] == '_')
				m_filenames[i][j] = ' ';
		loadable[i] = true;
		fclose(f);
	}
}

void M_Menu_Load_f()
{
	m_entersound = true;
	m_state = m_load;
	gpMenuExports->SetKeyDest(key_menu);
	M_ScanSaves();
}

void M_Menu_Save_f()
{
	if(!gpMenuExports->Host_IsServerActive())
		return;
	if(gpMenuExports->CL_Intermission())
		return;
	if(gpMenuExports->GetMaxClients() != 1)
		return;
	m_entersound = true;
	m_state = m_save;
	gpMenuExports->SetKeyDest(key_menu);
	M_ScanSaves();
}

void M_Load_Draw()
{
	int i;
	qpic_t *p;

	p = gpMenuExports->Draw_CachePic("gfx/p_load.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	for(i = 0; i < MAX_SAVEGAMES; i++)
		M_Print(16, 32 + 8 * i, m_filenames[i]);

	// line cursor
	M_DrawCharacter(8, 32 + load_cursor * 8, 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));
}

void M_Save_Draw()
{
	int i;
	qpic_t *p;

	p = gpMenuExports->Draw_CachePic("gfx/p_save.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	for(i = 0; i < MAX_SAVEGAMES; i++)
		M_Print(16, 32 + 8 * i, m_filenames[i]);

	// line cursor
	M_DrawCharacter(8, 32 + load_cursor * 8, 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));
}

void M_Load_Key(int k)
{
	switch(k)
	{
	case K_ESCAPE:
		M_Menu_SinglePlayer_f();
		break;

	case K_ENTER:
		gpMenuExports->S_LocalSound("misc/menu2.wav");
		if(!loadable[load_cursor])
			return;
		m_state = m_none;
		gpMenuExports->SetKeyDest(key_game);

		// Host_Loadgame_f can't bring up the loading plaque because too much
		// stack space has been used, so do it now
		gpMenuExports->SCR_BeginLoadingPlaque();

		// issue the load command
		gpMenuExports->Cbuf_AddText(va("load s%i\n", load_cursor));
		return;

	case K_UPARROW:
	case K_LEFTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		load_cursor--;
		if(load_cursor < 0)
			load_cursor = MAX_SAVEGAMES - 1;
		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		load_cursor++;
		if(load_cursor >= MAX_SAVEGAMES)
			load_cursor = 0;
		break;
	}
}

void M_Save_Key(int k)
{
	switch(k)
	{
	case K_ESCAPE:
		M_Menu_SinglePlayer_f();
		break;

	case K_ENTER:
		m_state = m_none;
		gpMenuExports->SetKeyDest(key_game);
		gpMenuExports->Cbuf_AddText(va("save s%i\n", load_cursor));
		return;

	case K_UPARROW:
	case K_LEFTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		load_cursor--;
		if(load_cursor < 0)
			load_cursor = MAX_SAVEGAMES - 1;
		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		load_cursor++;
		if(load_cursor >= MAX_SAVEGAMES)
			load_cursor = 0;
		break;
	}
}

//=============================================================================
/* MULTIPLAYER MENU */

int m_multiplayer_cursor;
#define MULTIPLAYER_ITEMS 3

void M_Menu_MultiPlayer_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_multiplayer;
	m_entersound = true;
}

void M_MultiPlayer_Draw()
{
	int f;
	qpic_t *p;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);
	M_DrawTransPic(72, 32, gpMenuExports->Draw_CachePic("gfx/mp_menu.lmp"));

	f = (int)(gpMenuExports->Host_GetTime() * 10) % 6;

	M_DrawTransPic(54, 32 + m_multiplayer_cursor * 20, gpMenuExports->Draw_CachePic(va("gfx/menudot%i.lmp", f + 1)));

	//if(serialAvailable || ipxAvailable || tcpipAvailable) // TODO
		//return;
	//M_PrintWhite((320 / 2) - ((27 * 8) / 2), 148, "No Communications Available");
}

void M_MultiPlayer_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		M_Menu_Main_f();
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(++m_multiplayer_cursor >= MULTIPLAYER_ITEMS)
			m_multiplayer_cursor = 0;
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(--m_multiplayer_cursor < 0)
			m_multiplayer_cursor = MULTIPLAYER_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		switch(m_multiplayer_cursor)
		{
		case 0:
			//if(serialAvailable || ipxAvailable || tcpipAvailable) // TODO
				M_Menu_Net_f();
			break;

		case 1:
			//if(serialAvailable || ipxAvailable || tcpipAvailable) // TODO
				M_Menu_Net_f();
			break;

		case 2:
			M_Menu_Setup_f();
			break;
		}
	}
}

//=============================================================================
/* SETUP MENU */

int setup_cursor = 4;
int setup_cursor_table[] = { 40, 56, 80, 104, 140 };

char setup_hostname[16];
char setup_myname[16];
int setup_oldtop;
int setup_oldbottom;
int setup_top;
int setup_bottom;

#define NUM_SETUP_CMDS 5

void M_Menu_Setup_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_setup;
	m_entersound = true;
	strcpy(setup_myname, gpEngine->pfnGetCvarString("name"));
	strcpy(setup_hostname, gpEngine->pfnGetCvarString("hostname"));
	setup_top = setup_oldtop = ((int)gpEngine->pfnGetCvarFloat("topcolor")) >> 4;
	setup_bottom = setup_oldbottom = ((int)gpEngine->pfnGetCvarFloat("bottomcolor")) & 15;
}

void M_Setup_Draw()
{
	qpic_t *p;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	M_Print(64, 40, "Hostname");
	M_DrawTextBox(160, 32, 16, 1);
	M_Print(168, 40, setup_hostname);

	M_Print(64, 56, "Your name");
	M_DrawTextBox(160, 48, 16, 1);
	M_Print(168, 56, setup_myname);

	M_Print(64, 80, "Shirt color");
	M_Print(64, 104, "Pants color");

	M_DrawTextBox(64, 140 - 8, 14, 1);
	M_Print(72, 140, "Accept Changes");

	p = gpMenuExports->Draw_CachePic("gfx/bigbox.lmp");
	M_DrawTransPic(160, 64, p);
	p = gpMenuExports->Draw_CachePic("gfx/menuplyr.lmp");
	M_BuildTranslationTable(setup_top * 16, setup_bottom * 16);
	M_DrawTransPicTranslate(172, 72, p);

	M_DrawCharacter(56, setup_cursor_table[setup_cursor], 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(setup_cursor == 0)
		M_DrawCharacter(168 + 8 * strlen(setup_hostname), setup_cursor_table[setup_cursor], 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(setup_cursor == 1)
		M_DrawCharacter(168 + 8 * strlen(setup_myname), setup_cursor_table[setup_cursor], 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));
}

void M_Setup_Key(int k)
{
	int l;

	switch(k)
	{
	case K_ESCAPE:
		M_Menu_MultiPlayer_f();
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		setup_cursor--;
		if(setup_cursor < 0)
			setup_cursor = NUM_SETUP_CMDS - 1;
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		setup_cursor++;
		if(setup_cursor >= NUM_SETUP_CMDS)
			setup_cursor = 0;
		break;

	case K_LEFTARROW:
		if(setup_cursor < 2)
			return;
		gpMenuExports->S_LocalSound("misc/menu3.wav");
		if(setup_cursor == 2)
			setup_top = setup_top - 1;
		if(setup_cursor == 3)
			setup_bottom = setup_bottom - 1;
		break;
	case K_RIGHTARROW:
		if(setup_cursor < 2)
			return;
	forward:
		gpMenuExports->S_LocalSound("misc/menu3.wav");
		if(setup_cursor == 2)
			setup_top = setup_top + 1;
		if(setup_cursor == 3)
			setup_bottom = setup_bottom + 1;
		break;

	case K_ENTER:
		if(setup_cursor == 0 || setup_cursor == 1)
			return;

		if(setup_cursor == 2 || setup_cursor == 3)
			goto forward;

		// setup_cursor == 4 (OK)
		if(strcmp(gpEngine->pfnGetCvarString("name"), setup_myname) != 0)
			gpMenuExports->Cbuf_AddText(va("name \"%s\"\n", setup_myname));
		if(strcmp(gpEngine->pfnGetCvarString("hostname"), setup_hostname) != 0)
			gpMenuExports->Cvar_Set("hostname", setup_hostname);
		if(setup_top != setup_oldtop || setup_bottom != setup_oldbottom)
			gpMenuExports->Cbuf_AddText(va("color %i %i\n", setup_top, setup_bottom));
		m_entersound = true;
		M_Menu_MultiPlayer_f();
		break;

	case K_BACKSPACE:
		if(setup_cursor == 0)
		{
			if(strlen(setup_hostname))
				setup_hostname[strlen(setup_hostname) - 1] = 0;
		}

		if(setup_cursor == 1)
		{
			if(strlen(setup_myname))
				setup_myname[strlen(setup_myname) - 1] = 0;
		}
		break;

	default:
		if(k < 32 || k > 127)
			break;
		if(setup_cursor == 0)
		{
			l = strlen(setup_hostname);
			if(l < 15)
			{
				setup_hostname[l + 1] = 0;
				setup_hostname[l] = k;
			}
		}
		if(setup_cursor == 1)
		{
			l = strlen(setup_myname);
			if(l < 15)
			{
				setup_myname[l + 1] = 0;
				setup_myname[l] = k;
			}
		}
	}

	if(setup_top > 13)
		setup_top = 0;
	if(setup_top < 0)
		setup_top = 13;
	if(setup_bottom > 13)
		setup_bottom = 0;
	if(setup_bottom < 0)
		setup_bottom = 13;
}

//=============================================================================
/* NET MENU */

int m_net_cursor;
int m_net_items;
int m_net_saveHeight;

const char *net_helpMessage[] =
{
  /* .........1.........2.... */
  "                        ",
  " Two computers connected",
  "   through two modems.  ",
  "                        ",

  "                        ",
  " Two computers connected",
  " by a null-modem cable. ",
  "                        ",

  " Novell network LANs    ",
  " or Windows 95 DOS-box. ",
  "                        ",
  "(LAN=Local Area Network)",

  " Commonly used to play  ",
  " over the Internet, but ",
  " also used on a Local   ",
  " Area Network.          "
};

void M_Menu_Net_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_net;
	m_entersound = true;
	m_net_items = 4;

	if(m_net_cursor >= m_net_items)
		m_net_cursor = 0;
	m_net_cursor--;
	M_Net_Key(K_DOWNARROW);
}

void M_Net_Draw()
{
	int f;
	qpic_t *p;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	f = 32;

	//if(serialAvailable) // TODO
	//{
		//p = gpMenuExports->Draw_CachePic("gfx/netmen1.lmp");
	//}
	//else
	{
#ifdef _WIN32
		p = NULL;
#else
		p = gpMenuExports->Draw_CachePic("gfx/dim_modm.lmp");
#endif
	}

	if(p)
		M_DrawTransPic(72, f, p);

	f += 19;

	//if(serialAvailable) // TODO
	//{
		//p = gpMenuExports->Draw_CachePic("gfx/netmen2.lmp");
	//}
	//else
	{
#ifdef _WIN32
		p = NULL;
#else
		p = gpMenuExports->Draw_CachePic("gfx/dim_drct.lmp");
#endif
	}

	if(p)
		M_DrawTransPic(72, f, p);

	f += 19;
	//if(ipxAvailable) // TODO
		//p = gpMenuExports->Draw_CachePic("gfx/netmen3.lmp");
	//else
		p = gpMenuExports->Draw_CachePic("gfx/dim_ipx.lmp");
	M_DrawTransPic(72, f, p);

	f += 19;
	//if(tcpipAvailable) // TODO
		//p = gpMenuExports->Draw_CachePic("gfx/netmen4.lmp");
	//else
		p = gpMenuExports->Draw_CachePic("gfx/dim_tcp.lmp");
	M_DrawTransPic(72, f, p);

	if(m_net_items == 5) // JDC, could just be removed
	{
		f += 19;
		p = gpMenuExports->Draw_CachePic("gfx/netmen5.lmp");
		M_DrawTransPic(72, f, p);
	}

	f = (320 - 26 * 8) / 2;
	M_DrawTextBox(f, 134, 24, 4);
	f += 8;
	M_Print(f, 142, net_helpMessage[m_net_cursor * 4 + 0]);
	M_Print(f, 150, net_helpMessage[m_net_cursor * 4 + 1]);
	M_Print(f, 158, net_helpMessage[m_net_cursor * 4 + 2]);
	M_Print(f, 166, net_helpMessage[m_net_cursor * 4 + 3]);

	f = (int)(gpMenuExports->Host_GetTime() * 10) % 6;
	M_DrawTransPic(54, 32 + m_net_cursor * 20, gpMenuExports->Draw_CachePic(va("gfx/menudot%i.lmp", f + 1)));
}

void M_Net_Key(int k)
{
again:
	switch(k)
	{
	case K_ESCAPE:
		M_Menu_MultiPlayer_f();
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(++m_net_cursor >= m_net_items)
			m_net_cursor = 0;
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(--m_net_cursor < 0)
			m_net_cursor = m_net_items - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch(m_net_cursor)
		{
		case 0:
			M_Menu_SerialConfig_f();
			break;

		case 1:
			M_Menu_SerialConfig_f();
			break;

		case 2:
			M_Menu_LanConfig_f();
			break;

		case 3:
			M_Menu_LanConfig_f();
			break;

		case 4:
			// multiprotocol
			break;
		}
	}

	// TODO
	//if(m_net_cursor == 0 && !serialAvailable)
		//goto again;
	//if(m_net_cursor == 1 && !serialAvailable)
		//goto again;
	//if(m_net_cursor == 2 && !ipxAvailable)
		//goto again;
	//if(m_net_cursor == 3 && !tcpipAvailable)
		//goto again;
}

//=============================================================================
/* OPTIONS MENU */

#ifdef _WIN32
#define OPTIONS_ITEMS 14
#else
#define OPTIONS_ITEMS 13
#endif

#define SLIDER_RANGE 10

int options_cursor;

void M_Menu_Options_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_options;
	m_entersound = true;

#ifdef _WIN32
	if((options_cursor == 13) && (!gpMenuExports->VID_IsWindowed()))
	{
		options_cursor = 0;
	}
#endif
}

void M_AdjustSliders(int dir)
{
	gpMenuExports->S_LocalSound("misc/menu3.wav");

	switch(options_cursor)
	{
	case 3: // screen size
		gpMenuExports->Cvar_SetValue("scr_viewsize", gpMenuExports->Cvar_VariableValue("scr_viewsize") + dir * 10);
		if(gpMenuExports->Cvar_VariableValue("scr_viewsize") < 30)
			gpMenuExports->Cvar_SetValue("scr_viewsize", 30);
		if(gpMenuExports->Cvar_VariableValue("scr_viewsize") > 120)
			gpMenuExports->Cvar_SetValue("scr_viewsize", 120);
		gpMenuExports->Cvar_SetValue("viewsize", gpMenuExports->Cvar_VariableValue("scr_viewsize"));
		break;
	case 4: // gamma
		gpMenuExports->Cvar_SetValue("v_gamma", gpMenuExports->Cvar_VariableValue("v_gamma") - dir * 0.05);
		if(gpMenuExports->Cvar_VariableValue("v_gamma") < 0.5)
			gpMenuExports->Cvar_SetValue("v_gamma", 0.5);
		if(gpMenuExports->Cvar_VariableValue("v_gamma") > 1)
			gpMenuExports->Cvar_SetValue("v_gamma", 1);
		gpMenuExports->Cvar_SetValue("gamma", gpMenuExports->Cvar_VariableValue("v_gamma"));
		break;
	case 5: // mouse speed
		gpMenuExports->Cvar_SetValue("sensitivity", gpMenuExports->Cvar_VariableValue("sensitivity") + dir * 0.5);
		if(gpMenuExports->Cvar_VariableValue("sensitivity") < 1)
			gpMenuExports->Cvar_SetValue("sensitivity", 1);
		if(gpMenuExports->Cvar_VariableValue("sensitivity") > 11)
			gpMenuExports->Cvar_SetValue("sensitivity", 11);
		gpMenuExports->Cvar_SetValue("sensitivity", gpMenuExports->Cvar_VariableValue("sensitivity"));
		break;
	case 6: // music volume
#ifdef _WIN32
		gpMenuExports->Cvar_SetValue("bgmvolume", gpMenuExports->Cvar_VariableValue("bgmvolume") + dir * 1.0);
#else
		gpMenuExports->Cvar_SetValue("bgmvolume", gpMenuExports->Cvar_VariableValue("bgmvolume") + dir * 0.1);
#endif
		if(gpMenuExports->Cvar_VariableValue("bgmvolume") < 0)
			gpMenuExports->Cvar_SetValue("bgmvolume", 0);
		if(gpMenuExports->Cvar_VariableValue("bgmvolume") > 1)
			gpMenuExports->Cvar_SetValue("bgmvolume", 1);
		gpMenuExports->Cvar_SetValue("bgmvolume", gpMenuExports->Cvar_VariableValue("bgmvolume"));
		break;
	case 7: // sfx volume
		gpEngine->Cvar_SetValue("volume", gpEngine->pfnGetCvarFloat("volume") + dir * 0.1);
		if(gpMenuExports->pfnGetCvarFloat("volume") < 0)
			gpEngine->Cvar_SetValue("volume", 0);
		if(gpEngine->pfnGetCvarFloat("volume") > 1)
			gpEngine->Cvar_SetValue("volume", 1);
		gpEngine->Cvar_SetValue("volume", gpEngine->pfnGetCvarFloat("volume"));
		break;

	case 8: // allways run
		if(gpMenuExports->pfnGetCvarFloat("cl_forwardspeed") > 200)
		{
			gpEngine->Cvar_SetValue("cl_forwardspeed", 200);
			gpEngine->Cvar_SetValue("cl_backspeed", 200);
		}
		else
		{
			gpEngine->Cvar_SetValue("cl_forwardspeed", 400);
			gpEngine->Cvar_SetValue("cl_backspeed", 400);
		}
		break;

	case 9: // invert mouse
		gpEngine->Cvar_SetValue("m_pitch", -gpEngine->pfnGetCvarFloat("m_pitch"));
		break;

	case 10: // lookspring
		gpEngine->Cvar_SetValue("lookspring", !gpEngine->pfnGetCvarFloat("lookspring"));
		break;

	case 11: // lookstrafe
		gpEngine->Cvar_SetValue("lookstrafe", !gpEngine->pfnGetCvarFloat("lookstrafe"));
		break;

#ifdef _WIN32
	case 13: // _windowed_mouse
		//gpEngine->Cvar_SetValue("_windowed_mouse", !gpEngine->pfnGetCvarFloat("_windowed_mouse")); // TODO
		break;
#endif
	}
}

void M_DrawSlider(int x, int y, float range)
{
	int i;

	if(range < 0)
		range = 0;
	if(range > 1)
		range = 1;
	M_DrawCharacter(x - 8, y, 128);
	for(i = 0; i < SLIDER_RANGE; i++)
		M_DrawCharacter(x + i * 8, y, 129);
	M_DrawCharacter(x + i * 8, y, 130);
	M_DrawCharacter(x + (SLIDER_RANGE - 1) * 8 * range, y, 131);
}

void M_DrawCheckbox(int x, int y, int on)
{
#if 0
	if (on)
		M_DrawCharacter (x, y, 131);
	else
		M_DrawCharacter (x, y, 129);
#endif
	if(on)
		M_Print(x, y, "on");
	else
		M_Print(x, y, "off");
}

void M_Options_Draw()
{
	float r;
	qpic_t *p;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_option.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	M_Print(16, 32, "    Customize controls");
	M_Print(16, 40, "         Go to console");
	M_Print(16, 48, "     Reset to defaults");

	M_Print(16, 56, "           Screen size");
	r = (gpMenuExports->Cvar_VariableValue("scr_viewsize") - 30) / (120 - 30);
	M_DrawSlider(220, 56, r);

	M_Print(16, 64, "            Brightness");
	r = (1.0 - gpMenuExports->Cvar_VariableValue("v_gamma")) / 0.5;
	M_DrawSlider(220, 64, r);

	M_Print(16, 72, "           Mouse Speed");
	r = (gpMenuExports->Cvar_VariableValue("sensitivity") - 1) / 10;
	M_DrawSlider(220, 72, r);

	M_Print(16, 80, "       CD Music Volume");
	r = gpMenuExports->Cvar_VariableValue("bgmvolume");
	M_DrawSlider(220, 80, r);

	M_Print(16, 88, "          Sound Volume");
	r = gpMenuExports->Cvar_VariableValue("volume");
	M_DrawSlider(220, 88, r);

	M_Print(16, 96, "            Always Run");
	M_DrawCheckbox(220, 96, gpMenuExports->Cvar_VariableValue("cl_forwardspeed") > 200);

	M_Print(16, 104, "          Invert Mouse");
	M_DrawCheckbox(220, 104, gpMenuExports->Cvar_VariableValue("m_pitch") < 0);

	M_Print(16, 112, "            Lookspring");
	M_DrawCheckbox(220, 112, gpMenuExports->Cvar_VariableValue("lookspring"));

	M_Print(16, 120, "            Lookstrafe");
	M_DrawCheckbox(220, 120, gpMenuExports->Cvar_VariableValue("lookstrafe"));

	if(vid_menudrawfn)
		M_Print(16, 128, "         Video Options");

#ifdef _WIN32
	if(gpMenuExports->VID_IsWindowed())
	{
		M_Print(16, 136, "             Use Mouse");
		//M_DrawCheckbox(220, 136, gpMenuExports->Cvar_VariableValue("_windowed_mouse")); // TODO
	}
#endif

	// cursor
	M_DrawCharacter(200, 32 + options_cursor * 8, 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));
}

void M_Options_Key(int k)
{
	switch(k)
	{
	case K_ESCAPE:
		M_Menu_Main_f();
		break;

	case K_ENTER:
		m_entersound = true;
		switch(options_cursor)
		{
		case 0:
			M_Menu_Keys_f();
			break;
		case 1:
			m_state = m_none;
			gpMenuExports->Con_ToggleConsole_f();
			break;
		case 2:
			gpMenuExports->Cbuf_AddText("exec default.cfg\n");
			break;
		case 12:
			M_Menu_Video_f();
			break;
		default:
			M_AdjustSliders(1);
			break;
		}
		return;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		options_cursor--;
		if(options_cursor < 0)
			options_cursor = OPTIONS_ITEMS - 1;
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		options_cursor++;
		if(options_cursor >= OPTIONS_ITEMS)
			options_cursor = 0;
		break;

	case K_LEFTARROW:
		M_AdjustSliders(-1);
		break;

	case K_RIGHTARROW:
		M_AdjustSliders(1);
		break;
	}

	if(options_cursor == 12 && vid_menudrawfn == NULL)
	{
		if(k == K_UPARROW)
			options_cursor = 11;
		else
			options_cursor = 0;
	}

#ifdef _WIN32
	if((options_cursor == 13) && (!gpMenuExports->VID_IsWindowed()))
	{
		if(k == K_UPARROW)
			options_cursor = 12;
		else
			options_cursor = 0;
	}
#endif
}

//=============================================================================
/* KEYS MENU */

const char *bindnames[][2] =
{
  { "+attack", "attack" },
  { "impulse 10", "change weapon" },
  { "+jump", "jump / swim up" },
  { "+forward", "walk forward" },
  { "+back", "backpedal" },
  { "+left", "turn left" },
  { "+right", "turn right" },
  { "+speed", "run" },
  { "+moveleft", "step left" },
  { "+moveright", "step right" },
  { "+strafe", "sidestep" },
  { "+lookup", "look up" },
  { "+lookdown", "look down" },
  { "centerview", "center view" },
  { "+mlook", "mouse look" },
  { "+klook", "keyboard look" },
  { "+moveup", "swim up" },
  { "+movedown", "swim down" }
};

#define NUMCOMMANDS (sizeof(bindnames) / sizeof(bindnames[0]))

int keys_cursor;
int bind_grab;

void M_Menu_Keys_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_keys;
	m_entersound = true;
}

void M_FindKeysForCommand(const char *command, int *twokeys)
{
	int count;
	int j;
	int l;
	const char *b;

	twokeys[0] = twokeys[1] = -1;
	l = strlen(command);
	count = 0;

	for(j = 0; j < 256; j++)
	{
		b = gpGameUIFuncs->Key_BindingForKey(j);
		if(!b)
			continue;
		if(!strncmp(b, command, l))
		{
			twokeys[count] = j;
			count++;
			if(count == 2)
				break;
		}
	}
}

void M_UnbindCommand(const char *command)
{
	int j;
	int l;
	const char *b;

	l = strlen(command);

	for(j = 0; j < 256; j++)
	{
		b = gpGameUIFuncs->Key_BindingForKey(j);
		if(!b)
			continue;
		if(!strncmp(b, command, l))
			gpMenuExports->Key_SetBinding(j, "");
	}
}

void M_Keys_Draw()
{
	int i, l;
	int keys[2];
	const char *name;
	int x, y;
	qpic_t *p;

	p = gpMenuExports->Draw_CachePic("gfx/ttl_cstm.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	if(bind_grab)
		M_Print(12, 32, "Press a key or button for this action");
	else
		M_Print(18, 32, "Enter to change, backspace to clear");

	// search for known bindings
	for(i = 0; i < NUMCOMMANDS; i++)
	{
		y = 48 + 8 * i;

		M_Print(16, y, bindnames[i][1]);

		l = strlen(bindnames[i][0]);

		M_FindKeysForCommand(bindnames[i][0], keys);

		if(keys[0] == -1)
		{
			M_Print(140, y, "???");
		}
		else
		{
			name = gpGameUIFuncs->Key_NameForKey(keys[0]);
			M_Print(140, y, name);
			x = strlen(name) * 8;
			if(keys[1] != -1)
			{
				M_Print(140 + x + 8, y, "or");
				M_Print(140 + x + 32, y, gpGameUIFuncs->Key_NameForKey(keys[1]));
			}
		}
	}

	if(bind_grab)
		M_DrawCharacter(130, 48 + keys_cursor * 8, '=');
	else
		M_DrawCharacter(130, 48 + keys_cursor * 8, 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));
}

void M_Keys_Key(int k)
{
	char cmd[80];
	int keys[2];

	if(bind_grab)
	{ // defining a key
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		if(k == K_ESCAPE)
		{
			bind_grab = false;
		}
		else if(k != '`')
		{
			sprintf(cmd, "bind \"%s\" \"%s\"\n", gpGameUIFuncs->Key_NameForKey(k), bindnames[keys_cursor][0]);
			gpMenuExports->Cbuf_InsertText(cmd);
		}

		bind_grab = false;
		return;
	}

	switch(k)
	{
	case K_ESCAPE:
		M_Menu_Options_f();
		break;

	case K_LEFTARROW:
	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		keys_cursor--;
		if(keys_cursor < 0)
			keys_cursor = NUMCOMMANDS - 1;
		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		keys_cursor++;
		if(keys_cursor >= NUMCOMMANDS)
			keys_cursor = 0;
		break;

	case K_ENTER: // go into bind mode
		M_FindKeysForCommand(bindnames[keys_cursor][0], keys);
		gpMenuExports->S_LocalSound("misc/menu2.wav");
		if(keys[1] != -1)
			M_UnbindCommand(bindnames[keys_cursor][0]);
		bind_grab = true;
		break;

	case K_BACKSPACE: // delete bindings
	case K_DEL:       // delete bindings
		gpMenuExports->S_LocalSound("misc/menu2.wav");
		M_UnbindCommand(bindnames[keys_cursor][0]);
		break;
	}
}

//=============================================================================
/* VIDEO MENU */

void M_Menu_Video_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_video;
	m_entersound = true;
}

void M_Video_Draw()
{
	(*vid_menudrawfn)();
}

void M_Video_Key(int key)
{
	(*vid_menukeyfn)(key);
}

//=============================================================================
/* HELP MENU */

int help_page;
#define NUM_HELP_PAGES 6

void M_Menu_Help_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_help;
	m_entersound = true;
	help_page = 0;
}

void M_Help_Draw()
{
	M_DrawPic(0, 0, gpMenuExports->Draw_CachePic(va("gfx/help%i.lmp", help_page)));
}

void M_Help_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		M_Menu_Main_f();
		break;

	case K_UPARROW:
	case K_RIGHTARROW:
		m_entersound = true;
		if(++help_page >= NUM_HELP_PAGES)
			help_page = 0;
		break;

	case K_DOWNARROW:
	case K_LEFTARROW:
		m_entersound = true;
		if(--help_page < 0)
			help_page = NUM_HELP_PAGES - 1;
		break;
	}
}

//=============================================================================
/* QUIT MENU */

int msgNumber;
menu_state_e m_quit_prevstate;
bool wasInMenus;

#ifndef _WIN32
const char *quitMessage[] =
{
  /* .........1.........2.... */
  "  Are you gonna quit    ",
  "  this game just like   ",
  "   everything else?     ",
  "                        ",

  " Milord, methinks that  ",
  "   thou art a lowly     ",
  " quitter. Is this true? ",
  "                        ",

  " Do I need to bust your ",
  "  face open for trying  ",
  "        to quit?        ",
  "                        ",

  " Man, I oughta smack you",
  "   for trying to quit!  ",
  "     Press Y to get     ",
  "      smacked out.      ",

  " Press Y to quit like a ",
  "   big loser in life.   ",
  "  Press N to stay proud ",
  "    and successful!     ",

  "   If you press Y to    ",
  "  quit, I will summon   ",
  "  Satan all over your   ",
  "      hard drive!       ",

  "  Um, Asmodeus dislikes ",
  " his children trying to ",
  " quit. Press Y to return",
  "   to your Tinkertoys.  ",

  "  If you quit now, I'll ",
  "  throw a blanket-party ",
  "   for you next time!   ",
  "                        "
};
#endif

void M_Menu_Quit_f()
{
	if(m_state == m_quit)
		return;
	wasInMenus = (gpMenuExports->GetKeyDest() == key_menu);
	gpMenuExports->SetKeyDest(key_menu);
	m_quit_prevstate = m_state;
	m_state = m_quit;
	m_entersound = true;
	msgNumber = rand() & 7;
}

void M_Quit_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
	case 'n':
	case 'N':
		if(wasInMenus)
		{
			m_state = m_quit_prevstate;
			m_entersound = true;
		}
		else
		{
			gpMenuExports->SetKeyDest(key_game);
			m_state = m_none;
		}
		break;

	case 'Y':
	case 'y':
		gpMenuExports->SetKeyDest(key_console);
		gpMenuExports->Host_Quit_f();
		break;

	default:
		break;
	}
}

void M_Quit_Draw()
{
	if(wasInMenus)
	{
		m_state = m_quit_prevstate;
		m_recursiveDraw = true;
		M_Draw();
		m_state = m_quit;
	}

#ifdef _WIN32
	M_DrawTextBox(0, 0, 38, 23);
	M_PrintWhite(16, 12, "  Quake version 1.09 by id Software\n\n");
	M_PrintWhite(16, 28, "Programming        Art \n");
	M_Print(16, 36, " John Carmack       Adrian Carmack\n");
	M_Print(16, 44, " Michael Abrash     Kevin Cloud\n");
	M_Print(16, 52, " John Cash          Paul Steed\n");
	M_Print(16, 60, " Dave 'Zoid' Kirsch\n");
	M_PrintWhite(16, 68, "Design             Biz\n");
	M_Print(16, 76, " John Romero        Jay Wilbur\n");
	M_Print(16, 84, " Sandy Petersen     Mike Wilson\n");
	M_Print(16, 92, " American McGee     Donna Jackson\n");
	M_Print(16, 100, " Tim Willits        Todd Hollenshead\n");
	M_PrintWhite(16, 108, "Support            Projects\n");
	M_Print(16, 116, " Barrett Alexander  Shawn Green\n");
	M_PrintWhite(16, 124, "Sound Effects\n");
	M_Print(16, 132, " Trent Reznor and Nine Inch Nails\n\n");
	M_PrintWhite(16, 140, "Quake is a trademark of Id Software,\n");
	M_PrintWhite(16, 148, "inc., (c)1996 Id Software, inc. All\n");
	M_PrintWhite(16, 156, "rights reserved. NIN logo is a\n");
	M_PrintWhite(16, 164, "registered trademark licensed to\n");
	M_PrintWhite(16, 172, "Nothing Interactive, Inc. All rights\n");
	M_PrintWhite(16, 180, "reserved. Press y to exit\n");
#else
	M_DrawTextBox(56, 76, 24, 4);
	M_Print(64, 84, quitMessage[msgNumber * 4 + 0]);
	M_Print(64, 92, quitMessage[msgNumber * 4 + 1]);
	M_Print(64, 100, quitMessage[msgNumber * 4 + 2]);
	M_Print(64, 108, quitMessage[msgNumber * 4 + 3]);
#endif
}

//=============================================================================

/* SERIAL CONFIG MENU */

int serialConfig_cursor;
int serialConfig_cursor_table[] = { 48, 64, 80, 96, 112, 132 };
#define NUM_SERIALCONFIG_CMDS 6

static int ISA_uarts[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
static int ISA_IRQs[] = { 4, 3, 4, 3 };
int serialConfig_baudrate[] = { 9600, 14400, 19200, 28800, 38400, 57600 };

int serialConfig_comport;
int serialConfig_irq;
int serialConfig_baud;
char serialConfig_phone[16];

void M_Menu_SerialConfig_f()
{
	int n;
	int port;
	int baudrate;
	bool useModem;

	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_serialconfig;
	m_entersound = true;
	if(JoiningGame && SerialConfig)
		serialConfig_cursor = 4;
	else
		serialConfig_cursor = 5;

	//(*GetComPortConfig)(0, &port, &serialConfig_irq, &baudrate, &useModem); // TODO

	// map uart's port to COMx
	for(n = 0; n < 4; n++)
		if(ISA_uarts[n] == port)
			break;
	if(n == 4)
	{
		n = 0;
		serialConfig_irq = 4;
	}
	serialConfig_comport = n + 1;

	// map baudrate to index
	for(n = 0; n < 6; n++)
		if(serialConfig_baudrate[n] == baudrate)
			break;
	if(n == 6)
		n = 5;
	serialConfig_baud = n;

	m_return_onerror = false;
	m_return_reason[0] = 0;
}

void M_SerialConfig_Draw()
{
	qpic_t *p;
	int basex;
	const char *startJoin;
	const char *directModem;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	basex = (320 - p->width) / 2;
	M_DrawPic(basex, 4, p);

	if(StartingGame)
		startJoin = "New Game";
	else
		startJoin = "Join Game";
	if(SerialConfig)
		directModem = "Modem";
	else
		directModem = "Direct Connect";
	M_Print(basex, 32, va("%s - %s", startJoin, directModem));
	basex += 8;

	M_Print(basex, serialConfig_cursor_table[0], "Port");
	M_DrawTextBox(160, 40, 4, 1);
	M_Print(168, serialConfig_cursor_table[0], va("COM%u", serialConfig_comport));

	M_Print(basex, serialConfig_cursor_table[1], "IRQ");
	M_DrawTextBox(160, serialConfig_cursor_table[1] - 8, 1, 1);
	M_Print(168, serialConfig_cursor_table[1], va("%u", serialConfig_irq));

	M_Print(basex, serialConfig_cursor_table[2], "Baud");
	M_DrawTextBox(160, serialConfig_cursor_table[2] - 8, 5, 1);
	M_Print(168, serialConfig_cursor_table[2], va("%u", serialConfig_baudrate[serialConfig_baud]));

	if(SerialConfig)
	{
		M_Print(basex, serialConfig_cursor_table[3], "Modem Setup...");
		if(JoiningGame)
		{
			M_Print(basex, serialConfig_cursor_table[4], "Phone number");
			M_DrawTextBox(160, serialConfig_cursor_table[4] - 8, 16, 1);
			M_Print(168, serialConfig_cursor_table[4], serialConfig_phone);
		}
	}

	if(JoiningGame)
	{
		M_DrawTextBox(basex, serialConfig_cursor_table[5] - 8, 7, 1);
		M_Print(basex + 8, serialConfig_cursor_table[5], "Connect");
	}
	else
	{
		M_DrawTextBox(basex, serialConfig_cursor_table[5] - 8, 2, 1);
		M_Print(basex + 8, serialConfig_cursor_table[5], "OK");
	}

	M_DrawCharacter(basex - 8, serialConfig_cursor_table[serialConfig_cursor], 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(serialConfig_cursor == 4)
		M_DrawCharacter(168 + 8 * strlen(serialConfig_phone), serialConfig_cursor_table[serialConfig_cursor], 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(*m_return_reason)
		M_PrintWhite(basex, 148, m_return_reason);
}

void M_SerialConfig_Key(int key)
{
	int l;

	switch(key)
	{
	case K_ESCAPE:
		M_Menu_Net_f();
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		serialConfig_cursor--;
		if(serialConfig_cursor < 0)
			serialConfig_cursor = NUM_SERIALCONFIG_CMDS - 1;
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		serialConfig_cursor++;
		if(serialConfig_cursor >= NUM_SERIALCONFIG_CMDS)
			serialConfig_cursor = 0;
		break;

	case K_LEFTARROW:
		if(serialConfig_cursor > 2)
			break;
		gpMenuExports->S_LocalSound("misc/menu3.wav");

		if(serialConfig_cursor == 0)
		{
			serialConfig_comport--;
			if(serialConfig_comport == 0)
				serialConfig_comport = 4;
			serialConfig_irq = ISA_IRQs[serialConfig_comport - 1];
		}

		if(serialConfig_cursor == 1)
		{
			serialConfig_irq--;
			if(serialConfig_irq == 6)
				serialConfig_irq = 5;
			if(serialConfig_irq == 1)
				serialConfig_irq = 7;
		}

		if(serialConfig_cursor == 2)
		{
			serialConfig_baud--;
			if(serialConfig_baud < 0)
				serialConfig_baud = 5;
		}

		break;

	case K_RIGHTARROW:
		if(serialConfig_cursor > 2)
			break;
	forward:
		gpMenuExports->S_LocalSound("misc/menu3.wav");

		if(serialConfig_cursor == 0)
		{
			serialConfig_comport++;
			if(serialConfig_comport > 4)
				serialConfig_comport = 1;
			serialConfig_irq = ISA_IRQs[serialConfig_comport - 1];
		}

		if(serialConfig_cursor == 1)
		{
			serialConfig_irq++;
			if(serialConfig_irq == 6)
				serialConfig_irq = 7;
			if(serialConfig_irq == 8)
				serialConfig_irq = 2;
		}

		if(serialConfig_cursor == 2)
		{
			serialConfig_baud++;
			if(serialConfig_baud > 5)
				serialConfig_baud = 0;
		}

		break;

	case K_ENTER:
		if(serialConfig_cursor < 3)
			goto forward;

		m_entersound = true;

		if(serialConfig_cursor == 3)
		{
			//(*SetComPortConfig)(0, ISA_uarts[serialConfig_comport - 1], serialConfig_irq, serialConfig_baudrate[serialConfig_baud], SerialConfig); // TODO

			M_Menu_ModemConfig_f();
			break;
		}

		if(serialConfig_cursor == 4)
		{
			serialConfig_cursor = 5;
			break;
		}

		// serialConfig_cursor == 5 (OK/CONNECT)
		//(*SetComPortConfig)(0, ISA_uarts[serialConfig_comport - 1], serialConfig_irq, serialConfig_baudrate[serialConfig_baud], SerialConfig); // TODO

		M_ConfigureNetSubsystem();

		if(StartingGame)
		{
			M_Menu_GameOptions_f();
			break;
		}

		m_return_state = m_state;
		m_return_onerror = true;
		gpMenuExports->SetKeyDest(key_game);
		m_state = m_none;

		if(SerialConfig)
			gpMenuExports->Cbuf_AddText(va("connect \"%s\"\n", serialConfig_phone));
		else
			gpMenuExports->Cbuf_AddText("connect\n");
		break;

	case K_BACKSPACE:
		if(serialConfig_cursor == 4)
		{
			if(strlen(serialConfig_phone))
				serialConfig_phone[strlen(serialConfig_phone) - 1] = 0;
		}
		break;

	default:
		if(key < 32 || key > 127)
			break;
		if(serialConfig_cursor == 4)
		{
			l = strlen(serialConfig_phone);
			if(l < 15)
			{
				serialConfig_phone[l + 1] = 0;
				serialConfig_phone[l] = key;
			}
		}
	}

	if(DirectConfig && (serialConfig_cursor == 3 || serialConfig_cursor == 4))
		if(key == K_UPARROW)
			serialConfig_cursor = 2;
		else
			serialConfig_cursor = 5;

	if(SerialConfig && StartingGame && serialConfig_cursor == 4)
		if(key == K_UPARROW)
			serialConfig_cursor = 3;
		else
			serialConfig_cursor = 5;
}

//=============================================================================
/* MODEM CONFIG MENU */

int modemConfig_cursor;
int modemConfig_cursor_table[] = { 40, 56, 88, 120, 156 };
#define NUM_MODEMCONFIG_CMDS 5

char modemConfig_dialing;
char modemConfig_clear[16];
char modemConfig_init[32];
char modemConfig_hangup[16];

void M_Menu_ModemConfig_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_modemconfig;
	m_entersound = true;
	//(*GetModemConfig)(0, &modemConfig_dialing, modemConfig_clear, modemConfig_init, modemConfig_hangup); // TODO
}

void M_ModemConfig_Draw()
{
	qpic_t *p;
	int basex;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	basex = (320 - p->width) / 2;
	M_DrawPic(basex, 4, p);
	basex += 8;

	if(modemConfig_dialing == 'P')
		M_Print(basex, modemConfig_cursor_table[0], "Pulse Dialing");
	else
		M_Print(basex, modemConfig_cursor_table[0], "Touch Tone Dialing");

	M_Print(basex, modemConfig_cursor_table[1], "Clear");
	M_DrawTextBox(basex, modemConfig_cursor_table[1] + 4, 16, 1);
	M_Print(basex + 8, modemConfig_cursor_table[1] + 12, modemConfig_clear);
	if(modemConfig_cursor == 1)
		M_DrawCharacter(basex + 8 + 8 * strlen(modemConfig_clear), modemConfig_cursor_table[1] + 12, 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	M_Print(basex, modemConfig_cursor_table[2], "Init");
	M_DrawTextBox(basex, modemConfig_cursor_table[2] + 4, 30, 1);
	M_Print(basex + 8, modemConfig_cursor_table[2] + 12, modemConfig_init);
	if(modemConfig_cursor == 2)
		M_DrawCharacter(basex + 8 + 8 * strlen(modemConfig_init), modemConfig_cursor_table[2] + 12, 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	M_Print(basex, modemConfig_cursor_table[3], "Hangup");
	M_DrawTextBox(basex, modemConfig_cursor_table[3] + 4, 16, 1);
	M_Print(basex + 8, modemConfig_cursor_table[3] + 12, modemConfig_hangup);
	if(modemConfig_cursor == 3)
		M_DrawCharacter(basex + 8 + 8 * strlen(modemConfig_hangup), modemConfig_cursor_table[3] + 12, 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	M_DrawTextBox(basex, modemConfig_cursor_table[4] - 8, 2, 1);
	M_Print(basex + 8, modemConfig_cursor_table[4], "OK");

	M_DrawCharacter(basex - 8, modemConfig_cursor_table[modemConfig_cursor], 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));
}

void M_ModemConfig_Key(int key)
{
	int l;

	switch(key)
	{
	case K_ESCAPE:
		M_Menu_SerialConfig_f();
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		modemConfig_cursor--;
		if(modemConfig_cursor < 0)
			modemConfig_cursor = NUM_MODEMCONFIG_CMDS - 1;
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		modemConfig_cursor++;
		if(modemConfig_cursor >= NUM_MODEMCONFIG_CMDS)
			modemConfig_cursor = 0;
		break;

	case K_LEFTARROW:
	case K_RIGHTARROW:
		if(modemConfig_cursor == 0)
		{
			if(modemConfig_dialing == 'P')
				modemConfig_dialing = 'T';
			else
				modemConfig_dialing = 'P';
			gpMenuExports->S_LocalSound("misc/menu1.wav");
		}
		break;

	case K_ENTER:
		if(modemConfig_cursor == 0)
		{
			if(modemConfig_dialing == 'P')
				modemConfig_dialing = 'T';
			else
				modemConfig_dialing = 'P';
			m_entersound = true;
		}

		if(modemConfig_cursor == 4)
		{
			//(*SetModemConfig)(0, va("%c", modemConfig_dialing), modemConfig_clear, modemConfig_init, modemConfig_hangup); // TODO
			m_entersound = true;
			M_Menu_SerialConfig_f();
		}
		break;

	case K_BACKSPACE:
		if(modemConfig_cursor == 1)
		{
			if(strlen(modemConfig_clear))
				modemConfig_clear[strlen(modemConfig_clear) - 1] = 0;
		}

		if(modemConfig_cursor == 2)
		{
			if(strlen(modemConfig_init))
				modemConfig_init[strlen(modemConfig_init) - 1] = 0;
		}

		if(modemConfig_cursor == 3)
		{
			if(strlen(modemConfig_hangup))
				modemConfig_hangup[strlen(modemConfig_hangup) - 1] = 0;
		}
		break;

	default:
		if(key < 32 || key > 127)
			break;

		if(modemConfig_cursor == 1)
		{
			l = strlen(modemConfig_clear);
			if(l < 15)
			{
				modemConfig_clear[l + 1] = 0;
				modemConfig_clear[l] = key;
			}
		}

		if(modemConfig_cursor == 2)
		{
			l = strlen(modemConfig_init);
			if(l < 29)
			{
				modemConfig_init[l + 1] = 0;
				modemConfig_init[l] = key;
			}
		}

		if(modemConfig_cursor == 3)
		{
			l = strlen(modemConfig_hangup);
			if(l < 15)
			{
				modemConfig_hangup[l + 1] = 0;
				modemConfig_hangup[l] = key;
			}
		}
	}
}

//=============================================================================
/* LAN CONFIG MENU */

int lanConfig_cursor = -1;
int lanConfig_cursor_table[] = { 72, 92, 124 };
#define NUM_LANCONFIG_CMDS 3

int lanConfig_port;
char lanConfig_portname[6];
char lanConfig_joinname[22];

void M_Menu_LanConfig_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_lanconfig;
	m_entersound = true;
	if(lanConfig_cursor == -1)
	{
		if(JoiningGame && TCPIPConfig)
			lanConfig_cursor = 2;
		else
			lanConfig_cursor = 1;
	}
	if(StartingGame && lanConfig_cursor == 2)
		lanConfig_cursor = 1;
	//lanConfig_port = DEFAULTnet_hostport; // TODO
	sprintf(lanConfig_portname, "%u", lanConfig_port);

	m_return_onerror = false;
	m_return_reason[0] = 0;
}

void M_LanConfig_Draw()
{
	qpic_t *p;
	int basex;
	const char *startJoin;
	const char *protocol;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	basex = (320 - p->width) / 2;
	M_DrawPic(basex, 4, p);

	if(StartingGame)
		startJoin = "New Game";
	else
		startJoin = "Join Game";
	if(IPXConfig)
		protocol = "IPX";
	else
		protocol = "TCP/IP";
	M_Print(basex, 32, va("%s - %s", startJoin, protocol));
	basex += 8;

	M_Print(basex, 52, "Address:");
	//if(IPXConfig)
		//M_Print(basex + 9 * 8, 52, my_ipx_address); // TODO
	//else
		//M_Print(basex + 9 * 8, 52, my_tcpip_address); // TODO

	M_Print(basex, lanConfig_cursor_table[0], "Port");
	M_DrawTextBox(basex + 8 * 8, lanConfig_cursor_table[0] - 8, 6, 1);
	M_Print(basex + 9 * 8, lanConfig_cursor_table[0], lanConfig_portname);

	if(JoiningGame)
	{
		M_Print(basex, lanConfig_cursor_table[1], "Search for local games...");
		M_Print(basex, 108, "Join game at:");
		M_DrawTextBox(basex + 8, lanConfig_cursor_table[2] - 8, 22, 1);
		M_Print(basex + 16, lanConfig_cursor_table[2], lanConfig_joinname);
	}
	else
	{
		M_DrawTextBox(basex, lanConfig_cursor_table[1] - 8, 2, 1);
		M_Print(basex + 8, lanConfig_cursor_table[1], "OK");
	}

	M_DrawCharacter(basex - 8, lanConfig_cursor_table[lanConfig_cursor], 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(lanConfig_cursor == 0)
		M_DrawCharacter(basex + 9 * 8 + 8 * strlen(lanConfig_portname), lanConfig_cursor_table[0], 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(lanConfig_cursor == 2)
		M_DrawCharacter(basex + 16 + 8 * strlen(lanConfig_joinname), lanConfig_cursor_table[2], 10 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(*m_return_reason)
		M_PrintWhite(basex, 148, m_return_reason);
}

void M_LanConfig_Key(int key)
{
	int l;

	switch(key)
	{
	case K_ESCAPE:
		M_Menu_Net_f();
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		lanConfig_cursor--;
		if(lanConfig_cursor < 0)
			lanConfig_cursor = NUM_LANCONFIG_CMDS - 1;
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		lanConfig_cursor++;
		if(lanConfig_cursor >= NUM_LANCONFIG_CMDS)
			lanConfig_cursor = 0;
		break;

	case K_ENTER:
		if(lanConfig_cursor == 0)
			break;

		m_entersound = true;

		M_ConfigureNetSubsystem();

		if(lanConfig_cursor == 1)
		{
			if(StartingGame)
			{
				M_Menu_GameOptions_f();
				break;
			}
			M_Menu_Search_f();
			break;
		}

		if(lanConfig_cursor == 2)
		{
			m_return_state = m_state;
			m_return_onerror = true;
			gpMenuExports->SetKeyDest(key_game);
			m_state = m_none;
			gpMenuExports->Cbuf_AddText(va("connect \"%s\"\n", lanConfig_joinname));
			break;
		}

		break;

	case K_BACKSPACE:
		if(lanConfig_cursor == 0)
		{
			if(strlen(lanConfig_portname))
				lanConfig_portname[strlen(lanConfig_portname) - 1] = 0;
		}

		if(lanConfig_cursor == 2)
		{
			if(strlen(lanConfig_joinname))
				lanConfig_joinname[strlen(lanConfig_joinname) - 1] = 0;
		}
		break;

	default:
		if(key < 32 || key > 127)
			break;

		if(lanConfig_cursor == 2)
		{
			l = strlen(lanConfig_joinname);
			if(l < 21)
			{
				lanConfig_joinname[l + 1] = 0;
				lanConfig_joinname[l] = key;
			}
		}

		if(key < '0' || key > '9')
			break;
		if(lanConfig_cursor == 0)
		{
			l = strlen(lanConfig_portname);
			if(l < 5)
			{
				lanConfig_portname[l + 1] = 0;
				lanConfig_portname[l] = key;
			}
		}
	}

	if(StartingGame && lanConfig_cursor == 2)
		if(key == K_UPARROW)
			lanConfig_cursor = 1;
		else
			lanConfig_cursor = 0;

	l = atoi(lanConfig_portname);
	if(l > 65535)
		l = lanConfig_port;
	else
		lanConfig_port = l;
	sprintf(lanConfig_portname, "%u", lanConfig_port);
}

//=============================================================================
/* GAME OPTIONS MENU */

typedef struct
{
	const char *name;
	const char *description;
} level_t;

level_t levels[] =
{
  { "start", "Entrance" }, // 0

  { "e1m1", "Slipgate Complex" }, // 1
  { "e1m2", "Castle of the Damned" },
  { "e1m3", "The Necropolis" },
  { "e1m4", "The Grisly Grotto" },
  { "e1m5", "Gloom Keep" },
  { "e1m6", "The Door To Chthon" },
  { "e1m7", "The House of Chthon" },
  { "e1m8", "Ziggurat Vertigo" },

  { "e2m1", "The Installation" }, // 9
  { "e2m2", "Ogre Citadel" },
  { "e2m3", "Crypt of Decay" },
  { "e2m4", "The Ebon Fortress" },
  { "e2m5", "The Wizard's Manse" },
  { "e2m6", "The Dismal Oubliette" },
  { "e2m7", "Underearth" },

  { "e3m1", "Termination Central" }, // 16
  { "e3m2", "The Vaults of Zin" },
  { "e3m3", "The Tomb of Terror" },
  { "e3m4", "Satan's Dark Delight" },
  { "e3m5", "Wind Tunnels" },
  { "e3m6", "Chambers of Torment" },
  { "e3m7", "The Haunted Halls" },

  { "e4m1", "The Sewage System" }, // 23
  { "e4m2", "The Tower of Despair" },
  { "e4m3", "The Elder God Shrine" },
  { "e4m4", "The Palace of Hate" },
  { "e4m5", "Hell's Atrium" },
  { "e4m6", "The Pain Maze" },
  { "e4m7", "Azure Agony" },
  { "e4m8", "The Nameless City" },

  { "end", "Shub-Niggurath's Pit" }, // 31

  { "dm1", "Place of Two Deaths" }, // 32
  { "dm2", "Claustrophobopolis" },
  { "dm3", "The Abandoned Base" },
  { "dm4", "The Bad Place" },
  { "dm5", "The Cistern" },
  { "dm6", "The Dark Zone" }
};

typedef struct
{
	const char *description;
	int firstLevel;
	int levels;
} episode_t;

episode_t episodes[] =
{
  { "Welcome to Quake", 0, 1 },
  { "Doomed Dimension", 1, 8 },
  { "Realm of Black Magic", 9, 7 },
  { "Netherworld", 16, 7 },
  { "The Elder World", 23, 8 },
  { "Final Level", 31, 1 },
  { "Deathmatch Arena", 32, 6 }
};

int startepisode;
int startlevel;
int maxplayers;
bool m_serverInfoMessage = false;
double m_serverInfoMessageTime;

void M_Menu_GameOptions_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_gameoptions;
	m_entersound = true;
	if(maxplayers == 0)
		maxplayers = gpMenuExports->GetMaxClients();
	if(maxplayers < 2)
		maxplayers = gpMenuExports->GetMaxClientsLimit();
}

int gameoptions_cursor_table[] = { 40, 56, 64, 72, 80, 88, 96, 112, 120 };
#define NUM_GAMEOPTIONS 9
int gameoptions_cursor;

void M_GameOptions_Draw()
{
	qpic_t *p;
	int x;

	M_DrawTransPic(16, 4, gpMenuExports->Draw_CachePic("gfx/qplaque.lmp"));
	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	M_DrawTextBox(152, 32, 10, 1);
	M_Print(160, 40, "begin game");

	M_Print(0, 56, "      Max players");
	M_Print(160, 56, va("%i", maxplayers));

	M_Print(0, 64, "        Game Type");
	if(gpMenuExports->Cvar_VariableValue("coop"))
		M_Print(160, 64, "Cooperative");
	else
		M_Print(160, 64, "Deathmatch");

	M_Print(0, 72, "        Teamplay");
	/*
	if(rogue)
	{
		const char *msg;

		switch((int)gpMenuExports->Cvar_VariableValue("teamplay"))
		{
		case 1:
			msg = "No Friendly Fire";
			break;
		case 2:
			msg = "Friendly Fire";
			break;
		case 3:
			msg = "Tag";
			break;
		case 4:
			msg = "Capture the Flag";
			break;
		case 5:
			msg = "One Flag CTF";
			break;
		case 6:
			msg = "Three Team CTF";
			break;
		default:
			msg = "Off";
			break;
		}
		M_Print(160, 72, msg);
	}
	else
	*/
	{
		const char *msg;

		switch((int)gpMenuExports->Cvar_VariableValue("teamplay"))
		{
		case 1:
			msg = "No Friendly Fire";
			break;
		case 2:
			msg = "Friendly Fire";
			break;
		default:
			msg = "Off";
			break;
		}
		M_Print(160, 72, msg);
	}

	M_Print(0, 80, "            Skill");
	if(gpMenuExports->Cvar_VariableValue("skill") == 0)
		M_Print(160, 80, "Easy difficulty");
	else if(gpMenuExports->Cvar_VariableValue("skill") == 1)
		M_Print(160, 80, "Normal difficulty");
	else if(gpMenuExports->Cvar_VariableValue("skill") == 2)
		M_Print(160, 80, "Hard difficulty");
	else
		M_Print(160, 80, "Nightmare difficulty");

	M_Print(0, 88, "       Frag Limit");
	if(gpMenuExports->Cvar_VariableValue("fraglimit") == 0)
		M_Print(160, 88, "none");
	else
		M_Print(160, 88, va("%i frags", (int)gpMenuExports->Cvar_VariableValue("fraglimit")));

	M_Print(0, 96, "       Time Limit");
	if(gpMenuExports->Cvar_VariableValue("timelimit") == 0)
		M_Print(160, 96, "none");
	else
		M_Print(160, 96, va("%i minutes", (int)gpMenuExports->Cvar_VariableValue("timelimit")));

	M_Print(0, 112, "         Episode");
	M_Print(160, 112, episodes[startepisode].description);

	M_Print(0, 120, "           Level");
	M_Print(160, 120, levels[episodes[startepisode].firstLevel + startlevel].description);
	M_Print(160, 128, levels[episodes[startepisode].firstLevel + startlevel].name);

	// line cursor
	M_DrawCharacter(144, gameoptions_cursor_table[gameoptions_cursor], 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(m_serverInfoMessage)
	{
		if((gpMenuExports->Host_GetRealTime() - m_serverInfoMessageTime) < 5.0)
		{
			x = (320 - 26 * 8) / 2;
			M_DrawTextBox(x, 138, 24, 4);
			x += 8;
			M_Print(x, 146, "  More than 4 players   ");
			M_Print(x, 154, " requires using command ");
			M_Print(x, 162, "line parameters; please ");
			M_Print(x, 170, "   see techinfo.txt.    ");
		}
		else
		{
			m_serverInfoMessage = false;
		}
	}
}

void M_NetStart_Change(int dir)
{
	int count;

	switch(gameoptions_cursor)
	{
	case 1:
		maxplayers += dir;
		if(maxplayers > gpMenuExports->GetMaxClientsLimit())
		{
			maxplayers = gpMenuExports->GetMaxClientsLimit();
			m_serverInfoMessage = true;
			m_serverInfoMessageTime = gpMenuExports->Host_GetRealTime();
		}
		if(maxplayers < 2)
			maxplayers = 2;
		break;

	case 2:
		gpMenuExports->Cvar_SetValue("coop", gpMenuExports->Cvar_VariableValue("coop") ? 0 : 1);
		break;

	case 3:
		count = 2;

		gpMenuExports->Cvar_SetValue("teamplay", gpMenuExports->Cvar_VariableValue("teamplay") + dir);
		if(gpMenuExports->Cvar_VariableValue("teamplay") > count)
			gpMenuExports->Cvar_SetValue("teamplay", 0);
		else if(gpMenuExports->Cvar_VariableValue("teamplay") < 0)
			gpMenuExports->Cvar_SetValue("teamplay", count);
		break;

	case 4:
		gpMenuExports->Cvar_SetValue("skill", gpMenuExports->Cvar_VariableValue("skill") + dir);
		if(gpMenuExports->Cvar_VariableValue("skill") > 3)
			gpMenuExports->Cvar_SetValue("skill", 0);
		if(gpMenuExports->Cvar_VariableValue("skill") < 0)
			gpMenuExports->Cvar_SetValue("skill", 3);
		break;

	case 5:
		gpMenuExports->Cvar_SetValue("fraglimit", gpMenuExports->Cvar_VariableValue("fraglimit") + dir * 10);
		if(gpMenuExports->Cvar_VariableValue("fraglimit") > 100)
			gpMenuExports->Cvar_SetValue("fraglimit", 0);
		if(gpMenuExports->Cvar_VariableValue("fraglimit") < 0)
			gpMenuExports->Cvar_SetValue("fraglimit", 100);
		break;

	case 6:
		gpMenuExports->Cvar_SetValue("timelimit", gpMenuExports->Cvar_VariableValue("timelimit") + dir * 5);
		if(gpMenuExports->Cvar_VariableValue("timelimit") > 60)
			gpMenuExports->Cvar_SetValue("timelimit", 0);
		if(gpMenuExports->Cvar_VariableValue("timelimit") < 0)
			gpMenuExports->Cvar_SetValue("timelimit", 60);
		break;

	case 7:
		startepisode += dir;
		count = 7;

		if(startepisode < 0)
			startepisode = count - 1;

		if(startepisode >= count)
			startepisode = 0;

		startlevel = 0;
		break;

	case 8:
		startlevel += dir;
		count = episodes[startepisode].levels;

		if(startlevel < 0)
			startlevel = count - 1;

		if(startlevel >= count)
			startlevel = 0;
		break;
	}
}

void M_GameOptions_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		M_Menu_Net_f();
		break;

	case K_UPARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		gameoptions_cursor--;
		if(gameoptions_cursor < 0)
			gameoptions_cursor = NUM_GAMEOPTIONS - 1;
		break;

	case K_DOWNARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		gameoptions_cursor++;
		if(gameoptions_cursor >= NUM_GAMEOPTIONS)
			gameoptions_cursor = 0;
		break;

	case K_LEFTARROW:
		if(gameoptions_cursor == 0)
			break;
		gpMenuExports->S_LocalSound("misc/menu3.wav");
		M_NetStart_Change(-1);
		break;

	case K_RIGHTARROW:
		if(gameoptions_cursor == 0)
			break;
		gpMenuExports->S_LocalSound("misc/menu3.wav");
		M_NetStart_Change(1);
		break;

	case K_ENTER:
		gpMenuExports->S_LocalSound("misc/menu2.wav");
		if(gameoptions_cursor == 0)
		{
			if(gpMenuExports->Host_IsServerActive())
				gpMenuExports->Cbuf_AddText("disconnect\n");
			gpMenuExports->Cbuf_AddText("listen 0\n"); // so host_netport will be re-examined
			gpMenuExports->Cbuf_AddText(va("maxplayers %u\n", maxplayers));
			gpMenuExports->SCR_BeginLoadingPlaque();

			gpMenuExports->Cbuf_AddText(va("map %s\n", levels[episodes[startepisode].firstLevel + startlevel].name));

			return;
		}

		M_NetStart_Change(1);
		break;
	}
}

//=============================================================================
/* SEARCH MENU */

bool searchComplete = false;
double searchCompleteTime;

void M_Menu_Search_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_search;
	m_entersound = false;
	//slistSilent = true; // TODO
	//slistLocal = false; // TODO
	searchComplete = false;
	//NET_Slist_f(); // TODO
}

void M_Search_Draw()
{
	qpic_t *p;
	int x;

	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);
	x = (320 / 2) - ((12 * 8) / 2) + 4;
	M_DrawTextBox(x - 8, 32, 12, 1);
	M_Print(x, 40, "Searching...");

	//if(slistInProgress)
	{
		//NET_Poll();
		return;
	}

	if(!searchComplete)
	{
		searchComplete = true;
		searchCompleteTime = gpMenuExports->Host_GetRealTime();
	}

	//if(hostCacheCount)
	{
		M_Menu_ServerList_f();
		return;
	}

	M_PrintWhite((320 / 2) - ((22 * 8) / 2), 64, "No servers found");
	if((gpMenuExports->Host_GetRealTime() - searchCompleteTime) < 3.0)
		return;

	M_Menu_LanConfig_f();
}

void M_Search_Key(int key)
{
}

//=============================================================================
/* SLIST MENU */

int slist_cursor;
bool slist_sorted;

void M_Menu_ServerList_f()
{
	gpMenuExports->SetKeyDest(key_menu);
	m_state = m_slist;
	m_entersound = true;
	slist_cursor = 0;
	m_return_onerror = false;
	m_return_reason[0] = 0;
	slist_sorted = false;
}

void M_ServerList_Draw()
{
	int n;
	char string[64];
	qpic_t *p;

	if(!slist_sorted)
	{
		// TODO
		/*
		if(hostCacheCount > 1)
		{
			int i, j;
			hostcache_t temp;
			for(i = 0; i < hostCacheCount; i++)
				for(j = i + 1; j < hostCacheCount; j++)
					if(strcmp(hostcache[j].name, hostcache[i].name) < 0)
					{
						Q_memcpy(&temp, &hostcache[j], sizeof(hostcache_t));
						Q_memcpy(&hostcache[j], &hostcache[i], sizeof(hostcache_t));
						Q_memcpy(&hostcache[i], &temp, sizeof(hostcache_t));
					}
		}
		*/
		slist_sorted = true;
	}

	p = gpMenuExports->Draw_CachePic("gfx/p_multi.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);
	// TODO
	/*
	for(n = 0; n < hostCacheCount; n++)
	{
		if(hostcache[n].maxusers)
			sprintf(string, "%-15.15s %-15.15s %2u/%2u\n", hostcache[n].name, hostcache[n].map, hostcache[n].users, hostcache[n].maxusers);
		else
			sprintf(string, "%-15.15s %-15.15s\n", hostcache[n].name, hostcache[n].map);
		M_Print(16, 32 + 8 * n, string);
	}
	*/
	M_DrawCharacter(0, 32 + slist_cursor * 8, 12 + ((int)(gpMenuExports->Host_GetRealTime() * 4) & 1));

	if(*m_return_reason)
		M_PrintWhite(16, 148, m_return_reason);
}

void M_ServerList_Key(int k)
{
	switch(k)
	{
	case K_ESCAPE:
		M_Menu_LanConfig_f();
		break;

	case K_SPACE:
		M_Menu_Search_f();
		break;

	case K_UPARROW:
	case K_LEFTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		slist_cursor--;
		//if(slist_cursor < 0)
			//slist_cursor = hostCacheCount - 1; // TODO
		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		slist_cursor++;
		//if(slist_cursor >= hostCacheCount) // TODO
			//slist_cursor = 0;
		break;

	case K_ENTER:
		gpMenuExports->S_LocalSound("misc/menu2.wav");
		m_return_state = m_state;
		m_return_onerror = true;
		slist_sorted = false;
		gpMenuExports->SetKeyDest(key_game);
		m_state = m_none;
		//Cbuf_AddText(va("connect \"%s\"\n", hostcache[slist_cursor].cname)); // TODO
		break;

	default:
		break;
	}
}

//=============================================================================
/* Menu Subsystem */

void M_Init()
{
	gpMenuExports->Cmd_AddCommand("togglemenu", M_ToggleMenu_f);

	gpMenuExports->Cmd_AddCommand("menu_main", M_Menu_Main_f);
	gpMenuExports->Cmd_AddCommand("menu_singleplayer", M_Menu_SinglePlayer_f);
	gpMenuExports->Cmd_AddCommand("menu_load", M_Menu_Load_f);
	gpMenuExports->Cmd_AddCommand("menu_save", M_Menu_Save_f);
	gpMenuExports->Cmd_AddCommand("menu_multiplayer", M_Menu_MultiPlayer_f);
	gpMenuExports->Cmd_AddCommand("menu_setup", M_Menu_Setup_f);
	gpMenuExports->Cmd_AddCommand("menu_options", M_Menu_Options_f);
	gpMenuExports->Cmd_AddCommand("menu_keys", M_Menu_Keys_f);
	gpMenuExports->Cmd_AddCommand("menu_video", M_Menu_Video_f);
	gpMenuExports->Cmd_AddCommand("help", M_Menu_Help_f);
	gpMenuExports->Cmd_AddCommand("menu_quit", M_Menu_Quit_f);
}

void M_Draw()
{
	if(m_state == m_none || gpMenuExports->GetKeyDest() != key_menu)
		return;

	if(!m_recursiveDraw)
	{
		gpMenuExports->SCR_SetCopyEverything(true);

		if(gpMenuExports->SCR_Con_Current())
		{
			int height;
			gpGameUIFuncs->GetCurrentVideoMode(nullptr, &height, nullptr);
			gpMenuExports->Draw_ConsoleBackground(height);
			gpMenuExports->VID_UnlockBuffer();
			gpMenuExports->S_ExtraUpdate();
			gpMenuExports->VID_LockBuffer();
		}
		else
			gpMenuExports->Draw_FadeScreen();

		gpMenuExports->SCR_SetFullUpdateActive(false);
	}
	else
	{
		m_recursiveDraw = false;
	}

	switch(m_state)
	{
	case m_none:
		break;

	case m_main:
		M_Main_Draw();
		break;

	case m_singleplayer:
		M_SinglePlayer_Draw();
		break;

	case m_load:
		M_Load_Draw();
		break;

	case m_save:
		M_Save_Draw();
		break;

	case m_multiplayer:
		M_MultiPlayer_Draw();
		break;

	case m_setup:
		M_Setup_Draw();
		break;

	case m_net:
		M_Net_Draw();
		break;

	case m_options:
		M_Options_Draw();
		break;

	case m_keys:
		M_Keys_Draw();
		break;

	case m_video:
		M_Video_Draw();
		break;

	case m_help:
		M_Help_Draw();
		break;

	case m_quit:
		M_Quit_Draw();
		break;

	case m_serialconfig:
		M_SerialConfig_Draw();
		break;

	case m_modemconfig:
		M_ModemConfig_Draw();
		break;

	case m_lanconfig:
		M_LanConfig_Draw();
		break;

	case m_gameoptions:
		M_GameOptions_Draw();
		break;

	case m_search:
		M_Search_Draw();
		break;

	case m_slist:
		M_ServerList_Draw();
		break;
	}

	if(m_entersound)
	{
		gpMenuExports->S_LocalSound("misc/menu2.wav");
		m_entersound = false;
	}
}

void M_Keydown(int key)
{
	switch(m_state)
	{
	case m_none:
		return;

	case m_main:
		M_Main_Key(key);
		return;

	case m_singleplayer:
		M_SinglePlayer_Key(key);
		return;

	case m_load:
		M_Load_Key(key);
		return;

	case m_save:
		M_Save_Key(key);
		return;

	case m_multiplayer:
		M_MultiPlayer_Key(key);
		return;

	case m_setup:
		M_Setup_Key(key);
		return;

	case m_net:
		M_Net_Key(key);
		return;

	case m_options:
		M_Options_Key(key);
		return;

	case m_keys:
		M_Keys_Key(key);
		return;

	case m_video:
		M_Video_Key(key);
		return;

	case m_help:
		M_Help_Key(key);
		return;

	case m_quit:
		M_Quit_Key(key);
		return;

	case m_serialconfig:
		M_SerialConfig_Key(key);
		return;

	case m_modemconfig:
		M_ModemConfig_Key(key);
		return;

	case m_lanconfig:
		M_LanConfig_Key(key);
		return;

	case m_gameoptions:
		M_GameOptions_Key(key);
		return;

	case m_search:
		M_Search_Key(key);
		break;

	case m_slist:
		M_ServerList_Key(key);
		return;
	}
}

void M_ConfigureNetSubsystem()
{
	// enable/disable net systems to match desired config

	gpMenuExports->Cbuf_AddText("stopdemo\n");
	if(SerialConfig || DirectConfig)
	{
		gpMenuExports->Cbuf_AddText("com1 enable\n");
	}

	//if(IPXConfig || TCPIPConfig)
		//net_hostport = lanConfig_port; // TODO
}

//========================================================
// Video menu stuff
//========================================================

#ifdef _WIN32 // TODO: ???
extern void M_Print(int cx, int cy, char *str);
extern void M_PrintWhite(int cx, int cy, char *str);
extern void M_DrawCharacter(int cx, int line, int num);
extern void M_DrawTransPic(int x, int y, qpic_t *pic);
extern void M_DrawPic(int x, int y, qpic_t *pic);

static int vid_line, vid_wmodes;

typedef struct
{
	int modenum;
	const char *desc;
	int iscur;
} modedesc_t;

#define MAX_COLUMN_SIZE 9
#define MODE_AREA_HEIGHT (MAX_COLUMN_SIZE + 2)
#define MAX_MODEDESCS (MAX_COLUMN_SIZE * 3)

static modedesc_t modedescs[MAX_MODEDESCS];

/*
================
VID_MenuDraw
================
*/
void VID_MenuDraw()
{
	qpic_t *p;
	const char *ptr;
	int lnummodes, i, j, k, column, row, dup, dupmode;
	char temp[100];
	vmode_t *pv;

	p = gpMenuExports->Draw_CachePic("gfx/vidmodes.lmp");
	M_DrawPic((320 - p->width) / 2, 4, p);

	vid_wmodes = 0;
	lnummodes = gpMenuExports->VID_NumModes();

	for(i = 1; (i < lnummodes) && (vid_wmodes < MAX_MODEDESCS); i++)
	{
		ptr = gpMenuExports->VID_GetModeDescription(i);
		pv = gpMenuExports->VID_GetModePtr(i);

		k = vid_wmodes;

		modedescs[k].modenum = i;
		modedescs[k].desc = ptr;
		modedescs[k].iscur = 0;

		if(i == gpMenuExports->VID_GetCurrentModeNum())
			modedescs[k].iscur = 1;

		vid_wmodes++;
	}

	if(vid_wmodes > 0)
	{
		M_Print(2 * 8, 36 + 0 * 8, "Fullscreen Modes (WIDTHxHEIGHTxBPP)");

		column = 8;
		row = 36 + 2 * 8;

		for(i = 0; i < vid_wmodes; i++)
		{
			if(modedescs[i].iscur)
				M_PrintWhite(column, row, modedescs[i].desc);
			else
				M_Print(column, row, modedescs[i].desc);

			column += 13 * 8;

			if((i % VID_ROW_SIZE) == (VID_ROW_SIZE - 1))
			{
				column = 8;
				row += 8;
			}
		}
	}

	M_Print(3 * 8, 36 + MODE_AREA_HEIGHT * 8 + 8 * 2,
	        "Video modes must be set from the");
	M_Print(3 * 8, 36 + MODE_AREA_HEIGHT * 8 + 8 * 3,
	        "command line with -width <width>");
	M_Print(3 * 8, 36 + MODE_AREA_HEIGHT * 8 + 8 * 4,
	        "and -bpp <bits-per-pixel>");
	M_Print(3 * 8, 36 + MODE_AREA_HEIGHT * 8 + 8 * 6,
	        "Select windowed mode with -window");
}

/*
================
VID_MenuKey
================
*/
void VID_MenuKey(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		gpMenuExports->S_LocalSound("misc/menu1.wav");
		M_Menu_Options_f();
		break;

	default:
		break;
	}
}
#endif // _WIN32