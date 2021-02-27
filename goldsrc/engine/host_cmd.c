/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2020 BlackPhrase
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

#include "quakedef.h"

extern cvar_t pausable;

int current_skill;

void Mod_Print();

typedef void (*pfnGiveFnptrsToDll)(enginefuncs_t *apEngFuncs, globalvars_t *apGlobals);

extern enginefuncs_t gEngineFuncs; // TODO

void *gamedll = NULL;

void LoadThisDll(const char *name)
{
	pfnGiveFnptrsToDll fnGiveFnptrsToDll = NULL;
	NEW_DLL_FUNCTIONS_FN fnGetNewDLLFunctions = NULL;
	APIFUNCTION fnGetEntityAPI = NULL;
	APIFUNCTION2 fnGetEntityAPI2 = NULL;

	gamedll = FS_LoadLibrary(va("%s/%s", com_gamedir, name)); // TODO: was Sys_LoadModule

	if(!gamedll)
		Sys_Error("PR_LoadProgs: couldn't load game dll");

	//pr_strings = (char *)progs + progs->ofs_strings; // TODO

	fnGiveFnptrsToDll = (pfnGiveFnptrsToDll)Sys_GetExport_Wrapper(gamedll, "GiveFnptrsToDll");

	if(!fnGiveFnptrsToDll)
		return;

	fnGiveFnptrsToDll(&gEngineFuncs, &gGlobalVariables); // TODO

	fnGetNewDLLFunctions = (NEW_DLL_FUNCTIONS_FN)Sys_GetExport_Wrapper(gamedll, "GetNewDLLFunctions");
	
	int nDLLVersion = NEW_DLL_FUNCTIONS_VERSION;
	
	if(fnGetNewDLLFunctions)
	{
		fnGetNewDLLFunctions(&gNewDLLFunctions, &nDLLVersion);
		
		if(nDLLVersion != NEW_DLL_FUNCTIONS_VERSION)
			Sys_Error("Extended API set has a wrong version (got %d, should be %d)", nDLLVersion, NEW_DLL_FUNCTIONS_VERSION);
	};
	
	fnGetEntityAPI = (APIFUNCTION)Sys_GetExport_Wrapper(gamedll, "GetEntityAPI");
	fnGetEntityAPI2 = (APIFUNCTION2)Sys_GetExport_Wrapper(gamedll, "GetEntityAPI2");

	nDLLVersion = INTERFACE_VERSION;
	
	if(fnGetEntityAPI2)
	{
		if(!fnGetEntityAPI2(&gEntityInterface, &nDLLVersion))
			return;

		if(nDLLVersion != INTERFACE_VERSION)
			Sys_Error("game dll has wrong version number (%i should be %i)", nDLLVersion, INTERFACE_VERSION);
	};

	if(fnGetEntityAPI)
	{
		if(!fnGetEntityAPI(&gEntityInterface, INTERFACE_VERSION))
			return;
	};
};

void PR_LoadProgs() // our temporary LoadEntityDLLs
{
	LoadThisDll("dlls/server");
};

/*
==================
Host_InitializeGameDLL

TODO: should be located somewhere here around 222 line, called by Sys_InitGame
==================
*/
void Host_InitializeGameDLL()
{
	static qboolean bLoaded = false;
	
	if(bLoaded)
		return;
	
	Cbuf_Execute();
	
	PR_LoadProgs(); // TODO: LoadEntityDLLs
	
	bLoaded = true;
};

/*
==================
Host_Quit_f
==================
*/

extern void M_Menu_Quit_f();

void Host_Quit_f()
{
	if(key_dest != key_console && cls.state != ca_dedicated)
	{
		Cbuf_AddText("menu_quit"); //M_Menu_Quit_f(); // TODO
		return;
	}
	CL_Disconnect();
	Host_ShutdownServer(false);

	Sys_Quit();
}

/*
==================
Host_Status_f
==================
*/
void Host_Status_f()
{
	client_t *client;
	int seconds;
	int minutes;
	int hours = 0;
	int j;
	void (*print)(const char *fmt, ...);

	if(cmd_source == src_command)
	{
		if(!sv.active)
		{
			Cmd_ForwardToServer();
			return;
		}
		print = Con_Printf;
	}
	else
		print = SV_ClientPrintf;

	print("host:    %s\n", Cvar_VariableString("hostname"));
	print("version: %4.2f\n", VERSION);
	//if (tcpipAvailable) // TODO
	//print ("tcp/ip:  %s\n", my_tcpip_address); // TODO
	//if (ipxAvailable) // TODO
	//print ("ipx:     %s\n", my_ipx_address); // TODO
	print("map:     %s\n", sv.name);
	//print ("players: %i active (%i max)\n\n", SV_GetClientCount(), svs.maxclients); // TODO
	for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if(!client->active)
			continue;
		seconds = 0; //(int)(net_time - client->netchan.connecttime); // TODO
		minutes = seconds / 60;
		if(minutes)
		{
			seconds -= (minutes * 60);
			hours = minutes / 60;
			if(hours)
				minutes -= (hours * 60);
		}
		else
			hours = 0;
		print("#%-2u %-16.16s  %3i  %2i:%02i:%02i\n", j + 1, client->name, (int)client->edict->v.frags, hours, minutes, seconds);
		print("   %s\n", client->netchan.remote_address);
	}
}

