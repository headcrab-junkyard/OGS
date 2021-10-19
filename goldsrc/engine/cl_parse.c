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
/// @brief parse a message received from the server

#include "quakedef.h"

const char *svc_strings[] =
{
  "svc_bad",
  "svc_nop",
  "svc_disconnect",
  "svc_event",
  "svc_version",   // [long] server version
  "svc_setview",   // [short] entity number
  "svc_sound",     // <see code>
  "svc_time",      // [float] server time
  "svc_print",     // [string] null terminated string
  "svc_stufftext", // [string] stuffed into client's console buffer
	               // the string should be \n terminated
  "svc_setangle",  // [vec3] set the view angle to this absolute value

  "svc_serverinfo",   // [long] version
	                  // [string] signon string
	                  // [string]..[0]model cache [string]...[0]sounds cache
	                  // [string]..[0]item cache
  "svc_lightstyle",   // [byte] [string]
  "svc_updateuserinfo",   // [byte] [string]
  "svc_deltadescription",
  "svc_clientdata",   // <shortbits + data>
  "svc_stopsound",    // <see code>
  "svc_pings",
  "svc_particle",     // [vec3] <variable>
  "svc_damage",       // [byte] impact [byte] blood [vec3] from

  "svc_spawnstatic",
  "svc_event_reliable",
  "svc_spawnbaseline",

  "svc_tempentity", // <variable>
  "svc_setpause",
  "svc_signonnum",
  "svc_centerprint",
  "svc_killedmonster",
  "svc_foundsecret",
  "svc_spawnstaticsound",
  "svc_intermission",
  "svc_finale",  // [string] music [string] text
  "svc_cdtrack", // [byte] track [byte] looptrack
  "svc_restore",
  "svc_cutscene",
  "svc_weaponanim",
  "svc_decalname",
  "svc_roomtype",
  "svc_addangle",
  "svc_newusermsg",
  "svc_packetentities",
  "svc_deltapacketentities",
  "svc_choke",
  "svc_resourcelist",
  "svc_newmovevars",
  "svc_resourcerequest",
  "svc_customization",
  "svc_crosshairangle",
  "svc_soundfade",
  "svc_filetxferfailed",
  "svc_hltv",
  "svc_director",
  "svc_voiceinit",
  "svc_voicedata",
  "svc_sendextrainfo",
  "svc_timescale",
  "svc_resourcelocation",
  "svc_sendcvarvalue",
  "svc_sendcvarvalue2",
  
  //NULL
};

//=============================================================================

/*
===============
CL_EntityNum

This error checks and tracks the total number of entities
===============
*/
cl_entity_t *CL_EntityNum(int num)
{
	if(num >= cl.num_entities)
	{
		if(num >= MAX_EDICTS)
			Host_Error("CL_EntityNum: %i is an invalid number", num);
		while(cl.num_entities <= num)
		{
			cl_entities[cl.num_entities].colormap = vid.colormap;
			cl.num_entities++;
		}
	}

	return &cl_entities[num];
}

/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseStartSoundPacket()
{
	vec3_t pos;
	int channel, ent;
	int sound_num;
	int volume;
	int field_mask;
	float attenuation;
	int pitch;
	int i;

	field_mask = MSG_ReadByte();

	if(field_mask & SND_VOLUME)
		volume = MSG_ReadByte();
	else
		volume = DEFAULT_SOUND_PACKET_VOLUME;

	if(field_mask & SND_ATTENUATION)
		attenuation = MSG_ReadByte() / 64.0;
	else
		attenuation = DEFAULT_SOUND_PACKET_ATTENUATION;
	
	if(field_mask & SND_PITCH)
		pitch = MSG_ReadByte() / 64.0; // 255?
	else
		pitch = DEFAULT_SOUND_PACKET_PITCH;

	channel = MSG_ReadShort();
	sound_num = MSG_ReadByte();

	ent = channel >> 3;
	channel &= 7;

	if(ent > MAX_EDICTS)
		Host_Error("CL_ParseStartSoundPacket: ent = %i", ent);

	for(i = 0; i < 3; i++)
		pos[i] = MSG_ReadCoord();

	S_StartDynamicSound(ent, channel, cl.sound_precache[sound_num], pos, volume / 255.0, attenuation, pitch);
}

/*
==================
CL_KeepaliveMessage

When the client is taking a long time to load stuff, send keepalive messages
so the server doesn't disconnect.
==================
*/
// TODO: unused?
/*
void CL_KeepaliveMessage ()
{
	float	time;
	static float lastmsg;
	int		ret;
	sizebuf_t	old;
	byte		olddata[8192];
	
	if (sv.active)
		return;		// no need if server is local
	if (cls.demoplayback)
		return;

// read messages from server, should just be nops
	old = net_message;
	memcpy (olddata, net_message.data, net_message.cursize);
	
	do
	{
		ret = CL_GetMessage ();
		switch (ret)
		{
		default:
			Host_Error ("CL_KeepaliveMessage: CL_GetMessage failed");		
		case 0:
			break;	// nothing waiting
		case 1:
			Host_Error ("CL_KeepaliveMessage: received a message");
			break;
		case 2:
			if (MSG_ReadByte() != svc_nop)
				Host_Error ("CL_KeepaliveMessage: datagram wasn't a nop");
			break;
		}
	} while (ret);

	net_message = old;
	memcpy (net_message.data, olddata, net_message.cursize);

// check time
	time = Sys_FloatTime ();
	if (time - lastmsg < 5)
		return;
	lastmsg = time;

// write out a nop
	Con_Printf ("--> client to server keepalive\n");

	MSG_WriteByte (&cls.netchan.message, clc_nop);
	NET_SendMessage (cls.netchan, &cls.netchan.message);
	SZ_Clear (&cls.netchan.message);
}
*/

