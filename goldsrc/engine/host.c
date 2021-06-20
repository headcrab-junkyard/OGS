/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-2001 Id Software, Inc.
 *	Copyright (C) 2018-2021 BlackPhrase
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
/// @brief coordinates spawning and killing of local servers

#include "quakedef.h"
#include "r_local.h"

/*

A server can always be started, even if the system started out as a client
to a remote system.

A client can NOT be started if the system started as a dedicated server.

Memory is cleared / released when a server or client begins, not when they end.

*/

quakeparms_t host_parms;

qboolean host_initialized; // true if into command execution
//qboolean	nomaster;

double host_frametime;
double host_time; // TODO: doesn't look like GS use that
double realtime;    // without any filtering or bounding
double oldrealtime; // last frame run
int host_framecount;

int host_hunklevel;

int minimum_memory;

client_t *host_client; // current client

jmp_buf host_abortserver;

byte *host_basepal;
byte *host_colormap;

cvar_t hostname = {"hostname", "unnamed"/*, FCVAR_SERVER*/};
cvar_t host_framerate = { "host_framerate", "0" }; // set for slow motion
cvar_t host_speeds = { "host_speeds", "0" };       // set for running times
cvar_t host_profile = { "host_profile", "0" };

cvar_t sys_ticrate = { "sys_ticrate", "0.05" };

cvar_t fraglimit = { "fraglimit", "0", false, true };
cvar_t timelimit = { "timelimit", "0", false, true };
cvar_t teamplay = { "teamplay", "0", false, true };

#if defined(OGS_DEV) || defined(OGS_DEBUG)
cvar_t developer = { "developer", "1" }; // show extra messages (should be 0 for release!)
#else
cvar_t developer = { "developer", "0" };
#endif

cvar_t skill = { "skill", "1" };           // 0 - 3
cvar_t deathmatch = { "deathmatch", "0", FCVAR_SERVER }; // 0, 1, or 2
cvar_t coop = { "coop", "0", FCVAR_SERVER };             // 0 or 1

cvar_t pausable = { "pausable", "1", FCVAR_SERVER };

playermove_t svpmove;

/*
================
Host_EndGame
================
*/
void Host_EndGame(const char *message, ...)
{
	va_list argptr;
	char string[1024];

	va_start(argptr, message);
	vsprintf(string, message, argptr);
	va_end(argptr);
	Con_DPrintf("Host_EndGame: %s\n", string);

	if(sv.active)
		Host_ShutdownServer(false);

	if(cls.state == ca_dedicated)
		Sys_Error("Host_EndGame: %s\n", string); // dedicated servers exit

	if(cls.demonum != -1)
		CL_NextDemo();
	else
		CL_Disconnect();

	longjmp(host_abortserver, 1);
}

/*
================
Host_Error

This shuts down both the client and server
================
*/
void Host_Error(const char *error, ...)
{
	va_list argptr;
	char string[1024];
	static qboolean inerror = false;

	if(inerror)
		Sys_Error("Host_Error: recursively entered");

	inerror = true;

	// TODO: revisit
	SCR_EndLoadingPlaque(); // reenable screen updates

	va_start(argptr, error);
	vsprintf(string, error, argptr);
	va_end(argptr);

	Con_Printf("Host_Error: %s\n", string);

	//qw
	//SV_FinalMessage (va("server crashed: %s\n", string));
	//SV_Shutdown();

	if(sv.active)
		Host_ShutdownServer(false);

	if(cls.state == ca_dedicated)
		Sys_Error("Host_Error: %s\n", string); // dedicated servers exit

	CL_Disconnect();
	cls.demonum = -1;

	inerror = false;

	longjmp(host_abortserver, 1);
}

