/*
	defs.qc

	global definitions

	Copyright (C) 1996-1997  Id Software, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

*/

/// @file

#pragma once

/*
==============================================================================

			SOURCE FOR GLOBALVARS_T C STRUCTURE

==============================================================================
*/

//
// system globals
//
entity          self;
entity          other;
entity          world;

entity          newmis;                         // if this is set, the entity that just
								// run created a new missile that should
								// be simulated immediately    
	
// set by traceline / tracebox
entity          trace_ent;

entity          msg_entity;                             // destination of single entity writes


//================================================
void            end_sys_globals;                // flag for structure dumping
//================================================

/*
==============================================================================

			SOURCE FOR ENTVARS_T C STRUCTURE

==============================================================================
*/
     
.float          lastruntime;    // *** to allow entities to run out of sequence             

.void()         blocked;                // for doors or plats, called when can't push other               

//================================================
void            end_sys_fields;                 // flag for structure dumping
//================================================

/*
==============================================================================

				VARS NOT REFERENCED BY C CODE

==============================================================================
*/


//
// constants
//

// edict.flags
float   FL_FLY                                  = 1;
float   FL_SWIM                                 = 2;
float   FL_CLIENT                               = 8;    // set for all client edicts
float   FL_INWATER                              = 16;   // for enter / leave water splash
float   FL_MONSTER                              = 32;
float   FL_GODMODE                              = 64;   // player cheat
float   FL_NOTARGET                             = 128;  // player cheat
float   FL_ITEM                                 = 256;  // extra wide size for bonus items
float   FL_ONGROUND                             = 512;  // standing on something
float   FL_PARTIALGROUND                = 1024; // not all corners are valid
float   FL_WATERJUMP                    = 2048; // player jumping out of water
float   FL_JUMPRELEASED                 = 4096; // for jump debouncing

// edict.movetype values
float   MOVETYPE_NONE                   = 0;    // never moves
//float MOVETYPE_ANGLENOCLIP    = 1;
//float MOVETYPE_ANGLECLIP              = 2;
float   MOVETYPE_WALK                   = 3;    // players only
float   MOVETYPE_STEP                   = 4;    // discrete, not real time unless fall
float   MOVETYPE_FLY                    = 5;
float   MOVETYPE_TOSS                   = 6;    // gravity
float   MOVETYPE_PUSH                   = 7;    // no clip to world, push and crush
float   MOVETYPE_NOCLIP                 = 8;
float   MOVETYPE_FLYMISSILE             = 9;    // fly with extra size against monsters
float   MOVETYPE_BOUNCE                 = 10;
float   MOVETYPE_BOUNCEMISSILE  = 11;   // bounce with extra size

// edict.solid values
float   SOLID_NOT                               = 0;    // no interaction with other objects
float   SOLID_TRIGGER                   = 1;    // touch on edge, but not blocking
float   SOLID_BBOX                              = 2;    // touch on edge, block
float   SOLID_SLIDEBOX                  = 3;    // touch on edge, but not an onground
float   SOLID_BSP                               = 4;    // bsp clip, touch on edge, block

// range values
float   RANGE_MELEE                             = 0;
float   RANGE_NEAR                              = 1;
float   RANGE_MID                               = 2;
float   RANGE_FAR                               = 3;

// deadflag values

float   DEAD_NO                                 = 0;
float   DEAD_DYING                              = 1;
float   DEAD_DEAD                               = 2;
float   DEAD_RESPAWNABLE                = 3;

// takedamage values

float   DAMAGE_NO                               = 0;
float   DAMAGE_YES                              = 1;
float   DAMAGE_AIM                              = 2;

// items
float   IT_AXE                                  = 4096;
float   IT_SHOTGUN                              = 1;
float   IT_SUPER_SHOTGUN                = 2;
float   IT_NAILGUN                              = 4;
float   IT_SUPER_NAILGUN                = 8;
float   IT_GRENADE_LAUNCHER             = 16;
float   IT_ROCKET_LAUNCHER              = 32;
float   IT_LIGHTNING                    = 64;
float   IT_EXTRA_WEAPON                 = 128;

float   IT_SHELLS                               = 256;
float   IT_NAILS                                = 512;
float   IT_ROCKETS                              = 1024;
float   IT_CELLS                                = 2048;

float   IT_ARMOR1                               = 8192;
float   IT_ARMOR2                               = 16384;
float   IT_ARMOR3                               = 32768;
float   IT_SUPERHEALTH                  = 65536;

float   IT_KEY1                                 = 131072;
float   IT_KEY2                                 = 262144;

float   IT_INVISIBILITY                 = 524288;
float   IT_INVULNERABILITY              = 1048576;
float   IT_SUIT                                 = 2097152;
float   IT_QUAD                                 = 4194304;

// point content values

float   CONTENT_EMPTY                   = -1;
float   CONTENT_SOLID                   = -2;
float   CONTENT_WATER                   = -3;
float   CONTENT_SLIME                   = -4;
float   CONTENT_LAVA                    = -5;
float   CONTENT_SKY                             = -6;

