/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-2001 Id Software, Inc.
 *	Copyright (C) 2018, 2020 BlackPhrase
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
/// @brief client main loop

#include "quakedef.h"

#ifdef _WIN32
#include "winquake.h"
#endif

// we need to declare some mouse variables here, because the menu system
// references them even when on a unix system.

cvar_t cl_name = { "_cl_name", "player", FCVAR_ARCHIVE }; // TODO: this should be removed

cvar_t cl_timeout = { "cl_timeout", "60" };

cvar_t cl_shownet = { "cl_shownet", "0" }; // can be 0, 1, or 2
cvar_t cl_nolerp = { "cl_nolerp", "0" };

/*
cvar_t	cl_sbar		= {"cl_sbar", "0", true};
cvar_t	cl_hudswap	= {"cl_hudswap", "0", true};
cvar_t	cl_maxfps	= {"fps_max", "0", true};
*/

cvar_t entlatency = { "entlatency", "20" };
cvar_t cl_predict_players = { "cl_predict_players", "1" };
cvar_t cl_predict_players2 = { "cl_predict_players2", "1" };
cvar_t cl_solid_players = { "cl_solid_players", "1" };

cvar_t localid = { "localid", "" };

cvar_t show_fps = {"cl_showfps", "0"}; // set for running times
int fps_count; // TODO: used by gl_screen

