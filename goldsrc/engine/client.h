/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2023 BlackPhrase
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

#pragma once

#include <common/usercmd.h>
#include <engine/custom.h>
#include <common/screenfade.h>

/*
typedef struct
{
	char		name[16];
	qboolean	failedload;		// the name isn't a valid skin
	cache_user_t	cache;
} skin_t;
*/

// TODO: was player_state_t
// local_state_t is the information needed by a player entity
// to do move prediction and to generate a drawable entity
/*
typedef struct
{
	int messagenum; // all player's won't be updated each frame

	double state_time; // not the same as the packet time,
	                   // because player commands come asyncronously
	usercmd_t command; // last command for prediction

	vec3_t origin;
	vec3_t viewangles; // only for demos, not from server
	vec3_t velocity;
	int weaponframe;

	int modelindex;
	int frame;
	int skinnum;
	int effects;

	int flags; // dead, gib, etc

	float waterjumptime;
	int onground; // -1 = in air, else pmove entity number
	int oldbuttons;
} local_state_t;
*/

typedef struct
{
	// generated on client side
	usercmd_t cmd;      // cmd that generated the frame
	double senttime;    // time cmd was sent off
	int delta_sequence; // sequence number to delta from, -1 = full update

	// received from server
	double receivedtime;                    // time message was received, or -1
	local_state_t playerstate[MAX_CLIENTS]; // message received that reflects performing the usercmd
	packet_entities_t packet_entities;
	qboolean invalid; // true if the packet_entities delta was invalid
} frame_t;

typedef struct
{
	int length;
	char map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct
{
	char name[MAX_SCOREBOARDNAME];
	float entertime;
	int frags;
	int colors; // two 4 bit fields
	byte translations[VID_GRADES * 256];
} scoreboard_t;

typedef struct
{
	int destcolor[3];
	int percent; // 0-256
} cshift_t;

enum
{
	CSHIFT_CONTENTS = 0,
	CSHIFT_DAMAGE,
	CSHIFT_BONUS,
	CSHIFT_POWERUP,

	NUM_CSHIFTS
};

#define NAME_LENGTH 64

//
// client_state_t should hold all pieces of the client state
//

#define SIGNONS 2 // signon messages to receive before connected

#define MAX_DLIGHTS 32
#include "dlight.h"

#define MAX_BEAMS 24
#include "beamdef.h"

#define MAX_EFRAGS 640 // TODO: 512 for qw

#define MAX_MAPSTRING 2048

#define MAX_DEMOS 8
#define MAX_DEMONAME 16

typedef enum {
	//ca_uninitialized = -1,
	ca_dedicated = 0,    // a dedicated server with no ability to start a client
	ca_disconnected, // full screen console with no connection
	ca_connecting,   // sending request packets to the server // TODO: ca_demostart in qw
	ca_connected,    // valid netcon, talking to a server
	//ca_onserver,   // processing data lists, donwloading, etc // TODO: qw
	ca_active        // everything is in, so frames can be rendered
} cactive_t;

/*
/// download type
typedef enum
{
	dl_none,
	dl_model,
	dl_sound,
	dl_skin,
	dl_single
} dltype_t;
*/

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	// connection information
	cactive_t state;

	// network stuff
	netchan_t netchan;

	// private userinfo for sending to masterless servers
	char userinfo[MAX_INFO_STRING];

	char servername[MAX_OSPATH]; // name of server from original connect

	int qport;

	// personalization data sent to server
	char mapstring[MAX_QPATH];
	char spawnparms[MAX_MAPSTRING]; // to restart a level

	// demo loop control
	int demonum;                         // -1 = don't play demos
	char demos[MAX_DEMOS][MAX_DEMONAME]; // when not playing

	// demo recording info must be here, because record is started before
	// entering a map (and clearing client_state_t)
	qboolean demorecording;
	qboolean demoplayback;
	qboolean timedemo;
	int forcetrack; // -1 = use normal cd track
	FileHandle_t demofile;
	int td_lastframe;   // to meter out one message a frame
	int td_startframe;  // host_framecount at start
	float td_starttime; // realtime at second frame of timedemo

	int challenge;

	// connection information
	int signon; // 0 to SIGNONS
	
	// QW
	
	FileHandle_t download; // file transfer from server
	char downloadtempname[MAX_OSPATH];
	char downloadname[MAX_OSPATH];
	int downloadnumber;
	resourcetype_t downloadtype;
	int downloadpercent;
} client_static_t;

