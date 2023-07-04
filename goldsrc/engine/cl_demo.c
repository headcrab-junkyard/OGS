/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021, 2023 BlackPhrase
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

void CL_FinishTimeDemo();

/*
==============================================================================

DEMO CODE

When a demo is playing back, all NET_SendMessages are skipped, and
NET_GetMessages are read from the demo file.

Whenever cl.time gets past the last received message, another message is
read from the demo file.
==============================================================================
*/

/*
==============
CL_StopPlayback

Called when a demo file runs out, or the user starts a game
==============
*/
void CL_StopPlayback()
{
	if(!cls.demoplayback)
		return;

	fclose(cls.demofile);
	cls.demoplayback = false;
	cls.demofile = NULL;
	cls.state = ca_disconnected;

	if(cls.timedemo)
		CL_FinishTimeDemo();
};

// TODO: qw
/*
#define dem_cmd		0
#define dem_read	1
#define dem_set		2
*/

// TODO: qw
/*
====================
CL_WriteDemoCmd

Writes the current user cmd
====================
*/
/*
void CL_WriteDemoCmd (usercmd_t *pcmd)
{
	int		i;
	float	fl;
	byte	c;
	usercmd_t cmd;

//Con_Printf("write: %ld bytes, %4.4f\n", msg->cursize, realtime);

	fl = LittleFloat((float)realtime);
	fwrite (&fl, sizeof(fl), 1, cls.demofile);

	c = dem_cmd;
	fwrite (&c, sizeof(c), 1, cls.demofile);

	// correct for byte order, bytes don't matter
	cmd = *pcmd;

	for (i = 0; i < 3; i++)
		cmd.angles[i] = LittleFloat(cmd.angles[i]);
	cmd.forwardmove = LittleShort(cmd.forwardmove);
	cmd.sidemove    = LittleShort(cmd.sidemove);
	cmd.upmove      = LittleShort(cmd.upmove);

	fwrite(&cmd, sizeof(cmd), 1, cls.demofile);

	for (i=0 ; i<3 ; i++)
	{
		fl = LittleFloat (cl.viewangles[i]);
		fwrite (&fl, 4, 1, cls.demofile);
	}

	fflush (cls.demofile);
}
*/

/*
====================
CL_WriteDemoMessage

Dumps the current net message, prefixed by the length and view angles
====================
*/
void CL_WriteDemoMessage(/*sizebuf_t *msg*/)
{
	int len;
	
	//Con_Printf("write: %ld bytes, %4.4f\n", msg->cursize, realtime);
	
	// TODO: qw
	//if(!cls.demorecording)
		//return;
	
	// TODO: qw
	/*
	float fl = LittleFloat((float)realtime);
	fwrite (&fl, sizeof(fl), 1, cls.demofile);

	byte c = dem_read;
	fwrite (&c, sizeof(c), 1, cls.demofile);

	len = LittleLong (msg->cursize);
	fwrite (&len, 4, 1, cls.demofile);
	fwrite (msg->data, msg->cursize, 1, cls.demofile);
	*/
	//
	len = LittleLong(net_message.cursize);
	fwrite(&len, 4, 1, cls.demofile);
	
	float f;
	for(int i = 0; i < 3; i++)
	{
		f = LittleFloat(cl.viewangles[i]);
		fwrite(&f, 4, 1, cls.demofile);
	};

	fwrite(net_message.data, net_message.cursize, 1, cls.demofile);
	//
	
	fflush(cls.demofile);
};