/*
==================
CL_ParseServerInfo
==================
*/
void CL_ParseServerInfo()
{
	char *str;
	int i;
	int nummodels, numsounds;
	char model_precache[MAX_MODELS][MAX_QPATH];
	char sound_precache[MAX_SOUNDS][MAX_QPATH];

	Con_DPrintf("Serverinfo packet received.\n");
	//
	// wipe the client_state_t struct
	//
	CL_ClearState();

	// parse protocol version number
	i = MSG_ReadLong();
	if(i != PROTOCOL_VERSION)
	{
		Con_Printf("Server returned version %i, not %i", i, PROTOCOL_VERSION);
		return;
	}
	
	// parse server spawn count
	cl.servercount = MSG_ReadLong();
	
	// parse map crc
	cl.mapcrc = MSG_ReadLong();
	
	// parse client dll hash
	for(int j = 0; j < 16; j++)
		cl.clientdllhash[j] = MSG_ReadByte();
	
	// parse maxclients
	cl.maxclients = MSG_ReadByte();
	if(cl.maxclients < 1 || cl.maxclients > MAX_SCOREBOARD)
	{
		Con_Printf("Bad maxclients (%u) from server\n", cl.maxclients);
		return;
	}
	cl.scores = Hunk_AllocName(cl.maxclients * sizeof(*cl.scores), "scores");

	// parse player index
	cl.playernum = MSG_ReadByte();
	
	// parse gametype
	cl.gametype = MSG_ReadByte(); // TODO: is deathmatch

	// parse game directory
	str = MSG_ReadString();
	if(Q_strcmp(str, com_gamedir))
	{
		Con_Printf("Invalid game directory: %s vs %s on server!\n", com_gamedir, str);
		return;
	};
	
	// parse host name
	str = MSG_ReadString();
	strncpy(cl.hostname, str, sizeof(cl.hostname) - 1);
	Con_DPrintf("Remote host: %s\n", cl.hostname);
	
	// parse level name
	str = MSG_ReadString();
	strncpy(cl.levelname, str, sizeof(cl.levelname) - 1);
	
	// parse map cycle
	str = MSG_ReadString();
	strncpy(cl.mapcycle, str, sizeof(cl.mapcycle) - 1);
	
	MSG_ReadByte();
	
	// TODO: the rest should be handled by svc_resourcelist
	
	// separate the printfs so the server message can have a color
	//Con_Printf("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n");
	//Con_Printf("%c%s\n", 2, str);

	//
	// first we go through and touch all of the precache data that still
	// happens to be in the cache, so precaching something else doesn't
	// needlessly purge it
	//

	// precache world model (temp)
	memset(cl.model_precache, 0, sizeof(cl.model_precache));
	
	strcpy(model_precache[1], va("maps/%s.bsp", cl.levelname));
	Mod_TouchModel(va("maps/%s.bsp", cl.levelname));
	cl.model_precache[1] = Mod_ForName(model_precache[1], false);

/*
	// precache models
	memset(cl.model_precache, 0, sizeof(cl.model_precache));
	for(nummodels = 1;; nummodels++)
	{
		str = MSG_ReadString();
		if(!str[0])
			break;
		if(nummodels == MAX_MODELS)
		{
			Con_Printf("Server sent too many model precaches\n");
			return;
		}
		strcpy(model_precache[nummodels], str);
		Mod_TouchModel(str);
	}

	// precache sounds
	memset(cl.sound_precache, 0, sizeof(cl.sound_precache));
	for(numsounds = 1;; numsounds++)
	{
		str = MSG_ReadString();
		if(!str[0])
			break;
		if(numsounds == MAX_SOUNDS)
		{
			Con_Printf("Server sent too many sound precaches\n");
			return;
		}
		strcpy(sound_precache[numsounds], str);
		S_TouchSound(str);
	}

	//
	// now we try to load everything else until a cache allocation fails
	//

	for(i = 1; i < nummodels; i++)
	{
		cl.model_precache[i] = Mod_ForName(model_precache[i], false);
		if(cl.model_precache[i] == NULL)
		{
			Con_Printf("Model %s not found\n", model_precache[i]);
			return;
		}
		//CL_KeepaliveMessage (); // TODO
	}

	S_BeginPrecaching();
	for(i = 1; i < numsounds; i++)
	{
		cl.sound_precache[i] = S_PrecacheSound(sound_precache[i]);
		//CL_KeepaliveMessage (); // TODO
	}
	S_EndPrecaching();
*/

	// local state
	cl_entities[0].model = cl.worldmodel = cl.model_precache[1];

	R_NewMap();

	Hunk_Check(); // make sure nothing is hurt

	noclip_anglehack = false; // noclip is turned off at start
}