/*
================
Host_FindMaxClients
================
*/
void Host_FindMaxClients()
{
	int i;

	svs.maxclients = 1;
	
	if(isDedicated)
	{
		cls.state = ca_dedicated;
		
		i = COM_CheckParm("-maxplayers");
	
		if(i)
		{
			if(i != (com_argc - 1))
				svs.maxclients = Q_atoi(com_argv[i + 1]);
			else
				svs.maxclients = 8;
		};
	}
	else
		cls.state = ca_disconnected;
	
	if(svs.maxclients < 1)
		svs.maxclients = 8;
	else if(svs.maxclients > MAX_SCOREBOARD) // TODO: MAX_PLAYERS
		svs.maxclients = MAX_SCOREBOARD; // TODO: MAX_PLAYERS

	svs.maxclientslimit = svs.maxclients;
	if(svs.maxclientslimit < 4)
		svs.maxclientslimit = 4;
	svs.clients = Hunk_AllocName(svs.maxclientslimit * sizeof(client_t), "clients");

	if(svs.maxclients > 1)
		Cvar_SetValue("deathmatch", 1.0);
	else
		Cvar_SetValue("deathmatch", 0.0);
}

/*
=======================
Host_InitLocal
======================
*/
void Host_InitLocal()
{
	Host_InitCommands();

	Cvar_RegisterVariable(&hostname);

	Cvar_RegisterVariable(&host_framerate);
	Cvar_RegisterVariable(&host_speeds);
	Cvar_RegisterVariable(&host_profile);

	Cvar_RegisterVariable(&sys_ticrate);

	Cvar_RegisterVariable(&fraglimit);
	Cvar_RegisterVariable(&timelimit);
	Cvar_RegisterVariable(&teamplay);

	Cvar_RegisterVariable(&developer);

	Cvar_RegisterVariable(&skill);
	Cvar_RegisterVariable(&deathmatch);
	Cvar_RegisterVariable(&coop);

	Cvar_RegisterVariable(&pausable);

	Host_FindMaxClients();

	host_time = 1.0; // so a think at time 0 won't get called
	
	if(COM_CheckParm("-developer"))
		Cvar_SetValue("developer", 1);
}

/*
===============
Host_WriteConfiguration

Writes key bindings and archived cvars to config.cfg
===============
*/
void Host_WriteConfiguration()
{
	FILE *f;

	// dedicated servers initialize the host but don't parse and set the config.cfg cvars
	if(host_initialized & !isDedicated)
	{
		f = fopen(va("%s/config.cfg", com_gamedir), "w");
		if(!f)
		{
			Con_Printf("Couldn't write config.cfg.\n");
			return;
		}
		
		fprintf(f, "// This file is overwritten whenever you change your user settings in the game.\n");
		fprintf(f, "// Add custom configurations to the file \"userconfig.cfg\".\n\n");
		
		fprintf(f, "unbindall\n");

		Key_WriteBindings(f);
		Cvar_WriteVariables(f);
		
		// TODO: in_mlook
		// TODO: in_jlook
		
		fprintf(f, "+mlook\n");
		//fprintf(f, "+jlook\n"); // TODO

		fprintf(f, "exec userconfig.cfg\n");
		
		fclose(f);
	}
}

/*
=================
SV_ClientPrintf

Sends text across to be displayed 
FIXME: make this just a stuffed echo?
=================
*/
void SV_ClientPrintf(client_t *cl, /*int level,*/ const char *fmt, ...)
{
	va_list argptr;
	char string[1024];

	//if (level < cl->messagelevel)
	//return;

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	MSG_WriteByte(&cl->netchan.message, svc_print);
	//MSG_WriteByte (&cl->netchan.message, level);
	MSG_WriteString(&cl->netchan.message, string);
}

/*
=================
SV_BroadcastPrintf

Sends text to all active clients
=================
*/
void SV_BroadcastPrintf(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];
	int i;

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	for(i = 0; i < svs.maxclients; i++)
		if(svs.clients[i].active && svs.clients[i].spawned)
		{
			MSG_WriteByte(&svs.clients[i].netchan.message, svc_print);
			MSG_WriteString(&svs.clients[i].netchan.message, string);
		}
}

