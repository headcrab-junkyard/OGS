/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2021, 2023 BlackPhrase
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
/// @brief communications protocols

#pragma once

//=========================================

const int PROTOCOL_VERSION = 48;

//=========================================

const int PORT_CLIENT = 27005;
const int PORT_MASTER = 27010;
const int PORT_SERVER = 27015;

//=========================================

// out of band message id bytes

// M = master, S = server, C = client, A = any
// the second character will allways be \n if the message isn't a single
// byte long (?? not true anymore?)

#define S2C_CHALLENGE 'c'
#define S2C_CONNECTION 'j'
#define A2A_PING 'k'  // respond with an A2A_ACK
#define A2A_ACK 'l'   // general acknowledgement without info
#define A2A_NACK 'm'  // [+ comment] general failure
#define A2A_ECHO 'e'  // for echoing
#define A2C_PRINT 'n' // print a message on client

#define S2M_HEARTBEAT 'a'      // + serverinfo + userlist + fraglist
#define A2C_CLIENT_COMMAND 'B' // + command line
#define S2M_SHUTDOWN 'C'

//=========================================

// if the high bit of the servercmd is set, the low bits are fast update flags:
#define U_MOREBITS (1 << 0)
#define U_ORIGIN1 (1 << 1)
#define U_ORIGIN2 (1 << 2)
#define U_ORIGIN3 (1 << 3)
#define U_ANGLE2 (1 << 4)
#define U_NOLERP (1 << 5) // don't interpolate movement
#define U_FRAME (1 << 6)
#define U_SIGNAL (1 << 7) // just differentiates from other updates

// svc_update can pass all of the fast update bits, plus more
#define U_ANGLE1 (1 << 8)
#define U_ANGLE3 (1 << 9)
#define U_MODEL (1 << 10)
#define U_COLORMAP (1 << 11)
#define U_SKIN (1 << 12)
#define U_EFFECTS (1 << 13)
#define U_LONGENTITY (1 << 14)

#define SU_VIEWHEIGHT (1 << 0)
#define SU_IDEALPITCH (1 << 1)
#define SU_PUNCH1 (1 << 2)
#define SU_PUNCH2 (1 << 3)
#define SU_PUNCH3 (1 << 4)
#define SU_VELOCITY1 (1 << 5)
#define SU_VELOCITY2 (1 << 6)
#define SU_VELOCITY3 (1 << 7)
//define	SU_AIMENT		(1<<8)  AVAILABLE BIT
#define SU_ITEMS (1 << 9)
#define SU_ONGROUND (1 << 10) // no data follows, the bit is it
#define SU_INWATER (1 << 11)  // no data follows, the bit is it
#define SU_WEAPONFRAME (1 << 12)
#define SU_ARMOR (1 << 13)
#define SU_WEAPON (1 << 14)

// a sound with no channel is a local only sound
#define SND_VOLUME (1 << 0)      // a byte
#define SND_ATTENUATION (1 << 1) // a byte
#define SND_LOOPING (1 << 2)     // a long
#define SND_PITCH (1 << 3)       // a byte // TODO

// defaults for clientinfo messages
#define DEFAULT_VIEWHEIGHT 22

// game types sent by serverinfo
// these determine which intermission screen plays
#define GAME_COOP 0
#define GAME_DEATHMATCH 1

//==============================================

//==================
// note that there are some defs.qc that mirror to these numbers
// also related to svc_strings[] in cl_parse
//==================

//
// server to client
//
// unused legacy Quake protocol entries
/*
#define	svc_updatestat		3	// [byte] [long]
#define	svc_updatename		13	// [byte] [string]
#define	svc_updatefrags		14	// [byte] [short]
#define	svc_updatecolors	17	// [byte] [byte]
#define	svc_updateping		36		// [byte] [short]
#define	svc_updateentertime	37		// [byte] [float]

#define	svc_updatestatlong	38		// [byte] [long]		

#define	svc_smallkick		34		// set client punchangle to 2
#define	svc_bigkick			35		// set client punchangle to 4

#define	svc_muzzleflash		39		// [short] entity
*/

