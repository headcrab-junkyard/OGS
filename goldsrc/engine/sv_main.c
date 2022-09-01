/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2020-2022 BlackPhrase
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
/// @brief server main program

#include "quakedef.h"

#define MAX_IPFILTERS 1024

// Log cvars decls
extern cvar_t mp_logfile;
extern cvar_t mp_logecho;

extern cvar_t sv_log_onefile;
extern cvar_t sv_log_singleplayer;

extern cvar_t sv_logsecret;

// Physics cvars decls
extern cvar_t sv_friction;
extern cvar_t sv_stopspeed;
extern cvar_t sv_gravity;
extern cvar_t sv_maxvelocity;
extern cvar_t sv_stepsize;
extern cvar_t sv_bounce;
extern cvar_t sv_spectatormaxspeed;

extern cvar_t sv_airaccelerate;
extern cvar_t sv_wateraccelerate;

extern cvar_t sv_waterfriction;
extern cvar_t sv_zmax;
extern cvar_t sv_wateramp;
extern cvar_t sv_skyname;

// User cvars decls
extern cvar_t mp_consistency;
extern cvar_t mp_footsteps;

extern cvar_t sv_edgefriction;

extern cvar_t sv_maxspeed;
extern cvar_t sv_accelerate;

extern cvar_t sv_rollspeed;
extern cvar_t sv_rollangle;

extern cvar_t sv_unlag;
extern cvar_t sv_maxunlag;

extern cvar_t sv_unlagpush;
extern cvar_t sv_unlagsamples;

extern cvar_t sv_voiceenable;

//

typedef struct
{
	unsigned mask;
	unsigned compare;
} ipfilter_t;

server_t sv;         // local server
server_static_t svs; // persistent server info

ipfilter_t ipfilters[MAX_IPFILTERS];
int numipfilters;

char localinfo[MAX_LOCALINFO_STRING+1]; // local game info

char localmodels[MAX_MODELS][5]; // inline model names for precache

cvar_t rcon_password = {"rcon_password", ""}; // password for remote server commands

cvar_t sv_enableoldqueries = {"sv_enableoldqueries", "0"}; // TODO: register

cvar_t sv_instancedbaseline = {"sv_instancedbaseline", "1"}; // TODO: register

cvar_t sv_contact = {"sv_contact", "", FCVAR_SERVER}; // TODO: register

cvar_t sv_minupdaterate = {"sv_minupdaterate", "10"};
cvar_t sv_maxupdaterate = {"sv_maxupdaterate", "30"};

cvar_t sv_minrate = {"sv_minrate", "0", FCVAR_SERVER};
cvar_t sv_maxrate = {"sv_maxrate", "0", FCVAR_SERVER};

cvar_t sv_filterban = { "sv_filterban", "1" };

cvar_t sv_logrelay = {"sv_logrelay", "0"}; // TODO: register

cvar_t sv_language = {"sv_language", "0"}; // TODO: register

cvar_t violence_hblood = {"violence_hblood", "1", FCVAR_ARCHIVE}; // TODO: register
cvar_t violence_ablood = {"violence_ablood", "1", FCVAR_ARCHIVE}; // TODO: register
cvar_t violence_hgibs = {"violence_hgibs", "1", FCVAR_ARCHIVE}; // TODO: register
cvar_t violence_agibs = {"violence_agibs", "1", FCVAR_ARCHIVE}; // TODO: register

cvar_t sv_newunit = {"sv_newunit", "0"}; // TODO: register

cvar_t sv_aim = { "sv_aim", "1", FCVAR_ARCHIVE | FCVAR_SERVER };

cvar_t laddermode = {"laddermode", "0"}; // TODO: register

cvar_t sv_clienttrace = {"sv_clienttrace", "1", FCVAR_SERVER}; // TODO: register

cvar_t sv_timeout = { "sv_timeout", "60" }; // seconds without any message

cvar_t sv_failuretime = {"sv_failuretime", "0.5"}; // TODO: register

cvar_t sv_cheats = {"sv_cheats", "0", FCVAR_SERVER};

cvar_t sv_password = {"sv_password", "", FCVAR_SERVER};	// password for entering the game

cvar_t sv_lan = {"sv_lan", "0"}; // TODO: register, set to 0?
cvar_t sv_lan_rate = {"sv_lan_rate", "20000"}; // TODO: register

cvar_t sv_proxies = {"sv_proxies", "1", FCVAR_SERVER}; // TODO: register

cvar_t sv_outofdatetime = {"sv_outofdatetime", "1800"}; // TODO: register

cvar_t mapcyclefile = {"mapcyclefile", "mapcycle.txt"}; // TODO: register, set to ""/0?
cvar_t motdfile = {"motdfile", "motd.txt"}; // TODO: register, set to ""/0?
cvar_t servercfgfile = {"servercfgfile", "server.cfg"}; // TODO: register, set to ""/0?
cvar_t mapchangecfgfile = {"mapchangecfgfile", ""}; // TODO: register
cvar_t lservercfgfile = {"lservercfgfile", "listenserver.cfg"}; // TODO: register, set to ""/0?
cvar_t logsdir = {"logsdir", "logs"}; // TODO: register, set to ""/0?
cvar_t bannedcfgfile = {"bannedcfgfile", "banned.cfg"}; // TODO: register, set to ""/0?

cvar_t sv_allow_download = {"sv_allowdownload", "1"};

cvar_t sv_send_logos = {"sv_send_logos", "1"}; // TODO: register
cvar_t sv_send_resources = {"sv_send_resources", "1"}; // TODO: register

cvar_t sv_logbans = {"sv_logbans", "0"}; // TODO: register

cvar_t sv_allow_upload = {"sv_allowupload", "1", FCVAR_SERVER};

cvar_t sv_uploadmax = {"sv_uploadmax", "0.5", FCVAR_SERVER}; // TODO: register

cvar_t hpk_maxsize = {"hpk_maxsize", "4", FCVAR_ARCHIVE}; // TODO: register

cvar_t sv_skycolor_r = {"sv_skycolor_r", "0"};
cvar_t sv_skycolor_g = {"sv_skycolor_g", "0"};
cvar_t sv_skycolor_b = {"sv_skycolor_b", "0"};

cvar_t sv_skyvec_x = {"sv_skyvec_x", "0"};
cvar_t sv_skyvec_y = {"sv_skyvec_y", "0"};
cvar_t sv_skyvec_z = {"sv_skyvec_z", "0"};

cvar_t sv_visiblemaxplayers = {"sv_visiblemaxplayers", "-1"}; // TODO: register

cvar_t max_queries_sec = {"max_queries_sec", "3", FCVAR_SERVER}; // TODO: register
cvar_t max_queries_sec_global = {"max_queries_sec_global", "30", FCVAR_SERVER}; // TODO: register
cvar_t max_queries_window = {"max_queries_window", "60", FCVAR_SERVER}; // TODO: register

cvar_t sv_logblocks = {"sv_logblocks", "0", FCVAR_SERVER}; // TODO: register

cvar_t sv_downloadurl = {"sv_downloadurl", ""}; // TODO: register

cvar_t sv_allow_dlfile = {"sv_allow_dlfile", "1"}; // TODO: register

cvar_t sv_version = {"sv_version", ""}; // TODO: register

cvar_t sv_rcon_minfailures = {"sv_rcon_minfailures", "5"}; // TODO: register
cvar_t sv_rcon_maxfailures = {"sv_rcon_maxfailures", "10"}; // TODO: register
cvar_t sv_rcon_minfailuretime = {"sv_rcon_minfailuretime", "30"}; // TODO: register
cvar_t sv_rcon_banpenalty = {"sv_rcon_banpenalty", "0"}; // TODO: register

//

extern cvar_t sv_voiceenable;
extern cvar_t sv_voicecodec;
extern cvar_t sv_voicequality;

extern cvar_t sv_zmax;
extern cvar_t sv_wateramp;

extern cvar_t mp_footsteps;

extern cvar_t sv_skyname;

/*
extern cvar_t sv_skycolor_r;
extern cvar_t sv_skycolor_g;
extern cvar_t sv_skycolor_b;

extern cvar_t sv_skyvec_x;
extern cvar_t sv_skyvec_y;
extern cvar_t sv_skyvec_z;
*/

//============================================================================

void SV_WriteSpawn(client_t *client)
{
	// don't begin again
	if(client->spawned)
		return;
	
	client->spawned = true;
	
	// TODO: client connection code? ClientPutInServer?
	
	// clear the net statistics, because connecting gives a bogus picture
	client->netchan.frame_latency = 0;
	client->netchan.frame_rate = 0;
	client->netchan.drop_count = 0;
	client->netchan.good_count = 0;
};

/*
==================
SV_Spawn_f
==================
*/
void SV_Spawn_f() // TODO: was Host_Spawn_f
{
	if(cmd_source == src_command)
	{
		Con_Printf("spawn is not valid from the console\n");
		return;
	}

	if(host_client->spawned)
	{
		Con_Printf("Spawn not valid -- already spawned\n");
		return;
	}
	
	// NOTE: prespawn
	SZ_Write(&host_client->netchan.message, sv.signon.data, sv.signon.cursize);
	
	// handle the case of a level changing while a client was connecting
	if ( atoi(Cmd_Argv(1)) != svs.spawncount )
	{
		Con_Printf ("SV_Spawn_f from different level\n");
		SV_New_f ();
		return;
	}
	
	SV_WriteSpawn(host_client);
	
	// actually spawn the player // TODO: QW
	//gGlobalVariables.time = sv.time; // TODO: QW
	gEntityInterface.pfnClientPutInServer(sv_player);
	
	//if ((Sys_FloatTime() - host_client->netchan.connecttime) <= sv.time) // TODO
		Sys_Printf("%s entered the game\n", host_client->name);
};

/*
================
SV_New_f

Sends the first message from the server to a connected client.
This will be sent on the initial connection and upon each server load.
================
*/
void SV_New_f ()
{
	int i;
	client_t *client;
	edict_t *ent;
	
	if (host_client->spawned)
		return;

	// run the entrance script
	if(sv.loadgame)
	{
		// loaded games are fully inited already
		// if this is the last client to be connected, unpause
		sv.paused = false;
	}
	else
	{
		// set up the edict
		ent = host_client->edict;

		memset(&ent->v, 0, sizeof(ent->v));
		ent->v.colormap = NUM_FOR_EDICT(ent);
		ent->v.team = (host_client->topcolor & 15) + 1;
		ent->v.netname = PR_SetString(host_client->name);

		// TODO: check for spectator in QW; unused in GS
		/*
		if(host_client->spectator)
		{
			SV_SpawnSpectator();

			if(gEntityInterface.pfnSpectatorConnect)
			{
				// copy spawn parms out of the client_t
				for (i=0 ; i< NUM_SPAWN_PARMS ; i++)
					(&gGlobalVariables.parm1)[i] = host_client->spawn_parms[i];
		
				// call the spawn function
				gGlobalVariables.time = sv.time;
				gEntityInterface.pfnSpectatorConnect(sv_player);
			};
		}
		else
		*/
		{
			// copy spawn parms out of the client_t
			//for(i = 0; i < NUM_SPAWN_PARMS; i++)
				//(&gGlobalVariables.parm1)[i] = host_client->spawn_parms[i];

			// call the spawn function

			gGlobalVariables.time = sv.time;
			if(!gEntityInterface.pfnClientConnect(sv_player, host_client->name, "TODO", NULL)) // TODO
				return; // TODO
		}
	}

	// send all current names, colors, and frag counts
	SZ_Clear(&host_client->netchan.message);
	
	host_client->connected = true; // TODO
	host_client->connection_started = realtime;
	host_client->spawned = false; // need prespawn, spawn, etc
	
	// send the info about the new client to all connected clients
	SV_FullClientUpdate(host_client, &sv.reliable_datagram);
	//host_client->sendinfo = true;

//NOTE:  This doesn't go through ClientReliableWrite since it's before the user
//spawns.  These functions are written to not overflow
	// TODO
	/*
	if (host_client->num_backbuf) {
		Con_Printf("WARNING %s: [SV_New] Back buffered (%d0, clearing", host_client->name, host_client->netchan.message.cursize); 
		host_client->num_backbuf = 0;
		SZ_Clear(&host_client->netchan.message);
	}
	*/
	
	// send the serverdata
	SV_SendServerinfo(host_client);
	
	SV_SendExtraServerinfo(host_client);
	
	SV_SendDeltaDescription(host_client);
	
	// send the movevars
	SV_SendMoveVars(host_client);

	// send music
	// TODO
	MSG_WriteByte(&host_client->netchan.message, svc_cdtrack);
	//MSG_WriteByte(&host_client->netchan.message, sv.edicts->v.sounds);
	//MSG_WriteByte(&host_client->netchan.message, sv.edicts->v.sounds);
	MSG_WriteByte(&host_client->netchan.message, 0); // TODO
	MSG_WriteByte(&host_client->netchan.message, 0); // TODO
	
	// set view
	MSG_WriteByte(&host_client->netchan.message, svc_setview);
	MSG_WriteShort(&host_client->netchan.message, NUM_FOR_EDICT(host_client->edict));

	// send user messages
	SV_SendUserMessages(host_client);
	
	// send server info string
	MSG_WriteByte (&host_client->netchan.message, svc_stufftext);
	MSG_WriteString (&host_client->netchan.message, va("fullserverinfo \"%s\"\n", svs.info) );
	
	SV_UpdateUserInfo(host_client); // NOTE: this will update/send info about each connected client to this client
	
	SV_RequestResourceList(host_client); // allow the client to send its resources (a custom logo decal)
	
	// TODO: after this the client might send a clc_resourcelist msg which should be parsed
	
	SV_SendResourceLocation(host_client); // TODO: some conditions (maybe if we actually need to send something or the cvar is not null)
	
	SV_SendResourceList(host_client);
	
	// TODO: cut here?
	// TODO: the rest is part of signon msg?
	
	//host_client->sendsignon = true;
	
	// TODO: svc_spawnbaseline
	
	// send time of update
	SV_SendTime(host_client);
	
	SV_UpdateUserInfo(host_client); // TODO: again?

	// send all current light styles
	SV_SendLightStyles(host_client);

	//
	// send some stats (force stats to be updated)
	//
	// TODO
	/*
	Q_memset (host_client->stats, 0, sizeof(host_client->stats));
	
	MSG_WriteByte(&host_client->netchan.message, svc_updatestat);
	MSG_WriteByte(&host_client->netchan.message, STAT_TOTALSECRETS);
	MSG_WriteLong(&host_client->netchan.message, gGlobalVariables.total_secrets);

	MSG_WriteByte(&host_client->netchan.message, svc_updatestat);
	MSG_WriteByte(&host_client->netchan.message, STAT_TOTALMONSTERS);
	MSG_WriteLong(&host_client->netchan.message, gGlobalVariables.total_monsters);

	MSG_WriteByte(&host_client->netchan.message, svc_updatestat);
	MSG_WriteByte(&host_client->netchan.message, STAT_SECRETS);
	MSG_WriteLong(&host_client->netchan.message, gGlobalVariables.found_secrets);

	MSG_WriteByte(&host_client->netchan.message, svc_updatestat);
	MSG_WriteByte(&host_client->netchan.message, STAT_MONSTERS);
	MSG_WriteLong(&host_client->netchan.message, gGlobalVariables.killed_monsters);
	*/

	//
	// send a fixangle
	// Never send a roll angle, because savegames can catch the server
	// in a state where it is expecting the client to correct the angle
	// and it won't happen if the game was just loaded, so you wind up
	// with a permanent head tilt
	ent = EDICT_NUM(1 + (host_client - svs.clients));
	MSG_WriteByte(&host_client->netchan.message, svc_setangle);
	for(i = 0; i < 2; i++)
		MSG_WriteAngle(&host_client->netchan.message, ent->v.angles[i]);
	MSG_WriteAngle(&host_client->netchan.message, 0);

	SV_WriteClientdataToMessage(sv_player, &host_client->netchan.message);
	
	MSG_WriteByte(&host_client->netchan.message, svc_signonnum);
	MSG_WriteByte(&host_client->netchan.message, 1);
	//host_client->sendsignon = true;
	
	SV_SendVoiceInit(host_client);
	
	MSG_WriteByte(&host_client->netchan.message, -1);
};