/*
==================
Host_God_f

Sets client to godmode
==================
*/
void Host_God_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	//if (gGlobalVariables.deathmatch && !host_client->privileged) // TODO: if cheats_active?
	//return;

	sv_player->v.flags = (int)sv_player->v.flags ^ FL_GODMODE;
	if(!((int)sv_player->v.flags & FL_GODMODE))
		SV_ClientPrintf(host_client, "godmode OFF\n");
	else
		SV_ClientPrintf(host_client, "godmode ON\n");
}

void Host_Notarget_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	//if (gGlobalVariables.deathmatch && !host_client->privileged) // TODO: if cheats_active?
	//return;

	sv_player->v.flags = (int)sv_player->v.flags ^ FL_NOTARGET;
	if(!((int)sv_player->v.flags & FL_NOTARGET))
		SV_ClientPrintf(host_client, "notarget OFF\n");
	else
		SV_ClientPrintf(host_client, "notarget ON\n");
}

qboolean noclip_anglehack;

void Host_Noclip_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	//if (gGlobalVariables.deathmatch && !host_client->privileged) // TODO: if cheats_active?
	//return;

	if(sv_player->v.movetype != MOVETYPE_NOCLIP)
	{
		noclip_anglehack = true;
		sv_player->v.movetype = MOVETYPE_NOCLIP;
		SV_ClientPrintf(host_client, "noclip ON\n");
	}
	else
	{
		noclip_anglehack = false;
		sv_player->v.movetype = MOVETYPE_WALK;
		SV_ClientPrintf(host_client, "noclip OFF\n");
	}
}

/*
==================
Host_Fly_f

Sets client to flymode
==================
*/
void Host_Fly_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	//if (gGlobalVariables.deathmatch && !host_client->privileged) // TODO: if cheats_active?
	//return;

	if(sv_player->v.movetype != MOVETYPE_FLY)
	{
		sv_player->v.movetype = MOVETYPE_FLY;
		SV_ClientPrintf(host_client, "flymode ON\n");
	}
	else
	{
		sv_player->v.movetype = MOVETYPE_WALK;
		SV_ClientPrintf(host_client, "flymode OFF\n");
	}
}