/*
====================
CL_GetDemoMessage

  FIXME...
====================
*/
qboolean CL_GetDemoMessage()
{
	int r, i;
	float f;
	// TODO: qw
	/*
	float	demotime;
	byte	c;
	usercmd_t *pcmd;
	*/

// TODO: qw
	/*
	int j;
	
	// read the time from the packet
	fread(&demotime, sizeof(demotime), 1, cls.demofile);
	demotime = LittleFloat(demotime);

// decide if it is time to grab the next message		
	if (cls.timedemo)
	{
		if (cls.td_lastframe < 0)
			cls.td_lastframe = demotime;
		else if (demotime > cls.td_lastframe)
		{
			cls.td_lastframe = demotime;
			// rewind back to time
			fseek(cls.demofile, ftell(cls.demofile) - sizeof(demotime),
					SEEK_SET);
			return 0;		// allready read this frame's message
		}
		if (!cls.td_starttime && cls.state == ca_active)
		{
			cls.td_starttime = Sys_DoubleTime();
			cls.td_startframe = host_framecount;
		}
		realtime = demotime; // warp
	}
	else if (!cl.paused && cls.state >= ca_onserver)
	{
		// always grab until fully connected
		if (realtime + 1.0 < demotime)
		{
			// too far back
			realtime = demotime - 1.0;
			// rewind back to time
			fseek(cls.demofile, ftell(cls.demofile) - sizeof(demotime),
					SEEK_SET);
			return 0;
		}
		else if (realtime < demotime)
		{
			// rewind back to time
			fseek(cls.demofile, ftell(cls.demofile) - sizeof(demotime),
					SEEK_SET);
			return 0;		// don't need another message yet
		}
	}
	else
		realtime = demotime; // we're warping

	if (cls.state < ca_demostart)
		Host_Error ("CL_GetDemoMessage: cls.state != ca_active");
	
	// get the msg type
	fread (&c, sizeof(c), 1, cls.demofile);
	
	switch (c)
	{
	case dem_cmd :
		// user sent input
		i = cls.netchan.outgoing_sequence & UPDATE_MASK;
		pcmd = &cl.frames[i].cmd;
		r = fread (pcmd, sizeof(*pcmd), 1, cls.demofile);
		if (r != 1)
		{
			CL_StopPlayback ();
			return 0;
		}
		// byte order stuff
		for (j = 0; j < 3; j++)
			pcmd->angles[j] = LittleFloat(pcmd->angles[j]);
		pcmd->forwardmove = LittleShort(pcmd->forwardmove);
		pcmd->sidemove    = LittleShort(pcmd->sidemove);
		pcmd->upmove      = LittleShort(pcmd->upmove);
		cl.frames[i].senttime = demotime;
		cl.frames[i].receivedtime = -1;		// we haven't gotten a reply yet
		cls.netchan.outgoing_sequence++;
		for (i=0 ; i<3 ; i++)
		{
			r = fread (&f, 4, 1, cls.demofile);
			cl.viewangles[i] = LittleFloat (f);
		}
		break;

	case dem_read:
		// get the next message
		fread (&net_message.cursize, 4, 1, cls.demofile);
		net_message.cursize = LittleLong (net_message.cursize);
	//Con_Printf("read: %ld bytes\n", net_message.cursize);
		if (net_message.cursize > MAX_MSGLEN)
			Sys_Error ("Demo message > MAX_MSGLEN");
		r = fread (net_message.data, net_message.cursize, 1, cls.demofile);
		if (r != 1)
		{
			CL_StopPlayback ();
			return 0;
		}
		break;

	case dem_set :
		fread (&i, 4, 1, cls.demofile);
		cls.netchan.outgoing_sequence = LittleLong(i);
		fread (&i, 4, 1, cls.demofile);
		cls.netchan.incoming_sequence = LittleLong(i);
		break;

	default :
		Con_Printf("Corrupted demo.\n");
		CL_StopPlayback ();
		return 0;
	}
	*/
//	
	// decide if it is time to grab the next message
	if(cls.signon == SIGNONS) // allways grab until fully connected
	{
		if(cls.timedemo)
		{
			if(host_framecount == cls.td_lastframe)
				return 0; // allready read this frame's message
			cls.td_lastframe = host_framecount;
			// if this is the second frame, grab the real td_starttime
			// so the bogus time on the first frame doesn't count
			if(host_framecount == cls.td_startframe + 1)
				cls.td_starttime = realtime;
		}
		else if(/* cl.time > 0 && */ cl.time <= cl.mtime[0])
			return 0; // don't need another message yet
	};

	// get the next message
	fread(&net_message.cursize, 4, 1, cls.demofile);
	VectorCopy(cl.mviewangles[0], cl.mviewangles[1]);
	
	for(i = 0; i < 3; i++)
	{
		r = fread(&f, 4, 1, cls.demofile);
		cl.mviewangles[0][i] = LittleFloat(f);
	};

	net_message.cursize = LittleLong(net_message.cursize);
	
	if(net_message.cursize > MAX_MSGLEN)
		Sys_Error("Demo message > MAX_MSGLEN");
	
	r = fread(net_message.data, net_message.cursize, 1, cls.demofile);
	
	if(r != 1)
	{
		CL_StopPlayback();
		return false;
	};
//
	
	return true;
};