/*
==================
CL_ParseUpdate

Parse an entity update message from the server
If an entities model or origin changes from frame to frame, it must be
relinked.  Other attributes can change without relinking.
==================
*/
int bitcounts[16];

void CL_ParseUpdate(int bits)
{
	int i;
	model_t *model;
	int modnum;
	qboolean forcelink;
	cl_entity_t *ent;
	int num;
	int skin;

	if(cls.signon == SIGNONS - 1)
	{ // first update is the final signon stage
		cls.signon = SIGNONS;
		CL_SignonReply();
	}

	if(bits & U_MOREBITS)
	{
		i = MSG_ReadByte();
		bits |= (i << 8);
	}

	if(bits & U_LONGENTITY)
		num = MSG_ReadShort();
	else
		num = MSG_ReadByte();

	ent = CL_EntityNum(num);

	for(i = 0; i < 16; i++)
		if(bits & (1 << i))
			bitcounts[i]++;

	if(ent->msgtime != cl.mtime[1])
		forcelink = true; // no previous frame to lerp from
	else
		forcelink = false;

	ent->msgtime = cl.mtime[0];

	if(bits & U_MODEL)
	{
		modnum = MSG_ReadByte();
		if(modnum >= MAX_MODELS)
			Host_Error("CL_ParseModel: bad modnum");
	}
	else
		modnum = ent->baseline.modelindex;

	model = cl.model_precache[modnum];
	if(model != ent->model)
	{
		ent->model = model;
		// automatic animation (torches, etc) can be either all together
		// or randomized
		if(model)
		{
			if(model->synctype == ST_RAND)
				ent->syncbase = (float)(rand() & 0x7fff) / 0x7fff;
			else
				ent->syncbase = 0.0;
		}
		else
			forcelink = true; // hack to make null model players work
#ifdef GLQUAKE
		if(num > 0 && num <= cl.maxclients)
			R_TranslatePlayerSkin(num - 1);
#endif
	}

	if(bits & U_FRAME)
		ent->frame = MSG_ReadByte();
	else
		ent->frame = ent->baseline.frame;

	if(bits & U_COLORMAP)
		i = MSG_ReadByte();
	else
		i = ent->baseline.colormap;
	if(!i)
		ent->colormap = vid.colormap;
	else
	{
		if(i > cl.maxclients)
			Sys_Error("i >= cl.maxclients");
		ent->colormap = cl.scores[i - 1].translations;
	}

#ifdef GLQUAKE
	if(bits & U_SKIN)
		skin = MSG_ReadByte();
	else
		skin = ent->baseline.skin;
	if(skin != ent->skinnum)
	{
		ent->skinnum = skin;
		if(num > 0 && num <= cl.maxclients)
			R_TranslatePlayerSkin(num - 1);
	}

#else

	if(bits & U_SKIN)
		ent->skinnum = MSG_ReadByte();
	else
		ent->skinnum = ent->baseline.skin;
#endif

	if(bits & U_EFFECTS)
		ent->effects = MSG_ReadByte();
	else
		ent->effects = ent->baseline.effects;

	// shift the known values for interpolation
	VectorCopy(ent->ph[0].origin, ent->ph[1].origin);
	VectorCopy(ent->ph[0].angles, ent->ph[1].angles);

	if(bits & U_ORIGIN1)
		ent->ph[0].origin[0] = MSG_ReadCoord();
	else
		ent->ph[0].origin[0] = ent->baseline.origin[0];
	if(bits & U_ANGLE1)
		ent->ph[0].angles[0] = MSG_ReadAngle();
	else
		ent->ph[0].angles[0] = ent->baseline.angles[0];

	if(bits & U_ORIGIN2)
		ent->ph[0].origin[1] = MSG_ReadCoord();
	else
		ent->ph[0].origin[1] = ent->baseline.origin[1];
	if(bits & U_ANGLE2)
		ent->ph[0].angles[1] = MSG_ReadAngle();
	else
		ent->ph[0].angles[1] = ent->baseline.angles[1];

	if(bits & U_ORIGIN3)
		ent->ph[0].origin[2] = MSG_ReadCoord();
	else
		ent->ph[0].origin[2] = ent->baseline.origin[2];
	if(bits & U_ANGLE3)
		ent->ph[0].angles[2] = MSG_ReadAngle();
	else
		ent->ph[0].angles[2] = ent->baseline.angles[2];

	if(bits & U_NOLERP)
		ent->forcelink = true;

	if(forcelink)
	{
		// didn't have an update last message
		VectorCopy(ent->ph[0].origin, ent->ph[1].origin);
		VectorCopy(ent->ph[0].origin, ent->origin);
		VectorCopy(ent->ph[0].angles, ent->ph[1].angles);
		VectorCopy(ent->ph[0].angles, ent->angles);
		ent->forcelink = true;
	}
}