/*
=================
Host_ClientCommands

Send text over to the client to be executed
=================
*/
void Host_ClientCommands(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
	MSG_WriteString(&host_client->netchan.message, string);
}

/*
=====================
SV_DropClient

Called when the player is getting totally kicked off the host
if (crash = true), don't bother sending signofs
=====================
*/
void SV_DropClient(client_t *drop, qboolean crash, char *fmt, ...)
{
	// TODO: fmt support

	// add the disconnect
	MSG_WriteByte(&drop->netchan.message, svc_disconnect);

	if(!crash)
	{
		// send any final messages (don't check for errors)
		if(Netchan_CanPacket(&drop->netchan)) // TODO: was NET_CanSendMessage; Netchan_CanReliable?
		{
			MSG_WriteByte(&drop->netchan.message, svc_disconnect);
			Netchan_Transmit(&drop->netchan, 0, drop->netchan.message.data);
		}

		if(drop->edict && drop->spawned)
		{
			// call the prog function for removing a client
			// this will set the body to a dead frame, among other things
			//gGlobalVariables.self = EDICT_TO_PROG(drop->edict);
			gEntityInterface.pfnClientDisconnect(drop->edict);
		}

		Sys_Printf("Client %s removed\n", drop->name);
	}

	// free the client (the body stays around)
	drop->active = false;
	drop->name[0] = 0;
	drop->old_frags = 0; // TODO: was -999999

	memset(drop->userinfo, 0, sizeof(drop->userinfo));

	// send notification to all remaining clients
	SV_FullClientUpdate(drop, &sv.reliable_datagram);
}