/*
====================
CL_GetMessage

Handles recording and playback of demos, on top of NET_ code
====================
*/
qboolean CL_GetMessage()
{
	if(cls.demoplayback)
		return CL_GetDemoMessage();
	
	int r;
	
	// TODO: BYE NETQUAKE!
	//while(1) // TODO
	{
		/*r =*/ if(!NET_GetPacket(NS_CLIENT, &net_from, &net_message)) // TODO: was NET_GetMessage
			return false;

		//if(r != 1 && r != 2)
			//return r;

		// discard nop keepalive message
		if(net_message.cursize == 1 && net_message.data[0] == svc_nop) // TODO: non-qw
			Con_Printf("<-- server to client keepalive\n"); // TODO: non-qw
		//else
			//break;
	};

	if(cls.demorecording) // TODO: non-qw
		CL_WriteDemoMessage(/*&net_message*/);

	return true;
};

/*
====================
CL_Stop_f

stop recording a demo
====================
*/
void CL_Stop_f()
{
	if(cmd_source != src_command) // TODO: non-qw
		return; // TODO: non-qw

	if(!cls.demorecording)
	{
		Con_Printf("Not recording a demo.\n");
		return;
	};

	// write a disconnect message to the demo file
	SZ_Clear(&net_message);
	//
	MSG_WriteByte(&net_message, svc_disconnect);
	// TODO: qw
	/*
	MSG_WriteLong(&net_message, -1); // -1 sequence means out of band
	MSG_WriteByte(&net_message, svc_disconnect);
	MSG_WriteString(&net_message, "EndOfDemo");
	*/
	CL_WriteDemoMessage(/*&net_message*/);

	// finish up
	fclose(cls.demofile);

	cls.demofile = NULL;
	cls.demorecording = false;

	Con_Printf("Completed demo\n");
};

// TODO: qw
/*
====================
CL_WriteDemoMessage

Dumps the current net message, prefixed by the length and view angles
====================
*/
/*
void CL_WriteRecordDemoMessage (sizebuf_t *msg, int seq)
{
	int		len;
	int		i;
	float	fl;
	byte	c;

//Con_Printf("write: %ld bytes, %4.4f\n", msg->cursize, realtime);

	if (!cls.demorecording)
		return;

	fl = LittleFloat((float)realtime);
	fwrite (&fl, sizeof(fl), 1, cls.demofile);

	c = dem_read;
	fwrite (&c, sizeof(c), 1, cls.demofile);

	len = LittleLong (msg->cursize + 8);
	fwrite (&len, 4, 1, cls.demofile);

	i = LittleLong(seq);
	fwrite (&i, 4, 1, cls.demofile);
	fwrite (&i, 4, 1, cls.demofile);

	fwrite (msg->data, msg->cursize, 1, cls.demofile);

	fflush (cls.demofile);
};

void CL_WriteSetDemoMessage()
{
	int		len;
	float	fl;
	byte	c;

	//Con_Printf("write: %ld bytes, %4.4f\n", msg->cursize, realtime);

	if (!cls.demorecording)
		return;

	fl = LittleFloat((float)realtime);
	fwrite (&fl, sizeof(fl), 1, cls.demofile);

	c = dem_set;
	fwrite (&c, sizeof(c), 1, cls.demofile);

	len = LittleLong(cls.netchan.outgoing_sequence);
	fwrite (&len, 4, 1, cls.demofile);
	len = LittleLong(cls.netchan.incoming_sequence);
	fwrite (&len, 4, 1, cls.demofile);

	fflush (cls.demofile);
};
*/