/*
==================
Host_Ping_f

==================
*/
void Host_Ping_f()
{
	int i, j;
	float total;
	client_t *client;

	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	SV_ClientPrintf(host_client, "Client ping times:\n");
	for(i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	{
		if(!client->active)
			continue;
		total = 0;
		for(j = 0; j < NUM_PING_TIMES; j++)
			total += client->ping_times[j];
		total /= NUM_PING_TIMES;
		SV_ClientPrintf(host_client, "%4i %s\n", (int)(total * 1000), client->name);
	}
}

/*
===============================================================================

SERVER TRANSITIONS

===============================================================================
*/

/*
======================
Host_Map

TODO: probably contains everything from Host_Map_f and Host_Map_f contains something else + redirects to Host_Map
======================
*/
void Host_Map(qboolean loadGame, const char *mapName, const char *mapstring, qboolean bIsDemo)
{
	int i;
	char name[MAX_QPATH];

	cls.demonum = -1; // stop demo loop in case this fails

	CL_Disconnect();
	Host_ShutdownServer(false);

	key_dest = key_game; // remove console or menu
	
	if(loadGame) // TODO
		SCR_BeginLoadingPlaque();

	Q_strcpy(cls.mapstring, mapstring);

	svs.serverflags = 0; // haven't completed an episode yet
	strcpy(name, mapName);

	SV_SpawnServer(name, NULL);

	if(!sv.active)
		return;

	if(cls.state != ca_dedicated)
	{
		strcpy(cls.spawnparms, "");

		for(i = 2; i < Cmd_Argc(); i++)
		{
			strcat(cls.spawnparms, Cmd_Argv(i));
			strcat(cls.spawnparms, " ");
		}

		Cmd_ExecuteString("connect local", src_command);
	}
	
	if(loadGame)
	{
		// TODO
	};
	
	if(bIsDemo)
	{
		// TODO
	};
};

/*
======================
Host_Map_f

handle a 
map <servername>
command from the console.  Active clients are kicked off.
======================
*/
void Host_Map_f()
{
	int i;
	char mapstring[MAX_MAPSTRING]; // TODO: high enough? or too much?
	
	if(cmd_source != src_command)
		return;
	
	mapstring[0] = 0;
	for(i = 0; i < Cmd_Argc(); i++)
	{
		strcat(mapstring, Cmd_Argv(i));
		strcat(mapstring, " ");
	};
	
	strcat(mapstring, "\n");
	
	Host_Map(false, Cmd_Argv(1), mapstring, false);
}

/*
==================
Host_Changelevel_f

Goes to a new map, taking all clients along
==================
*/
void Host_Changelevel_f()
{
#if 1 // QUAKE2
	char level[MAX_QPATH];
	char _startspot[MAX_QPATH];
	char *startspot;

	if(Cmd_Argc() < 2)
	{
		Con_Printf("changelevel <levelname> : continue game on a new level\n");
		return;
	}
	if(!sv.active || cls.demoplayback)
	{
		Con_Printf("Only the server may changelevel\n");
		return;
	}

	strcpy(level, Cmd_Argv(1));
	if(Cmd_Argc() == 2)
		startspot = NULL;
	else
	{
		strcpy(_startspot, Cmd_Argv(2));
		startspot = _startspot;
	}

	SV_SaveSpawnparms();
	SV_SpawnServer(level, startspot);
#else
	char level[MAX_QPATH];

	if(Cmd_Argc() != 2)
	{
		Con_Printf("changelevel <levelname> : continue game on a new level\n");
		return;
	}
	if(!sv.active || cls.demoplayback)
	{
		Con_Printf("Only the server may changelevel\n");
		return;
	}
	SV_SaveSpawnparms();
	strcpy(level, Cmd_Argv(1));
	SV_SpawnServer(level);
#endif
}

/*
==================
Host_Restart_f

Restarts the current server for a dead player
==================
*/
void Host_Restart_f()
{
	char mapname[MAX_QPATH];
	char startspot[MAX_QPATH];

	if(cls.demoplayback || !sv.active)
		return;

	if(cmd_source != src_command)
		return;
	strcpy(mapname, sv.name); // must copy out, because it gets cleared
	                          // in sv_spawnserver
	strcpy(startspot, sv.startspot);
	SV_SpawnServer(mapname, startspot);
}

/*
==================
Host_Reconnect_f

This command causes the client to wait for the signon messages again.
This is sent just before a server changes levels
==================
*/
void Host_Reconnect_f()
{
	SCR_BeginLoadingPlaque();
	cls.signon = 0; // need new connection messages
}

/*
===============================================================================

LOAD / SAVE GAME

===============================================================================
*/

#define SAVEGAME_VERSION 5

/*
===============
Host_SavegameComment

Writes a SAVEGAME_COMMENT_LENGTH character comment describing the current 
===============
*/
void Host_SavegameComment(char *text)
{
	int i;
	char kills[20];

	for(i = 0; i < SAVEGAME_COMMENT_LENGTH; i++)
		text[i] = ' ';
	memcpy(text, cl.levelname, strlen(cl.levelname));
	sprintf(kills, "kills:%3i/%3i", cl.stats[STAT_MONSTERS], cl.stats[STAT_TOTALMONSTERS]);
	memcpy(text + 22, kills, strlen(kills));
	// convert space to _ to make stdio happy
	for(i = 0; i < SAVEGAME_COMMENT_LENGTH; i++)
		if(text[i] == ' ')
			text[i] = '_';
	text[SAVEGAME_COMMENT_LENGTH] = '\0';
}

/*
===============
Host_Savegame_f
===============
*/
void Host_Savegame_f()
{
	char name[256];
	FILE *f;
	int i;
	char comment[SAVEGAME_COMMENT_LENGTH + 1];

	if(cmd_source != src_command)
		return;

	if(!sv.active)
	{
		Con_Printf("Not playing a local game.\n");
		return;
	}

	if(cl.intermission)
	{
		Con_Printf("Can't save in intermission.\n");
		return;
	}

	if(svs.maxclients != 1)
	{
		Con_Printf("Can't save multiplayer games.\n");
		return;
	}

	if(Cmd_Argc() != 2)
	{
		Con_Printf("save <savename> : save a game\n");
		return;
	}

	if(strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}

	for(i = 0; i < svs.maxclients; i++)
	{
		if(svs.clients[i].active && (svs.clients[i].edict->v.health <= 0))
		{
			Con_Printf("Can't savegame with a dead player\n");
			return;
		}
	}

	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));
	COM_DefaultExtension(name, ".sav");

	Con_Printf("Saving game to %s...\n", name);
	f = fopen(name, "w");
	if(!f)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	fprintf(f, "%i\n", SAVEGAME_VERSION);
	Host_SavegameComment(comment);
	fprintf(f, "%s\n", comment);
	for(i = 0; i < NUM_SPAWN_PARMS; i++)
		fprintf(f, "%f\n", svs.clients->spawn_parms[i]);
	fprintf(f, "%d\n", current_skill);
	fprintf(f, "%s\n", sv.name);
	fprintf(f, "%f\n", sv.time);

	// write the light styles

	for(i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		if(sv.lightstyles[i])
			fprintf(f, "%s\n", sv.lightstyles[i]);
		else
			fprintf(f, "m\n");
	}

	//ED_WriteGlobals(f); // TODO
	for(i = 0; i < sv.num_edicts; i++)
	{
		//ED_Write(f, EDICT_NUM(i)); // TODO
		fflush(f);
	}
	fclose(f);
	Con_Printf("done.\n");
}