// TODO: TODO what?
void SV_SendServerInfoChange(const char *key, const char *value)
{
	if (!sv.state)
		return;

	MSG_WriteByte (&sv.reliable_datagram, svc_serverinfo);
	MSG_WriteString (&sv.reliable_datagram, key);
	MSG_WriteString (&sv.reliable_datagram, value);
};

/*
==================
SV_UpdateUserInfo
==================
*/
// TODO: this func updates info about each other client, not only one
void SV_UpdateUserInfo(client_t *client) // TODO: client_t *otherclient?
{
	int i;
	client_t *otherclient;
	
	for(i = 0, otherclient = svs.clients; i < svs.maxclients; ++i, ++otherclient)
	{
		MSG_WriteByte(&client->netchan.message, svc_updateuserinfo);
		MSG_WriteByte(&client->netchan.message, i);
		MSG_WriteLong(&client->netchan.message, otherclient->userid);
		MSG_WriteString(&client->netchan.message, otherclient->userinfo);
		for(int j = 0; j <= 15; ++j) // TODO: MSG_WriteBuf
			MSG_WriteByte(&client->netchan.message, otherclient->cdkeyhash[j]);
	}
};

/*
==================
SV_BanID_f
==================
*/
void SV_BanID_f()
{
	// TODO
};

/*
==================
SV_WriteID_f
==================
*/
void SV_WriteID_f()
{
	// TODO
};

/*
==================
SV_RemoveID_f
==================
*/
void SV_RemoveID_f()
{
	// TODO
};

/*
==================
SV_ListID_f
==================
*/
void SV_ListID_f()
{
	// TODO
};

/*
==================
SV_ResetRcon_f
==================
*/
void SV_ResetRcon_f()
{
	// TODO
};

void SV_LogAddress_f()
{
	// TODO
};

void SV_LogAddressAdd_f()
{
	// TODO
};

void SV_LogAddressDel_f()
{
	// TODO
};

void SV_Log_f()
{
	// TODO
};

/*
===========
SV_Serverinfo_f

  Examine or change the serverinfo string
===========
*/
char *CopyString(const char *s);
void SV_Serverinfo_f ()
{
	cvar_t	*var;

	if (Cmd_Argc() == 1)
	{
		Con_Printf ("Server info settings:\n");
		Info_Print (svs.info);
		return;
	};

	if (Cmd_Argc() != 3)
	{
		Con_Printf ("usage: serverinfo [ <key> <value> ]\n");
		return;
	};

	if (Cmd_Argv(1)[0] == '*')
	{
		Con_Printf ("Star variables cannot be changed.\n");
		return;
	};
	
	Info_SetValueForKey (svs.info, Cmd_Argv(1), Cmd_Argv(2), MAX_SERVERINFO_STRING);

	// if this is a cvar, change it too	
	var = Cvar_FindVar (Cmd_Argv(1));
	if (var)
	{
		Z_Free (var->string);	// free the old value string	
		var->string = CopyString (Cmd_Argv(2));
		var->value = Q_atof (var->string);
	};

	SV_SendServerInfoChange(Cmd_Argv(1), Cmd_Argv(2));
};

void SV_FullServerInfo_f()
{
	// TODO
};

/*
===========
SV_Localinfo_f

  Examine or change the localinfo string
===========
*/
void SV_Localinfo_f ()
{
	if (Cmd_Argc() == 1)
	{
		Con_Printf ("Local info settings:\n");
		Info_Print (localinfo);
		return;
	};

	if (Cmd_Argc() != 3)
	{
		Con_Printf ("usage: localinfo [ <key> <value> ]\n");
		return;
	};

	if (Cmd_Argv(1)[0] == '*')
	{
		Con_Printf ("Star variables cannot be changed.\n");
		return;
	};
	Info_SetValueForKey (localinfo, Cmd_Argv(1), Cmd_Argv(2), MAX_LOCALINFO_STRING);
};

void SV_ShowInfo_f()
{
	// TODO
};

// TODO
/*
==================
SV_SetPlayer

Sets host_client and sv_player to the player with idnum Cmd_Argv(1)
==================
*/
qboolean SV_SetPlayer ()
{
	client_t	*cl;
	int			i;
	int			idnum;

	idnum = atoi(Cmd_Argv(1));

	for (i=0,cl=svs.clients ; i<svs.maxclients ; i++,cl++)
	{
		if (!cl->connected)
			continue;
		if (cl->userid == idnum)
		{
			host_client = cl;
			sv_player = host_client->edict;
			return true;
		}
	}
	Con_Printf ("Userid %i is not on the server\n", idnum);
	return false;
};

/*
===========
SV_User_f

Examine a users info strings
===========
*/
void SV_User_f ()
{
	if (Cmd_Argc() != 2)
	{
		Con_Printf ("Usage: info <userid>\n");
		return;
	};

	if (!SV_SetPlayer ())
		return;

	Info_Print (host_client->userinfo);
};

/*
===========
SV_Users_f
===========
*/
void SV_Users_f()
{
	// TODO
};

/*
===========
SV_DownloadFile_f
===========
*/
void SV_DownloadFile_f()
{
	// TODO
};

/*
=================
StringToFilter
=================
*/
qboolean StringToFilter(const char *s, ipfilter_t *f)
{
	char num[128];
	int i, j;
	byte b[4];
	byte m[4];

	for(i = 0; i < 4; i++)
	{
		b[i] = 0;
		m[i] = 0;
	}

	for(i = 0; i < 4; i++)
	{
		if(*s < '0' || *s > '9')
		{
			Con_Printf("Bad filter address: %s\n", s);
			return false;
		}

		j = 0;
		while(*s >= '0' && *s <= '9')
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		if(b[i] != 0)
			m[i] = 255;

		if(!*s)
			break;
		s++;
	}

	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;

	return true;
};

/*
=================
SV_AddIP_f
=================
*/
void SV_AddIP_f ()
{
	int		i;
	
	for (i=0 ; i<numipfilters ; i++)
		if (ipfilters[i].compare == 0xffffffff)
			break;		// free spot
	if (i == numipfilters)
	{
		if (numipfilters == MAX_IPFILTERS)
		{
			Con_Printf ("IP filter list is full\n");
			return;
		}
		numipfilters++;
	}
	
	if (!StringToFilter (Cmd_Argv(1), &ipfilters[i]))
		ipfilters[i].compare = 0xffffffff;
}

/*
=================
SV_RemoveIP_f
=================
*/
void SV_RemoveIP_f ()
{
	ipfilter_t	f;
	int			i, j;

	if (!StringToFilter (Cmd_Argv(1), &f))
		return;
	for (i=0 ; i<numipfilters ; i++)
		if (ipfilters[i].mask == f.mask
		&& ipfilters[i].compare == f.compare)
		{
			for (j=i+1 ; j<numipfilters ; j++)
				ipfilters[j-1] = ipfilters[j];
			numipfilters--;
			Con_Printf ("Removed.\n");
			return;
		}
	Con_Printf ("Didn't find %s.\n", Cmd_Argv(1));
}

/*
=================
SV_ListIP_f
=================
*/
void SV_ListIP_f ()
{
	int		i;
	byte	b[4];

	Con_Printf ("Filter list:\n");
	for (i=0 ; i<numipfilters ; i++)
	{
		*(unsigned *)b = ipfilters[i].compare;
		Con_Printf ("%3i.%3i.%3i.%3i\n", b[0], b[1], b[2], b[3]);
	}
}

/*
=================
SV_WriteIP_f
=================
*/
void SV_WriteIP_f ()
{
	FILE	*f;
	char	name[MAX_OSPATH];
	byte	b[4];
	int		i;

	sprintf (name, "%s/listip.cfg", com_gamedir);

	Con_Printf ("Writing %s.\n", name);

	f = fopen (name, "wb");
	if (!f)
	{
		Con_Printf ("Couldn't open %s\n", name);
		return;
	}
	
	for (i=0 ; i<numipfilters ; i++)
	{
		*(unsigned *)b = ipfilters[i].compare;
		fprintf (f, "addip %i.%i.%i.%i\n", b[0], b[1], b[2], b[3]);
	}
	
	fclose (f);
}

/*
===============
SV_DropClient_f
===============
*/
void SV_DropClient_f()
{
	// TODO
};

/*
===============
SV_SendResource_f
===============
*/
void SV_SendResource_f()
{
	// TODO
};

/*
===============
SV_SendEnts_f
===============
*/
void SV_SendEnts_f()
{
	// TODO
};

/*
===============
SV_FullUpdate_f
===============
*/
void SV_FullUpdate_f()
{
	// TODO
};

/*
===============
SV_Init
===============
*/
void SV_Init()
{
	int i;
	
	// TODO: SV_InitOperatorCommands()?
	
	Cmd_AddCommand("banid", SV_BanID_f);
	Cmd_AddCommand("writeid", SV_WriteID_f);
	Cmd_AddCommand("removeid", SV_RemoveID_f);
	Cmd_AddCommand("listid", SV_ListID_f);
	
	Cmd_AddCommand("resetrcon", SV_ResetRcon_f);
	
	Cmd_AddCommand("logaddress", SV_LogAddress_f);
	Cmd_AddCommand("logaddress_add", SV_LogAddressAdd_f);
	Cmd_AddCommand("logaddress_del", SV_LogAddressDel_f);
	
	Cmd_AddCommand("log", SV_Log_f);
	
	Cmd_AddCommand("serverinfo", SV_Serverinfo_f);
	Cmd_AddCommand("localinfo", SV_Localinfo_f);
	
	Cmd_AddCommand("showinfo", SV_ShowInfo_f);
	
	Cmd_AddCommand("user", SV_User_f);
	Cmd_AddCommand("users", SV_Users_f);
	
	Cmd_AddCommand("dlfile", SV_DownloadFile_f);
	
	Cmd_AddCommand("addip", SV_AddIP_f);
	Cmd_AddCommand("removeip", SV_RemoveIP_f);
	Cmd_AddCommand("listip", SV_ListIP_f);
	Cmd_AddCommand("writeip", SV_WriteIP_f);
	
	Cmd_AddCommand("dropclient", SV_DropClient_f);
	Cmd_AddCommand("spawn", SV_Spawn_f);
	Cmd_AddCommand("new", SV_New_f);
	
	Cmd_AddCommand("sendres", SV_SendResource_f);
	Cmd_AddCommand("sendents", SV_SendEnts_f);
	
	Cmd_AddCommand("fullupdate", SV_FullUpdate_f);
	
	//
	
	Cvar_RegisterVariable(&sv_timeout);

	Cvar_RegisterVariable(&sv_maxvelocity);
	
	Cvar_RegisterVariable(&sv_gravity);
	
	Cvar_RegisterVariable(&sv_friction);
	Cvar_RegisterVariable(&sv_edgefriction);
	Cvar_RegisterVariable(&sv_waterfriction);
	
	Cvar_RegisterVariable(&sv_stopspeed);
	
	Cvar_RegisterVariable(&sv_stepsize);
	
	Cvar_RegisterVariable(&sv_bounce);
	
	Cvar_RegisterVariable(&sv_maxspeed);
	Cvar_RegisterVariable(&sv_spectatormaxspeed);
	
	Cvar_RegisterVariable(&sv_accelerate);
	Cvar_RegisterVariable(&sv_airaccelerate);
	Cvar_RegisterVariable(&sv_wateraccelerate);
	
	Cvar_RegisterVariable(&sv_aim);
	
	Cvar_RegisterVariable(&sv_filterban);
	
	Cvar_RegisterVariable(&rcon_password);
	Cvar_RegisterVariable(&sv_password);
	
	Cvar_RegisterVariable(&sv_allow_download);
	
	Cvar_RegisterVariable(&sv_allow_upload);
	
	Cvar_RegisterVariable(&sv_cheats);
	
	Cvar_RegisterVariable(&sv_rollspeed);
	Cvar_RegisterVariable(&sv_rollangle);
	
	Cvar_RegisterVariable(&sv_unlag);
	Cvar_RegisterVariable(&sv_maxunlag);
	
	Cvar_RegisterVariable(&sv_unlagpush);
	Cvar_RegisterVariable(&sv_unlagsamples);
	
	// TODO: unused
	Cvar_RegisterVariable(&sv_minrate);
	Cvar_RegisterVariable(&sv_maxrate);
	
	// TODO: unused
	Cvar_RegisterVariable(&sv_minupdaterate);
	Cvar_RegisterVariable(&sv_maxupdaterate);
	
	Cvar_RegisterVariable(&sv_voiceenable); // TODO: unused
	
	Cvar_RegisterVariable(&sv_zmax);
	Cvar_RegisterVariable(&sv_wateramp);
	
	Cvar_RegisterVariable(&mp_consistency);
	Cvar_RegisterVariable(&mp_footsteps);
	
	Cvar_RegisterVariable(&sv_skyname);
	
	Cvar_RegisterVariable(&sv_skycolor_r);
	Cvar_RegisterVariable(&sv_skycolor_g);
	Cvar_RegisterVariable(&sv_skycolor_b);
	
	Cvar_RegisterVariable(&sv_skyvec_x);
	Cvar_RegisterVariable(&sv_skyvec_y);
	Cvar_RegisterVariable(&sv_skyvec_z);
	
	// Allow cheats if dev mode is enabled
	if(COM_CheckParm("-dev"))
		Cvar_SetValue("sv_cheats", 1);
	
	for(i = 0; i < MAX_MODELS; i++)
		sprintf(localmodels[i], "*%i", i);
};