/*
==================
CL_ParseBaseline
==================
*/
void CL_ParseBaseline(cl_entity_t *ent)
{
	int i;

	ent->baseline.modelindex = MSG_ReadByte();
	ent->baseline.frame = MSG_ReadByte();
	ent->baseline.colormap = MSG_ReadByte();
	ent->baseline.skin = MSG_ReadByte();
	for(i = 0; i < 3; i++)
	{
		ent->baseline.origin[i] = MSG_ReadCoord();
		ent->baseline.angles[i] = MSG_ReadAngle();
	}
}

/*
==================
CL_ParseClientdata

Server information pertaining to this client only
==================
*/
void CL_ParseClientdata(int bits)
{
	int i, j;

	if(bits & SU_VIEWHEIGHT)
		cl.viewheight = MSG_ReadChar();
	else
		cl.viewheight = DEFAULT_VIEWHEIGHT;

	if(bits & SU_IDEALPITCH)
		cl.idealpitch = MSG_ReadChar();
	else
		cl.idealpitch = 0;

	VectorCopy(cl.mvelocity[0], cl.mvelocity[1]);
	for(i = 0; i < 3; i++)
	{
		if(bits & (SU_PUNCH1 << i))
			cl.punchangle[i] = MSG_ReadChar();
		else
			cl.punchangle[i] = 0;
		if(bits & (SU_VELOCITY1 << i))
			cl.mvelocity[0][i] = MSG_ReadChar() * 16;
		else
			cl.mvelocity[0][i] = 0;
	}

	// [always sent]	if (bits & SU_ITEMS)
	//i = MSG_ReadLong();

/*
	if(cl.items != i)
	{ // set flash times
		Sbar_Changed();
		for(j = 0; j < 32; j++)
			if((i & (1 << j)) && !(cl.items & (1 << j)))
				cl.item_gettime[j] = cl.time;
		cl.items = i;
	}
*/

	cl.onground = (bits & SU_ONGROUND) != 0;
	cl.inwater = (bits & SU_INWATER) != 0;

	//if(bits & SU_WEAPONFRAME)
		//cl.stats[STAT_WEAPONFRAME] = MSG_ReadByte();
	//else
		cl.stats[STAT_WEAPONFRAME] = 0;

	if(bits & SU_ARMOR)
		i = MSG_ReadByte();
	else
		i = 0;
	if(cl.stats[STAT_ARMOR] != i)
	{
		cl.stats[STAT_ARMOR] = i;
		Sbar_Changed();
	}

	if(bits & SU_WEAPON)
		i = MSG_ReadByte();
	else
		i = 0;
	if(cl.stats[STAT_WEAPON] != i)
	{
		cl.stats[STAT_WEAPON] = i;
		Sbar_Changed();
	}

	i = MSG_ReadShort();
	if(cl.stats[STAT_HEALTH] != i)
	{
		cl.stats[STAT_HEALTH] = i;
		Sbar_Changed();
	}

/*
	i = MSG_ReadByte();
	if(cl.stats[STAT_AMMO] != i)
	{
		cl.stats[STAT_AMMO] = i;
		Sbar_Changed();
	}

	for(i = 0; i < 4; i++)
	{
		j = MSG_ReadByte();
		if(cl.stats[STAT_SHELLS + i] != j)
		{
			cl.stats[STAT_SHELLS + i] = j;
			Sbar_Changed();
		}
	}
*/

	i = MSG_ReadByte();

	if(cl.stats[STAT_ACTIVEWEAPON] != i) // != (1 << i))
	{
		cl.stats[STAT_ACTIVEWEAPON] = i; // = (1 << i);
		Sbar_Changed();
	}
}