float   STATE_TOP               = 0;
float   STATE_BOTTOM    = 1;
float   STATE_UP                = 2;
float   STATE_DOWN              = 3;

vector  VEC_ORIGIN = '0 0 0';
vector  VEC_HULL_MIN = '-16 -16 -24';
vector  VEC_HULL_MAX = '16 16 32';

vector  VEC_HULL2_MIN = '-32 -32 -24';
vector  VEC_HULL2_MAX = '32 32 64';

// protocol bytes
float   SVC_TEMPENTITY          = 23;
float   SVC_KILLEDMONSTER       = 27;
float   SVC_FOUNDSECRET         = 28;
float   SVC_INTERMISSION        = 30;
float   SVC_FINALE                      = 31;
float   SVC_CDTRACK                     = 32;
float   SVC_SELLSCREEN          = 33;
float   SVC_SMALLKICK           = 34;
float   SVC_BIGKICK                     = 35;
float   SVC_MUZZLEFLASH         = 39;


float   TE_SPIKE                = 0;
float   TE_SUPERSPIKE   = 1;
float   TE_GUNSHOT              = 2;
float   TE_EXPLOSION    = 3;
float   TE_TAREXPLOSION = 4;
float   TE_LIGHTNING1   = 5;
float   TE_LIGHTNING2   = 6;
float   TE_WIZSPIKE             = 7;
float   TE_KNIGHTSPIKE  = 8;
float   TE_LIGHTNING3   = 9;
float   TE_LAVASPLASH   = 10;
float   TE_TELEPORT             = 11;
float   TE_BLOOD                = 12;
float   TE_LIGHTNINGBLOOD = 13;

// sound channels
// channel 0 never willingly overrides
// other channels (1-7) allways override a playing sound on that channel
float   CHAN_AUTO               = 0;
float   CHAN_WEAPON             = 1;
float   CHAN_VOICE              = 2;
float   CHAN_ITEM               = 3;
float   CHAN_BODY               = 4;
float   CHAN_NO_PHS_ADD = 8;    // ie: CHAN_BODY+CHAN_NO_PHS_ADD

float   ATTN_NONE               = 0;
float   ATTN_NORM               = 1;
float   ATTN_IDLE               = 2;
float   ATTN_STATIC             = 3;

// update types

float   UPDATE_GENERAL  = 0;
float   UPDATE_STATIC   = 1;
float   UPDATE_BINARY   = 2;
float   UPDATE_TEMP             = 3;

// entity effects

//float EF_BRIGHTFIELD  = 1;
//float EF_MUZZLEFLASH  = 2;
float   EF_BRIGHTLIGHT  = 4;
float   EF_DIMLIGHT     = 8;
float   EF_FLAG1                = 16;
float   EF_FLAG2                = 32;
// GLQuakeWorld Stuff
float 	EF_BLUE		=	64;	// Blue Globe effect for Quad
float	EF_RED		=	128;	// Red Globe effect for Pentagram
// messages
float   MSG_BROADCAST   = 0;            // unreliable to all
float   MSG_ONE                 = 1;            // reliable to one (msg_entity)
float   MSG_ALL                 = 2;            // reliable to all
float   MSG_INIT                = 3;            // write to the init string
float   MSG_MULTICAST   = 4;            // for multicast() call

// message levels
float   PRINT_LOW               = 0;            // pickup messages
float   PRINT_MEDIUM    	= 1;            // death messages
float   PRINT_HIGH              = 2;            // critical messages
float   PRINT_CHAT              = 3;            // also goes to chat console

// multicast sets
float   MULTICAST_ALL   = 0;            // every client
float   MULTICAST_PHS   = 1;            // within hearing
float   MULTICAST_PVS   = 2;            // within sight
float   MULTICAST_ALL_R = 3;            // every client, reliable
float   MULTICAST_PHS_R = 4;            // within hearing, reliable
float   MULTICAST_PVS_R = 5;            // within sight, reliable




//================================================

//
// globals
//
float   movedist;

string  string_null;    // null string, nothing should be held here
float   empty_float;

entity  activator;              // the entity that activated a trigger or brush

//
// cvars checked each frame
//
float           teamplay;
float           fraglimit;
float           deathmatch;
float           rj                      =       1;

//================================================

//
// world fields (FIXME: make globals)
//
.string         wad;
.string         map;
.float          worldtype;      // 0=medieval 1=metal 2=base

//================================================

.string         killtarget;

//
// quakeed fields
//
.float          light_lev;              // not used by game, but parsed by light util
.float          style;


//
// monster ai
//
.void()         th_stand;
.void()         th_walk;
.void()         th_run;
.void()         th_missile;
.void()         th_melee;
.void(entity attacker, float damage)            th_pain;
.void()         th_die;

.entity         oldenemy;               // mad at this player before taking damage

.float          speed;

.float  lefty;

.float  search_time;
.float  attack_state;

float   AS_STRAIGHT             = 1;
float   AS_SLIDING              = 2;
float   AS_MELEE                = 3;
float   AS_MISSILE              = 4;