/*
===============
Host_Loadgame_f
===============
*/
void Host_Loadgame_f()
{
	char name[MAX_OSPATH];
	FILE *f;
	char mapname[MAX_QPATH];
	float time, tfloat;
	char str[32768], *start;
	int i, r;
	edict_t *ent;
	int entnum;
	int version;
	float spawn_parms[NUM_SPAWN_PARMS];

	if(cmd_source != src_command)
		return;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("load <savename> : load a game\n");
		return;
	}

	cls.demonum = -1; // stop demo loop in case this fails

	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));
	COM_DefaultExtension(name, ".sav");

	// we can't call SCR_BeginLoadingPlaque, because too much stack space has
	// been used.  The menu calls it before stuffing loadgame command
	//	SCR_BeginLoadingPlaque ();

	Con_Printf("Loading game from %s...\n", name);
	f = fopen(name, "r");
	if(!f)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	fscanf(f, "%i\n", &version);
	if(version != SAVEGAME_VERSION)
	{
		fclose(f);
		Con_Printf("Savegame is version %i, not %i\n", version, SAVEGAME_VERSION);
		return;
	}
	fscanf(f, "%s\n", str);
	for(i = 0; i < NUM_SPAWN_PARMS; i++)
		fscanf(f, "%f\n", &spawn_parms[i]);
	// this silliness is so we can load 1.06 save files, which have float skill values
	fscanf(f, "%f\n", &tfloat);
	current_skill = (int)(tfloat + 0.1);
	Cvar_SetValue("skill", (float)current_skill);

	Cvar_SetValue("deathmatch", 0);
	Cvar_SetValue("coop", 0);
	//Cvar_SetValue ("teamplay", 0);

	fscanf(f, "%s\n", mapname);
	fscanf(f, "%f\n", &time);

	CL_Disconnect_f();

	SV_SpawnServer(mapname, NULL);

	if(!sv.active)
	{
		Con_Printf("Couldn't load map\n");
		return;
	}
	sv.paused = true; // pause until all clients connect
	sv.loadgame = true;

	// load the light styles

	for(i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		fscanf(f, "%s\n", str);
		sv.lightstyles[i] = Hunk_Alloc(strlen(str) + 1);
		strcpy(sv.lightstyles[i], str);
	}

	// load the edicts out of the savegame file
	entnum = -1; // -1 is the globals
	while(!feof(f))
	{
		for(i = 0; i < sizeof(str) - 1; i++)
		{
			r = fgetc(f);
			if(r == EOF || !r)
				break;
			str[i] = r;
			if(r == '}')
			{
				i++;
				break;
			}
		}
		if(i == sizeof(str) - 1)
			Sys_Error("Loadgame buffer overflow");
		str[i] = 0;
		start = str;
		start = COM_Parse(str);
		if(!com_token[0])
			break; // end of file
		if(strcmp(com_token, "{"))
			Sys_Error("First token isn't a brace");

		if(entnum == -1)
		{ // parse the global vars
			//ED_ParseGlobals(start); // TODO
		}
		else
		{ // parse an edict

			ent = EDICT_NUM(entnum);
			memset(&ent->v, 0, sizeof(ent->v));
			ent->free = false;
			//ED_ParseEdict(start, ent); // TODO

			// link it into the bsp tree
			if(!ent->free)
				SV_LinkEdict(ent, false);
		}

		entnum++;
	}

	sv.num_edicts = entnum;
	sv.time = time;

	fclose(f);

	for(i = 0; i < NUM_SPAWN_PARMS; i++)
		svs.clients->spawn_parms[i] = spawn_parms[i];

	if(cls.state != ca_dedicated)
	{
		//CL_EstablishConnection ("local"); // TODO
		Host_Reconnect_f();
	}
}

void SaveGamestate()
{
	char name[256];
	FILE *f;
	int i;
	char comment[SAVEGAME_COMMENT_LENGTH + 1];
	edict_t *ent;

	sprintf(name, "%s/%s.gip", com_gamedir, sv.name);

	Con_Printf("Saving game to %s...\n", name);
	f = fopen(name, "w");
	if(!f)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	fprintf(f, "%i\n", SAVEGAME_VERSION);
	Host_SavegameComment(comment);
	fprintf(f, "%s\n", comment);
	//	for (i=0 ; i<NUM_SPAWN_PARMS ; i++)
	//		fprintf (f, "%f\n", svs.clients->spawn_parms[i]);
	fprintf(f, "%f\n", skill.value);
	fprintf(f, "%s\n", sv.name);
	fprintf(f, "%f\n", sv.time);

	// write the light styles

	for(i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		if(sv.lightstyles[i])
			fprintf(f, "%s\n", sv.lightstyles[i]);
		else
			fprintf(f, "m\n");
	}

	for(i = svs.maxclients + 1; i < sv.num_edicts; i++)
	{
		ent = EDICT_NUM(i);
		//if ((int)ent->v.flags & FL_ARCHIVE_OVERRIDE) // TODO
		//continue;
		fprintf(f, "%i\n", i);
		//ED_Write(f, ent); // TODO
		fflush(f);
	}
	fclose(f);
	Con_Printf("done.\n");
}