/*
====================
CL_Record_f

record <demoname> <cd track>
====================
*/
void CL_Record_f()
{
	int c;
	char name[MAX_OSPATH];
	/*
	sizebuf_t	buf;
	char	buf_data[MAX_MSGLEN];
	int n, i, j;
	char *s;
	entity_t *ent;
	entity_state_t *es, blankes;
	player_info_t *player;
	extern	char gamedirfile[];
	int seq = 1;
	*/
	
	if(cmd_source != src_command) // TODO: non-qw
		return; // TODO: non-qw

	c = Cmd_Argc();

	if(c != 2 && c != 3) // TODO
	{
		Con_Printf("record <demoname> <cd>\n");
		return;
	};
	
	if(cls.demorecording)
	{
		Con_Printf("Already recording.\n");
		//CL_Stop_f(); // TODO: qw
		return;
	};
	
	if(cls.demoplayback)
	{
		Con_Printf("Can't record during demo playback.\n");
		return;
	};
	
	if(c != 2 && c != 3)
	{
		Con_Printf("record <demoname> <cd track>\n");
		return;
	};
	
	// TODO: qw, unused
	//if(cls.state != ca_active)
	//{
		//Con_Printf ("You must be connected to record.\n");
		//return;
	//};

	if(strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	};

	if(c == 2 && cls.state == ca_connected)
	{
		Con_Printf("Can not record - already connected to server\nClient demo recording must be started before connecting\n");
		return;
	};
	
	int track = -1;
	
	// write the forced cd track number, or -1
	if(c == 4)
	{
		track = atoi(Cmd_Argv(3));
		Con_Printf("Forcing CD track to %i\n", cls.forcetrack);
	};

	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	//
	// start the map up
	//
	//if(c > 2)
		//Cmd_ExecuteString(va("map %s", Cmd_Argv(2)), src_command);

	//
	// open the demo file
	//
	COM_DefaultExtension(name, ".dem");

	Con_Printf("recording to %s.\n", name);
	cls.demofile = FS_Open(name, "wb");

	if(!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	};

	cls.forcetrack = track; // TODO: non-qw
	FS_FPrintf(cls.demofile, "%i\n", cls.forcetrack); // TODO: non-qw
	
	//Con_Printf("recording to %s.\n", name); // TODO: qw
	cls.demorecording = true;
	
	/*-------------------------------------------------*/
	
	// TODO: qw
	/*
	// serverdata
	// send the info about the new client to all connected clients
	memset(&buf, 0, sizeof(buf));
	buf.data = buf_data;
	buf.maxsize = sizeof(buf_data);

	// send the serverdata
	MSG_WriteByte (&buf, svc_serverdata);
	MSG_WriteLong (&buf, PROTOCOL_VERSION);
	MSG_WriteLong (&buf, cl.servercount);
	MSG_WriteString (&buf, gamedirfile);

	if (cl.spectator)
		MSG_WriteByte (&buf, cl.playernum | 128);
	else
		MSG_WriteByte (&buf, cl.playernum);

	// send full levelname
	MSG_WriteString (&buf, cl.levelname);

	// send the movevars
	MSG_WriteFloat(&buf, movevars.gravity);
	MSG_WriteFloat(&buf, movevars.stopspeed);
	MSG_WriteFloat(&buf, movevars.maxspeed);
	MSG_WriteFloat(&buf, movevars.spectatormaxspeed);
	MSG_WriteFloat(&buf, movevars.accelerate);
	MSG_WriteFloat(&buf, movevars.airaccelerate);
	MSG_WriteFloat(&buf, movevars.wateraccelerate);
	MSG_WriteFloat(&buf, movevars.friction);
	MSG_WriteFloat(&buf, movevars.waterfriction);
	MSG_WriteFloat(&buf, movevars.entgravity);

	// send music
	MSG_WriteByte (&buf, svc_cdtrack);
	MSG_WriteByte (&buf, 0); // none in demos

	// send server info string
	MSG_WriteByte (&buf, svc_stufftext);
	MSG_WriteString (&buf, va("fullserverinfo \"%s\"\n", cl.serverinfo) );

	// flush packet
	CL_WriteRecordDemoMessage (&buf, seq++);
	SZ_Clear (&buf); 

	// soundlist
	MSG_WriteByte (&buf, svc_soundlist);
	MSG_WriteByte (&buf, 0);

	n = 0;
	s = cl.sound_name[n+1];
	while (*s) {
		MSG_WriteString (&buf, s);
		if (buf.cursize > MAX_MSGLEN/2) {
			MSG_WriteByte (&buf, 0);
			MSG_WriteByte (&buf, n);
			CL_WriteRecordDemoMessage (&buf, seq++);
			SZ_Clear (&buf); 
			MSG_WriteByte (&buf, svc_soundlist);
			MSG_WriteByte (&buf, n + 1);
		}
		n++;
		s = cl.sound_name[n+1];
	}
	if (buf.cursize) {
		MSG_WriteByte (&buf, 0);
		MSG_WriteByte (&buf, 0);
		CL_WriteRecordDemoMessage (&buf, seq++);
		SZ_Clear (&buf); 
	}

	// modellist
	MSG_WriteByte (&buf, svc_modellist);
	MSG_WriteByte (&buf, 0);

	n = 0;
	s = cl.model_name[n+1];
	while (*s) {
		MSG_WriteString (&buf, s);
		if (buf.cursize > MAX_MSGLEN/2) {
			MSG_WriteByte (&buf, 0);
			MSG_WriteByte (&buf, n);
			CL_WriteRecordDemoMessage (&buf, seq++);
			SZ_Clear (&buf); 
			MSG_WriteByte (&buf, svc_modellist);
			MSG_WriteByte (&buf, n + 1);
		}
		n++;
		s = cl.model_name[n+1];
	}
	if (buf.cursize) {
		MSG_WriteByte (&buf, 0);
		MSG_WriteByte (&buf, 0);
		CL_WriteRecordDemoMessage (&buf, seq++);
		SZ_Clear (&buf); 
	}

	// spawnstatic

	for (i = 0; i < cl.num_statics; i++) {
		ent = cl_static_entities + i;

		MSG_WriteByte (&buf, svc_spawnstatic);

		for (j = 1; j < MAX_MODELS; j++)
			if (ent->model == cl.model_precache[j])
				break;
		if (j == MAX_MODELS)
			MSG_WriteByte (&buf, 0);
		else
			MSG_WriteByte (&buf, j);

		MSG_WriteByte (&buf, ent->frame);
		MSG_WriteByte (&buf, 0);
		MSG_WriteByte (&buf, ent->skinnum);
		for (j=0 ; j<3 ; j++)
		{
			MSG_WriteCoord (&buf, ent->origin[j]);
			MSG_WriteAngle (&buf, ent->angles[j]);
		}

		if (buf.cursize > MAX_MSGLEN/2) {
			CL_WriteRecordDemoMessage (&buf, seq++);
			SZ_Clear (&buf); 
		}
	}

	// spawnstaticsound
	// static sounds are skipped in demos, life is hard

	// baselines

	memset(&blankes, 0, sizeof(blankes));
	for (i = 0; i < MAX_EDICTS; i++) {
		es = cl_baselines + i;

		if (memcmp(es, &blankes, sizeof(blankes))) {
			MSG_WriteByte (&buf,svc_spawnbaseline);		
			MSG_WriteShort (&buf, i);

			MSG_WriteByte (&buf, es->modelindex);
			MSG_WriteByte (&buf, es->frame);
			MSG_WriteByte (&buf, es->colormap);
			MSG_WriteByte (&buf, es->skinnum);
			for (j=0 ; j<3 ; j++)
			{
				MSG_WriteCoord(&buf, es->origin[j]);
				MSG_WriteAngle(&buf, es->angles[j]);
			}

			if (buf.cursize > MAX_MSGLEN/2) {
				CL_WriteRecordDemoMessage (&buf, seq++);
				SZ_Clear (&buf); 
			}
		}
	}

	MSG_WriteByte (&buf, svc_stufftext);
	MSG_WriteString (&buf, va("cmd spawn %i 0\n", cl.servercount) );

	if (buf.cursize) {
		CL_WriteRecordDemoMessage (&buf, seq++);
		SZ_Clear (&buf); 
	}

	// send current status of all other players

	for (i = 0; i < MAX_CLIENTS; i++) {
		player = cl.players + i;

		MSG_WriteByte (&buf, svc_updatefrags);
		MSG_WriteByte (&buf, i);
		MSG_WriteShort (&buf, player->frags);
		
		MSG_WriteByte (&buf, svc_updateping);
		MSG_WriteByte (&buf, i);
		MSG_WriteShort (&buf, player->ping);
		
		MSG_WriteByte (&buf, svc_updatepl);
		MSG_WriteByte (&buf, i);
		MSG_WriteByte (&buf, player->pl);
		
		MSG_WriteByte (&buf, svc_updateentertime);
		MSG_WriteByte (&buf, i);
		MSG_WriteFloat (&buf, player->entertime);

		MSG_WriteByte (&buf, svc_updateuserinfo);
		MSG_WriteByte (&buf, i);
		MSG_WriteLong (&buf, player->userid);
		MSG_WriteString (&buf, player->userinfo);

		if (buf.cursize > MAX_MSGLEN/2) {
			CL_WriteRecordDemoMessage (&buf, seq++);
			SZ_Clear (&buf); 
		}
	}
	
	// send all current light styles
	for (i=0 ; i<MAX_LIGHTSTYLES ; i++)
	{
		MSG_WriteByte (&buf, svc_lightstyle);
		MSG_WriteByte (&buf, (char)i);
		MSG_WriteString (&buf, cl_lightstyle[i].map);
	}

	for (i = 0; i < MAX_CL_STATS; i++) {
		MSG_WriteByte (&buf, svc_updatestatlong);
		MSG_WriteByte (&buf, i);
		MSG_WriteLong (&buf, cl.stats[i]);
		if (buf.cursize > MAX_MSGLEN/2) {
			CL_WriteRecordDemoMessage (&buf, seq++);
			SZ_Clear (&buf); 
		}
	}

#if 0
	MSG_WriteByte (&buf, svc_updatestatlong);
	MSG_WriteByte (&buf, STAT_TOTALMONSTERS);
	MSG_WriteLong (&buf, cl.stats[STAT_TOTALMONSTERS]);

	MSG_WriteByte (&buf, svc_updatestatlong);
	MSG_WriteByte (&buf, STAT_SECRETS);
	MSG_WriteLong (&buf, cl.stats[STAT_SECRETS]);

	MSG_WriteByte (&buf, svc_updatestatlong);
	MSG_WriteByte (&buf, STAT_MONSTERS);
	MSG_WriteLong (&buf, cl.stats[STAT_MONSTERS]);
#endif

	// get the client to check and download skins
	// when that is completed, a begin command will be issued
	MSG_WriteByte (&buf, svc_stufftext);
	MSG_WriteString (&buf, va("skins\n") );

	CL_WriteRecordDemoMessage (&buf, seq++);

	CL_WriteSetDemoMessage();

	// done
	*/
};