/*
=====================
CL_NewTranslation
=====================
*/
void CL_NewTranslation(int slot)
{
	int i, j;
	int top, bottom;
	byte *dest, *source;

	if(slot > cl.maxclients)
		Sys_Error("CL_NewTranslation: slot > cl.maxclients");
	dest = cl.scores[slot].translations;
	source = vid.colormap;
	memcpy(dest, vid.colormap, sizeof(cl.scores[slot].translations));
	top = cl.scores[slot].colors & 0xf0;
	bottom = (cl.scores[slot].colors & 15) << 4;
#ifdef GLQUAKE
	R_TranslatePlayerSkin(slot);
#endif

	for(i = 0; i < VID_GRADES; i++, dest += 256, source += 256)
	{
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
}

/*
=====================
CL_ParseStatic
=====================
*/
void CL_ParseStatic()
{
	cl_entity_t *ent;
	int i;

	i = cl.num_statics;
	if(i >= MAX_STATIC_ENTITIES)
		Host_Error("Too many static entities");
	ent = &cl_static_entities[i];
	cl.num_statics++;
	CL_ParseBaseline(ent);

	// copy it to the current state
	ent->model = cl.model_precache[ent->baseline.modelindex];
	ent->frame = ent->baseline.frame;
	ent->colormap = vid.colormap;
	ent->skinnum = ent->baseline.skin;
	ent->effects = ent->baseline.effects;

	VectorCopy(ent->baseline.origin, ent->origin);
	VectorCopy(ent->baseline.angles, ent->angles);
	R_AddEfrags(ent);
}

/*
===================
CL_ParseStaticSound
===================
*/
void CL_ParseStaticSound()
{
	vec3_t org;
	int sound_num, vol, atten, pitch;
	int i;

	for(i = 0; i < 3; i++)
		org[i] = MSG_ReadCoord();
	sound_num = MSG_ReadByte();
	vol = MSG_ReadByte();
	atten = MSG_ReadByte();
	pitch = MSG_ReadByte();

	S_StartStaticSound(cl.sound_precache[sound_num], org, vol, atten, pitch);
}

void CL_Parse_Version()
{
	int i = MSG_ReadLong();
	if(i != PROTOCOL_VERSION)
		Host_Error("CL_Parse_Version: Server is protocol %i instead of %i\n", i, PROTOCOL_VERSION);
};

/*
===============
CL_CheckOrDownloadFile

Returns true if the file exists, otherwise it attempts
to start a download from the server.
===============
*/
qboolean CL_CheckOrDownloadFile(const char *filename)
{
	if(strstr(filename, ".."))
	{
		Con_Printf ("Refusing to download a path with ..\n");
		return true;
	};

	// it exists, no need to download
	if(FS_FileExists(filename))
		return true;

	//ZOID - can't download when recording
	if (cls.demorecording)
	{
		Con_Printf("Unable to download %s in record mode.\n", cls.downloadname);
		return true;
	};
	
	//ZOID - can't download when playback
	if (cls.demoplayback)
		return true;

	Q_strcpy(cls.downloadname, filename);
	Con_Printf("Downloading %s...\n", cls.downloadname);

	// download to a temp name, and only rename
	// to the real name when done, so if interrupted
	// a runt file wont be left
	COM_StripExtension(cls.downloadname, cls.downloadtempname);
	Q_strcat(cls.downloadtempname, ".tmp");

	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	MSG_WriteString(&cls.netchan.message, va("download %s", cls.downloadname));

	cls.downloadnumber++;

	return false;
};

/*
=================
Sound_NextDownload
=================
*/
void Sound_NextDownload ()
{
	char	*s;
	int		i;

	if (cls.downloadnumber == 0)
	{
		Con_Printf ("Checking sounds...\n");
		cls.downloadnumber = 1;
	};

	cls.downloadtype = t_sound;
	for ( 
		; cl.sound_name[cls.downloadnumber][0]
		; cls.downloadnumber++)
	{
		s = cl.sound_name[cls.downloadnumber];
		if (!CL_CheckOrDownloadFile(va("sound/%s",s)))
			return;		// started a download
	};

	for (i=1 ; i<MAX_SOUNDS ; i++)
	{
		if (!cl.sound_name[i][0])
			break;
		cl.sound_precache[i] = S_PrecacheSound (cl.sound_name[i]);
	};

	// done with sounds, request models now
	memset (cl.model_precache, 0, sizeof(cl.model_precache));
	//cl_playerindex = -1; // TODO
	//cl_spikeindex = -1; // TODO
	//cl_flagindex = -1; // TODO
	//MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
//	MSG_WriteString (&cls.netchan.message, va("modellist %i 0", cl.servercount));
	//MSG_WriteString (&cls.netchan.message, va(modellist_name, cl.servercount, 0)); // TODO
};

/*
=================
Model_NextDownload
=================
*/
void Model_NextDownload ()
{
	char *s;
	int i;
	extern char gamedirfile[];

	if (cls.downloadnumber == 0)
	{
		Con_Printf ("Checking models...\n");
		cls.downloadnumber = 1;
	};

	cls.downloadtype = t_model;
	for ( 
		; cl.model_name[cls.downloadnumber][0]
		; cls.downloadnumber++)
	{
		s = cl.model_name[cls.downloadnumber];
		if (s[0] == '*')
			continue;	// inline brush model
		if (!CL_CheckOrDownloadFile(s))
			return;		// started a download
	};

	for (i=1 ; i<MAX_MODELS ; i++)
	{
		if (!cl.model_name[i][0])
			break;

		cl.model_precache[i] = Mod_ForName (cl.model_name[i], false);

		if (!cl.model_precache[i])
		{
			Con_Printf ("\nThe required model file '%s' could not be found or downloaded.\n\n"
				, cl.model_name[i]);
			//Con_Printf("You may need to download or purchase a %s client pack in order to play on this server.\n\n", gamedirfile);
			CL_Disconnect();
			return;
		};
	};

	// all done
	cl.worldmodel = cl.model_precache[1];	
	R_NewMap ();
	Hunk_Check (); // make sure nothing is hurt

	// done with modellist, request first of static signon messages
	//MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
//	MSG_WriteString(&cls.netchan.message, va("prespawn %i 0 %i", cl.servercount, cl.worldmodel->checksum2));
	//MSG_WriteString(&cls.netchan.message, va(prespawn_name, cl.servercount, cl.worldmodel->checksum2)); // TODO
};

void CL_ParseResourceList()
{
	//begin CL_ParseResourceList()    end   CL_ParseResourceList()   
	Con_Printf("Verifying and downloading resources...");
	//GameUI_SetLoadingProgressText("#GameUI_VerifyingResources"); // TODO
	
	short nResCount = MSG_ReadShort();
	for(int i = 0; i < nResCount; ++i)
	{
		// TODO: leaks
		//resource_t *pResource = (resource_t*)Z_Malloc(sizeof(resource_t));
		
		resourcetype_t eResType = MSG_ReadByte();
		
		char sResFileName[MAX_QPATH];
		Q_strncpy(sResFileName, MSG_ReadString(), sizeof(sResFileName));
		
		switch(eResType)
		{
		case t_sound:
			Sound_NextDownload();
			break;
		case t_skin:
			// TODO
			break;
		case t_world:
		case t_model:
			Model_NextDownload();
			break;
		case t_decal:
		{
			int nDecalIndex = MSG_ReadByte();
			//Decal_NextDownload(); // TODO
			break;
		}
		case t_generic:
			//Generic_NextDownload(); // TODO
			break;
		case t_eventscript:
			// TODO
			break;
		//case dl_none:
		default:
			Con_DPrintf("Unknown resource type (%d).\n", eResType);
		};
		
		int nDownloadSize = MSG_ReadByte();
		
		byte nFlags = MSG_ReadByte();
		
		if(nFlags & RES_CUSTOM)
		{
			byte rgucMD5_hash[16];
			Q_strncpy(rgucMD5_hash, MSG_ReadString(), sizeof(rgucMD5_hash));
		};
		
		qboolean bHasExtraInfo = MSG_ReadOneBit();
		
		if(bHasExtraInfo)
		{
			// TODO
		};
		
		// TODO: consistency
		
		MSG_ReadOneBit();
		
		// TODO: alignment to byte boundary
	};
};

void CL_ParseVoiceData()
{
	int nPlayerIndex = MSG_ReadByte();
	
	int nSize = MSG_ReadShort();
	
	// TODO: check nSize?
	
	byte *pData = (byte*)Mem_Alloc(nSize);
	
	for(int i = 0; i < nSize; i++)
		pData[i] = MSG_ReadByte();
};

void CL_ParseFileTxferFailed()
{
	Con_Printf("Error: server failed to transmit file '%s'\n", MSG_ReadString());
};

void CL_Parse_HLTV()
{
	int nMode = MSG_ReadByte(); // TODO: see hltv.h
};

void CL_Parse_DeltaDescription() // TODO: replace with DELTA_ParseDescription
{
	// TODO
};

/*
==============
CL_UpdateUserinfo
==============
*/
void CL_ProcessUserInfo(int slot, player_info_t *player)
{
	strncpy(player->name, Info_ValueForKey(player->userinfo, "name"), sizeof(player->name) - 1);

	player->topcolor = atoi(Info_ValueForKey(player->userinfo, "topcolor"));
	player->bottomcolor = atoi(Info_ValueForKey(player->userinfo, "bottomcolor"));

	if(Info_ValueForKey(player->userinfo, "*spectator")[0])
		player->spectator = true;
	else
		player->spectator = false;

	// TODO
	//if (cls.state == ca_active)
		//Skin_Find (player);

	Sbar_Changed();
	CL_NewTranslation(slot);
};

/*
==============
CL_UpdateUserinfo
==============
*/
void CL_UpdateUserinfo()
{
	int slot;
	player_info_t *player;

	slot = MSG_ReadByte();
	if(slot >= MAX_CLIENTS)
		Host_EndGame("CL_ParseServerMessage: svc_updateuserinfo > MAX_SCOREBOARD");

	player = &cl.players[slot];
	player->userid = MSG_ReadLong();
	strncpy(player->userinfo, MSG_ReadString(), sizeof(player->userinfo) - 1);

	CL_ProcessUserInfo(slot, player);
};

#define SHOWNET(x)            \
	if(cl_shownet.value == 2) \
		Con_Printf("%3i:%s\n", msg_readcount - 1, x);

/*
=====================
CL_ParseServerMessage
=====================
*/
extern void R_SetSky (const char *name, float rotate, vec3_t axis); // TODO: temp
void CL_ParseServerMessage()
{
	int cmd;
	int i;

	//
	// if recording demos, copy the message out
	//
	if(cl_shownet.value == 1)
		Con_Printf("%i ", net_message.cursize);
	else if(cl_shownet.value == 2)
		Con_Printf("------------------\n");

	cl.onground = false; // unless the server says otherwise
	                     //
	                     // parse the message
	                     //

	while(1)
	{
		if(msg_badread)
			Host_Error("CL_ParseServerMessage: Bad server message %3i:%s", cmd, svc_strings[cmd]); // TODO

		cmd = MSG_ReadByte();

		if(cmd == -1)
		{
			SHOWNET("END OF MESSAGE");
			return; // end of message
		}

		// if the high bit of the command byte is set, it is a fast update
		if(cmd & 128)
		{
			SHOWNET("fast update");
			CL_ParseUpdate(cmd & 127);
			continue;
		}

		SHOWNET(svc_strings[cmd]);

		// other commands
		switch(cmd)
		{
		default:
			Host_Error("CL_ParseServerMessage: Illegible server message - %s\n", svc_strings[cmd]); // TODO
			break;
		case svc_nop:
			//Con_Printf ("svc_nop\n");
			break;
		case svc_disconnect:
			Host_EndGame("Server disconnected\n");
		case svc_event:
			CL_ParseEvent();
			break;
		case svc_version:
			CL_Parse_Version();
			break;
		case svc_setview:
			cl.viewentity = MSG_ReadShort();
			break;
		case svc_sound:
			CL_ParseStartSoundPacket();
			break;
		case svc_time:
			cl.mtime[1] = cl.mtime[0];
			cl.mtime[0] = MSG_ReadFloat();
			break;
		case svc_print:
			Con_Printf("%s", MSG_ReadString());
			break;
		case svc_stufftext:
			Cbuf_AddText(MSG_ReadString());
			break;
		case svc_setangle:
			for(i = 0; i < 3; i++)
				cl.viewangles[i] = MSG_ReadAngle();
			break;
		case svc_serverinfo:
			CL_ParseServerInfo();
			vid.recalc_refdef = true; // leave intermission full screen
			break;
		case svc_lightstyle:
			i = MSG_ReadByte();
			if(i >= MAX_LIGHTSTYLES)
				Sys_Error("svc_lightstyle > MAX_LIGHTSTYLES");
			Q_strcpy(cl_lightstyle[i].map, MSG_ReadString());
			cl_lightstyle[i].length = Q_strlen(cl_lightstyle[i].map);
			break;
		case svc_updateuserinfo:
			CL_UpdateUserinfo();
			break;
		case svc_deltadescription:
			CL_Parse_DeltaDescription();
			break;
		case svc_clientdata:
			i = MSG_ReadShort();
			CL_ParseClientdata(i);
			break;
		case svc_stopsound:
			i = MSG_ReadShort();
			S_StopSound(i >> 3, i & 7);
			break;
		case svc_pings:
			// TODO
			break;
		case svc_particle:
			R_ParseParticleEffect();
			break;
		case svc_damage:
			// Deprecated
			//V_ParseDamage();
			break;
		case svc_spawnstatic:
			CL_ParseStatic();
			break;
		case svc_event_reliable:
			// TODO
			break;
		case svc_spawnbaseline:
			i = MSG_ReadShort();
			// must use CL_EntityNum() to force cl.num_entities up
			CL_ParseBaseline(CL_EntityNum(i));
			break;
		case svc_tempentity:
			CL_ParseTEnt();
			break;
		case svc_setpause:
			cl.paused = MSG_ReadByte();

			if(cl.paused)
			{
				CDAudio_Pause();
#ifdef _WIN32
				VID_HandlePause(true);
#endif
			}
			else
			{
				CDAudio_Resume();
#ifdef _WIN32
				VID_HandlePause(false);
#endif
			};
			break;
		case svc_signonnum:
			i = MSG_ReadByte();
			//if(i <= cls.signon) // TODO: skipping this for now...
				//Host_Error("Received signon %i when at %i", i, cls.signon);
			cls.signon = i;
			CL_SignonReply();
			break;
		case svc_centerprint:
			SCR_CenterPrint(MSG_ReadString());
			break;
		case svc_killedmonster:
			// Deprecated
			//cl.stats[STAT_MONSTERS]++;
			break;
		case svc_foundsecret:
			// Deprecated
			//cl.stats[STAT_SECRETS]++;
			break;
		case svc_spawnstaticsound:
			CL_ParseStaticSound();
			break;
		case svc_intermission:
			cl.intermission = 1;
			cl.completed_time = cl.time;
			vid.recalc_refdef = true; // go to full screen
			break;
		case svc_finale:
			cl.intermission = 2;
			cl.completed_time = cl.time;
			vid.recalc_refdef = true; // go to full screen
			SCR_CenterPrint(MSG_ReadString());
			break;
		case svc_cdtrack:
			cl.cdtrack = MSG_ReadByte();
			cl.looptrack = MSG_ReadByte();
			if((cls.demoplayback || cls.demorecording) && (cls.forcetrack != -1))
				CDAudio_Play((byte)cls.forcetrack, true);
			else
				CDAudio_Play((byte)cl.cdtrack, true);
			break;
		case svc_restore:
			// TODO
			break;
		case svc_cutscene:
			cl.intermission = 3;
			cl.completed_time = cl.time;
			vid.recalc_refdef = true; // go to full screen
			SCR_CenterPrint(MSG_ReadString());
			break;
		case svc_weaponanim:
			CL_WeaponAnim(MSG_ReadByte(), MSG_ReadByte());
			break;
		case svc_decalname:
		{
			int nPositionIndex = MSG_ReadByte();
			const char *sDecalName = MSG_ReadString();
			break;
		}
		case svc_roomtype:
			Cvar_SetValue("room_type", MSG_ReadShort());
			break;
		case svc_addangle:
			cl.viewangles[YAW] += (MSG_ReadShort() * (65536 / 360));
			break;
		case svc_newusermsg:
			RegClUserMsg(MSG_ReadByte(), MSG_ReadByte(), MSG_ReadString()); // TODO
			break;
		case svc_packetentities:
			CL_ParsePacketEntities(false);
			break;
		case svc_deltapacketentities:
			CL_ParsePacketEntities(true);
			break;
		case svc_choke:
			// TODO
			break;
		case svc_resourcelist:
			CL_ParseResourceList();
			break;
		case svc_newmovevars:
		{
			float fGravity = MSG_ReadFloat();
			float fStopSpeed = MSG_ReadFloat();
			float fMaxSpeed = MSG_ReadFloat();
			float fSpectatorMaxSpeed = MSG_ReadFloat();
			float fAccelerate = MSG_ReadFloat();
			float fAirAccelerate = MSG_ReadFloat();
			float fWaterAccelerate = MSG_ReadFloat();
			float fFriction = MSG_ReadFloat();
			float fEdgeFriction = MSG_ReadFloat();
			float fWaterFriction = MSG_ReadFloat();
			float fEntGravity = MSG_ReadFloat();
			float fBounce = MSG_ReadFloat();
			float fStepSize = MSG_ReadFloat();
			float fMaxVelocity = MSG_ReadFloat();
			float fZMax = MSG_ReadFloat();
			float fWaveHeight = MSG_ReadFloat();
			
			int nFootSteps = MSG_ReadByte();
			
			float fRollAngle = MSG_ReadFloat();
			float fRollSpeed = MSG_ReadFloat();
			
			float fSkyColorRed = MSG_ReadFloat();
			float fSkyColorGreen = MSG_ReadFloat();
			float fSkyColorBlue = MSG_ReadFloat();
			
			float fSkyVecX = MSG_ReadFloat();
			float fSkyVecY = MSG_ReadFloat();
			float fSkyVecZ = MSG_ReadFloat();
			
			const char *sSkyName = MSG_ReadString();
			vec3_t skyaxis = {0.0f, 0.0f, 0.0f};
			
			R_SetSky(sSkyName, 0.0f, skyaxis);
			break;
		}
		case svc_resourcerequest:
		{
			int nSpawnCount = MSG_ReadLong();
			int nUnused = MSG_ReadLong();
			
			// TODO: send clc_resourcelist to server
			break;
		}
		case svc_customization:
		{
			int nPlayerIndex = MSG_ReadByte();
			int nType = MSG_ReadByte();
			const char *sFileName = MSG_ReadString();
			int nIndex = MSG_ReadShort();
			int nDownloadSize = MSG_ReadLong();
			int nFlags = MSG_ReadByte();
			
			char sMD5Hash[16];
			
			// TODO
			/*
			if(nFlags & RES_CUSTOM)
				for(int i = 0; i < 16; ++i)
					sMD5Hash[i] = MSG_ReadByte();
			*/
			break;
		}
		case svc_crosshairangle:
			// TODO
			//MSG_ReadChar();
			//MSG_ReadChar();
			break;
		case svc_soundfade:
		{
			int nInitialPercent = MSG_ReadByte();
			int nHoldTime = MSG_ReadByte();
			int nFadeOutTime = MSG_ReadByte();
			int nFadeInTime = MSG_ReadByte();
			break;
		}
		case svc_filetxferfailed:
			CL_ParseFileTxferFailed();
			break;
		case svc_hltv:
			CL_Parse_HLTV();
			break;
		case svc_director:
			// TODO
			break;
		case svc_voiceinit:
		{
			const char *sCodecName = MSG_ReadString();
			int nVoiceQuality = MSG_ReadByte();
			break;
		}
		case svc_voicedata:
			CL_ParseVoiceData();
			break;
		case svc_sendextrainfo:
		{
			const char *sFallbackDir = MSG_ReadString();
			int nCanCheat = MSG_ReadByte();
			break;
		}
		case svc_timescale:
		{
			int host_timescale = MSG_ReadByte();
			break;
		}
		case svc_resourcelocation:
		{
			const char *sDownloadURL /*cl.resourcelocation*/ = MSG_ReadString();
			break;
		}
		case svc_sendcvarvalue:
		{
			const char *sName = MSG_ReadString();
			break;
		}
		case svc_sendcvarvalue2:
		{
			int nRequestID = MSG_ReadLong();
			const char *sName = MSG_ReadString();
			break;
		}
			/* TODO: Original Quake protocol remnants unused in GS
		case svc_updatename:
			Sbar_Changed ();
			i = MSG_ReadByte ();
			if (i >= cl.maxclients)
				Host_Error ("CL_ParseServerMessage: svc_updatename > MAX_SCOREBOARD");
			strcpy (cl.scores[i].name, MSG_ReadString ());
			break;
			
		case svc_updatefrags:
			Sbar_Changed ();
			i = MSG_ReadByte ();
			if (i >= cl.maxclients)
				Host_Error ("CL_ParseServerMessage: svc_updatefrags > MAX_SCOREBOARD");
			cl.scores[i].frags = MSG_ReadShort ();
			break;			

		case svc_updatecolors:
			Sbar_Changed ();
			i = MSG_ReadByte ();
			if (i >= cl.maxclients)
				Host_Error ("CL_ParseServerMessage: svc_updatecolors > MAX_SCOREBOARD");
			cl.scores[i].colors = MSG_ReadByte ();
			CL_NewTranslation (i);
			break;
		case svc_updatestat:
			i = MSG_ReadByte ();
			if (i < 0 || i >= MAX_CL_STATS)
				Sys_Error ("svc_updatestat: %i is invalid", i);
			cl.stats[i] = MSG_ReadLong ();
			break;
		*/
		};
	};
};