int LoadGamestate(char *level, char *startspot)
{
	char name[MAX_OSPATH];
	FILE *f;
	char mapname[MAX_QPATH];
	float time, sk;
	char str[32768], *start;
	int i, r;
	edict_t *ent;
	int entnum;
	int version;
	//	float	spawn_parms[NUM_SPAWN_PARMS];

	sprintf(name, "%s/%s.gip", com_gamedir, level);

	Con_Printf("Loading game from %s...\n", name);
	f = fopen(name, "r");
	if(!f)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return -1;
	}

	fscanf(f, "%i\n", &version);
	if(version != SAVEGAME_VERSION)
	{
		fclose(f);
		Con_Printf("Savegame is version %i, not %i\n", version, SAVEGAME_VERSION);
		return -1;
	}
	fscanf(f, "%s\n", str);
	//	for (i=0 ; i<NUM_SPAWN_PARMS ; i++)
	//		fscanf (f, "%f\n", &spawn_parms[i]);
	fscanf(f, "%f\n", &sk);
	Cvar_SetValue("skill", sk);

	fscanf(f, "%s\n", mapname);
	fscanf(f, "%f\n", &time);

	SV_SpawnServer(mapname, startspot);

	if(!sv.active)
	{
		Con_Printf("Couldn't load map\n");
		return -1;
	}

	// load the light styles
	for(i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		fscanf(f, "%s\n", str);
		sv.lightstyles[i] = Hunk_Alloc(strlen(str) + 1);
		strcpy(sv.lightstyles[i], str);
	}

	// load the edicts out of the savegame file
	while(!feof(f))
	{
		fscanf(f, "%i\n", &entnum);
		for(i = 0; i < sizeof(str) - 1; i++)
		{
			r = fgetc(f);
			if(r == EOF || !r)
				break;
			str[i] = r;
			if(r == '}')
			{
				i++;
				break;
			}
		}
		if(i == sizeof(str) - 1)
			Sys_Error("Loadgame buffer overflow");
		str[i] = 0;
		start = str;
		start = COM_Parse(str);
		if(!com_token[0])
			break; // end of file
		if(strcmp(com_token, "{"))
			Sys_Error("First token isn't a brace");

		// parse an edict

		ent = EDICT_NUM(entnum);
		memset(&ent->v, 0, sizeof(ent->v));
		ent->free = false;
		//ED_ParseEdict(start, ent); // TODO

		// link it into the bsp tree
		if(!ent->free)
			SV_LinkEdict(ent, false);
	}

	//	sv.num_edicts = entnum;
	sv.time = time;
	fclose(f);

	//	for (i=0 ; i<NUM_SPAWN_PARMS ; i++)
	//		svs.clients->spawn_parms[i] = spawn_parms[i];

	return 0;
}

// changing levels within a unit
void Host_Changelevel2_f()
{
	char level[MAX_QPATH];
	char _startspot[MAX_QPATH];
	char *startspot;

	if(Cmd_Argc() < 2)
	{
		Con_Printf("changelevel2 <levelname> : continue game on a new level in the unit\n");
		return;
	}
	if(!sv.active || cls.demoplayback)
	{
		Con_Printf("Only the server may changelevel\n");
		return;
	}

	strcpy(level, Cmd_Argv(1));
	if(Cmd_Argc() == 2)
		startspot = NULL;
	else
	{
		strcpy(_startspot, Cmd_Argv(2));
		startspot = _startspot;
	}

	SV_SaveSpawnparms();

	// save the current level's state
	SaveGamestate();

	// try to restore the new level
	if(LoadGamestate(level, startspot))
		SV_SpawnServer(level, startspot);
}

//============================================================================

/*
======================
Host_Name_f
======================
*/
/*
void Host_Name_f()
{
	char *newName;

	if(Cmd_Argc() == 1)
	{
		Con_Printf("\"name\" is \"%s\"\n", cl_name.string);
		return;
	}
	if(Cmd_Argc() == 2)
		newName = Cmd_Argv(1);
	else
		newName = Cmd_Args();
	newName[15] = 0;

	if(cmd_source == src_command)
	{
		if(Q_strcmp(cl_name.string, newName) == 0)
			return;
		Cvar_Set("_cl_name", newName);
		if(cls.state == ca_connected)
			Cmd_ForwardToServer();
		return;
	}

	if(host_client->name[0] && strcmp(host_client->name, "unconnected"))
		if(Q_strcmp(host_client->name, newName) != 0)
			Con_Printf("%s renamed to %s\n", host_client->name, newName);
	Q_strcpy(host_client->name, newName);
	host_client->edict->v.netname = host_client->name - pr_strings;

	// send notification to all clients

	MSG_WriteByte(&sv.reliable_datagram, svc_updatename);
	MSG_WriteByte(&sv.reliable_datagram, host_client - svs.clients);
	MSG_WriteString(&sv.reliable_datagram, host_client->name);
}
*/

/*
=======================
Host_Version_f
======================
*/
void Host_Version_f() // TODO: CL_Version_f?
{
	Con_Printf("Version %4.2f\n", VERSION);
	Con_Printf("Exe: " __TIME__ " " __DATE__ "\n");
}

#ifdef HEADCRABGODS
void Host_Please_f()
{
	client_t *cl;
	int j;

	if(cmd_source != src_command)
		return;

	if((Cmd_Argc() == 3) && Q_strcmp(Cmd_Argv(1), "#") == 0)
	{
		j = Q_atof(Cmd_Argv(2)) - 1;
		if(j < 0 || j >= svs.maxclients)
			return;
		if(!svs.clients[j].active)
			return;
		cl = &svs.clients[j];
		if(cl->privileged)
		{
			cl->privileged = false;
			cl->edict->v.flags = (int)cl->edict->v.flags & ~(FL_GODMODE | FL_NOTARGET);
			cl->edict->v.movetype = MOVETYPE_WALK;
			noclip_anglehack = false;
		}
		else
			cl->privileged = true;
	}

	if(Cmd_Argc() != 2)
		return;

	for(j = 0, cl = svs.clients; j < svs.maxclients; j++, cl++)
	{
		if(!cl->active)
			continue;
		if(Q_strcasecmp(cl->name, Cmd_Argv(1)) == 0)
		{
			if(cl->privileged)
			{
				cl->privileged = false;
				cl->edict->v.flags = (int)cl->edict->v.flags & ~(FL_GODMODE | FL_NOTARGET);
				cl->edict->v.movetype = MOVETYPE_WALK;
				noclip_anglehack = false;
			}
			else
				cl->privileged = true;
			break;
		}
	}
}
#endif