//
// object stuff
//
.string         mdl;
.vector         mangle;                 // angle at start

.vector         oldorigin;              // only used by secret door

.float          t_length, t_width;

//
// doors, etc
//
.vector         dest, dest1, dest2;
.float          wait;                   // time from firing to restarting
.float          delay;                  // time from activation to firing
.entity         trigger_field;  // door's trigger entity
.string         noise4;

//
// monsters
//
.float          pausetime;
.entity         movetarget;
	
//
// misc
//
.float          cnt;                    // misc flag
	
//
// subs
//
.void()         think1;
.vector         finaldest, finalangle;

//
// triggers
//
.float          count;                  // for counting triggers


//
// plats / doors / buttons
//
.float          lip;
.float          state;
.vector         pos1, pos2;             // top and bottom positions
.float          height;

//
// sounds
//
.float          waitmin, waitmax;
.float          distance;
.float          volume;




//===========================================================================
	

//
// builtin functions
//

void(vector ang)        makevectors             = #1;           // sets v_forward, etc globals

void(entity e, string m) setmodel       = #3;           // set movetype and solid first

void(entity e, vector min, vector max) setsize = #4;
// #5 was removed
void() break                                            = #6;
float() random                                          = #7;           // returns 0 - 1
void(entity e, float chan, string samp, float vol, float atten) sound = #8;
vector(vector v) normalize                      = #9;
void(string e) error                            = #10;
void(string e) objerror                         = #11;
float(vector v) vlen                            = #12;
float(vector v) vectoyaw                        = #13;
entity() spawn                                          = #14;
void(entity e) remove                           = #15;

// sets trace_* globals
// nomonsters can be:
// An entity will also be ignored for testing if forent == test,
// forent->owner == test, or test->owner == forent
// a forent of world is ignored
void(vector v1, vector v2, float nomonsters, entity forent) traceline = #16;    

entity() checkclient                            = #17;  // returns a client to look for
entity(entity start, .string fld, string match) find = #18;
void(entity client, string s)stuffcmd = #21;
entity(vector org, float rad) findradius = #22;
void(float level, string s) bprint                              = #23;
void(entity client, float level, string s) sprint = #24;
string(float f) ftos                            = #26;
string(vector v) vtos                           = #27;
void() coredump                                         = #28;          // prints all edicts
void() traceon                                          = #29;          // turns statment trace on
void() traceoff                                         = #30;
void(entity e) eprint                           = #31;          // prints an entire edict
float(float yaw, float dist) walkmove   = #32;  // returns TRUE or FALSE
// #33 was removed
float(float yaw, float dist) droptofloor= #34;  // TRUE if landed on floor
void(float style, string value) lightstyle = #35;
float(float v) rint                                     = #36;          // round to nearest int
float(float v) floor                            = #37;          // largest integer <= v
float(float v) ceil                                     = #38;          // smallest integer >= v
// #39 was removed
float(entity e) checkbottom                     = #40;          // true if self is on ground
float(vector v) pointcontents           = #41;          // returns a CONTENT_*
// #42 was removed
float(float f) fabs = #43;
vector(entity e, float speed) aim = #44;                // returns the shooting vector
float(string s) cvar = #45;                                             // return cvar.value
void(string s) localcmd = #46;                                  // put string into local que
entity(entity e) nextent = #47;                                 // for looping through all ents
// #48 was removed
void() ChangeYaw = #49;                                         // turn towards self.ideal_yaw
											// at self.yaw_speed
// #50 was removed
vector(vector v) vectoangles                    = #51;


void(float to, float f) WriteByte               = #52;
void(float to, float f) WriteChar               = #53;
void(float to, float f) WriteShort              = #54;
void(float to, float f) WriteLong               = #55;
void(float to, float f) WriteCoord              = #56;
void(float to, float f) WriteAngle              = #57;
void(float to, string s) WriteString    = #58;
void(float to, entity s) WriteEntity    = #59;

// several removed

void(float step) movetogoal                             = #67;

void(entity e) makestatic               = #69;

void(string var, string val) cvar_set = #72;    // sets cvar.value

void(entity client, string s) centerprint = #73;        // sprint, but in middle

void(vector pos, string samp, float vol, float atten) ambientsound = #74;

void(entity e) setspawnparms            = #78;          // set parm1... to the
												// values at level start
												// for coop respawn
void(entity killer, entity killee) logfrag = #79;       // add to stats

string(entity e, string key) infokey    = #80;  // get a key value (world = serverinfo)
float(string s) stof                            = #81;          // convert string to float
void(vector where, float set) multicast = #82;  // sends the temp message to a set
												// of clients, possibly in PVS or PHS

//============================================================================

//
// subs.qc
//
void(vector tdest, float tspeed, void() func) SUB_CalcMove;
void(entity ent, vector tdest, float tspeed, void() func) SUB_CalcMoveEnt;
void(vector destangle, float tspeed, void() func) SUB_CalcAngleMove;

void() SUB_Null;
void() SUB_Remove;

float (entity e, float healamount, float ignore) T_Heal; // health function

float(entity targ, entity inflictor) CanDamage;