// TODO: qw, unused
/*
====================
CL_ReRecord_f

record <demoname>
====================
*/
/*
void CL_ReRecord_f ()
{
	int		c;
	char	name[MAX_OSPATH];

	c = Cmd_Argc();
	if (c != 2)
	{
		Con_Printf ("rerecord <demoname>\n");
		return;
	}

	if (!*cls.servername) {
		Con_Printf("No server to reconnect to...\n");
		return;
	}

	if (cls.demorecording)
		CL_Stop_f();
  
	sprintf (name, "%s/%s", com_gamedir, Cmd_Argv(1));

//
// open the demo file
//
	COM_DefaultExtension (name, ".dem");

	cls.demofile = fopen (name, "wb");
	if (!cls.demofile)
	{
		Con_Printf ("ERROR: couldn't open.\n");
		return;
	}

	Con_Printf ("recording to %s.\n", name);
	cls.demorecording = true;

	CL_Disconnect();
	CL_BeginServerConnect();
}
*/

/*
====================
CL_PlayDemo_f

play [demoname]
====================
*/
void CL_PlayDemo_f()
{
	char name[256];

	if(cmd_source != src_command) // TODO: non-qw
		return; // TODO: non-qw

	if(Cmd_Argc() != 2)
	{
		Con_Printf("play <demoname> : plays a demo\n");
		return;
	};

	//
	// disconnect from server
	//
	CL_Disconnect();

	//
	// open the demo file
	//
	strcpy(name, Cmd_Argv(1));
	COM_DefaultExtension(name, ".dem");
	
	Con_Printf("Playing demo from %s.\n", name);
	cls.demofile = FS_Open(name, "rb"); // TODO: COM_FOpenFile(name, &cls.demofile); in qw
	
	if(!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open.\n");
		cls.demonum = -1; // stop demo loop
		return;
	};

	cls.demoplayback = true;
	cls.state = ca_connected; // TODO: ca_demostart in qw
	cls.forcetrack = 0; // TODO: non-qw
	
	// TODO: qw
	//Netchan_Setup(&cls.netchan, net_from, 0);
	//realtime = 0;
	//
	// TODO: non-qw
	//
	int c;
	qboolean neg = false;
	
	while((c = fgetc((FILE*)cls.demofile)) != '\n') // TODO: was getc
		if(c == '-')
			neg = true;
		else
			cls.forcetrack = cls.forcetrack * 10 + (c - '0');

	if(neg)
		cls.forcetrack = -cls.forcetrack;
	
	// ZOID, fscanf is evil
	//fscanf(cls.demofile, "%i\n", &cls.forcetrack);
	//
};