void Host_Say(qboolean teamonly)
{
	client_t *client;
	client_t *save;
	int j;
	char *p;
	unsigned char text[64];
	qboolean fromServer = false;

	if(cmd_source == src_command)
	{
		if(cls.state == ca_dedicated)
		{
			fromServer = true;
			teamonly = false;
		}
		else
		{
			Cmd_ForwardToServer();
			return;
		}
	}

	if(Cmd_Argc() < 2)
		return;

	save = host_client;

	p = Cmd_Args();
	// remove quotes if present
	if(*p == '"')
	{
		p++;
		p[Q_strlen(p) - 1] = 0;
	}

	// turn on color set 1
	if(!fromServer)
		sprintf(text, "%c%s: ", 1, save->name);
	else
		sprintf(text, "%c<%s> ", 1, hostname.string);

	j = sizeof(text) - 2 - Q_strlen(text); // -2 for /n and null terminator
	if(Q_strlen(p) > j)
		p[j] = 0;

	strcat(text, p);
	strcat(text, "\n");

	for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if(!client || !client->active || !client->spawned)
			continue;
		if(teamplay.value && teamonly && client->edict->v.team != save->edict->v.team)
			continue;
		host_client = client;
		SV_ClientPrintf(host_client, "%s", text);
	}
	host_client = save;

	Sys_Printf("%s", &text[1]);
}

void Host_Say_f()
{
	Host_Say(false);
}

void Host_Say_Team_f()
{
	Host_Say(true);
}

void Host_Tell_f()
{
	client_t *client;
	client_t *save;
	int j;
	char *p;
	char text[64];

	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	if(Cmd_Argc() < 3)
		return;

	Q_strcpy(text, host_client->name);
	Q_strcat(text, ": ");

	p = Cmd_Args();

	// remove quotes if present
	if(*p == '"')
	{
		p++;
		p[Q_strlen(p) - 1] = 0;
	}

	// check length & truncate if necessary
	j = sizeof(text) - 2 - Q_strlen(text); // -2 for /n and null terminator
	if(Q_strlen(p) > j)
		p[j] = 0;

	strcat(text, p);
	strcat(text, "\n");

	save = host_client;
	for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if(!client->active || !client->spawned)
			continue;
		if(Q_strcasecmp(client->name, Cmd_Argv(1)))
			continue;
		host_client = client;
		SV_ClientPrintf(host_client, "%s", text);
		break;
	}
	host_client = save;
}

/*
==================
Host_Color_f
==================
*/
/*
void Host_Color_f()
{
	int top, bottom;
	int playercolor;

	if(Cmd_Argc() == 1)
	{
		Con_Printf("\"color\" is \"%i %i\"\n", ((int)cl_color.value) >> 4, ((int)cl_color.value) & 0x0f);
		Con_Printf("color <0-13> [0-13]\n");
		return;
	}

	if(Cmd_Argc() == 2)
		top = bottom = atoi(Cmd_Argv(1));
	else
	{
		top = atoi(Cmd_Argv(1));
		bottom = atoi(Cmd_Argv(2));
	}

	top &= 15;
	if(top > 13)
		top = 13;
	bottom &= 15;
	if(bottom > 13)
		bottom = 13;

	playercolor = top * 16 + bottom;

	if(cmd_source == src_command)
	{
		Cvar_SetValue("_cl_color", playercolor);
		if(cls.state == ca_connected)
			Cmd_ForwardToServer();
		return;
	}

	host_client->topcolor = playercolor;
	host_client->bottomcolor = playercolor;
	host_client->edict->v.team = bottom + 1;

	// send notification to all clients
	MSG_WriteByte(&sv.reliable_datagram, svc_updatecolors);
	MSG_WriteByte(&sv.reliable_datagram, host_client - svs.clients);
	MSG_WriteByte(&sv.reliable_datagram, host_client->topcolor);
	MSG_WriteByte(&sv.reliable_datagram, host_client->bottomcolor);
}
*/

/*
==================
Host_Kill_f
==================
*/
void Host_Kill_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	if(sv_player->v.health <= 0)
	{
		SV_ClientPrintf(host_client, /*PRINT_HIGH,*/ "Can't suicide -- already dead!\n");
		return;
	}

	gGlobalVariables.time = sv.time;
	gEntityInterface.pfnClientKill(sv_player);
}

/*
==================
Host_Pause_f
==================
*/
void Host_Pause_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	if(!pausable.value)
		SV_ClientPrintf(host_client, "Pause not allowed.\n");
	else
	{
		sv.paused ^= 1;

		if(sv.paused)
			SV_BroadcastPrintf("%s paused the game\n", pr_strings + sv_player->v.netname);
		else
			SV_BroadcastPrintf("%s unpaused the game\n", pr_strings + sv_player->v.netname);

		// send notification to all clients
		MSG_WriteByte(&sv.reliable_datagram, svc_setpause);
		MSG_WriteByte(&sv.reliable_datagram, sv.paused);
	}
}

//===========================================================================