/*
================
SV_Shutdown

Quake calls this before calling Sys_Quit or Sys_Error
================
*/
void SV_Shutdown ()
{
	// TODO
/*
	Master_Shutdown ();
	
	if (sv_logfile)
	{
		fclose (sv_logfile);
		sv_logfile = NULL;
	};
	
	if (sv_fraglogfile)
	{
		fclose (sv_fraglogfile);
		sv_logfile = NULL;
	};
*/
};

/*
=================
SV_SendBan
=================
*/
void SV_SendBan()
{
	char data[128];

	data[0] = data[1] = data[2] = data[3] = 0xff;
	data[4] = A2C_PRINT;
	data[5] = 0;
	
	Q_strcat(data, "\nbanned.\n");

	NET_SendPacket(NS_SERVER, Q_strlen(data), data, net_from);
}

/*
==============================================================================

PACKET FILTERING
 

You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

writeip
Dumps "addip <ip>" commands to listip.cfg so it can be execed at a later date.  The filter lists are not saved and restored by default, because I beleive it would cause too much confusion.

sv_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket()
{
	int i;
	unsigned in;

	in = *(unsigned *)net_from.ip;

	for(i = 0; i < numipfilters; i++)
		if((in & ipfilters[i].mask) == ipfilters[i].compare)
			return sv_filterban.value;

	return !sv_filterban.value;
}

/*
=================
SV_ReadPackets
=================
*/
void SV_ReadPackets()
{
	int i;
	client_t *cl;
	qboolean good; // TODO: not present in q2
	int qport;

	good = false;
	while(NET_GetPacket(NS_SERVER, &net_from, &net_message))
	{
		if(SV_FilterPacket()) // TODO: not present in q2
		{
			SV_SendBan(); // tell them we aren't listening...
			continue;
		}

		// check for connectionless packet (0xffffffff) first
		if(*(int *)net_message.data == -1)
		{
			SV_ConnectionlessPacket();
			continue;
		}

		// read the qport out of the message so we can fix up
		// stupid address translating routers
		MSG_BeginReading();
		MSG_ReadLong(); // sequence number
		MSG_ReadLong(); // sequence number
		qport = MSG_ReadShort() & 0xffff;

		// check for packets from connected clients
		for(i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
		{
			if(!cl->connected) // TODO: cl->state == cs_free in qw/q2
				continue;
			if(!NET_CompareBaseAdr(net_from, cl->netchan.remote_address))
				continue;
			if(cl->netchan.qport != qport)
				continue;
			if(cl->netchan.remote_address.port != net_from.port)
			{
				Con_DPrintf("SV_ReadPackets: fixing up a translated port\n");
				cl->netchan.remote_address.port = net_from.port;
			}
			if(Netchan_Process(&cl->netchan))
			{ // this is a valid, sequenced packet, so process it
				svs.stats.packets++;
				good = true;
				cl->send_message = true; // reply at end of frame
				SV_ExecuteClientMessage(cl);
			}
			break;
		}

		if(i != svs.maxclients)
			continue;

		// packet is not from a known client
		//	Con_Printf ("%s:sequenced packet without connection\n", NET_AdrToString(net_from));
	}
}

/*
==================
SV_CheckTimeouts

If a packet has not been received from a client in sv_timeout.value
seconds, drop the conneciton.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
void SV_CheckTimeouts()
{
	int i;
	client_t *cl;
	float droptime;
	int nclients;

	droptime = realtime - sv_timeout.value;
	nclients = 0;

	for(i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if(cl->connected || cl->spawned)
		{
			//if(!cl->spectator) // TODO
				//nclients++;
			if(cl->netchan.last_received < droptime)
			{
				SV_BroadcastPrintf(/*PRINT_HIGH,*/ "%s timed out\n", cl->name); // TODO
				SV_DropClient(cl, false, "Timed out");
				cl->active = false; // don't bother with zombie state
			}
		};

		//if (cl->active == true && realtime - cl->connection_started > zombietime.value)
			//cl->active = false;	// can now be reused
	};

	// TODO: non-GS behavior
	//if(sv.paused && !nclients)
	{
		// nobody left, unpause the server
		//SV_TogglePause("Pause released since no players are left.\n");
	}
}

/*
==================
SV_Frame

==================
*/
#ifdef FPS_20

void _SV_Frame()
{
	// run the world state
	gGlobalVariables.frametime = host_frametime;

	// read client messages
	SV_RunClients();

	// move things around and think
	// always pause in single player if in console or menus
	if(!sv.paused && (svs.maxclients > 1 || key_dest == key_game))
		SV_Physics();
}

void SV_Frame()
{
	float save_host_frametime;
	float temp_host_frametime;

	// run the world state
	gGlobalVariables.frametime = host_frametime;

	// set the time and clear the general datagram
	SV_ClearDatagram();

	// check for new clients
	//SV_CheckForNewClients ();

	temp_host_frametime = save_host_frametime = host_frametime;
	while(temp_host_frametime > (1.0 / 72.0))
	{
		if(temp_host_frametime > 0.05)
			host_frametime = 0.05;
		else
			host_frametime = temp_host_frametime;
		temp_host_frametime -= host_frametime;
		_SV_Frame();
	}
	host_frametime = save_host_frametime;

	// send all messages to the clients
	SV_SendClientMessages();
}

#else

void SV_Frame()
{
	// run the world state
	gGlobalVariables.frametime = host_frametime;

	// set the time and clear the general datagram
	//SV_ClearDatagram (); // TODO: remove?

	// check timeouts
	SV_CheckTimeouts();

	// toggle the log buffer if full
	//SV_CheckLog (); // TODO: qw

	// move things around and think
	// always pause in single player if in console or menus
	if(!sv.paused && (svs.maxclients > 1 || key_dest == key_game))
		SV_Physics();

	// get packets
	SV_ReadPackets();

	// process console commands
	//Cbuf_Execute (); // TODO: qw

	//SV_CheckVars (); // TODO: qw

	// send messages back to the clients that had packets read this frame
	SV_SendClientMessages();

	// send a heartbeat to the master if needed
	//Master_Heartbeat (); // TODO: qw

	// TODO: qw

	// collect timing statistics
	/*
	end = Sys_FloatTime ();
	svs.stats.active += end-start;
	if (++svs.stats.count == STATFRAMES)
	{
		svs.stats.latched_active = svs.stats.active;
		svs.stats.latched_idle = svs.stats.idle;
		svs.stats.latched_packets = svs.stats.packets;
		svs.stats.active = 0;
		svs.stats.idle = 0;
		svs.stats.packets = 0;
		svs.stats.count = 0;
	}
*/
}

#endif

/*
==============================================================================

CONNECTIONLESS COMMANDS

==============================================================================
*/

/*
================
SVC_Status

Responds with all the info that qplug or qspy can see
This message can be up to around 5k with worst case string lengths.
================
*/
void SVC_Status()
{
	int i;
	client_t *cl;
	int ping;
	int top, bottom;

	Cmd_TokenizeString("status");
	SV_BeginRedirect(RD_PACKET);
	Con_Printf("%s\n", svs.info);
	for(i = 0; i < svs.maxclients; i++)
	{
		cl = &svs.clients[i];
		if((cl->connected || cl->spawned) /*&& !cl->spectator*/) // TODO
		{
			top = atoi(Info_ValueForKey(cl->userinfo, "topcolor"));
			bottom = atoi(Info_ValueForKey(cl->userinfo, "bottomcolor"));
			top = (top < 0) ? 0 : ((top > 13) ? 13 : top);
			bottom = (bottom < 0) ? 0 : ((bottom > 13) ? 13 : bottom);
			//ping = SV_CalcPing(cl); // TODO
			Con_Printf("%i %i %i %i \"%s\" \"%s\" %i %i\n", cl->userid,
			           cl->old_frags, (int)(realtime - cl->connection_started) / 60,
			           ping, cl->name, Info_ValueForKey(cl->userinfo, "skin"), top, bottom);
		}
	}
	SV_EndRedirect();
}

/*
===================
SV_CheckLog

===================
*/
#define LOG_HIGHWATER 4096
#define LOG_FLUSH 10 * 60
void SV_CheckLog()
{
	// TODO
/*
	sizebuf_t *sz;

	sz = &svs.log[svs.logsequence & 1];

	// bump sequence if allmost full, or ten minutes have passed and
	// there is something still sitting there
	if(sz->cursize > LOG_HIGHWATER || (realtime - svs.logtime > LOG_FLUSH && sz->cursize))
	{
		// swap buffers and bump sequence
		svs.logtime = realtime;
		svs.logsequence++;
		sz = &svs.log[svs.logsequence & 1];
		sz->cursize = 0;
		Con_Printf("beginning fraglog sequence %i\n", svs.logsequence);
	}
*/
}

/*
================
SVC_Log

Responds with all the logged frags for ranking programs.
If a sequence number is passed as a parameter and it is
the same as the current sequence, an A2A_NACK will be returned
instead of the data.
================
*/
void SVC_Log()
{
	// TODO
/*
	int seq;
	char data[MAX_DATAGRAM + 64];

	if(Cmd_Argc() == 2)
		seq = atoi(Cmd_Argv(1));
	else
		seq = -1;

	if(seq == svs.logsequence - 1 || !sv_fraglogfile)
	{ // they allready have this data, or we aren't logging frags
		data[0] = A2A_NACK;
		NET_SendPacket(NS_SERVER, 1, data, net_from);
		return;
	}

	Con_DPrintf("sending log %i to %s\n", svs.logsequence - 1, NET_AdrToString(net_from));

	sprintf(data, "stdlog %i\n", svs.logsequence - 1);
	strcat(data, (char *)svs.log_buf[((svs.logsequence - 1) & 1)]);

	NET_SendPacket(NS_SERVER, strlen(data) + 1, data, net_from);
*/
}

/*
================
SVC_Ping

Just responds with an acknowledgement
================
*/
void SVC_Ping()
{
	char data;

	data = A2A_ACK;

	NET_SendPacket(NS_SERVER, 1, &data, net_from);
}

/*
=================
SVC_GetChallenge

Returns a challenge number that can be used
in a subsequent client_connect command.
We do this to prevent denial of service attacks that
flood the server with invalid connection IPs.  With a
challenge, they must give a valid IP address.
=================
*/
void SVC_GetChallenge()
{
	int i;
	int oldest;
	int oldestTime;

	oldest = 0;
	oldestTime = 0x7fffffff;

	// see if we already have a challenge for this ip
	for(i = 0; i < MAX_CHALLENGES; i++)
	{
		if(NET_CompareBaseAdr(net_from, svs.challenges[i].adr))
			break;
		if(svs.challenges[i].time < oldestTime)
		{
			oldestTime = svs.challenges[i].time;
			oldest = i;
		}
	}

	if(i == MAX_CHALLENGES)
	{
		// overwrite the oldest
		svs.challenges[oldest].challenge = (rand() << 16) ^ rand();
		svs.challenges[oldest].adr = net_from;
		svs.challenges[oldest].time = realtime;
		i = oldest;
	}

	// send it back
	Netchan_OutOfBandPrint(NS_SERVER, net_from, "%c%i", S2C_CHALLENGE, svs.challenges[i].challenge);
}

// TODO
void SV_ExtractFromUserinfo (client_t *cl);

int Rcon_Validate()
{
	// TODO
/*
	if(!strlen(rcon_password.string))
		return 0;

	if(strcmp(Cmd_Argv(1), rcon_password.string))
		return 0;
*/

	return 1;
}

/*
===============
SVC_RemoteCommand

A client issued an rcon command.
Shift down the remaining args
Redirect all printfs
===============
*/
void SVC_RemoteCommand()
{
	int i;
	char remaining[1024];

	if(!Rcon_Validate())
	{
		Con_Printf("Bad rcon from %s:\n%s\n", NET_AdrToString(net_from), net_message.data + 4);

		SV_BeginRedirect(RD_PACKET);

		Con_Printf("Bad rcon_password.\n");
	}
	else
	{
		Con_Printf("Rcon from %s:\n%s\n", NET_AdrToString(net_from), net_message.data + 4);

		SV_BeginRedirect(RD_PACKET);

		remaining[0] = 0;

		for(i = 2; i < Cmd_Argc(); i++)
		{
			strcat(remaining, Cmd_Argv(i));
			strcat(remaining, " ");
		};

		Cmd_ExecuteString(remaining, src_command); // TODO: src_client?
	}

	SV_EndRedirect();
}