/*
==================
Host_ShutdownServer

This only happens at the end of a game, not between levels
==================
*/
void Host_ShutdownServer(qboolean crash)
{
	int i;
	int count;
	sizebuf_t buf;
	char message[4];
	double start;

	if(!sv.active)
		return;

	sv.active = false;

	// stop all client sounds immediately
	if(cls.state == ca_connected)
		CL_Disconnect();

	// flush any pending messages - like the score!!!
	start = Sys_FloatTime();
	do
	{
		count = 0;
		for(i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
		{
			if(host_client->active && host_client->netchan.message.cursize)
			{
				if(Netchan_CanPacket(&host_client->netchan)) // TODO: was NET_CanSendMessage; Netchan_CanReliable?
				{
					Netchan_Transmit(&host_client->netchan, strlen(message), buf.data);
					SZ_Clear(&host_client->netchan.message);
				}
				else
				{
					NET_GetPacket(NS_SERVER, &net_from, &host_client->netchan.message); // TODO: was NET_GetMessage; REVISIT: We're reading into write buffer?????????
					count++;
				}
			}
		}
		if((Sys_FloatTime() - start) > 3.0)
			break;
	} while(count);

	// make sure all the clients know we're disconnecting
	buf.data = message;
	buf.maxsize = 4;
	buf.cursize = 0;
	MSG_WriteByte(&buf, svc_disconnect);
	SV_BroadcastCommand("disconnect"); // TODO: was count = NET_SendToAll(&buf, 5);
	if(count)
		Con_Printf("Host_ShutdownServer: NET_SendToAll failed for %u clients\n", count);

	for(i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
		if(host_client->active)
			SV_DropClient(host_client, crash, "???");

	//
	// clear structures
	//
	memset(&sv, 0, sizeof(sv));
	memset(svs.clients, 0, svs.maxclientslimit * sizeof(client_t));
}

/*
================
Host_ClearMemory

This clears all the memory used by both the client and server, but does
not reinitialize anything.
================
*/
void Host_ClearMemory()
{
	Con_DPrintf("Clearing memory\n");
	D_FlushCaches();
	Mod_ClearAll();
	if(host_hunklevel) // FIXME: check this...
		Hunk_FreeToLowMark(host_hunklevel);

	cls.signon = 0;
	memset(&sv, 0, sizeof(sv));
	memset(&cl, 0, sizeof(cl));
}

//============================================================================

/*
===================
Host_FilterTime

Returns false if the time is too short to run a frame
===================
*/
qboolean Host_FilterTime(float time)
{
	realtime += time;

	if(!cls.timedemo && realtime - oldrealtime < 1.0 / 72.0)
		return false; // framerate is too high

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

	if(host_framerate.value > 0)
		host_frametime = host_framerate.value;
	else
	{ // don't allow really long or short frames
		if(host_frametime > 0.1)
			host_frametime = 0.1;
		if(host_frametime < 0.001)
			host_frametime = 0.001;
	}

	return true;
}

void Host_UpdateScreen()
{
	// TODO: something else?

	SCR_UpdateScreen();
	ClientDLL_HudRedraw(cl.intermission);

	// TODO: something else?
};

// TODO: void Host_GetHostInfo( float *fps, int *nActive, int *nSpectators, int *nMaxPlayers, char *pszMap );
void Host_UpdateStatus(float *fps, int *nActive, /*int *nSpectators, */ int *nMaxPlayers, char *pszMap)
{
	// TODO
	
	if(fps)
		*fps = 0.0f;
	
	if(nActive)
		*nActive = 0;
	
	if(nMaxPlayers)
		*nMaxPlayers = 0;
	
	if(pszMap)
		*pszMap = "none";
};

void Host_EndSection(const char *sName)
{
	// TODO
};

/*
==================
Host_Frame

Runs all active servers
==================
*/
void _Host_Frame(float time)
{
	static double time1 = 0;
	static double time2 = 0;
	static double time3 = 0;
	int pass1, pass2, pass3;

	if(setjmp(host_abortserver))
		return; // something bad happened, or the server disconnected

	// keep the random time dependent
	rand();

	// decide the simulation time
	if(!Host_FilterTime(time))
		return; // don't run too fast, or packets will flood out

	// get new key events
	Sys_SendKeyEvents();

	// allow mice or other external controllers to add commands
	ClientDLL_UpdateClientData(); // TODO: was IN_Commands(); is this the right place?

	// process console commands
	Cbuf_Execute();

	// fetch results from server
	//CL_ReadPackets(); // TODO: instead of NET_Poll

	// if running the server locally, make intentions now
	// resend a connection request if necessary
	if(sv.active)
	{
		if(cls.state == ca_disconnected)
			CL_CheckForResend();
		else
			CL_SendCmd();
	};

	//-------------------
	//
	// server operations
	//
	//-------------------

	if(sv.active)
		SV_Frame();

	//-------------------
	//
	// client operations
	//
	//-------------------
	
	// if in the debugger last frame, don't timeout
	if (time > 5.0f)
		cls.netchan.last_received = Sys_FloatTime ();
	
	// fetch results from server
	//if(cls.state == ca_connected)
	{
		CL_ReadPackets();
	};
	
	// if running the server remotely, send intentions now after
	// the incoming messages have been read
	if(!sv.active)
	{
		if(cls.state == ca_disconnected)
			CL_CheckForResend();
		else
			CL_SendCmd();
	};

	host_time += host_frametime;
	
	// TODO: should be somewhere around here
	//CL_FireEvents();
	
	// Set up prediction for other players
	CL_SetUpPlayerPrediction(false);

	// do client side motion prediction
	CL_PredictMove();

	// Set up prediction for other players
	CL_SetUpPlayerPrediction(true);

	// build a refresh entity list
	CL_EmitEntities();
	
	ClientDLL_Frame(host_frametime);

	// update video
	if(host_speeds.value)
		time1 = Sys_FloatTime();

	Host_UpdateScreen(); // TODO: was SCR_UpdateScreen
	
	if(host_speeds.value)
		time2 = Sys_FloatTime();

	// update audio
	if(cls.signon == SIGNONS)
	{
		S_Update(r_origin, vpn, vright, vup);
		CL_DecayLights();
	}
	else
		S_Update(vec3_origin, vec3_origin, vec3_origin, vec3_origin);

	CDAudio_Update();

	if(host_speeds.value)
	{
		pass1 = (time1 - time3) * 1000;
		time3 = Sys_FloatTime();
		pass2 = (time2 - time1) * 1000;
		pass3 = (time3 - time2) * 1000;
		Con_Printf("%3i tot %3i server %3i gfx %3i snd\n",
		           pass1 + pass2 + pass3, pass1, pass2, pass3);
	}

	host_framecount++;
}

int Host_Frame(float time, int iState, int *stateInfo)
{
	double time1, time2;
	static double timetotal;
	static int timecount;
	int i, c, m;

	if(!host_profile.value)
	{
		_Host_Frame(time);
		return 1;
	}

	time1 = Sys_FloatTime();
	_Host_Frame(time);
	time2 = Sys_FloatTime();

	timetotal += time2 - time1;
	timecount++;

	if(timecount < 1000)
		return 0;

	m = timetotal * 1000 / timecount;
	timecount = 0;
	timetotal = 0;
	c = 0;
	for(i = 0; i < svs.maxclients; i++)
	{
		if(svs.clients[i].active)
			c++;
	}

	Con_Printf("host_profile: %2i clients %2i msec\n", c, m);
	return 1;
}

//============================================================================

/*
extern int vcrFile;
#define	VCR_SIGNATURE	0x56435231
// "VCR1"

void Host_InitVCR (quakeparms_t *parms)
{
	int		i, len, n;
	char	*p;
	
	if (COM_CheckParm("-playback"))
	{
		if (com_argc != 2)
			Sys_Error("No other parameters allowed with -playback\n");

		FS_FileOpenRead("quake.vcr", &vcrFile);
		if (vcrFile == -1)
			Sys_Error("playback file not found\n");

		FS_FileRead (vcrFile, &i, sizeof(int));
		if (i != VCR_SIGNATURE)
			Sys_Error("Invalid signature in vcr file\n");

		FS_FileRead (vcrFile, &com_argc, sizeof(int));
		com_argv = malloc(com_argc * sizeof(char *));
		com_argv[0] = parms->argv[0];
		for (i = 0; i < com_argc; i++)
		{
			FS_FileRead (vcrFile, &len, sizeof(int));
			p = malloc(len);
			FS_FileRead (vcrFile, p, len);
			com_argv[i+1] = p;
		}
		com_argc++; // add one for arg[0]
		parms->argc = com_argc;
		parms->argv = com_argv;
	}

	if ( (n = COM_CheckParm("-record")) != 0)
	{
		vcrFile = FS_FileOpenWrite("quake.vcr");

		i = VCR_SIGNATURE;
		FS_FileWrite(vcrFile, &i, sizeof(int));
		i = com_argc - 1;
		FS_FileWrite(vcrFile, &i, sizeof(int));
		for (i = 1; i < com_argc; i++)
		{
			if (i == n)
			{
				len = 10;
				FS_FileWrite(vcrFile, &len, sizeof(int));
				FS_FileWrite(vcrFile, "-playback", len);
				continue;
			}
			len = Q_strlen(com_argv[i]) + 1;
			FS_FileWrite(vcrFile, &len, sizeof(int));
			FS_FileWrite(vcrFile, com_argv[i], len);
		}
	}
	
}
*/

/*
====================
Host_Init
====================
*/
void Host_Init(quakeparms_t *parms)
{
	minimum_memory = MINIMUM_MEMORY;

	if(COM_CheckParm("-minmemory"))
		parms->memsize = minimum_memory;

	host_parms = *parms;

	if(parms->memsize < minimum_memory)
		Sys_Error("Only %4.1f megs of memory available, can't execute game", parms->memsize / (float)0x100000);

	com_argc = parms->argc;
	com_argv = parms->argv;

	// TODO: check for -console/-toconsole/-dev startup args
	
	Memory_Init(parms->membase, parms->memsize);
	Cbuf_Init();
	Cmd_Init();
	Cvar_Init();

	V_Init();
	Chase_Init();
	//Host_InitVCR (parms);
	COM_Init(parms->basedir); // TODO: no basedir?
	Host_InitLocal();
	//Sys_Init ();

	W_LoadWadFile("gfx.wad");
	//W_LoadWadFile("fonts.wad"); // TODO

	Key_Init();
	Con_Init();
	PR_Init();
	Mod_Init();
	NET_Init();
	Netchan_Init();
	SV_Init();
	Pmove_Init(&svpmove);

	Con_Printf("Protocol version %d\n", PROTOCOL_VERSION);
	Con_Printf("Exe version %s/%s (%s)\n", "TODO", "TODO", com_gamedir); // Exe version 1.1.2.2/Stdio (tfc)
	Con_Printf("Exe build: " __TIME__ " " __DATE__ " (%04d)\n", build_number()); // TODO
	Con_Printf("%4.1f Mb heap\n", parms->memsize / (1024 * 1024.0));

	R_InitTextures(); // needed even for dedicated servers

	if(cls.state != ca_dedicated)
	{
		host_basepal = (byte *)COM_LoadHunkFile("gfx/palette.lmp");
		if(!host_basepal)
			Sys_Error("Host_Init: Couldn't load gfx/palette.lmp");
		
		// TODO: unused?
		host_colormap = (byte *)COM_LoadHunkFile("gfx/colormap.lmp");
		if(!host_colormap)
			Sys_Error("Couldn't load gfx/colormap.lmp");
		//

#ifndef _WIN32 // on non win32, mouse comes before video for security reasons
		IN_Init();
#endif
		VID_Init(host_basepal);
		// TODO: GL_Init() here + no VID_Shutdown in GS (at least for hw)

		Draw_Init();
		SCR_Init();
		R_Init();
#ifndef _WIN32
		// on Win32, sound initialization has to come before video initialization, so we
		// can put up a popup if the sound hardware is in use
		S_Init();
#else

#ifdef GLQUAKE
		// FIXME: doesn't use the new one-window approach yet
		S_Init();
#endif

#endif // _WIN32
		CDAudio_Init();
		Sbar_Init();
		CL_Init();
#ifdef _WIN32 // on non win32, mouse comes before video for security reasons
		IN_Init();
#endif

		// GUI should be initialized before the client dll
		VGui_Startup();

		// Initialize the client dll now
		ClientDLL_Init();
		
		Voice_RegisterCvars();
	}

	//Voice_LoadCodec("voice_speex");
	Cbuf_InsertText("exec valve.rc\n");
	
	// TODO
	//Cbuf_InsertText("exec language.cfg\n");
	//Cbuf_InsertText("exec autoexec.cfg\n");
	//Cbuf_InsertText("exec skill.cfg\n");
	//Cbuf_InsertText("exec config.cfg\n");
	//Cbuf_InsertText("exec userconfig.cfg\n");

	Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
	host_hunklevel = Hunk_LowMark();

	//Cbuf_InsertText ("exec server.cfg\n"); // TODO: dedicated

	host_initialized = true;

	//Con_Printf ("\nServer Version %4.2f (Build %04d)\n\n", VERSION, build_number());
	
	//return true;
}

/*
===============
Host_Shutdown

FIXME: this is a callback from Sys_Quit and Sys_Error.  It would be better
to run quit through here before the final handoff to the sys code.
===============
*/
void Host_Shutdown()
{
	static qboolean isdown = false;

	if(isdown)
	{
		printf("recursive shutdown\n");
		return;
	}
	isdown = true;

	// keep Con_Printf from trying to update the screen
	scr_disabled_for_loading = true; // TODO: revisit

	Host_WriteConfiguration();

	ClientDLL_Shutdown();
	
	VGui_Shutdown();

	CDAudio_Shutdown();
	
	SV_Shutdown();
	NET_Shutdown();
	S_Shutdown();
	IN_Shutdown();

	if(cls.state != ca_dedicated)
		VID_Shutdown();
}