// PROTOCOL 48
enum
{
	svc_bad = 0,
	svc_nop = 1,
	svc_disconnect = 2,
	svc_event = 3,
	svc_version = 4,     // [long] server version
	svc_setview = 5,     // [short] entity number
	svc_sound = 6,       // <see code>
	svc_time = 7,        // [float] server time
	svc_print = 8,       // [string] null terminated string
	svc_stufftext = 9,   // [string] stuffed into client's console buffer; the string should be \n terminated
	svc_setangle = 10,   // [angle3] set the view angle to this absolute value
	svc_serverinfo = 11, // [long] version [string] signon string [string]..[0]model cache [string]...[0]sounds cache
	svc_lightstyle = 12, // [byte] [string]
	svc_updateuserinfo = 13, // [byte] slot [long] uid [string] userinfo
	svc_deltadescription = 14,
	svc_clientdata = 15, // <shortbits + data>
	svc_stopsound = 16,  // <see code>
	svc_pings = 17,
	svc_particle = 18, // [vec3] <variable>
	svc_damage = 19, // UNUSED
	svc_spawnstatic = 20,
	svc_event_reliable = 21,
	svc_spawnbaseline = 22,
	svc_tempentity = 23, // variable
	svc_setpause = 24,    // [byte] on / off
	svc_signonnum = 25,   // [byte]  used for the signon sequence
	svc_centerprint = 26, // [string] to put in center of the screen
	svc_killedmonster = 27, // UNUSED
	svc_foundsecret = 28, // UNUSED
	svc_spawnstaticsound = 29, // [coord3] [byte] samp [byte] vol [byte] aten pitch
	svc_intermission = 30,     // [string] music (TODO: revisit)
	svc_finale = 31,           // [string] music [string] text (TODO: revisit)
	svc_cdtrack = 32,          // [byte] track [byte] looptrack (TODO: revisit)
	svc_restore = 33,
	svc_cutscene = 34,
	svc_weaponanim = 35,
	svc_decalname = 36,
	svc_roomtype = 37,
	svc_addangle = 38,
	svc_newusermsg = 39,
	svc_packetentities = 40, // [...]
	svc_deltapacketentities = 41, // [...]
	svc_choke = 42,
	svc_resourcelist = 43,
	svc_newmovevars = 44,
	svc_resourcerequest = 45,
	svc_customization = 46,
	svc_crosshairangle = 47,
	svc_soundfade = 48,
	svc_filetxferfailed = 49,
	svc_hltv = 50,
	svc_director = 51,
	svc_voiceinit = 52,
	svc_voicedata = 53,
	svc_sendextrainfo = 54,
	svc_timescale = 55,
	svc_resourcelocation = 56,
	svc_sendcvarvalue = 57,
	svc_sendcvarvalue2 = 58,
	svc_exec = 59,
	
	svc_sizeof
};

//==============================================

//
// client to server
//
enum
{
	clc_bad = 0,
	clc_nop,
	clc_move,      // [usercmd_t]
	clc_stringcmd, // [string] message
	clc_delta, // [byte] sequence number, requests delta compression of message
	clc_resourcelist,
	clc_tmove, // teleport request, spectator only
	clc_fileconsistency,
	clc_voicedata,
	clc_hltv,
	clc_cvarvalue,
	clc_cvarvalue2,
	
	clc_sizeof
};

//==============================================

const int DEFAULT_SOUND_PACKET_VOLUME = 255;
const float DEFAULT_SOUND_PACKET_ATTENUATION = 1.0;
const int DEFAULT_SOUND_PACKET_PITCH = PITCH_NORM;

// TODO: QW

/*
==========================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

==========================================================
*/

#define MAX_CLIENTS 32

#define UPDATE_BACKUP 64 // copies of entity_state_t to keep buffered
                         // must be power of two
#define UPDATE_MASK (UPDATE_BACKUP - 1)

#define MAX_PACKET_ENTITIES 64 // doesn't count nails

typedef struct
{
	int num_entities;
	entity_state_t entities[MAX_PACKET_ENTITIES];
} packet_entities_t;