extern client_static_t cls;

//
// the client_state_t structure is wiped completely at every
// server signon
//
typedef struct
{
	int servercount; // server identification for prespawns

	char serverinfo[MAX_SERVERINFO_STRING];

	int parsecount;    // server message counter
	int validsequence; // this is the sequence number of the last good
	                   // packetentity_t we got.  If this is 0, we can't
	                   // render a frame yet
	int movemessages;  // since connecting to this server
	                   // throw out the first couple, so the player
	                   // doesn't accidentally do something the
	                   // first frame

	int spectator;

	//usercmd_t	cmd;			// last command sent to the server

	double last_ping_request; // while showing scoreboard
	double last_servermessage;

	// sentcmds[cl.netchan.outgoing_sequence & UPDATE_MASK] = cmd
	frame_t frames[UPDATE_BACKUP];

	// information for local display
	int stats[MAX_CL_STATS]; // health, etc
	int items;               // inventory bit flags
	float item_gettime[32];  // cl.time of aquiring item, for blinking
	float faceanimtime;      // use anim frame if cl.time < this

	cshift_t cshifts[NUM_CSHIFTS];      // color shifts for damage, powerups
	cshift_t prev_cshifts[NUM_CSHIFTS]; // and content types

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  The server sets punchangle when
	// the view is temporarliy offset, and an angle reset commands at the start
	// of each level and after teleporting.
	vec3_t mviewangles[2]; // during demo playback viewangles is lerped
	                       // between these
	vec3_t viewangles;

	vec3_t mvelocity[2]; // update by server, used for lean+bob
	                     // (0 is newest)
	vec3_t velocity;     // lerped between mvelocity[0] and [1]

	vec3_t punchangle; // temporary offset

	// pitch drifting vars
	float idealpitch;

	float viewheight;
	float crouch; // local amount for smoothing stepups

	qboolean paused; // send over by server
	qboolean onground;
	qboolean inwater;

	int intermission;   // don't change view angle, full screen, etc
	int completed_time; // latched at intermission start

	double mtime[2]; // the timestamp of last two messages
	double time;     // clients view of time, should be between
	                 // servertime and oldservertime to generate
	                 // a lerp point for other data
	double oldtime;  // previous cl.time, time-oldtime is used
	                 // to decay light values and smooth step ups

	float last_received_message; // (realtime) for net trouble icon

	//
	// information that is static for the entire time connected to a server
	//
	// TODO: needed?
	char model_name[MAX_MODELS][MAX_QPATH];
	char sound_name[MAX_SOUNDS][MAX_QPATH];

	struct model_s *model_precache[MAX_MODELS];
	struct sfx_s *sound_precache[MAX_SOUNDS];
	struct event_s *event_precache[MAX_EVENTS]; // TODO: events

	struct usermsg_s *messages[MAX_USER_MESSAGES];

	char levelname[40]; // for display on solo scoreboard
	int viewentity; // cl_entitites[cl.viewentity] = player
	int maxclients;
	int gametype;
	int playernum;

	// refresh related state
	struct model_s *worldmodel; // cl_entitites[0].model
	struct efrag_s *free_efrags;
	int num_entities; // held in cl_entities array
	int num_statics;  // held in cl_staticentities array

	cl_entity_t viewent; // the gun/weapon model

	// all player information
	player_info_t players[MAX_CLIENTS];

	// BELOW: WINQUAKE-SPECIFIC

	int cdtrack, looptrack; // cd audio

	// frag scoreboard
	scoreboard_t *scores; // [cl.maxclients]

#ifdef QUAKE2
	// light level at player's position including dlights
	// this is sent back to the server each frame
	// architectually ugly but it works
	int light_level;
#endif

	vec3_t		simorg;
	vec3_t		simvel;
	vec3_t		simangles;

	// TODO: temp(?) entries to support protocol 48
	
	int mapcrc;
	char clientdllhash[16];
	char hostname[32];
	char mapcycle[8192];
	
	screenfade_t screenfade; ///< screen fade vars
} client_state_t;