/*
=================
SV_ConnectionlessPacket

A connectionless packet has four leading 0xff
characters to distinguish it from a game channel.
Clients that are in the game can still send
connectionless packets.
=================
*/
void SV_ConnectionlessPacket()
{
	char *s;
	char *c;

	MSG_BeginReading();
	MSG_ReadLong(); // skip the -1 marker

	s = MSG_ReadStringLine();

	Cmd_TokenizeString(s);

	c = Cmd_Argv(0);

	if(!strcmp(c, "ping") || (c[0] == A2A_PING && (c[1] == 0 || c[1] == '\n')))
	{
		SVC_Ping();
		return;
	}
	if(c[0] == A2A_ACK && (c[1] == 0 || c[1] == '\n'))
	{
		Con_Printf("A2A_ACK from %s\n", NET_AdrToString(net_from));
		return;
	}
	else if(!strcmp(c, "status"))
	{
		SVC_Status();
		return;
	}
	else if(!strcmp(c, "log"))
	{
		SVC_Log();
		return;
	}
	else if(!strcmp(c, "connect"))
	{
		SV_ConnectClient();
		return;
	}
	else if(!strcmp(c, "getchallenge"))
	{
		SVC_GetChallenge();
		return;
	}
	else if(!strcmp(c, "rcon"))
		SVC_RemoteCommand();
	else
		if(!gEntityInterface.pfnConnectionlessPacket(&net_from, c, NULL, NULL)) // TODO
			Con_Printf("bad connectionless packet from %s:\n%s\n", NET_AdrToString(net_from), s);
}

/*
=============================================================================

Con_Printf redirection

=============================================================================
*/

char outputbuf[8000];

redirect_t sv_redirected;

extern cvar_t sv_phs;

/*
==================
SV_FlushRedirect
==================
*/
void SV_FlushRedirect ()
{
	char	send[8000+6];

	if (sv_redirected == RD_PACKET)
	{
		send[0] = 0xff;
		send[1] = 0xff;
		send[2] = 0xff;
		send[3] = 0xff;
		send[4] = A2C_PRINT;
		
		Q_memcpy (send+5, outputbuf, Q_strlen(outputbuf)+1);

		NET_SendPacket (NS_SERVER, Q_strlen(send)+1, send, net_from);
	}
	else if (sv_redirected == RD_CLIENT)
	{
		MSG_WriteByte (&host_client->netchan.message, svc_print);
		MSG_WriteByte (&host_client->netchan.message, PRINT_HIGH); // TODO
		MSG_WriteString (&host_client->netchan.message, outputbuf);
	}

	// clear it
	outputbuf[0] = 0;
}

/*
==================
SV_BeginRedirect

  Send Con_Printf data to the remote client
  instead of the console
==================
*/
void SV_BeginRedirect (redirect_t rd)
{
	sv_redirected = rd;
	outputbuf[0] = 0;
}

void SV_EndRedirect ()
{
	SV_FlushRedirect ();
	sv_redirected = RD_NONE;
}

/*
=============================================================================

EVENT MESSAGES

=============================================================================
*/

/*  
==================
SV_StartParticle

Make sure the event gets sent to all clients
==================
*/
void SV_StartParticle(vec3_t org, vec3_t dir, int color, int count)
{
	int i, v;

	if(sv.datagram.cursize > MAX_DATAGRAM - 16)
		return;
	MSG_WriteByte(&sv.datagram, svc_particle);
	MSG_WriteCoord(&sv.datagram, org[0]);
	MSG_WriteCoord(&sv.datagram, org[1]);
	MSG_WriteCoord(&sv.datagram, org[2]);
	for(i = 0; i < 3; i++)
	{
		v = dir[i] * 16;
		if(v > 127)
			v = 127;
		else if(v < -128)
			v = -128;
		MSG_WriteChar(&sv.datagram, v);
	}
	MSG_WriteByte(&sv.datagram, count);
	MSG_WriteByte(&sv.datagram, color);
}