/*
==================
Host_Kick_f

Kicks a user off of the server
==================
*/
void Host_Kick_f()
{
	char *who;
	char *message = NULL;
	client_t *save;
	int i;
	qboolean byNumber = false;

	if(cmd_source == src_command)
	{
		if(!sv.active)
		{
			Cmd_ForwardToServer();
			return;
		}
	}
	//else if (gGlobalVariables.deathmatch && !host_client->privileged) // TODO: if cheats_active?
	//return;

	save = host_client;

	if(Cmd_Argc() > 2 && Q_strcmp(Cmd_Argv(1), "#") == 0)
	{
		i = Q_atof(Cmd_Argv(2)) - 1;
		if(i < 0 || i >= svs.maxclients)
			return;
		if(!svs.clients[i].active)
			return;
		host_client = &svs.clients[i];
		byNumber = true;
	}
	else
	{
		for(i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
		{
			if(!host_client->active)
				continue;
			if(Q_strcasecmp(host_client->name, Cmd_Argv(1)) == 0)
				break;
		}
	}

	if(i < svs.maxclients)
	{
		if(cmd_source == src_command)
			if(cls.state == ca_dedicated)
				who = "Console";
			else
				who = cl_name.string;
		else
			who = save->name;

		// can't kick yourself!
		if(host_client == save)
			return;

		if(Cmd_Argc() > 2)
		{
			message = COM_Parse(Cmd_Args());
			if(byNumber)
			{
				message++;             // skip the #
				while(*message == ' ') // skip white space
					message++;
				message += Q_strlen(Cmd_Argv(2)); // skip the number
			}
			while(*message && *message == ' ')
				message++;
		}
		if(message)
			SV_DropClient(host_client, false, "Kicked by %s: %s\n", who, message);
		else
			SV_DropClient(host_client, false, "Kicked by %s\n", who);
	}

	host_client = save;
}

/*
===============================================================================

DEBUGGING TOOLS

===============================================================================
*/

/*
==================
Host_Give_f
==================
*/
void Host_Give_f()
{
	char *t;
	int v, w;

	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}

	//if (gGlobalVariables.deathmatch && !host_client->privileged) // TODO: if cheats_active?
	//return;

	t = Cmd_Argv(1);
	v = atoi(Cmd_Argv(2));

	switch(t[0])
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if(t[0] >= '2')
			sv_player->v.items = (int)sv_player->v.items | (IT_SHOTGUN << (t[0] - '2'));
		break;

	case 's':
		sv_player->v.ammo_shells = v;
		break;
	case 'n':
		sv_player->v.ammo_nails = v;
		break;
	case 'r':
		sv_player->v.ammo_rockets = v;
		break;
	case 'h':
		sv_player->v.health = v;
		break;
	case 'c':
		sv_player->v.ammo_cells = v;
		break;
	}
}

edict_t *FindViewthing()
{
	int i;
	edict_t *e;

	for(i = 0; i < sv.num_edicts; i++)
	{
		e = EDICT_NUM(i);
		if(!strcmp(pr_strings + e->v.classname, "viewthing"))
			return e;
	}
	Con_Printf("No viewthing on map\n");
	return NULL;
}

/*
==================
Host_Viewmodel_f
==================
*/
void Host_Viewmodel_f()
{
	edict_t *e;
	model_t *m;

	e = FindViewthing();
	if(!e)
		return;

	m = Mod_ForName(Cmd_Argv(1), false);
	if(!m)
	{
		Con_Printf("Can't load %s\n", Cmd_Argv(1));
		return;
	}

	e->v.frame = 0;
	cl.model_precache[(int)e->v.modelindex] = m;
}

/*
==================
Host_Viewframe_f
==================
*/
void Host_Viewframe_f()
{
	edict_t *e;
	int f;
	model_t *m;

	e = FindViewthing();
	if(!e)
		return;
	m = cl.model_precache[(int)e->v.modelindex];

	f = atoi(Cmd_Argv(1));
	if(f >= m->numframes)
		f = m->numframes - 1;

	e->v.frame = f;
}

void PrintFrameName(model_t *m, int frame)
{
	aliashdr_t *hdr;
	maliasframedesc_t *pframedesc;

	hdr = (aliashdr_t *)Mod_Extradata(m);
	if(!hdr)
		return;
	pframedesc = &hdr->frames[frame];

	Con_Printf("frame %i: %s\n", frame, pframedesc->name);
}

/*
==================
Host_Viewnext_f
==================
*/
void Host_Viewnext_f()
{
	edict_t *e;
	model_t *m;

	e = FindViewthing();
	if(!e)
		return;
	m = cl.model_precache[(int)e->v.modelindex];

	e->v.frame = e->v.frame + 1;
	if(e->v.frame >= m->numframes)
		e->v.frame = m->numframes - 1;

	PrintFrameName(m, e->v.frame);
}

/*
==================
Host_Viewprev_f
==================
*/
void Host_Viewprev_f()
{
	edict_t *e;
	model_t *m;

	e = FindViewthing();
	if(!e)
		return;

	m = cl.model_precache[(int)e->v.modelindex];

	e->v.frame = e->v.frame - 1;
	if(e->v.frame < 0)
		e->v.frame = 0;

	PrintFrameName(m, e->v.frame);
}

/*
===============================================================================

DEMO LOOP CONTROL

===============================================================================
*/