typedef void (*pfnEventHook)(struct event_args_s *args);

typedef struct event_s
{
	char name[16];
	pfnEventHook fnHook;
} event_t;

//
// cvars
//
extern cvar_t cl_name;

extern cvar_t cl_shownet;
extern cvar_t cl_nolerp;

extern cvar_t sensitivity;

#define MAX_TEMP_ENTITIES 64 // lightning bolts, etc
#define MAX_STATIC_ENTITIES 128 // torches, etc

extern client_state_t cl;

typedef struct playermove_s playermove_t; // TODO
extern playermove_t clpmove;

// FIXME, allocate dynamically
extern efrag_t cl_efrags[MAX_EFRAGS];
extern cl_entity_t cl_entities[MAX_EDICTS]; // TODO: make dynamic
extern cl_entity_t cl_static_entities[MAX_STATIC_ENTITIES];
extern lightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
extern dlight_t cl_dlights[MAX_DLIGHTS];
extern cl_entity_t cl_temp_entities[MAX_TEMP_ENTITIES];
extern beam_t cl_beams[MAX_BEAMS];

//=============================================================================

//
// cl_main
//
dlight_t *CL_AllocDlight(int key);
void CL_DecayLights();

void CL_Init();

void CL_Disconnect();
void CL_Disconnect_f();
void CL_NextDemo();
//qboolean CL_DemoBehind();

void CL_BeginServerConnect();

void CL_CheckForResend();

#define MAX_VISEDICTS 256
extern int cl_numvisedicts;
extern cl_entity_t *cl_visedicts[MAX_VISEDICTS];

//
// cl_input
//
#include "kbutton.h"

void CL_SendCmd();
void CL_SendMove(usercmd_t *cmd);

void CL_ParseTEnt();
void CL_UpdateTEnts();

void CL_ClearState();

void CL_ReadPackets();

void CL_BaseMove(usercmd_t *cmd);

float CL_KeyState(kbutton_t *key);

//
// cl_demo.c
//
void CL_StopPlayback();
qboolean CL_GetMessage();

void CL_Stop_f();
void CL_Record_f();
//void CL_ReRecord_f ();
void CL_PlayDemo_f();
void CL_TimeDemo_f();

//
// cl_parse.c
//
// TODO: qw
/*
#define NET_TIMINGS 256
#define NET_TIMINGSMASK 255
extern int	packet_latency[NET_TIMINGS];
int CL_CalcNet ();
*/
void CL_ParseServerMessage();
void CL_NewTranslation(int slot);
// TODO: qw
/*
qboolean CL_CheckOrDownloadFile(const char *filename);
qboolean CL_IsUploading();
void CL_NextUpload();
void CL_StartUpload(byte *data, int size);
void CL_StopUpload();
*/

//
// cl_tent
//
void CL_InitTEnts();
void CL_SignonReply();
//void CL_ClearTEnts(); // TODO: qw

//
// cl_ents.c
//
void CL_SetSolidPlayers(int playernum);
void CL_SetUpPlayerPrediction(qboolean dopred);
void CL_EmitEntities();
void CL_ClearProjectiles();
void CL_ParseProjectiles();
void CL_ParsePacketEntities(qboolean delta);
void CL_SetSolidEntities();
void CL_ParsePlayerinfo();

//
// cl_pred.c
//
void CL_InitPrediction();
void CL_PredictMove();
void CL_PredictUsercmd(local_state_t *from, local_state_t *to, usercmd_t *u, qboolean spectator); // TODO
//void CL_CheckPredictionError ();

void CL_WeaponAnim(int anim, int body);