/*  
==================
SV_StartSound

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

Channel 0 is an auto-allocate channel, the others override anything
allready running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the level.
Larger attenuations will drop off.  (max 4 attenuation)

==================
*/
void SV_StartSound(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int pitch)
{
	int sound_num;
	int field_mask;
	int i;
	int ent;

	if(volume < 0 || volume > 255)
		Sys_Error("SV_StartSound: volume = %i", volume);

	if(attenuation < 0 || attenuation > 4)
		Sys_Error("SV_StartSound: attenuation = %f", attenuation);

	if(channel < 0 || channel > 7)
		Sys_Error("SV_StartSound: channel = %i", channel);
	
	if(pitch < 0 || pitch > 255) // TODO
		Sys_Error("SV_StartSound: pitch = %i", pitch);

	// TODO: invalid sentence number: %s
	
	if(sv.datagram.cursize > MAX_DATAGRAM - 16)
		return;

	// find precache number for sound
	for(sound_num = 1; sound_num < MAX_SOUNDS && sv.sound_precache[sound_num]; sound_num++)
		if(!strcmp(sample, sv.sound_precache[sound_num]))
			break;

	if(sound_num == MAX_SOUNDS || !sv.sound_precache[sound_num])
	{
		Con_Printf("SV_StartSound: %s not precached (%d)\n", sample, sound_num);
		return;
	}

	ent = NUM_FOR_EDICT(entity);

	channel = (ent << 3) | channel;

	field_mask = 0;
	if(volume != DEFAULT_SOUND_PACKET_VOLUME)
		field_mask |= SND_VOLUME;
	if(attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
		field_mask |= SND_ATTENUATION;

	// directed messages go only to the entity the are targeted on
	MSG_WriteByte(&sv.datagram, svc_sound);
	MSG_WriteByte(&sv.datagram, field_mask);
	if(field_mask & SND_VOLUME)
		MSG_WriteByte(&sv.datagram, volume);
	if(field_mask & SND_ATTENUATION)
		MSG_WriteByte(&sv.datagram, attenuation * 64);
	MSG_WriteShort(&sv.datagram, channel);
	MSG_WriteByte(&sv.datagram, sound_num);
	for(i = 0; i < 3; i++)
		MSG_WriteCoord(&sv.datagram, entity->v.origin[i] + 0.5 * (entity->v.mins[i] + entity->v.maxs[i]));
}

/*
==============================================================================

CLIENT SPAWNING

==============================================================================
*/

/*
================
SV_SendServerinfo

Sends the first message from the server to a connected client.
This will be sent on the initial connection and upon each server load.
================
*/
void SV_SendServerinfo(client_t *client)
{
	char		*gamedir;
	int			playernum;
	
	char **s;
	char message[2048];

	gamedir = Info_ValueForKey (svs.info, "*gamedir");
	if (!gamedir[0])
		gamedir = com_gamedir;
	
	MSG_WriteByte(&client->netchan.message, svc_print);
	sprintf(message, "BUILD %d SERVER (%i CRC)\nServer # %d\n", build_number(), 0 /*pr_crc*/, svs.spawncount);
	MSG_WriteString(&client->netchan.message, message);
	
	MSG_WriteByte(&client->netchan.message, svc_serverinfo);
	MSG_WriteLong(&client->netchan.message, PROTOCOL_VERSION);
	MSG_WriteLong (&client->netchan.message, svs.spawncount);
	MSG_WriteLong (&client->netchan.message, 0); // TODO: map crc
	
	for(int i = 0; i < 16; i++)
		MSG_WriteByte (&client->netchan.message, 0); // TODO: client dll hash
	
	MSG_WriteByte(&client->netchan.message, svs.maxclients);

	playernum = NUM_FOR_EDICT(client->edict)-1;
	//if (client->spectator) // TODO
		//playernum |= 128;
	MSG_WriteByte (&client->netchan.message, playernum);
	
	// TODO
	//MSG_WriteByte (&client->netchan.message, deathmatch.value);
	if(!coop.value && deathmatch.value)
		MSG_WriteByte(&client->netchan.message, GAME_DEATHMATCH);
	else
		MSG_WriteByte(&client->netchan.message, GAME_COOP);

	MSG_WriteString (&client->netchan.message, gamedir);
	
	MSG_WriteString (&client->netchan.message, hostname.string);
	
	// send full levelname
	MSG_WriteString (&client->netchan.message, sv.name);
	//MSG_WriteString (&client->netchan.message, PR_GetString(sv.edicts->v.message));
	
	//sprintf(message, pr_strings + sv.edicts->v.message);
	//MSG_WriteString(&client->netchan.message, message);

	MSG_WriteString (&client->netchan.message, " "); // TODO: mapcycle
	
	MSG_WriteByte(&client->netchan.message, 0);

/*
================
SV_SendExtraServerinfo

Sends some extra information regarding the server.

Note: This message is sent at player's connection right after SVC_SERVERINFO.
Note: The sv_cheats cvar will be set on the client with the value provided.
Note: It appears FallbackDir is always null.
================
*/
void SV_SendExtraServerinfo(client_t *client)
{
	MSG_WriteByte(&client->netchan.message, svc_sendextrainfo);
	
	MSG_WriteString(&client->netchan.message, " "); // fallback dir
	MSG_WriteByte(&client->netchan.message, sv_cheats.value); // cheats state
};

void SV_SendDeltaDescription(client_t *client)
{
	// TODO
};

void SV_SendMoveVars(client_t *client)
{
	MSG_WriteByte(&client->netchan.message, svc_newmovevars);
	MSG_WriteFloat(&client->netchan.message, movevars.gravity);
	MSG_WriteFloat(&client->netchan.message, movevars.stopspeed);
	MSG_WriteFloat(&client->netchan.message, movevars.maxspeed);
	MSG_WriteFloat(&client->netchan.message, movevars.spectatormaxspeed);
	MSG_WriteFloat(&client->netchan.message, movevars.accelerate);
	MSG_WriteFloat(&client->netchan.message, movevars.airaccelerate);
	MSG_WriteFloat(&client->netchan.message, movevars.wateraccelerate);
	MSG_WriteFloat(&client->netchan.message, movevars.friction);
	MSG_WriteFloat(&client->netchan.message, movevars.edgefriction);
	MSG_WriteFloat(&client->netchan.message, movevars.waterfriction);
	MSG_WriteFloat(&client->netchan.message, movevars.entgravity);
	MSG_WriteFloat(&client->netchan.message, movevars.bounce);
	MSG_WriteFloat(&client->netchan.message, movevars.stepsize);
	MSG_WriteFloat(&client->netchan.message, movevars.maxvelocity);
	MSG_WriteFloat(&client->netchan.message, movevars.zmax);
	MSG_WriteFloat(&client->netchan.message, movevars.waveheight);
	MSG_WriteByte(&client->netchan.message, movevars.footsteps);
	MSG_WriteFloat(&client->netchan.message, movevars.rollangle);
	MSG_WriteFloat(&client->netchan.message, movevars.rollspeed);
	MSG_WriteFloat(&client->netchan.message, movevars.skycolorred);
	MSG_WriteFloat(&client->netchan.message, movevars.skycolorgreen);
	MSG_WriteFloat(&client->netchan.message, movevars.skycolorblue);
	MSG_WriteFloat(&client->netchan.message, movevars.skyvecx);
	MSG_WriteFloat(&client->netchan.message, movevars.skyvecy);
	MSG_WriteFloat(&client->netchan.message, movevars.skyvecz);
	MSG_WriteString(&client->netchan.message, movevars.skyname);
};

void SV_SendUserMessages(client_t *client)
{
	// TODO
};

/*
================
SV_SendResourceList
================
*/
void SV_RequestResourceList(client_t *client)
{
	MSG_WriteByte(&client->netchan.message, svc_resourcerequest);
	MSG_WriteByte(&client->netchan.message, svs.spawncount);
	MSG_WriteByte(&client->netchan.message, 0);
};

/*
================
SV_SendResourceLocation
================
*/
void SV_SendResourceLocation(client_t *client)
{
	//MSG_WriteByte(&client->netchan.message, svc_resourcelocation);
	//MSG_WriteString(&client->netchan.message, sv_downloadurl.string); // TODO
};
void SV_SendResourceList(client_t *client)
{
	// TODO: svc_resourcelist
	
	//for(s = sv.model_precache + 1; *s; s++)
		//MSG_WriteString(&client->netchan.message, *s);
	//MSG_WriteByte(&client->netchan.message, 0);

	//for(s = sv.sound_precache + 1; *s; s++)
		//MSG_WriteString(&client->netchan.message, *s);
	//MSG_WriteByte(&client->netchan.message, 0);
};

void SV_SendTime(client_t *client)
{
	MSG_WriteByte(&client->netchan.message, svc_time);
	MSG_WriteFloat(&client->netchan.message, sv.time);
};

void SV_SendLightStyles(client_t *client)
{
	for(int i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		MSG_WriteByte(&client->netchan.message, svc_lightstyle);
		MSG_WriteByte(&client->netchan.message, (char)i);
		MSG_WriteString(&client->netchan.message, sv.lightstyles[i]);
	};
};

void SV_SendSoundFade(client_t *client, int initialpercent, int holdtime, int fadeouttime, int fadeintime)
{
	MSG_WriteByte(&client->netchan.message, svc_soundfade);
	MSG_WriteByte(&client->netchan.message, initialpercent);
	MSG_WriteByte(&client->netchan.message, holdtime);
	MSG_WriteByte(&client->netchan.message, fadeouttime);
	MSG_WriteByte(&client->netchan.message, fadeintime);
};

// TODO: SV_InitVoiceForClient?
void SV_SendVoiceInit(client_t *client)
{
	MSG_WriteByte(&client->netchan.message, svc_voiceinit);
	MSG_WriteString(&client->netchan.message, sv_voicecodec.string);
	MSG_WriteByte(&client->netchan.message, sv_voicequality.value);
};

void SV_SendVoiceData(client_t *client, int playerindex, const void *data, int bufsize)
{
	MSG_WriteByte(&client->netchan.message, svc_voicedata);
	MSG_WriteByte(&client->netchan.message, playerindex);
	MSG_WriteShort(&client->netchan.message, bufsize);
	MSG_WriteBuf(&client->netchan.message, data);
};

void SV_SendCrosshairAngle(client_t *client, float pitch, float yaw)
{
	MSG_WriteByte(&client->netchan.message, svc_crosshairangle);
	//MSG_WriteFloat(&client->netchan.message, pitch);
	//MSG_WriteFloat(&client->netchan.message, yaw);
	// TODO
	MSG_WriteChar(&client->netchan.message, pitch * 5);
	MSG_WriteChar(&client->netchan.message, yaw * 5);
};

// TODO: unused
void SV_SendWeaponAnim(client_t *client, int sequence, int bodygroup)
{
	MSG_WriteByte(&client->netchan.message, svc_weaponanim);	
	MSG_WriteByte(&client->netchan.message, sequence);
	MSG_WriteByte(&client->netchan.message, bodygroup);
};

// TODO: SV_RequestCvarValue?
void SV_QueryCvarValue(client_t *client, const char *cvarname)
{
	MSG_WriteByte(&client->netchan.message, svc_sendcvarvalue);	
	MSG_WriteString(&client->netchan.message, cvarname);
};

void SV_QueryCvarValueEx(client_t *client, const char *cvarname, int requestID)
{
	MSG_WriteByte(&client->netchan.message, svc_sendcvarvalue2);
	MSG_WriteLong(&client->netchan.message, requestID);
	MSG_WriteString(&client->netchan.message, cvarname);
};

/*
================
SV_SendCustomization
================
*/
// TODO: unused
// TODO: or pass a pointer to resource_t?
void SV_SendCustomization(client_t *client, customization_t *custom)
{
	MSG_WriteByte(&client->netchan.message, svc_customization);
	MSG_WriteByte(&client->netchan.message, custom->resource.playernum);
	MSG_WriteByte(&client->netchan.message, custom->resource.type);
	MSG_WriteString(&client->netchan.message, custom->resource.szFileName);
	MSG_WriteShort(&client->netchan.message, custom->resource.nIndex);
	MSG_WriteLong(&client->netchan.message, custom->resource.nDownloadSize);
	MSG_WriteByte(&client->netchan.message, custom->resource.ucFlags);
	for(int i = 0; i <= 15; ++i) // TODO: MSG_WriteBuf?
		MSG_WriteByte(&client->netchan.message, custom->resource.rgucMD5_hash[i]);
};

/*
================
SV_ConnectClient

Initializes a client_t for a new net connection (A connection request that did not come from the master).
This will only be called once for a player each game, not once for each level change.
================
*/
void SV_ConnectClient()
{
	static int userid;
	char userinfo[1024];
	netadr_t adr;
	char *s;
	edict_t			*ent;
	client_t		*client, *cl, *newcl;
	client_t temp;
	int				edictnum;
	int				i;
	float			spawn_parms[NUM_SPAWN_PARMS];
	int clients, spectators;
	qboolean spectator;
	int qport;
	int version;
	int challenge;
	
	version = atoi(Cmd_Argv(1));
	if(version != PROTOCOL_VERSION)
	{
		Netchan_OutOfBandPrint(NS_SERVER, net_from, "%c\nServer is version %4.2f.\n", A2C_PRINT, VERSION);
		Con_Printf("* rejected connect from version %i\n", version);
		return;
	};

	qport = atoi(Cmd_Argv(2));

	challenge = atoi(Cmd_Argv(3));
	
	// note an extra byte is needed to replace spectator key
	strncpy(userinfo, Cmd_Argv(4), sizeof(userinfo) - 2);
	userinfo[sizeof(userinfo) - 2] = 0;

	// see if the challenge is valid
	for(i = 0; i < MAX_CHALLENGES; i++)
	{
		if(NET_CompareBaseAdr(net_from, svs.challenges[i].adr))
		{
			if(challenge == svs.challenges[i].challenge)
				break; // good
			Netchan_OutOfBandPrint(NS_SERVER, net_from, "%c\nBad challenge.\n", A2C_PRINT);
			return;
		};
	};
	
	if(i == MAX_CHALLENGES)
	{
		Netchan_OutOfBandPrint(NS_SERVER, net_from, "%c\nNo challenge for address.\n", A2C_PRINT);
		return;
	};
	
	// check for password
	s = Info_ValueForKey(userinfo, "spectator");
	if(s[0] && strcmp(s, "0"))
	{
		// TODO
		/*
		if (spectator_password.string[0] && 
			stricmp(spectator_password.string, "none") &&
			strcmp(spectator_password.string, s) )
		{	// failed
			Con_Printf ("%s:spectator password failed\n", NET_AdrToString (net_from));
			Netchan_OutOfBandPrint (NS_SERVER, net_from, "%c\nrequires a spectator password\n\n", A2C_PRINT);
			return;
		}
		*/
		Info_RemoveKey(userinfo, "spectator"); // remove passwd
		Info_SetValueForStarKey(userinfo, "*spectator", "1", MAX_INFO_STRING);
		spectator = true;
	}
	else
	{
		s = Info_ValueForKey(userinfo, "password");
		// TODO
		/*
		if(password.string[0] &&
		   stricmp(password.string, "none") &&
		   strcmp(password.string, s))
		{
			Con_Printf("%s:password failed\n", NET_AdrToString(net_from));
			Netchan_OutOfBandPrint(NS_SERVER, net_from, "%c\nserver requires a password\n\n", A2C_PRINT);
			return;
		}
		*/
		spectator = false;
		Info_RemoveKey(userinfo, "password"); // remove passwd
	};
	
	adr = net_from;
	userid++; // so every client gets a unique id

	if (sv.loadgame)
		Q_memcpy (spawn_parms, newcl->spawn_parms, sizeof(spawn_parms));
	
	newcl = &temp;
	memset(newcl, 0, sizeof(client_t));

	newcl->userid = userid;

	// works properly
	if(false) //if(!sv_highchars.value) // TODO
	{
		byte *p, *q;

		for(p = (byte *)newcl->userinfo, q = (byte *)userinfo;
		    *q && p < (byte *)newcl->userinfo + sizeof(newcl->userinfo) - 1; q++)
			if(*q > 31 && *q <= 127)
				*p++ = *q;
	}
	else
		strncpy(newcl->userinfo, userinfo, sizeof(newcl->userinfo) - 1);

	SV_UserinfoChanged(newcl);
	
	// if there is already a slot for this ip, drop it
	for(i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if(!cl->active)
			continue;
		if(NET_CompareBaseAdr(adr, cl->netchan.remote_address) && (cl->netchan.qport == qport || adr.port == cl->netchan.remote_address.port))
		{
			if(cl->connected)
			{
				Con_Printf("%s:dup connect\n", NET_AdrToString(adr));
				userid--;
				return;
			}

			Con_Printf("%s:reconnect\n", NET_AdrToString(adr));
			SV_DropClient(cl, false, "reconnect");
			break;
		}
	}

	// count up the clients and spectators
	clients = 0;
	spectators = 0;
	for(i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if(!cl->active)
			continue;
		//if(cl->spectator) // TODO
			//spectators++;
		//else
			clients++;
	}

	// if at server limits, refuse connection
	//if(maxplayers.value > MAX_CLIENTS) // TODO
		//Cvar_SetValue("maxplayers", MAX_CLIENTS);

	if(/*(spectator && spectators >= (int)maxspectators.value) ||*/ (!spectator && clients >= 4/*(int)maxplayers.value*/)) // TODO
	{
		Con_Printf("%s:full connect\n", NET_AdrToString(adr));
		Netchan_OutOfBandPrint(NS_SERVER, adr, "%c\nserver is full\n\n", A2C_PRINT);
		return;
	}

	// find a client slot
	newcl = NULL;
	for(i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		if(!cl->active)
		{
			newcl = cl;
			break;
		};
	};

	if(!newcl)
	{
		Con_Printf("WARNING: miscounted available clients\n");
		return;
	};
	
	// build a new connection
	// accept the new client
	// this is the only place a client_t is ever initialized
	*newcl = temp;

	client = newcl;

	//Con_DPrintf ("Client %s connected\n", client->netchan.remote_address);

	Netchan_OutOfBandPrint(NS_SERVER, adr, "%c", S2C_CONNECTION);

	edictnum = (newcl - svs.clients) + 1;
	ent = EDICT_NUM(edictnum);
	
	// set up the client_t
	
	//memset (client, 0, sizeof(*client));
	
	Netchan_Setup(NS_SERVER, &client->netchan, adr, qport);

	Q_strcpy (client->name, "unconnected");
	
	client->connected = true;
	client->active = false; // TODO: should be marked as true during the spawn
	client->spawned = false;
	client->edict = ent;
	
	// spectator mode can ONLY be set at join time
	//client->spectator = spectator; // TODO
	
	client->datagram.allowoverflow = true;
	client->datagram.data = client->datagram_buf;
	client->datagram.maxsize = sizeof(client->datagram_buf);
	
	//client->netchan.message.data = client->msgbuf;
	//client->netchan.message.maxsize = sizeof(client->msgbuf);
	client->netchan.message.allowoverflow = true;		// we can catch it
	
	// parse some info from the info strings
	SV_ExtractFromUserinfo(client);

	// TODO
	// JACK: Init the floodprot stuff.
	//for (i=0; i<10; i++)
	//	client->whensaid[i] = 0.0;
	//client->whensaidhead = 0;
	//client->lockedtill = 0;
	
	if (sv.loadgame)
		Q_memcpy (client->spawn_parms, spawn_parms, sizeof(spawn_parms));
	else
	{
		// call the progs to get default spawn parms for the new client
		gEntityInterface.pfnParmsNewLevel();
		//for (i=0 ; i<NUM_SPAWN_PARMS ; i++)
			//client->spawn_parms[i] = (&gGlobalVariables.parm1)[i];
	};
	
	// TODO
	//if(client->spectator)
		//Con_Printf("Spectator %s connected\n", client->name);
	//else
		Con_DPrintf("Client %s connected\n", client->name);

	client->sendinfo = true; //SV_SendServerinfo (client);
}

/*
===============================================================================

FRAME UPDATES

===============================================================================
*/

/*
==================
SV_ClearDatagram

==================
*/
void SV_ClearDatagram()
{
	SZ_Clear(&sv.datagram);
}

/*
=============================================================================

The PVS must include a small area around the client to allow head bobbing
or other small motion on the client side.  Otherwise, a bob might cause an
entity that should be visible to not show up, especially when the bob
crosses a waterline.

=============================================================================
*/

int fatbytes;
byte fatpvs[MAX_MAP_LEAFS / 8];

void SV_AddToFatPVS(vec3_t org, mnode_t *node)
{
	int i;
	byte *pvs;
	mplane_t *plane;
	float d;

	while(1)
	{
		// if this is a leaf, accumulate the pvs bits
		if(node->contents < 0)
		{
			if(node->contents != CONTENTS_SOLID)
			{
				pvs = Mod_LeafPVS((mleaf_t *)node, sv.worldmodel);
				for(i = 0; i < fatbytes; i++)
					fatpvs[i] |= pvs[i];
			}
			return;
		}

		plane = node->plane;
		d = DotProduct(org, plane->normal) - plane->dist;
		if(d > 8)
			node = node->children[0];
		else if(d < -8)
			node = node->children[1];
		else
		{ // go down both
			SV_AddToFatPVS(org, node->children[0]);
			node = node->children[1];
		}
	}
}

/*
=============
SV_FatPVS

Calculates a PVS that is the inclusive or of all leafs within 8 pixels of the
given point.
=============
*/
byte *SV_FatPVS(vec3_t org)
{
	fatbytes = (sv.worldmodel->numleafs + 31) >> 3;
	Q_memset(fatpvs, 0, fatbytes);
	SV_AddToFatPVS(org, sv.worldmodel->nodes);
	return fatpvs;
}

//=============================================================================

qboolean SV_ShouldUpdatePing(client_t *cl)
{
	// TODO
	
	return true;
};

/*
===================
SV_CalcPing

===================
*/
int SV_CalcPing (client_t *cl)
{
	float ping = 0.0f;
	int i;
	int count = 0;
	register client_frame_t *frame;

	for (frame = cl->frames, i=0 ; i<UPDATE_BACKUP ; i++, frame++)
	{
		if (frame->ping_time > 0)
		{
			ping += frame->ping_time;
			count++;
		};
	};
	
	if (!count)
		return 9999;
	
	ping /= count;

	return ping * 1000;
}

/*
===================
SV_FullClientUpdate

Writes all update values to a sizebuf
===================
*/
void SV_FullClientUpdate(client_t *client, sizebuf_t *buf)
{
	//for(i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
	//{
		//if(!client->active)
			//continue;
	//}
	
	char info[MAX_INFO_STRING];

	int i = client - svs.clients;

	//Sys_Printf("SV_FullClientUpdate:  Updated frags for client %d\n", i);

/*
	MSG_WriteByte(buf, svc_updatefrags);
	MSG_WriteByte(buf, i);
	MSG_WriteShort(buf, client->old_frags);
*/

	MSG_WriteByte(buf, svc_pings);
	MSG_WriteByte(buf, i);
	MSG_WriteShort(buf, SV_CalcPing(client));
	MSG_WriteByte(buf, client->lossage);

/*	
	MSG_WriteByte(buf, svc_updateentertime);
	MSG_WriteByte(buf, i);
	MSG_WriteFloat(buf, realtime - client->connection_started);
*/

	Q_strcpy(info, client->userinfo);
	Info_RemovePrefixedKeys(info, '_');	// server passwords, etc

	MSG_WriteByte(buf, svc_updateuserinfo);
	MSG_WriteByte(buf, i);
	MSG_WriteLong(buf, client->userid);
	MSG_WriteString(buf, info);
	
	byte unusedcdkey[16];
	MSG_WriteBuf(buf, unusedcdkey, sizeof(unusedcdkey));
}

/*
===================
SV_FullClientUpdateToClient

Writes all update values to a client's reliable stream
===================
*/
void SV_FullClientUpdateToClient(client_t *client, client_t *cl)
{
	// TODO
	//ClientReliableCheckBlock(cl, 24 + Q_strlen(client->userinfo));
	//if(cl->num_backbuf)
	//{
		//SV_FullClientUpdate(client, &cl->backbuf);
		//ClientReliable_FinishWrite(cl);
	//}
	//else
		SV_FullClientUpdate(client, &cl->netchan.message);
}

/*
==================
SV_WriteDelta

Writes part of a packetentities message.
Can delta from either a baseline or a previous packet_entity
==================
*/
void SV_WriteDelta(entity_state_t *from, entity_state_t *to, sizebuf_t *msg, qboolean force)
{
	// TODO
/*
	int		bits;
	int		i;
	float	miss;

// send an update
	bits = 0;
	
	for (i=0 ; i<3 ; i++)
	{
		miss = to->origin[i] - from->origin[i];
		if ( miss < -0.1 || miss > 0.1 )
			bits |= U_ORIGIN1<<i;
	}

	if ( to->angles[0] != from->angles[0] )
		bits |= U_ANGLE1;
		
	if ( to->angles[1] != from->angles[1] )
		bits |= U_ANGLE2;
		
	if ( to->angles[2] != from->angles[2] )
		bits |= U_ANGLE3;
		
	if ( to->colormap != from->colormap )
		bits |= U_COLORMAP;
		
	if ( to->skin != from->skin )
		bits |= U_SKIN;
		
	if ( to->frame != from->frame )
		bits |= U_FRAME;
	
	if ( to->effects != from->effects )
		bits |= U_EFFECTS;
	
	if ( to->modelindex != from->modelindex )
		bits |= U_MODEL;

	if (bits & 511)
		bits |= U_MOREBITS;

	//if (to->flags & U_SOLID) // TODO
		//bits |= U_SOLID;

	//
	// write the message
	//
	if (!to->number)
		Host_Error ("Unset entity number");
	if (to->number >= 512)
		Host_Error ("Entity number >= 512");

	if (!bits && !force)
		return;		// nothing to send!
	i = to->number | (bits&~511);
	//if (i & U_REMOVE) // TODO
		//Sys_Error ("U_REMOVE");
	MSG_WriteShort (msg, i);
	
	if (bits & U_MOREBITS)
		MSG_WriteByte (msg, bits&255);
	if (bits & U_MODEL)
		MSG_WriteByte (msg,	to->modelindex);
	if (bits & U_FRAME)
		MSG_WriteByte (msg, to->frame);
	if (bits & U_COLORMAP)
		MSG_WriteByte (msg, to->colormap);
	if (bits & U_SKIN)
		MSG_WriteByte (msg, to->skin);
	if (bits & U_EFFECTS)
		MSG_WriteByte (msg, to->effects);
	if (bits & U_ORIGIN1)
		MSG_WriteCoord (msg, to->origin[0]);		
	if (bits & U_ANGLE1)
		MSG_WriteAngle(msg, to->angles[0]);
	if (bits & U_ORIGIN2)
		MSG_WriteCoord (msg, to->origin[1]);
	if (bits & U_ANGLE2)
		MSG_WriteAngle(msg, to->angles[1]);
	if (bits & U_ORIGIN3)
		MSG_WriteCoord (msg, to->origin[2]);
	if (bits & U_ANGLE3)
		MSG_WriteAngle(msg, to->angles[2]);
*/
}

/*
=============
SV_EmitPacketEntities

Writes a delta update of a packet_entities_t to the message.

=============
*/
void SV_EmitPacketEntities (client_t *client, packet_entities_t *to, sizebuf_t *msg)
{
	edict_t	*ent;
	client_frame_t	*fromframe;
	packet_entities_t *from;
	int		oldindex, newindex;
	int		oldnum, newnum;
	int		oldmax;

	// this is the frame that we are going to delta update from
	if (client->delta_sequence != -1)
	{
		fromframe = &client->frames[client->delta_sequence & UPDATE_MASK];
		from = &fromframe->entities;
		oldmax = from->num_entities;

		MSG_WriteByte (msg, svc_deltapacketentities);
		MSG_WriteByte (msg, client->delta_sequence);
	}
	else
	{
		oldmax = 0;	// no delta update
		from = NULL;

		MSG_WriteByte (msg, svc_packetentities);
	}

	newindex = 0;
	oldindex = 0;
//Con_Printf ("---%i to %i ----\n", client->delta_sequence & UPDATE_MASK
//			, client->netchan.outgoing_sequence & UPDATE_MASK);
	while (newindex < to->num_entities || oldindex < oldmax)
	{
		newnum = newindex >= to->num_entities ? 9999 : to->entities[newindex].number;
		oldnum = oldindex >= oldmax ? 9999 : from->entities[oldindex].number;

		if (newnum == oldnum)
		{	// delta update from old position
//Con_Printf ("delta %i\n", newnum);
			SV_WriteDelta (&from->entities[oldindex], &to->entities[newindex], msg, false);
			oldindex++;
			newindex++;
			continue;
		}

		if (newnum < oldnum)
		{	// this is a new entity, send it from the baseline
			ent = EDICT_NUM(newnum);
//Con_Printf ("baseline %i\n", newnum);
			SV_WriteDelta (&ent->baseline, &to->entities[newindex], msg, true);
			newindex++;
			continue;
		}

		if (newnum > oldnum)
		{	// the old entity isn't present in the new message
//Con_Printf ("remove %i\n", oldnum);
			MSG_WriteShort (msg, oldnum /*| U_REMOVE*/); // TODO
			oldindex++;
			continue;
		}
	}

	MSG_WriteShort (msg, 0);	// end of packetentities
}

/*
=============
SV_WriteEntitiesToClient

=============
*/
void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg)
{
	int e, i;
	byte *pvs;
	vec3_t org;
	edict_t *ent;
	packet_entities_t *pack;
	entity_state_t *state;
	
	// this is the frame we are creating
	client_frame_t *frame = &client->frames[client->netchan.incoming_sequence & UPDATE_MASK];
	
	// find the client's PVS
	edict_t *clent = client->edict;
	VectorAdd(clent->v.origin, clent->v.view_ofs, org);
	pvs = SV_FatPVS(org);

	// send over all entities (excpet the client) that touch the pvs
	ent = NEXT_EDICT(sv.edicts);
	for(e = 1; e < sv.num_edicts; e++, ent = NEXT_EDICT(ent))
	{
#ifdef QUAKE2
		// don't send if flagged for NODRAW and there are no lighting effects
		if(ent->v.effects == EF_NODRAW)
			continue;
#endif

		// ignore if not touching a PV leaf
		if(ent != clent) // clent is ALWAYS sent
		{
			// ignore ents without visible models
			if(!ent->v.modelindex || !pr_strings[ent->v.model])
				continue;

			for(i = 0; i < ent->num_leafs; i++)
				if(pvs[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
					break;

			if(i == ent->num_leafs)
				continue; // not visible
		}

		if(msg->maxsize - msg->cursize < 16)
		{
			Con_Printf("packet overflow\n");
			return;
		}

		// send an update
		int bits = 0;

		float miss;
		
		for(i = 0; i < 3; i++)
		{
			miss = ent->v.origin[i] - ent->baseline.origin[i];
			if(miss < -0.1 || miss > 0.1)
				bits |= U_ORIGIN1 << i;
		}

		if(ent->v.angles[0] != ent->baseline.angles[0])
			bits |= U_ANGLE1;

		if(ent->v.angles[1] != ent->baseline.angles[1])
			bits |= U_ANGLE2;

		if(ent->v.angles[2] != ent->baseline.angles[2])
			bits |= U_ANGLE3;

		if(ent->v.movetype == MOVETYPE_STEP)
			bits |= U_NOLERP; // don't mess up the step animation

		if(ent->baseline.colormap != ent->v.colormap)
			bits |= U_COLORMAP;

		if(ent->baseline.skin != ent->v.skin)
			bits |= U_SKIN;

		if(ent->baseline.frame != ent->v.frame)
			bits |= U_FRAME;

		if(ent->baseline.effects != ent->v.effects)
			bits |= U_EFFECTS;

		if(ent->baseline.modelindex != ent->v.modelindex)
			bits |= U_MODEL;

		if(e >= 256)
			bits |= U_LONGENTITY;

		if(bits >= 256)
			bits |= U_MOREBITS;

		//
		// write the message
		//
		MSG_WriteByte(msg, bits | U_SIGNAL);

		if(bits & U_MOREBITS)
			MSG_WriteByte(msg, bits >> 8);
		if(bits & U_LONGENTITY)
			MSG_WriteShort(msg, e);
		else
			MSG_WriteByte(msg, e);

		if(bits & U_MODEL)
			MSG_WriteByte(msg, ent->v.modelindex);
		if(bits & U_FRAME)
			MSG_WriteByte(msg, ent->v.frame);
		if(bits & U_COLORMAP)
			MSG_WriteByte(msg, ent->v.colormap);
		if(bits & U_SKIN)
			MSG_WriteByte(msg, ent->v.skin);
		if(bits & U_EFFECTS)
			MSG_WriteByte(msg, ent->v.effects);
		if(bits & U_ORIGIN1)
			MSG_WriteCoord(msg, ent->v.origin[0]);
		if(bits & U_ANGLE1)
			MSG_WriteAngle(msg, ent->v.angles[0]);
		if(bits & U_ORIGIN2)
			MSG_WriteCoord(msg, ent->v.origin[1]);
		if(bits & U_ANGLE2)
			MSG_WriteAngle(msg, ent->v.angles[1]);
		if(bits & U_ORIGIN3)
			MSG_WriteCoord(msg, ent->v.origin[2]);
		if(bits & U_ANGLE3)
			MSG_WriteAngle(msg, ent->v.angles[2]);
	}
	
	// encode the packet entities as a delta from the
	// last packetentities acknowledged by the client
	
	SV_EmitPacketEntities(client, pack, msg);
	
	// now add the specialized nail update
	//SV_EmitNailUpdate(msg);
}

/*
=============
SV_CleanupEnts

=============
*/
void SV_CleanupEnts()
{
	int e;
	edict_t *ent;

	ent = NEXT_EDICT(sv.edicts);
	for(e = 1; e < sv.num_edicts; e++, ent = NEXT_EDICT(ent))
	{
		ent->v.effects = (int)ent->v.effects & ~EF_MUZZLEFLASH;
	}
}

/*
==================
SV_WriteClientdataToMessage

==================
*/
void SV_WriteClientdataToMessage(edict_t *ent, sizebuf_t *msg)
{
	int bits;
	int i;
	edict_t *other;
	int items;
	//#ifndef QUAKE2
	//eval_t	*val;
	//#endif

	//
	// send a damage message
	//
	if(ent->v.dmg_take || ent->v.dmg_save)
	{
		other = ent->v.dmg_inflictor;
		MSG_WriteByte(msg, svc_damage);
		MSG_WriteByte(msg, ent->v.dmg_save);
		MSG_WriteByte(msg, ent->v.dmg_take);
		for(i = 0; i < 3; i++)
			MSG_WriteCoord(msg, other->v.origin[i] + 0.5 * (other->v.mins[i] + other->v.maxs[i]));

		ent->v.dmg_take = 0;
		ent->v.dmg_save = 0;
	}

	//
	// send the current viewpos offset from the view entity
	//
	SV_SetIdealPitch(); // how much to look up / down ideally

	// a fixangle might get lost in a dropped packet.  Oh well.
	if(ent->v.fixangle)
	{
		MSG_WriteByte(msg, svc_setangle);
		for(i = 0; i < 3; i++)
			MSG_WriteAngle(msg, ent->v.angles[i]);
		ent->v.fixangle = 0;
	}

	bits = 0;

	if(ent->v.view_ofs[2] != DEFAULT_VIEWHEIGHT)
		bits |= SU_VIEWHEIGHT;

	if(ent->v.idealpitch)
		bits |= SU_IDEALPITCH;

	// stuff the sigil bits into the high bits of items for sbar, or else
	// mix in items2
	//#ifdef QUAKE2
	//items = (int)ent->v.items;
	/*
#else
	val = GetEdictFieldValue(ent, "items2");

	if (val)
		items = (int)ent->v.items | ((int)val->_float << 23);
	else
		items = (int)ent->v.items | ((int)gGlobalVariables.serverflags << 28);
#endif
*/

	bits |= SU_ITEMS;

	if((int)ent->v.flags & FL_ONGROUND)
		bits |= SU_ONGROUND;

	if(ent->v.waterlevel >= 2)
		bits |= SU_INWATER;

	for(i = 0; i < 3; i++)
	{
		if(ent->v.punchangle[i])
			bits |= (SU_PUNCH1 << i);
		if(ent->v.velocity[i])
			bits |= (SU_VELOCITY1 << i);
	}

	//if(ent->v.weaponframe)
		//bits |= SU_WEAPONFRAME;

	if(ent->v.armorvalue)
		bits |= SU_ARMOR;

	//if(ent->v.weapon)
	bits |= SU_WEAPON;

	// send the data

	MSG_WriteByte(msg, svc_clientdata);
	MSG_WriteShort(msg, bits);

	if(bits & SU_VIEWHEIGHT)
		MSG_WriteChar(msg, ent->v.view_ofs[2]);

	if(bits & SU_IDEALPITCH)
		MSG_WriteChar(msg, ent->v.idealpitch);

	for(i = 0; i < 3; i++)
	{
		if(bits & (SU_PUNCH1 << i))
			MSG_WriteChar(msg, ent->v.punchangle[i]);
		if(bits & (SU_VELOCITY1 << i))
			MSG_WriteChar(msg, ent->v.velocity[i] / 16);
	}

	// [always sent]	if (bits & SU_ITEMS)
	//MSG_WriteLong(msg, items);

	//if(bits & SU_WEAPONFRAME)
		//MSG_WriteByte(msg, ent->v.weaponframe);
	if(bits & SU_ARMOR)
		MSG_WriteByte(msg, ent->v.armorvalue);
	if(bits & SU_WEAPON)
		MSG_WriteByte(msg, SV_ModelIndex(pr_strings + ent->v.weaponmodel));

	MSG_WriteShort(msg, ent->v.health);
/*
	MSG_WriteByte(msg, ent->v.currentammo);
	MSG_WriteByte(msg, ent->v.ammo_shells);
	MSG_WriteByte(msg, ent->v.ammo_nails);
	MSG_WriteByte(msg, ent->v.ammo_rockets);
	MSG_WriteByte(msg, ent->v.ammo_cells);
*/

	// TODO
	//if(standard_quake)
	{
		MSG_WriteByte(msg, ent->v.weapon);
	}
	/*
	else
	{
		for(i = 0; i < 32; i++)
		{
			if(((int)ent->v.weapon) & (1 << i))
			{
				MSG_WriteByte(msg, i);
				break;
			}
		}
	}
	*/
}

/*
=================
SV_ExtractFromUserinfo

Pull specific info from a newly changed userinfo string
into a more C freindly form.
=================
*/
void SV_ExtractFromUserinfo(client_t *cl)
{
	char *val, *p, *q;
	int i;
	client_t *client;
	int dupc = 1;
	char newname[80];

	// name for C code
	val = Info_ValueForKey(cl->userinfo, "name");

	// trim user name
	strncpy(newname, val, sizeof(newname) - 1);
	newname[sizeof(newname) - 1] = 0;

	for(p = newname; (*p == ' ' || *p == '\r' || *p == '\n') && *p; p++)
		;

	if(p != newname && !*p)
	{
		//white space only
		strcpy(newname, "unnamed");
		p = newname;
	}

	if(p != newname && *p)
	{
		for(q = newname; *p; *q++ = *p++)
			;
		*q = 0;
	}
	for(p = newname + strlen(newname) - 1; p != newname && (*p == ' ' || *p == '\r' || *p == '\n'); p--)
		;
	p[1] = 0;

	if(strcmp(val, newname))
	{
		Info_SetValueForKey(cl->userinfo, "name", newname, MAX_INFO_STRING);
		val = Info_ValueForKey(cl->userinfo, "name");
	}

	if(!val[0] || !stricmp(val, "console"))
	{
		Info_SetValueForKey(cl->userinfo, "name", "unnamed", MAX_INFO_STRING);
		val = Info_ValueForKey(cl->userinfo, "name");
	}

	// check to see if another user by the same name exists
	while(1)
	{
		for(i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
		{
			if(!client->spawned || client == cl)
				continue;
			if(!stricmp(client->name, val))
				break;
		}
		if(i != svs.maxclients)
		{ // dup name
			if(strlen(val) > sizeof(cl->name) - 1)
				val[sizeof(cl->name) - 4] = 0;
			p = val;

			if(val[0] == '(')
				if(val[2] == ')')
					p = val + 3;
				else if(val[3] == ')')
					p = val + 4;

			sprintf(newname, "(%d)%-.40s", dupc++, p);
			Info_SetValueForKey(cl->userinfo, "name", newname, MAX_INFO_STRING);
			val = Info_ValueForKey(cl->userinfo, "name");
		}
		else
			break;
	}

	if(strncmp(val, cl->name, strlen(cl->name)))
	{
		if(!sv.paused)
		{
			// TODO
			/*
			if(!cl->lastnametime || realtime - cl->lastnametime > 5)
			{
				cl->lastnamecount = 0;
				cl->lastnametime = realtime;
			}
			else if(cl->lastnamecount++ > 4)
			{
				SV_BroadcastPrintf(/*PRINT_HIGH,/ "%s was kicked for name spam\n", cl->name); // TODO
				SV_ClientPrintf(cl, /*PRINT_HIGH,/ "You were kicked from the game for name spamming\n"); // TODO
				SV_DropClient(cl);
				return;
			}
			*/
		}

		if(cl->spawned /*&& !cl->spectator*/) // TODO: was cl->state >= cs_spawned
			SV_BroadcastPrintf(/*PRINT_HIGH,*/ "%s changed name to %s\n", cl->name, val); // TODO
	}

	strncpy(cl->name, val, sizeof(cl->name) - 1);

	// rate command
	val = Info_ValueForKey(cl->userinfo, "rate");
	if(strlen(val))
	{
		i = atoi(val);
		if(i < 500)
			i = 500;
		if(i > 10000)
			i = 10000;
		cl->netchan.rate = 1.0 / i;
	}

	// msg command
	val = Info_ValueForKey(cl->userinfo, "msg");
	if(strlen(val))
	{
		//cl->messagelevel = atoi(val); // TODO
	}
}

/*
=================
SV_UserinfoChanged

Pull specific info from a newly changed userinfo string
into a more C freindly form.
=================
*/
void SV_UserinfoChanged (client_t *cl)
{
	char	*val;
	int		i;

	// call prog code to allow overrides
	gEntityInterface.pfnClientUserInfoChanged (cl->edict, cl->userinfo);
	
	// name for C code
	strncpy (cl->name, Info_ValueForKey (cl->userinfo, "name"), sizeof(cl->name)-1);
	// mask off high bit
	for (i=0 ; i<sizeof(cl->name) ; i++)
		cl->name[i] &= 127;

	// rate command
	// TODO
	/*
	val = Info_ValueForKey (cl->userinfo, "rate");
	if (Q_strlen(val))
	{
		i = atoi(val);
		cl->rate = i;
		if (cl->rate < 100)
			cl->rate = 100;
		if (cl->rate > 15000)
			cl->rate = 15000;
	}
	else
		cl->rate = 5000;
	*/

	// msg command
	// TODO
	/*
	val = Info_ValueForKey (cl->userinfo, "msg");
	if (Q_strlen(val))
		cl->messagelevel = atoi(val);
	*/
};

/*
=======================
SV_SendClientDatagram
=======================
*/
qboolean SV_SendClientDatagram(client_t *client)
{
	byte buf[MAX_DATAGRAM];
	sizebuf_t msg;

	msg.data = buf;
	msg.maxsize = sizeof(buf);
	msg.cursize = 0;

	MSG_WriteByte(&msg, svc_time);
	MSG_WriteFloat(&msg, sv.time);

	// add the client specific data to the datagram
	SV_WriteClientdataToMessage(client->edict, &msg);

	SV_WriteEntitiesToClient(client, &msg);

	// copy the server datagram if there is space
	if(msg.cursize + sv.datagram.cursize < msg.maxsize)
		SZ_Write(&msg, sv.datagram.data, sv.datagram.cursize);

	// send the datagram
	// TODO
	Netchan_Transmit(&client->netchan, msg.cursize, msg.data);
	//if(NET_SendUnreliableMessage(client->netchan, &msg) == -1)
	{
		//SV_DropClient(client, true, "datagram"); // if the message couldn't send, kick off
		//return false;
	}

	return true;
}

/*
=======================
SV_UpdateToReliableMessages
=======================
*/
void SV_UpdateToReliableMessages()
{
	int i, j;
	client_t *client;

	// check for changes to be sent over the reliable streams
	for(i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if(host_client->old_frags != host_client->edict->v.frags)
		{
			for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
			{
				if(!client->active)
					continue;
				//MSG_WriteByte(&client->netchan.message, svc_updatefrags); // TODO
				//MSG_WriteByte(&client->netchan.message, i); // TODO
				//MSG_WriteShort(&client->netchan.message, host_client->edict->v.frags); // TODO
			}

			host_client->old_frags = host_client->edict->v.frags;
		}
	}

	for(j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if(!client->active)
			continue;
		SZ_Write(&client->netchan.message, sv.reliable_datagram.data, sv.reliable_datagram.cursize);
	}

	SZ_Clear(&sv.reliable_datagram);
}

/*
=======================
SV_SendNop

Send a nop message without trashing or sending the accumulated client
message buffer
=======================
*/
void SV_SendNop(client_t *client)
{
	sizebuf_t msg;
	byte buf[4];

	msg.data = buf;
	msg.maxsize = sizeof(buf);
	msg.cursize = 0;

	MSG_WriteChar(&msg, svc_nop);

	// TODO
	Netchan_Transmit(&client->netchan, msg.cursize, msg.data);
	//if(NET_SendUnreliableMessage(client->netchan, &msg) == -1)
		//SV_DropClient(client, true, "NOP"); // if the message couldn't send, kick off
	
	//client->last_message = realtime;
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages()
{
	int i, j;
	client_t *c;

	// update frags, names, etc
	SV_UpdateToReliableMessages();

	// build individual updates
	for(i = 0, c = svs.clients; i < svs.maxclients; i++, c++)
	{
		if(!c->connected)
			continue;

		if(c->drop)
		{
			SV_DropClient(c, false, "Dropped by the server!");
			c->drop = false;
			continue;
		}

		// TODO
		/*
		// check to see if we have a backbuf to stick in the reliable
		if (c->num_backbuf)
		{
			// will it fit?
			if (c->netchan.message.cursize + c->backbuf_size[0] < c->netchan.message.maxsize)
			{
				Con_DPrintf("%s: backbuf %d bytes\n", c->name, c->backbuf_size[0]);

				// it'll fit
				SZ_Write(&c->netchan.message, c->backbuf_data[0], c->backbuf_size[0]);
				
				//move along, move along
				for (j = 1; j < c->num_backbuf; j++)
				{
					memcpy(c->backbuf_data[j - 1], c->backbuf_data[j], c->backbuf_size[j]);
					c->backbuf_size[j - 1] = c->backbuf_size[j];
				}

				c->num_backbuf--;
				
				if (c->num_backbuf)
				{
					memset(&c->backbuf, 0, sizeof(c->backbuf));
					c->backbuf.data = c->backbuf_data[c->num_backbuf - 1];
					c->backbuf.cursize = c->backbuf_size[c->num_backbuf - 1];
					c->backbuf.maxsize = sizeof(c->backbuf_data[c->num_backbuf - 1]);
				}
			}
		}
		*/

		// the player isn't totally in the game yet
		// send small keepalive messages if too much time has passed
		// send a full message when the next signon stage has been requested
		// some other message data (name changes, etc) may accumulate
		// between signon stages
		/*
		if (!c->sendsignon)
		{
			if (realtime - c->last_message > 5)
				SV_SendNop (c);
			continue;	// don't send out non-signon messages
		}
		*/

		// if the reliable message overflowed, drop the client
		if(c->netchan.message.overflowed)
		{
			SZ_Clear(&c->netchan.message);
			SZ_Clear(&c->datagram);
			SV_BroadcastPrintf(/*PRINT_HIGH,*/ "%s overflowed\n", c->name); // TODO
			Con_Printf("WARNING: reliable overflow for %s\n", c->name);
			SV_DropClient(c, true, "Overflowed!");
			c->send_message = true;
			c->netchan.cleartime = 0; // don't choke this message
		}

		// only send messages if the client has sent one
		// and the bandwidth is not choked
		if(!c->send_message)
			continue;

		if(!sv.paused && !Netchan_CanPacket(&c->netchan))
		{
			c->chokecount++;
			continue; // bandwidth choke
		};
		
		c->send_message = false;

		/*
		if (c->netchan.message.cursize)
		{
			if (!Netchan_CanPacket (&c->netchan)) // TODO: was NET_CanSendMessage; Netchan_CanReliable?
			{
//				I_Printf ("can't write\n");
				continue;
			}

			if (c->dropasap)
				SV_DropClient (false);	// went to another level
			else
			{
				if (NET_SendMessage (c->netchan, &c->netchan.message) == -1)
					SV_DropClient (true);	// if the message couldn't send, kick off
				SZ_Clear (&c->netchan.message);
				c->last_message = realtime;
				c->sendsignon = false;
			}
		}
		*/

		if(c->spawned)
			SV_SendClientDatagram(c);
		else
			Netchan_Transmit(&c->netchan, 0, NULL); // just update reliable
	}

	// clear muzzle flashes
	SV_CleanupEnts();
}

/*
==============================================================================

SERVER SPAWNING

==============================================================================
*/

/*
================
SV_ModelIndex

================
*/
int SV_ModelIndex(const char *name)
{
	int i;

	if(!name || !name[0])
		return 0;

	for(i = 0; i < MAX_MODELS && sv.model_precache[i]; i++)
		if(!strcmp(sv.model_precache[i], name))
			return i;
	
	if(i == MAX_MODELS || !sv.model_precache[i])
		Sys_Error("SV_ModelIndex: model %s not precached", name);
	
	return i;
}

/*
================
SV_FlushSignon

Moves to the next signon buffer if needed
================
*/
// TODO: qw
/*
void SV_FlushSignon()
{
	if (sv.signon.cursize < sv.signon.maxsize - 512)
		return;

	if (sv.num_signon_buffers == MAX_SIGNON_BUFFERS-1)
		SV_Error ("sv.num_signon_buffers == MAX_SIGNON_BUFFERS-1");

	sv.signon_buffer_size[sv.num_signon_buffers-1] = sv.signon.cursize;
	sv.signon.data = sv.signon_buffers[sv.num_signon_buffers];
	sv.num_signon_buffers++;
	sv.signon.cursize = 0;
}
*/

/*
================
SV_CreateBaseline

Entity baselines are used to compress the update messages
to the clients - only the fields that differ from the
baseline will be transmitted
================
*/
void SV_CreateBaseline()
{
	int i;
	edict_t *svent;
	int entnum;

	for(entnum = 0; entnum < sv.num_edicts; entnum++)
	{
		// get the current server version
		svent = EDICT_NUM(entnum);
		if(svent->free)
			continue;
		// create baselines for all player slots,
		// and any other edict that has a visible model
		if(entnum > svs.maxclients && !svent->v.modelindex)
			continue;

		//
		// create entity baseline
		//
		VectorCopy(svent->v.origin, svent->baseline.origin);
		VectorCopy(svent->v.angles, svent->baseline.angles);
		svent->baseline.frame = svent->v.frame;
		svent->baseline.skin = svent->v.skin; // TODO: baseline.skinnum in qw
		if(entnum > 0 && entnum <= svs.maxclients)
		{
			svent->baseline.colormap = entnum;
			svent->baseline.modelindex = 0; // SV_ModelIndex("models/player.mdl"); // TODO
		}
		else
		{
			svent->baseline.colormap = 0;
			svent->baseline.modelindex = SV_ModelIndex(pr_strings + svent->v.model); // TODO: PR_GetString
		}
		
		//
		// flush the signon message out to a seperate buffer if
		// nearly full
		//
		//SV_FlushSignon(); // TODO: qw

		//
		// add to the message
		//
		MSG_WriteByte(&sv.signon, svc_spawnbaseline);
		MSG_WriteShort(&sv.signon, entnum);

		MSG_WriteByte(&sv.signon, svent->baseline.modelindex);
		MSG_WriteByte(&sv.signon, svent->baseline.frame);
		MSG_WriteByte(&sv.signon, svent->baseline.colormap);
		MSG_WriteByte(&sv.signon, svent->baseline.skin); // TODO: skinnum in qw
		for(i = 0; i < 3; i++)
		{
			MSG_WriteCoord(&sv.signon, svent->baseline.origin[i]);
			MSG_WriteAngle(&sv.signon, svent->baseline.angles[i]);
		}
		
		// TODO: int player, int entindex, struct entity_state_s *baseline, edict_t *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs
		gEntityInterface.pfnCreateBaseline(0, 0, &svent->baseline, NULL, 0, vec3_origin, vec3_origin);
	}
}

/*
=================
SV_BroadcastCommand

Sends text to all active clients
=================
*/
void SV_BroadcastCommand(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];

	if(!sv.state)
		return;
	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	MSG_WriteByte(&sv.reliable_datagram, svc_stufftext);
	MSG_WriteString(&sv.reliable_datagram, string);
}

/*
================
SV_SendReconnect

Tell all the clients that the server is changing levels
================
*/
void SV_SendReconnect()
{
	char data[128];
	sizebuf_t msg;

	msg.data = data;
	msg.cursize = 0;
	msg.maxsize = sizeof(data);

	//MSG_WriteChar (&msg, svc_stufftext);
	//MSG_WriteString (&msg, "reconnect\n");
	//NET_SendToAll (&msg, 5);
	SV_BroadcastCommand("reconnect\n");

	if(cls.state != ca_dedicated)
#ifdef QUAKE2
		Cbuf_InsertText("reconnect\n");
#else
		Cmd_ExecuteString("reconnect\n", src_command);
#endif
}

/*
================
SV_SaveSpawnparms

Grabs the current state of (QW: the progs serverinfo flags and) 
each client for saving across the
transition to another level
================
*/
void SV_SaveSpawnparms()
{
	int i, j;

	// no progs loaded yet
	//if(!sv.state)
		//return;
	
	// serverflags is the only game related thing maintained
	svs.serverflags = gGlobalVariables.serverflags;

	for(i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if(!host_client->active) // TODO: !->spawned in qw
			continue;

		// TODO: qw
		// needs to reconnect
		//host_client->connected = true;
		
		// call the progs to get default spawn parms for the new client
		//gGlobalVariables.self = EDICT_TO_PROG(host_client->edict); // TODO
		gEntityInterface.pfnParmsChangeLevel();
		//for(j = 0; j < NUM_SPAWN_PARMS; j++)
			//host_client->spawn_parms[j] = (&gGlobalVariables.parm1)[j];
	}
}

/*
================
SV_CalcPHS

Expands the PVS and calculates the PHS
(Potentially Hearable Set)
================
*/
/*
void SV_CalcPHS (void)
{
	int		rowbytes, rowwords;
	int		i, j, k, l, index, num;
	int		bitbyte;
	unsigned	*dest, *src;
	byte	*scan;
	int		count, vcount;

	Con_Printf ("Building PHS...\n");

	num = sv.worldmodel->numleafs;
	rowwords = (num+31)>>5;
	rowbytes = rowwords*4;

	sv.pvs = Hunk_Alloc (rowbytes*num);
	scan = sv.pvs;
	vcount = 0;
	for (i=0 ; i<num ; i++, scan+=rowbytes)
	{
		memcpy (scan, Mod_LeafPVS(sv.worldmodel->leafs+i, sv.worldmodel),
			rowbytes);
		if (i == 0)
			continue;
		for (j=0 ; j<num ; j++)
		{
			if ( scan[j>>3] & (1<<(j&7)) )
			{
				vcount++;
			}
		}
	}


	sv.phs = Hunk_Alloc (rowbytes*num);
	count = 0;
	scan = sv.pvs;
	dest = (unsigned *)sv.phs;
	for (i=0 ; i<num ; i++, dest += rowwords, scan += rowbytes)
	{
		memcpy (dest, scan, rowbytes);
		for (j=0 ; j<rowbytes ; j++)
		{
			bitbyte = scan[j];
			if (!bitbyte)
				continue;
			for (k=0 ; k<8 ; k++)
			{
				if (! (bitbyte & (1<<k)) )
					continue;
				// or this pvs row into the phs
				// +1 because pvs is 1 based
				index = ((j<<3)+k+1);
				if (index >= num)
					continue;
				src = (unsigned *)sv.pvs + index*rowwords;
				for (l=0 ; l<rowwords ; l++)
					dest[l] |= src[l];
			}
		}

		if (i == 0)
			continue;
		for (j=0 ; j<num ; j++)
			if ( ((byte *)dest)[j>>3] & (1<<(j&7)) )
				count++;
	}

	Con_Printf ("Average leafs visible / hearable / total: %i / %i / %i\n"
		, vcount/num, count/num, num);
}
*/

// TODO: qw
/*
unsigned SV_CheckModel(char *mdl)
{
	byte	stackbuf[1024];		// avoid dirtying the cache heap
	byte *buf;
	unsigned short crc;
//	int len;

	buf = (byte *)COM_LoadStackFile (mdl, stackbuf, sizeof(stackbuf));
	crc = CRC_Block(buf, com_filesize);
//	for (len = com_filesize; len; len--, buf++)
//		CRC_ProcessByte(&crc, *buf);

	return crc;
}
*/

void SV_ActivateServer()
{
	sv.active = true; // TODO: non-qw

	// all setup/spawning is completed, any further precache statements 
	// (QW: or prog writes to the signon message) are errors
	sv.state = ss_active;
	
	gEntityInterface.pfnServerActivate(sv.edicts, sv.max_edicts, svs.maxclients);
};

// TODO: use
void SV_DeactivateServer()
{
	sv.state = ss_loading;
	
	gEntityInterface.pfnServerDeactivate();
};

/*
================
SV_SpawnServer

Change the server to a new map, taking all connected
clients along with it.

This is called at the start of each level
(QW: This is only called from the Host_Map_f() function.)
================
*/
extern float scr_centertime_off;

void SV_SpawnServer(const char *server, const char *startspot)
{
	edict_t *ent;
	int i;

	// TODO: non-qw
	// let's not have any servers with no name
	if(hostname.string[0] == 0)
		Cvar_Set("hostname", "UNNAMED");
	scr_centertime_off = 0;
	//

	// load progs to get entity field count
	// which determines how big each edict is
	Host_InitializeGameDLL();
	
	gEntityInterface.pfnGameInit(); // TODO
	
	Con_DPrintf("Spawn Server %s\n", server);
	svs.changelevel_issued = false; // now safe to issue another // TODO: non-qw

	//SV_SaveSpawnparms (); // TODO: qw
	
	// TODO: qw
	//svs.spawncount++; // any partially connected client will be restarted

// TODO: non-qw
	//
	// tell all connected clients that we are going to a new level
	//
	if(sv.active)
	{
		SV_SendReconnect();
	}

	//
	// make cvars consistant
	//
	if(coop.value)
		Cvar_SetValue("deathmatch", 0);
	current_skill = (int)(skill.value + 0.5);
	if(current_skill < 0)
		current_skill = 0;
	if(current_skill > 3)
		current_skill = 3;

	Cvar_SetValue("skill", (float)current_skill);
//
	
	//sv.state = ss_dead; // TODO: qw
	
	//
	// set up the new server
	//
	Host_ClearMemory();
	//
	// TODO: qw
	//Mod_ClearAll ();
	//Hunk_FreeToLowMark (host_hunklevel);
	//
	
	// wipe the entire per-level structure
	memset(&sv, 0, sizeof(sv));

	// TODO: qw
	/*
	sv.datagram.maxsize = sizeof(sv.datagram_buf);
	sv.datagram.data = sv.datagram_buf;
	sv.datagram.allowoverflow = true;

	sv.reliable_datagram.maxsize = sizeof(sv.reliable_datagram_buf);
	sv.reliable_datagram.data = sv.reliable_datagram_buf;
	
	sv.multicast.maxsize = sizeof(sv.multicast_buf);
	sv.multicast.data = sv.multicast_buf;
	
	sv.master.maxsize = sizeof(sv.master_buf);
	sv.master.data = sv.master_buf;
	
	sv.signon.maxsize = sizeof(sv.signon_buffers[0]);
	sv.signon.data = sv.signon_buffers[0];
	sv.num_signon_buffers = 1;
	*/
	
	strcpy(sv.name, server);

	if(startspot)
		strcpy(sv.startspot, startspot);

// TODO: non-qw
	// allocate server memory
	sv.max_edicts = MAX_EDICTS;

	// allocate edicts
	sv.edicts = Hunk_AllocName(sv.max_edicts * sizeof(edict_t), "edicts");

	sv.datagram.maxsize = sizeof(sv.datagram_buf);
	sv.datagram.cursize = 0;
	sv.datagram.data = sv.datagram_buf;

	sv.reliable_datagram.maxsize = sizeof(sv.reliable_datagram_buf);
	sv.reliable_datagram.cursize = 0;
	sv.reliable_datagram.data = sv.reliable_datagram_buf;

	sv.signon.maxsize = sizeof(sv.signon_buf);
	sv.signon.cursize = 0;
	sv.signon.data = sv.signon_buf;
//

	// leave slots at start for clients only
	sv.num_edicts = svs.maxclients + 1;
	for(i = 0; i < svs.maxclients; i++)
	{
		ent = EDICT_NUM(i + 1);
		svs.clients[i].edict = ent;
		//ZOID - make sure we update frags right
		//svs.clients[i].old_frags = 0; // TODO: qw
	}

	sv.state = ss_loading; // TODO: non-qw
	sv.paused = false; // TODO: non-qw

	sv.time = 1.0;

	strcpy(sv.name, server);
	sprintf(sv.modelname, "maps/%s.bsp", server);
	sv.worldmodel = Mod_ForName(sv.modelname, false); // TODO: true in qw
	//SV_CalcPHS (); // TODO: qw
	
	// TODO: non-qw
	if(!sv.worldmodel)
	{
		Con_Printf("Couldn't spawn server %s\n", sv.modelname);
		sv.active = false;
		return;
	};
	sv.models[1] = sv.worldmodel;
	//

	//
	// clear world interaction links
	//
	SV_ClearWorld();

	sv.sound_precache[0] = pr_strings;

	sv.model_precache[0] = pr_strings;
	sv.model_precache[1] = sv.modelname;
	//sv.models[1] = sv.worldmodel; // TODO: qw
	for(i = 1; i < sv.worldmodel->numsubmodels; i++)
	{
		sv.model_precache[1 + i] = localmodels[i];
		sv.models[i + 1] = Mod_ForName(localmodels[i], false);
	}
	
	// TODO: qw
	// check player/eyes models for hacks
	//sv.model_player_checksum = SV_CheckModel("progs/player.mdl");
	//sv.eyes_player_checksum = SV_CheckModel("progs/eyes.mdl");

	//
	// load/spawn the rest of the entities on the map
	//
	
	// precache and static commands can be issued during map initialization
	//sv.state = ss_loading; // TODO: qw
	
	ent = EDICT_NUM(0);
	memset(&ent->v, 0, sizeof(entvars_t)); // TODO: non-qw
	ent->free = false;
	ent->v.model = sv.worldmodel->name - pr_strings; // TODO: PR_SetString
	// TODO: non-qw
	ent->v.modelindex = 1; // world model
	ent->v.solid = SOLID_BSP;
	ent->v.movetype = MOVETYPE_PUSH;
	//

	// TODO: non-qw
	if(coop.value)
		gGlobalVariables.coop = coop.value;
	else
		gGlobalVariables.deathmatch = deathmatch.value;
	//

	gGlobalVariables.mapname = sv.name - pr_strings; // TODO: PR_SetString
	gGlobalVariables.startspot = sv.startspot - pr_strings; // TODO: PR_SetString

	// TODO: non-qw
	// serverflags are for cross level information (sigils)
	gGlobalVariables.serverflags = svs.serverflags;

	//// look up some model indexes for specialized message compression
	//SV_FindModelNumbers (); // TODO: qw
	
	// TODO: non-qw?
	// run the frame start qc function to let progs check cvars
	//SV_ProgStartFrame ();

	// load and spawn all other entities
	ED_LoadFromFile(sv.worldmodel->entities);

	SV_ActivateServer();

	// TODO: below: non-qw?
	
	// run two frames to allow everything to settle
	host_frametime = 0.1;
	SV_Physics();
	SV_Physics();

	// save movement vars
	SV_SetMoveVars();

	// create a baseline for more efficient communications
	//SV_CreateBaseline(); // TODO
	//sv.signon_buffer_size[sv.num_signon_buffers-1] = sv.signon.cursize;


	// TODO: not present in qw
	// send serverinfo to all connected clients
	for(i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
		if(host_client->active)
			SV_SendServerinfo(host_client);
	//
	
	//Info_SetValueForKey (svs.info, "map", sv.name, MAX_SERVERINFO_STRING);
	Con_DPrintf("Server spawned.\n");
}