/*
==================
Host_Startdemos_f
==================
*/
void Host_Startdemos_f()
{
	int i, c;

	if(cls.state == ca_dedicated)
	{
		if(!sv.active)
			Cbuf_AddText("map start\n");
		return;
	}

	c = Cmd_Argc() - 1;
	if(c > MAX_DEMOS)
	{
		Con_Printf("Max %i demos in demoloop\n", MAX_DEMOS);
		c = MAX_DEMOS;
	}
	Con_Printf("%i demo(s) in loop\n", c);

	for(i = 1; i < c + 1; i++)
		strncpy(cls.demos[i - 1], Cmd_Argv(i), sizeof(cls.demos[0]) - 1);

	if(!sv.active && cls.demonum != -1 && !cls.demoplayback)
	{
		cls.demonum = 0;
		CL_NextDemo();
	}
	else
		cls.demonum = -1;
}

/*
==================
Host_Demos_f

Return to looping demos
==================
*/
void Host_Demos_f()
{
	if(cls.state == ca_dedicated)
		return;
	if(cls.demonum == -1)
		cls.demonum = 1;
	CL_Disconnect_f();
	CL_NextDemo();
}

/*
==================
Host_Stopdemo_f

Return to looping demos
==================
*/
void Host_Stopdemo_f()
{
	if(cls.state == ca_dedicated)
		return;
	if(!cls.demoplayback)
		return;
	CL_StopPlayback();
	CL_Disconnect();
}

//=============================================================================

/*
====================
SV_SetMaster_f

Make a master server current
====================
*/
void SV_SetMaster_f () // TODO: Master_SetMaster_f
{
	// TODO
/*
	char	data[2];
	int		i;

	memset (&master_adr, 0, sizeof(master_adr));

	for (i=1 ; i<Cmd_Argc() ; i++)
	{
		if (!strcmp(Cmd_Argv(i), "none") || !NET_StringToAdr (Cmd_Argv(i), &master_adr[i-1]))
		{
			Con_Printf ("Setting nomaster mode.\n");
			return;
		}
		if (master_adr[i-1].port == 0)
			master_adr[i-1].port = BigShort (27000);

		Con_Printf ("Master server at %s\n", NET_AdrToString (master_adr[i-1]));

		Con_Printf ("Sending a ping.\n");

		data[0] = A2A_PING;
		data[1] = 0;
		NET_SendPacket (NS_SERVER, 2, data, master_adr[i-1]);
	}

	svs.last_heartbeat = -99999;
*/
}

/*
==================
SV_Heartbeat_f
==================
*/
void SV_Heartbeat_f () // TODO: Master_Heartbeat_f
{
	//svs.last_heartbeat = -9999; // TODO
}

/*
==================
Host_InitCommands
==================
*/
void Host_InitCommands()
{
	/*
	if (COM_CheckParm ("-cheats"))
	{
		sv_cheats.value = 1;
		Info_SetValueForStarKey (svs.info, "*cheats", "ON", MAX_SERVERINFO_STRING);
	}
	*/

	Cmd_AddCommand("status", Host_Status_f);
	Cmd_AddCommand("quit", Host_Quit_f);
	Cmd_AddCommand("exit", Host_Quit_f);
	Cmd_AddCommand("god", Host_God_f);
	Cmd_AddCommand("notarget", Host_Notarget_f);
	Cmd_AddCommand("fly", Host_Fly_f);
	Cmd_AddCommand("map", Host_Map_f);
	Cmd_AddCommand("restart", Host_Restart_f);
	Cmd_AddCommand("changelevel", Host_Changelevel_f);
	Cmd_AddCommand("changelevel2", Host_Changelevel2_f);
	Cmd_AddCommand("reconnect", Host_Reconnect_f);
	Cmd_AddCommand("noclip", Host_Noclip_f);
	Cmd_AddCommand("version", Host_Version_f);
#ifdef HEADCRABGODS
	Cmd_AddCommand("please", Host_Please_f);
#endif
	Cmd_AddCommand("say", Host_Say_f);
	Cmd_AddCommand("say_team", Host_Say_Team_f);
	Cmd_AddCommand("tell", Host_Tell_f);
	Cmd_AddCommand("kill", Host_Kill_f);
	Cmd_AddCommand("pause", Host_Pause_f);
	Cmd_AddCommand("kick", Host_Kick_f);
	Cmd_AddCommand("ping", Host_Ping_f);
	Cmd_AddCommand("load", Host_Loadgame_f);
	Cmd_AddCommand("save", Host_Savegame_f);
	Cmd_AddCommand("give", Host_Give_f);

	Cmd_AddCommand("startdemos", Host_Startdemos_f);
	Cmd_AddCommand("demos", Host_Demos_f);
	Cmd_AddCommand("stopdemo", Host_Stopdemo_f);

	Cmd_AddCommand("viewmodel", Host_Viewmodel_f);
	Cmd_AddCommand("viewframe", Host_Viewframe_f);
	Cmd_AddCommand("viewnext", Host_Viewnext_f);
	Cmd_AddCommand("viewprev", Host_Viewprev_f);

	Cmd_AddCommand("mcache", Mod_Print);
	
	Cmd_AddCommand ("setmaster", SV_SetMaster_f);
	Cmd_AddCommand ("heartbeat", SV_Heartbeat_f);
};