/*
====================
CL_ViewDemo_f
====================
*/
void CL_ViewDemo_f()
{
	// TODO
};

/*
====================
CL_FinishTimeDemo
====================
*/
void CL_FinishTimeDemo()
{
	int frames;
	float time;

	cls.timedemo = false;

	// the first frame didn't count
	frames = (host_framecount - cls.td_startframe) - 1;
	time = realtime - cls.td_starttime; // TODO: realtime -> Sys_FloatTime in qw
	if(!time)
		time = 1;
	Con_Printf("%i frames %5.1f seconds %5.1f fps\n", frames, time, frames / time);
};

/*
====================
CL_TimeDemo_f

timedemo [demoname]
====================
*/
void CL_TimeDemo_f()
{
	if(cmd_source != src_command) // TODO: non-qw
		return; // TODO: non-qw

	if(Cmd_Argc() != 2)
	{
		Con_Printf("timedemo <demoname> : gets demo speeds\n");
		return;
	};

	CL_PlayDemo_f();
	
	// TODO: qw
	//if(cls.state != ca_demostart)
		//return;

	// cls.td_starttime will be grabbed at the second frame of the demo, so
	// all the loading time doesn't get counted

	cls.timedemo = true;
	//cls.td_starttime = 0; // TODO: qw
	cls.td_startframe = host_framecount;
	cls.td_lastframe = -1; // get a new message this frame
};

/*
====================
CL_ListDemo_f
====================
*/
void CL_ListDemo_f()
{
	// TODO
};

/*
====================
CL_AppendDemo_f
====================
*/
void CL_AppendDemo_f()
{
	// TODO
};

/*
====================
CL_RemoveDemo_f
====================
*/
void CL_RemoveDemo_f()
{
	// TODO
};

/*
====================
CL_SwapDemo_f
====================
*/
void CL_SwapDemo_f()
{
	// TODO
};

/*
====================
CL_SetDemoInfo_f
====================
*/
void CL_SetDemoInfo_f()
{
	// TODO
};