//
// info mirrors
//
cvar_t password = { "password", "", FCVAR_USERINFO };
cvar_t spectator = { "spectator", "", FCVAR_USERINFO };
cvar_t model = { "model", "", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t name = { "name", "unnamed", FCVAR_ARCHIVE | FCVAR_USERINFO }; // TODO: cl_name?
cvar_t team = { "team", "", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t skin = { "skin", "", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t topcolor = { "topcolor", "0", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t bottomcolor = { "bottomcolor", "0", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t rate = { "rate", "2500", FCVAR_ARCHIVE | FCVAR_USERINFO };

cvar_t cl_dlmax = { "cl_dlmax", "80", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t cl_lc = { "cl_lc", "1", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t cl_lw = { "cl_lw", "1", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t cl_updaterate = { "cl_updaterate", "30", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t cl_autowepswitch = { "_cl_autowepswitch", "1", FCVAR_ARCHIVE | FCVAR_USERINFO };

static qboolean allowremotecmd = true; // TODO: purpose?

client_static_t cls;
client_state_t cl;

// FIXME: put these on hunk?
efrag_t cl_efrags[MAX_EFRAGS];
cl_entity_t cl_entities[MAX_EDICTS]; // TODO: dynamically-sized
cl_entity_t cl_static_entities[MAX_STATIC_ENTITIES];
lightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
dlight_t cl_dlights[MAX_DLIGHTS];

int cl_numvisedicts;
cl_entity_t *cl_visedicts[MAX_VISEDICTS];

// TODO: cls.connect_time?
double connect_time = -1; // for connection retransmits

playermove_t clpmove;

/*
================
CL_Connect_f (CL_EstablishConnection)

User command to connect to server
Address argument should be either "local(host)" or a net address to be passed on
================
*/
void CL_Connect_f()
{
	if (cls.state == ca_dedicated)
		return;
	
	//char	server[MAX_QPATH];
	const char *server;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("usage: connect <server>\n");
		return;
	};

	if (cls.demoplayback)
		return;
	
	/*
	cls.demonum = -1;		// stop demo loop in case this fails
	
	if (cls.demoplayback)
	{
		CL_StopPlayback ();
		CL_Disconnect ();
	};
	*/

	// Q2
	/*
	if (Com_ServerState ())
	{
		// if running a local server, kill it and reissue
		SV_Shutdown (va("Server quit\n", msg), false);
	}
	else
		CL_Disconnect ();
	*/
	
	//strcpy (server, Cmd_Argv(1));
	server = Cmd_Argv(1);
	
	NET_Config(true); // allow remote
	
	CL_Disconnect();
	
	cls.demonum = -1; // not in the demo loop now
	cls.state = ca_connecting;
	cls.signon = 0; // need all the signon messages before playing
	
	// TODO: dirty hack to replace "local" with "localhost"
	if(!Q_strcmp(server, "local"))
		strncpy(cls.servername, "localhost", sizeof(cls.servername) - 1);
	else
		strncpy(cls.servername, server, sizeof(cls.servername) - 1);
	
	/*cls.*/connect_time = -99999; // CL_CheckForResend() will fire immediately
	
	//Con_DPrintf ("CL_EstablishConnection: connecting to %s\n", cls.servername);
	
	CL_BeginServerConnect();
};

/*
=======================
CL_SendConnectPacket

called by CL_Connect_f and CL_CheckResend
======================
*/
void CL_SendConnectPacket()
{
	netadr_t adr;
	char data[2048];
	double t1, t2;
	// JACK: Fixed bug where DNS lookups would cause two connects real fast
	//       Now, adds lookup time to the connect time.
	//		 Should I add it to realtime instead?!?!

	//if(cls.state != ca_disconnected)
		//return;

	t1 = Sys_FloatTime();

	if(!NET_StringToAdr(cls.servername, &adr))
	{
		Con_Printf("Bad server address\n");
		connect_time = -1; // cls.connect_time
		return;
	};

	if(adr.port == 0)
		adr.port = BigShort(PORT_SERVER);

	t2 = Sys_FloatTime();

	connect_time = realtime + t2 - t1; // for retransmit requests

	cls.qport = Cvar_VariableValue("qport");

	Info_SetValueForStarKey(cls.userinfo, "*ip", NET_AdrToString(adr), MAX_INFO_STRING);

	//	Con_Printf ("Connecting to %s...\n", cls.servername);
	sprintf(data, "%c%c%c%cconnect %i %i %i \"%s\"\n",
	        255, 255, 255, 255, PROTOCOL_VERSION, cls.qport, cls.challenge, cls.userinfo);
	NET_SendPacket(NS_CLIENT, strlen(data), data, adr);
};

/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend()
{
	netadr_t adr;
	char data[2048];
	double t1, t2;

	if(connect_time == -1)
		return;

	// resend if we haven't gotten a reply yet
	if (cls.state != ca_connecting)
		return;
	
	//if(cls.state != ca_disconnected)
		//return;

	if(connect_time && realtime - connect_time < 5.0)
		return;

	t1 = Sys_FloatTime();

	if(!NET_StringToAdr(cls.servername, &adr))
	{
		Con_Printf("Bad server address\n");
		cls.state = ca_disconnected;
		connect_time = -1;
		return;
	};

	if(adr.port == 0)
		adr.port = BigShort(PORT_SERVER);

	// TODO: q2
	///*cls.*/connect_time = /*cls.*/realtime; // for retransmit requests
	
	t2 = Sys_FloatTime();

	connect_time = realtime + t2 - t1; // for retransmit requests

	Con_Printf("Connecting to %s...\n", cls.servername);
	
	// TODO: q2
	//Netchan_OutOfBandPrint (NS_CLIENT, adr, "getchallenge\n");
	//
	sprintf(data, "%c%c%c%cgetchallenge\n", 255, 255, 255, 255);
	NET_SendPacket(NS_CLIENT, Q_strlen(data), data, adr);
	//
};

void CL_BeginServerConnect()
{
	connect_time = 0;
	CL_CheckForResend();
};

/*
=====================
CL_ClearState

=====================
*/
void CL_ClearState()
{
	int i;

	//S_StopAllSounds (true);

	if(!sv.active)
		Host_ClearMemory();

	//CL_ClearEffects (); // Q2
	//CL_ClearTEnts ();

	// wipe the entire cl structure
	memset(&cl, 0, sizeof(cl));
	//memset (&cl_entities, 0, sizeof(cl_entities));

	SZ_Clear(&cls.netchan.message);

	// clear other arrays
	memset(cl_efrags, 0, sizeof(cl_efrags));
	memset(cl_entities, 0, sizeof(cl_entities));
	memset(cl_dlights, 0, sizeof(cl_dlights));
	memset(cl_lightstyle, 0, sizeof(cl_lightstyle));
	memset(cl_temp_entities, 0, sizeof(cl_temp_entities));
	memset(cl_beams, 0, sizeof(cl_beams));

	//
	// allocate the efrags and chain together into a free list
	//
	cl.free_efrags = cl_efrags;
	for(i = 0; i < MAX_EFRAGS - 1; i++)
		cl.free_efrags[i].entnext = &cl.free_efrags[i + 1];
	cl.free_efrags[i].entnext = NULL;
};

/*
=====================
CL_Disconnect

Sends a disconnect message to the server
This is also called on Host_Error, so it shouldn't cause any errors
=====================
*/
void CL_Disconnect()
{
	byte final[10];

	connect_time = -1;

	// stop sounds (especially looping!)
	S_StopAllSounds(true);

	// bring the console down and fade the colors back to normal
	//	SCR_BringDownConsole ();
	
	//GameUI_DisconnectFromServer(); // TODO

	// if running a local server, shut it down
	if(cls.demoplayback)
		CL_StopPlayback();
	else if(cls.state == ca_connected)
	{
		if(cls.demorecording)
			CL_Stop_f();

		//Con_DPrintf ("Sending clc_disconnect\n");
		//SZ_Clear (&cls.netchan.message);
		//MSG_WriteByte (&cls.netchan.message, clc_disconnect);
		//NET_SendUnreliableMessage (cls.netchan, &cls.netchan.message);
		//SZ_Clear (&cls.netchan.message);

		final[0] = clc_stringcmd;
		strcpy(final + 1, "drop");
		Netchan_Transmit(&cls.netchan, 6, final);
		Netchan_Transmit(&cls.netchan, 6, final);
		Netchan_Transmit(&cls.netchan, 6, final);

		cls.state = ca_disconnected;

		if(sv.active)
			Host_ShutdownServer(false);
	};

	Cam_Reset();

	cls.demoplayback = cls.timedemo = false;
	cls.signon = 0;
};

void CL_Disconnect_f()
{
	CL_Disconnect();
	if(sv.active)
		Host_ShutdownServer(false);
};

/*
=====================
CL_SignonReply

An svc_signonnum has been received, perform a client side setup
=====================
*/
void CL_SignonReply()
{
	char str[8192];

	Con_DPrintf("CL_SignonReply: %i\n", cls.signon);

	switch(cls.signon)
	{
	case 1:
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		sprintf(str, "spawn %s", cls.spawnparms);
		MSG_WriteString(&cls.netchan.message, str);
		
		Cache_Report(); // print remaining memory
		break;

	case 2:
		SCR_EndLoadingPlaque(); // allow normal screen updates
		break;
	};
};

/*
=====================
CL_NextDemo

Called to play the next demo in the demo loop
=====================
*/
void CL_NextDemo()
{
	char str[1024];

	if(cls.demonum == -1)
		return; // don't play demos

	SCR_BeginLoadingPlaque();

	if(!cls.demos[cls.demonum][0] || cls.demonum == MAX_DEMOS)
	{
		cls.demonum = 0;
		if(!cls.demos[cls.demonum][0])
		{
			Con_Printf("No demos listed with startdemos\n");
			cls.demonum = -1;
			return;
		};
	};

	sprintf(str, "playdemo %s\n", cls.demos[cls.demonum]);
	Cbuf_InsertText(str);
	cls.demonum++;
};

/*
==============
CL_PrintEntities_f
==============
*/
void CL_PrintEntities_f()
{
	cl_entity_t *ent;
	int i;

	for(i = 0, ent = cl_entities; i < cl.num_entities; i++, ent++)
	{
		Con_Printf("%3i:", i);
		if(!ent->model)
		{
			Con_Printf("EMPTY\n");
			continue;
		};
		Con_Printf("%s:%2i  (%5.1f,%5.1f,%5.1f) [%5.1f %5.1f %5.1f]\n", ent->model->name, ent->frame, ent->origin[0], ent->origin[1], ent->origin[2], ent->angles[0], ent->angles[1], ent->angles[2]);
	};
};

/*
===============
SetPal

Debugging tool, just flashes the screen
===============
*/
void SetPal(int i)
{
#if 0
	static int old;
	byte	pal[768];
	int		c;
	
	if (i == old)
		return;
	old = i;

	if (i==0)
		VID_SetPalette (host_basepal);
	else if (i==1)
	{
		for (c=0 ; c<768 ; c+=3)
		{
			pal[c] = 0;
			pal[c+1] = 255;
			pal[c+2] = 0;
		};
		VID_SetPalette (pal);
	}
	else
	{
		for (c=0 ; c<768 ; c+=3)
		{
			pal[c] = 0;
			pal[c+1] = 0;
			pal[c+2] = 255;
		};
		VID_SetPalette (pal);
	};
#endif
};

/*
===============
CL_AllocDlight

===============
*/
dlight_t *CL_AllocDlight(int key)
{
	int i;
	dlight_t *dl;

	// first look for an exact key match
	if(key)
	{
		dl = cl_dlights;
		for(i = 0; i < MAX_DLIGHTS; i++, dl++)
		{
			if(dl->key == key)
			{
				memset(dl, 0, sizeof(*dl));
				dl->key = key;
				return dl;
			};
		};
	};

	// then look for anything else
	dl = cl_dlights;
	for(i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if(dl->die < cl.time)
		{
			memset(dl, 0, sizeof(*dl));
			dl->key = key;
			return dl;
		};
	};

	dl = &cl_dlights[0];
	memset(dl, 0, sizeof(*dl));
	dl->key = key;
	return dl;
};

/*
===============
CL_DecayLights

===============
*/
void CL_DecayLights()
{
	int i;
	dlight_t *dl;
	float time;

	time = cl.time - cl.oldtime;

	dl = cl_dlights;
	for(i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if(dl->die < cl.time || !dl->radius)
			continue;

		dl->radius -= time * dl->decay;
		if(dl->radius < 0)
			dl->radius = 0;
	};
};

void CL_WeaponAnim(int anAnim, int anBody)
{
	// TODO
};

/*
===============
CL_LerpPoint

Determines the fraction between the last two messages that the objects
should be put at.
===============
*/
float CL_LerpPoint()
{
	float f, frac;

	f = cl.mtime[0] - cl.mtime[1];

	if(!f || cl_nolerp.value || cls.timedemo || sv.active)
	{
		cl.time = cl.mtime[0];
		return 1;
	};

	if(f > 0.1)
	{ // dropped packet, or start of demo
		cl.mtime[1] = cl.mtime[0] - 0.1;
		f = 0.1;
	};
	frac = (cl.time - cl.mtime[1]) / f;
	//Con_Printf ("frac: %f\n",frac);
	if(frac < 0)
	{
		if(frac < -0.01)
		{
			SetPal(1);
			cl.time = cl.mtime[1];
			//				Con_Printf ("low frac\n");
		};
		frac = 0;
	}
	else if(frac > 1)
	{
		if(frac > 1.01)
		{
			SetPal(2);
			cl.time = cl.mtime[0];
			//				Con_Printf ("high frac\n");
		};
		frac = 1;
	}
	else
		SetPal(0);

	return frac;
};

/*
===============
CL_RelinkEntities
===============
*/
void CL_RelinkEntities()
{
	cl_entity_t *ent;
	int i, j;
	float frac, f, d;
	vec3_t delta;
	float bobjrotate;
	vec3_t oldorg;
	dlight_t *dl;

	// determine partial update time
	frac = CL_LerpPoint();

	cl_numvisedicts = 0;

	//
	// interpolate player info
	//
	for(i = 0; i < 3; i++)
		cl.velocity[i] = cl.mvelocity[1][i] +
		frac * (cl.mvelocity[0][i] - cl.mvelocity[1][i]);

	if(cls.demoplayback)
	{
		// interpolate the angles
		for(j = 0; j < 3; j++)
		{
			d = cl.mviewangles[0][j] - cl.mviewangles[1][j];
			if(d > 180)
				d -= 360;
			else if(d < -180)
				d += 360;
			cl.viewangles[j] = cl.mviewangles[1][j] + frac * d;
		};
	};

	bobjrotate = anglemod(100 * cl.time);

	// start on the entity after the world
	for(i = 1, ent = cl_entities + 1; i < cl.num_entities; i++, ent++)
	{
		if(!ent->model)
		{ // empty slot
			if(ent->forcelink)
				R_RemoveEfrags(ent); // just became empty
			continue;
		};

		// if the object wasn't included in the last packet, remove it
		if(ent->msgtime != cl.mtime[0])
		{
			ent->model = NULL;
			continue;
		};

		VectorCopy(ent->origin, oldorg);

		if(ent->forcelink)
		{ // the entity was not updated in the last message
			// so move to the final spot
			VectorCopy(ent->ph[0].origin, ent->origin);
			VectorCopy(ent->ph[0].angles, ent->angles);
		}
		else
		{ // if the delta is large, assume a teleport and don't lerp
			f = frac;
			for(j = 0; j < 3; j++)
			{
				delta[j] = ent->ph[0].origin[j] - ent->ph[1].origin[j];
				if(delta[j] > 100 || delta[j] < -100)
					f = 1; // assume a teleportation, not a motion
			};

			// interpolate the origin and angles
			for(j = 0; j < 3; j++)
			{
				ent->origin[j] = ent->ph[1].origin[j] + f * delta[j];

				d = ent->ph[0].angles[j] - ent->ph[1].angles[j];
				if(d > 180)
					d -= 360;
				else if(d < -180)
					d += 360;
				ent->angles[j] = ent->ph[1].angles[j] + f * d;
			};
		};

		// rotate binary objects locally
		if(ent->model->flags & EF_ROTATE)
			ent->angles[1] = bobjrotate;

		if(ent->effects & EF_BRIGHTFIELD)
			R_EntityParticles(ent);
#ifdef QUAKE2
		if(ent->effects & EF_DARKFIELD)
			R_DarkFieldParticles(ent);
#endif
		if(ent->effects & EF_MUZZLEFLASH)
		{
			vec3_t fv, rv, uv;

			dl = CL_AllocDlight(i);
			VectorCopy(ent->origin, dl->origin);
			dl->origin[2] += 16;
			AngleVectors(ent->angles, fv, rv, uv);

			VectorMA(dl->origin, 18, fv, dl->origin);
			dl->radius = 200 + (rand() & 31);
			dl->minlight = 32;
			dl->die = cl.time + 0.1;
		};

		if(ent->effects & EF_BRIGHTLIGHT)
		{
			dl = CL_AllocDlight(i);
			VectorCopy(ent->origin, dl->origin);
			dl->origin[2] += 16;
			dl->radius = 400 + (rand() & 31);
			dl->die = cl.time + 0.001;
		};

		if(ent->effects & EF_DIMLIGHT)
		{
			dl = CL_AllocDlight(i);
			VectorCopy(ent->origin, dl->origin);
			dl->radius = 200 + (rand() & 31);
			dl->die = cl.time + 0.001;
		};

#ifdef QUAKE2
		if(ent->effects & EF_DARKLIGHT)
		{
			dl = CL_AllocDlight(i);
			VectorCopy(ent->origin, dl->origin);
			dl->radius = 200.0 + (rand() & 31);
			dl->die = cl.time + 0.001;
			dl->dark = true;
		};

		if(ent->effects & EF_LIGHT)
		{
			dl = CL_AllocDlight(i);
			VectorCopy(ent->origin, dl->origin);
			dl->radius = 200;
			dl->die = cl.time + 0.001;
		};
#endif

		if(ent->model->flags & EF_GIB)
			R_RocketTrail(oldorg, ent->origin, 2);
		else if(ent->model->flags & EF_ZOMGIB)
			R_RocketTrail(oldorg, ent->origin, 4);
		else if(ent->model->flags & EF_TRACER)
			R_RocketTrail(oldorg, ent->origin, 3);
		else if(ent->model->flags & EF_TRACER2)
			R_RocketTrail(oldorg, ent->origin, 5);
		else if(ent->model->flags & EF_ROCKET)
		{
			R_RocketTrail(oldorg, ent->origin, 0);
			dl = CL_AllocDlight(i);
			VectorCopy(ent->origin, dl->origin);
			dl->radius = 200;
			dl->die = cl.time + 0.01;
		}
		else if(ent->model->flags & EF_GRENADE)
			R_RocketTrail(oldorg, ent->origin, 1);
		else if(ent->model->flags & EF_TRACER3)
			R_RocketTrail(oldorg, ent->origin, 6);

		ent->forcelink = false;

		if(i == cl.viewentity && !chase_active.value)
			continue;

#ifdef QUAKE2
		if(ent->effects & EF_NODRAW)
			continue;
#endif

		if(cl_numvisedicts < MAX_VISEDICTS)
		{
			cl_visedicts[cl_numvisedicts] = ent;
			cl_numvisedicts++;
		};
	};
};

/*
=================
CL_ConnectionlessPacket

Responses to broadcasts, etc
=================
*/
void CL_ConnectionlessPacket()
{
	char *s;
	int c;

	MSG_BeginReading();
	MSG_ReadLong(); // skip the -1

	c = MSG_ReadByte();

	if(!cls.demoplayback)
		Con_Printf("%s: ", NET_AdrToString(net_from));

	//Con_DPrintf ("%s", net_message.data + 5);

	if(c == S2C_CONNECTION)
	{
		Con_Printf("connection\n");

		if(cls.state >= ca_connected)
		{
			if(!cls.demoplayback)
				Con_Printf("Dup connect received.  Ignored.\n");
			return;
		};

		Netchan_Setup(NS_CLIENT, &cls.netchan, net_from, cls.qport);
		
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, "new");
		
		cls.state = ca_connected;
		Con_Printf("Connected.\n");
		
		allowremotecmd = false; // localid required now for remote cmds
		return;
	};

	// remote command from gui front end
	if(c == A2C_CLIENT_COMMAND)
	{
		char cmdtext[2048];

		Con_Printf("client command\n");

		if((*(unsigned *)net_from.ip != *(unsigned *)net_local_adr.ip && *(unsigned *)net_from.ip != htonl(INADDR_LOOPBACK)))
		{
			Con_Printf("Command packet from remote host.  Ignored.\n");
			return;
		};

#ifdef _WIN32
		ShowWindow(mainwindow, SW_RESTORE);
		SetForegroundWindow(mainwindow);
#endif

		s = MSG_ReadString();

		strncpy(cmdtext, s, sizeof(cmdtext) - 1);
		cmdtext[sizeof(cmdtext) - 1] = 0;

		s = MSG_ReadString();

		while(*s && isspace(*s))
			s++;
		while(*s && isspace(s[strlen(s) - 1]))
			s[strlen(s) - 1] = 0;

		if(!allowremotecmd && (!*localid.string || strcmp(localid.string, s)))
		{
			if(!*localid.string)
			{
				Con_Printf("===========================\n");
				Con_Printf("Command packet received from local host, but no "
				           "localid has been set.  You may need to upgrade your server "
				           "browser.\n");
				Con_Printf("===========================\n");
				return;
			};

			Con_Printf("===========================\n");
			Con_Printf("Invalid localid on command packet received from local host. "
			           "\n|%s| != |%s|\n"
			           "You may need to reload your server browser and QuakeWorld.\n",
			           s, localid.string);
			Con_Printf("===========================\n");
			Cvar_Set("localid", "");
			return;
		};

		Cbuf_AddText(cmdtext);
		allowremotecmd = false;
		return;
	};

	// print command from somewhere
	if(c == A2C_PRINT)
	{
		Con_Printf("print\n");

		s = MSG_ReadString();
		Con_Print(s);
		return;
	};

	// ping from somewhere
	if(c == A2A_PING)
	{
		char data[6];

		Con_Printf("ping\n");

		data[0] = 0xff;
		data[1] = 0xff;
		data[2] = 0xff;
		data[3] = 0xff;
		data[4] = A2A_ACK;
		data[5] = 0;

		NET_SendPacket(NS_CLIENT, 6, &data, net_from);
		return;
	};

	if(c == S2C_CHALLENGE)
	{
		Con_Printf("challenge\n");

		s = MSG_ReadString();
		cls.challenge = atoi(s);
		CL_SendConnectPacket();
		return;
	};

#if 0
	if (c == svc_disconnect)
	{
		Con_Printf ("disconnect\n");

		Host_EndGame ("Server disconnected");
		return;
	};
#endif

	//char *response_buffer;
	//int response_buffer_size;
	if(ClientDLL_ConnectionlessPacket(&net_from, s, NULL, NULL)) // TODO
		return;

	Con_Printf("unknown:  %c\n", c);
};

/*
===============
CL_ReadFromServer

Read all incoming data from the server
===============
*/
/*
void CL_ReadFromServer()
{
	int ret;

	cl.oldtime = cl.time;
	cl.time += host_frametime;

	do
	{
		ret = CL_GetMessage();
		if(ret == -1)
			Host_Error("CL_ReadFromServer: lost server connection");
		if(!ret)
			break;

		cl.last_received_message = realtime;
		CL_ParseServerMessage();
	} while(ret && cls.state == ca_connected);

	if(cl_shownet.value)
		Con_Printf("\n");

	CL_RelinkEntities();
	CL_UpdateTEnts();

	//
	// bring the links up to date
	//
}
*/

/*
=================
CL_ReadPackets
=================
*/
void CL_ReadPackets ()
{
//	while (NET_GetPacket ())
	while (CL_GetMessage())
	{
		//
		// remote command packet
		//
		if (*(int *)net_message.data == -1)
		{
			CL_ConnectionlessPacket ();
			continue;
		};

		if (net_message.cursize < 8)
		{
			Con_Printf ("%s: Runt packet\n",NET_AdrToString(net_from));
			continue;
		};

		//
		// packet from server
		//
		if (!cls.demoplayback && !NET_CompareAdr (net_from, cls.netchan.remote_address))
		{
			Con_DPrintf ("%s:sequenced packet without connection\n", NET_AdrToString(net_from));
			continue;
		};
		
		if (!Netchan_Process(&cls.netchan))
			continue; // wasn't accepted for some reason
		
		CL_ParseServerMessage ();

//		if (cls.demoplayback && cls.state >= ca_active && !CL_DemoBehind())
//			return;
	};

	//
	// check timeout
	//
	if (cls.state >= ca_connected && realtime - cls.netchan.last_received > cl_timeout.value)
	{
		Con_Printf ("\nServer connection timed out.\n");
		CL_Disconnect ();
		return;
	};	
};

/*
=================
CL_SendCmd
=================
*/
void CL_SendCmd()
{
	int i;
	usercmd_t *cmd;
	int seq_hash;

	//if (cls.state != ca_connected) // NetQuake
		//return;

	// save this command off for prediction
	i = cls.netchan.outgoing_sequence & UPDATE_MASK;
	cmd = &cl.frames[i].cmd;
	cl.frames[i].senttime = realtime;
	cl.frames[i].receivedtime = -1; // we haven't gotten a reply yet

	//	seq_hash = (cls.netchan.outgoing_sequence & 0xffff) ; // ^ QW_CHECK_HASH;
	seq_hash = cls.netchan.outgoing_sequence;

	if (cls.signon == SIGNONS)
	{
		// get basic movement from keyboard
		ClientDLL_CreateMove(host_frametime, cmd, true); // TODO: passing true here for now; should actually check for "active" (cl.active?)		

		// send the unreliable message
		CL_SendMove(cmd);
	};

	// TODO: these lines below are from NetQuake (upgraded to look like QW) should probably be in CL_SendMove
	
	if(cls.demoplayback)
	{
		//SZ_Clear (&cls.netchan.message); // NetQuake
		return; // sendcmds come from the demo
	};
	
	// send the reliable message
	if (!cls.netchan.message.cursize)
		return; // no message at all

	if (!Netchan_CanPacket (&cls.netchan)) // TODO: was NET_CanSendMessage; Netchan_CanReliable?
	{
		//Con_DPrintf ("CL_WriteToServer: can't send\n");
		return;
	}

	//if (NET_SendMessage (cls.netchan, &cls.netchan.message) == -1)
		//Host_Error ("CL_WriteToServer: lost server connection");

	//SZ_Clear (&cls.netchan.message);
	
	//
	// deliver the message
	//
	Netchan_Transmit(&cls.netchan, cls.netchan.message.cursize, cls.netchan.message.data);
};

/*
=================
CL_Init
=================
*/
void CL_Init()
{
	//cls.state = ca_disconnected;

	SZ_Alloc(&cls.netchan.message, 1024);

	Info_SetValueForKey(cls.userinfo, "name", "unnamed", MAX_INFO_STRING);
	Info_SetValueForKey(cls.userinfo, "model", "", MAX_INFO_STRING);
	Info_SetValueForKey(cls.userinfo, "topcolor", "0", MAX_INFO_STRING);
	Info_SetValueForKey(cls.userinfo, "bottomcolor", "0", MAX_INFO_STRING);
	Info_SetValueForKey(cls.userinfo, "rate", "2500", MAX_INFO_STRING);
	//Info_SetValueForKey (cls.userinfo, "msg", "1", MAX_INFO_STRING);

	//sprintf (st, "%4.2f-%04d", VERSION, build_number());
	//Info_SetValueForStarKey (cls.userinfo, "*ver", st, MAX_INFO_STRING);

	//CL_InitInput(); // TODO: moved to client dll
	CL_InitTEnts();
	CL_InitPrediction();
	CL_InitCam();
	Pmove_Init(&clpmove);

	//
	// register our commands
	//
	Cvar_RegisterVariable(&cl_name);

	//Cvar_RegisterVariable (&cl_warncmd);

	Cvar_RegisterVariable(&cl_shownet);

	//Cvar_RegisterVariable (&cl_sbar);
	//Cvar_RegisterVariable (&cl_hudswap);
	//Cvar_RegisterVariable (&cl_maxfps);
	Cvar_RegisterVariable(&cl_timeout);

	//Cvar_RegisterVariable (&rcon_password);
	//Cvar_RegisterVariable (&rcon_address);

	Cvar_RegisterVariable(&cl_nolerp);

	Cvar_RegisterVariable(&entlatency);
	Cvar_RegisterVariable(&cl_predict_players2);
	Cvar_RegisterVariable(&cl_predict_players);
	Cvar_RegisterVariable(&cl_solid_players);

	Cvar_RegisterVariable(&localid);

	//
	// info mirrors
	//
	Cvar_RegisterVariable(&name);
	Cvar_RegisterVariable(&password);
	Cvar_RegisterVariable(&spectator);
	Cvar_RegisterVariable(&skin);
	Cvar_RegisterVariable(&team);
	Cvar_RegisterVariable(&topcolor);
	Cvar_RegisterVariable(&bottomcolor);
	Cvar_RegisterVariable(&rate);

	Cmd_AddCommand("entities", CL_PrintEntities_f);

	Cmd_AddCommand("connect", CL_Connect_f);
	Cmd_AddCommand("disconnect", CL_Disconnect_f);

	Cmd_AddCommand("record", CL_Record_f);
	//Cmd_AddCommand ("rerecord", CL_ReRecord_f);
	Cmd_AddCommand("stop", CL_Stop_f);

	Cmd_AddCommand("playdemo", CL_PlayDemo_f);
	Cmd_AddCommand("timedemo", CL_TimeDemo_f);
};