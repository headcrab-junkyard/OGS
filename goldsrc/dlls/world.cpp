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
/// @brief main/world setup functions

#include "exports.h"
#include "engine.h"
#include "edict.h"
#include "BaseEntity.hpp"
#include "Util.hpp"
#include "World.hpp"

extern enginefuncs_t gEngFuncs;
enginefuncs_t *gpEngine = &gEngFuncs;

void InitBodyQue();

CBaseEntity *lastspawn;

//=======================
/*QUAKED worldspawn (0 0 0) ?
Only used for the world entity.
Set message to the level name.
Set sounds to the cd track to play.

World Types:
0: medieval
1: metal
2: base
*/
//=======================
LINK_ENTITY_TO_CLASS(worldspawn, CWorldSpawn);

void CWorldSpawn::Spawn()
{
	lastspawn = nullptr; // nullptr = world entity

// custom map attributes

	gpEngine->pfnCVarSetString("sv_gravity", "800");
	gpEngine->pfnCVarSetString("sv_stepsize", "18");
	
	gpEngine->pfnCVarSetString("room_type", "0");

	InitBodyQue();

// the area based ambient sounds MUST be the first precache_sounds

// player precaches     
	//W_Precache(); // get weapon precaches // TODO

// sounds used from C physics code
	gpEngine->pfnPrecacheSound ("demon/dland2.wav");            // landing thud
	gpEngine->pfnPrecacheSound ("misc/h2ohit1.wav");            // landing splash

// setup precaches allways needed
	gpEngine->pfnPrecacheSound ("items/itembk2.wav");           // item respawn sound
	gpEngine->pfnPrecacheSound ("player/plyrjmp8.wav");         // player jump
	gpEngine->pfnPrecacheSound ("player/land.wav");                     // player landing
	gpEngine->pfnPrecacheSound ("player/land2.wav");            // player hurt landing
	gpEngine->pfnPrecacheSound ("player/drown1.wav");           // drowning pain
	gpEngine->pfnPrecacheSound ("player/drown2.wav");           // drowning pain
	gpEngine->pfnPrecacheSound ("player/gasp1.wav");            // gasping for air
	gpEngine->pfnPrecacheSound ("player/gasp2.wav");            // taking breath
	gpEngine->pfnPrecacheSound ("player/h2odeath.wav");         // drowning death

	gpEngine->pfnPrecacheSound ("misc/talk.wav");                       // talk
	gpEngine->pfnPrecacheSound ("player/teledth1.wav");         // telefrag
	gpEngine->pfnPrecacheSound ("misc/r_tele1.wav");            // teleport sounds
	gpEngine->pfnPrecacheSound ("misc/r_tele2.wav");
	gpEngine->pfnPrecacheSound ("misc/r_tele3.wav");
	gpEngine->pfnPrecacheSound ("misc/r_tele4.wav");
	gpEngine->pfnPrecacheSound ("misc/r_tele5.wav");
	gpEngine->pfnPrecacheSound ("weapons/lock4.wav");           // ammo pick up
	gpEngine->pfnPrecacheSound ("weapons/pkup.wav");            // weapon up
	gpEngine->pfnPrecacheSound ("items/armor1.wav");            // armor up
	gpEngine->pfnPrecacheSound ("weapons/lhit.wav");            //lightning
	gpEngine->pfnPrecacheSound ("weapons/lstart.wav");          //lightning start
	gpEngine->pfnPrecacheSound ("items/damage3.wav");

	gpEngine->pfnPrecacheSound ("misc/power.wav");                      //lightning for boss

// player gib sounds
	gpEngine->pfnPrecacheSound ("player/gib.wav");                      // player gib sound
	gpEngine->pfnPrecacheSound ("player/udeath.wav");           // player gib sound
	gpEngine->pfnPrecacheSound ("player/tornoff2.wav");         // gib sound

// player pain sounds

	gpEngine->pfnPrecacheSound ("player/pain1.wav");
	gpEngine->pfnPrecacheSound ("player/pain2.wav");
	gpEngine->pfnPrecacheSound ("player/pain3.wav");
	gpEngine->pfnPrecacheSound ("player/pain4.wav");
	gpEngine->pfnPrecacheSound ("player/pain5.wav");
	gpEngine->pfnPrecacheSound ("player/pain6.wav");

// player death sounds
	gpEngine->pfnPrecacheSound ("player/death1.wav");
	gpEngine->pfnPrecacheSound ("player/death2.wav");
	gpEngine->pfnPrecacheSound ("player/death3.wav");
	gpEngine->pfnPrecacheSound ("player/death4.wav");
	gpEngine->pfnPrecacheSound ("player/death5.wav");

	gpEngine->pfnPrecacheSound ("boss1/sight1.wav");

// ax sounds    
	gpEngine->pfnPrecacheSound ("weapons/ax1.wav");                     // ax swoosh
	gpEngine->pfnPrecacheSound ("player/axhit1.wav");           // ax hit meat
	gpEngine->pfnPrecacheSound ("player/axhit2.wav");           // ax hit world

	gpEngine->pfnPrecacheSound ("player/h2ojump.wav");          // player jumping into water
	gpEngine->pfnPrecacheSound ("player/slimbrn2.wav");         // player enter slime
	gpEngine->pfnPrecacheSound ("player/inh2o.wav");            // player enter water
	gpEngine->pfnPrecacheSound ("player/inlava.wav");           // player enter lava
	gpEngine->pfnPrecacheSound ("misc/outwater.wav");           // leaving water sound

	gpEngine->pfnPrecacheSound ("player/lburn1.wav");           // lava burn
	gpEngine->pfnPrecacheSound ("player/lburn2.wav");           // lava burn

	gpEngine->pfnPrecacheSound ("misc/water1.wav");                     // swimming
	gpEngine->pfnPrecacheSound ("misc/water2.wav");                     // swimming

// Invulnerability sounds
	gpEngine->pfnPrecacheSound ("items/protect.wav");
	gpEngine->pfnPrecacheSound ("items/protect2.wav");
	gpEngine->pfnPrecacheSound ("items/protect3.wav");

	/*
	gpEngine->pfnPrecacheModel ("models/player.mdl");
	gpEngine->pfnPrecacheModel ("models/eyes.mdl");
	gpEngine->pfnPrecacheModel ("models/h_player.mdl");
	gpEngine->pfnPrecacheModel ("models/gib1.mdl");
	gpEngine->pfnPrecacheModel ("models/gib2.mdl");
	gpEngine->pfnPrecacheModel ("models/gib3.mdl");

	gpEngine->pfnPrecacheModel ("sprites/s_bubble.spr");  // drowning bubbles
	gpEngine->pfnPrecacheModel ("sprites/s_explod.spr");  // sprite explosion

	gpEngine->pfnPrecacheModel ("models/v_axe.mdl");
	gpEngine->pfnPrecacheModel ("models/v_shot.mdl");
	gpEngine->pfnPrecacheModel ("models/v_nail.mdl");

	gpEngine->pfnPrecacheModel ("models/bolt.mdl");              // for lightning gun
	gpEngine->pfnPrecacheModel ("models/bolt2.mdl");             // for lightning gun
	gpEngine->pfnPrecacheModel ("models/bolt3.mdl");             // for boss shock
	gpEngine->pfnPrecacheModel ("models/lavaball.mdl");  // for testing
	
	gpEngine->pfnPrecacheModel ("models/missile.mdl");
	gpEngine->pfnPrecacheModel ("models/grenade.mdl");
	gpEngine->pfnPrecacheModel ("models/spike.mdl");
	gpEngine->pfnPrecacheModel ("models/s_spike.mdl");

	gpEngine->pfnPrecacheModel ("models/backpack.mdl");

	gpEngine->pfnPrecacheModel ("models/zom_gib.mdl");

	gpEngine->pfnPrecacheModel ("models/v_light.mdl");
	*/

//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	gpEngine->pfnLightStyle(0, "m");
	
	// 1 FLICKER (first variety)
	gpEngine->pfnLightStyle(1, "mmnmmommommnonmmonqnmmo");
	
	// 2 SLOW STRONG PULSE
	gpEngine->pfnLightStyle(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	
	// 3 CANDLE (first variety)
	gpEngine->pfnLightStyle(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	
	// 4 FAST STROBE
	gpEngine->pfnLightStyle(4, "mamamamamama");
	
	// 5 GENTLE PULSE 1
	gpEngine->pfnLightStyle(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	
	// 6 FLICKER (second variety)
	gpEngine->pfnLightStyle(6, "nmonqnmomnmomomno");
	
	// 7 CANDLE (second variety)
	gpEngine->pfnLightStyle(7, "mmmaaaabcdefgmmmmaaaammmaamm");
	
	// 8 CANDLE (third variety)
	gpEngine->pfnLightStyle(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	
	// 9 SLOW STROBE (fourth variety)
	gpEngine->pfnLightStyle(9, "aaaaaaaazzzzzzzz");
	
	// 10 FLUORESCENT FLICKER
	gpEngine->pfnLightStyle(10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	gpEngine->pfnLightStyle(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");
	
	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	gpEngine->pfnLightStyle(63, "a");
};

bool CWorldSpawn::HandleKeyValue(KeyValueData *apKVData)
{
	// TODO
	return false;
};

/*
==============================================================================

BODY QUE

==============================================================================
*/

edict_t *bodyque_head;

class CBodyQue : public CBaseEntity
{
};

LINK_ENTITY_TO_CLASS(bodyque, CBodyQue);

void InitBodyQue()
{
	// TODO
/*
	bodyque_head = gpEngine->pfnSpawn();
	bodyque_head->v.classname = "bodyque";
	bodyque_head->v.owner = gpEngine->pfnSpawn();
	bodyque_head->v.owner->v.classname = "bodyque";
	bodyque_head->v.owner->v.owner = gpEngine->pfnSpawn();
	bodyque_head->v.owner->v.owner->v.classname = "bodyque";
	bodyque_head->v.owner->v.owner->v.owner = gpEngine->pfnSpawn();
	bodyque_head->v.owner->v.owner->v.owner->v.classname = "bodyque";
	bodyque_head->v.owner->v.owner->v.owner->v.owner = bodyque_head;
*/
};

// make a body que entry for the given ent so the ent can be
// respawned elsewhere
void CopyToBodyQue(edict_t *ent)
{
	// TODO
/*
	bodyque_head.angles = ent.angles;
	bodyque_head.model = ent.model;
	bodyque_head.modelindex = ent.modelindex;
	bodyque_head.frame = ent.frame;
	bodyque_head.colormap = ent.colormap;
	bodyque_head.movetype = ent.movetype;
	bodyque_head.velocity = ent.velocity;
	bodyque_head.flags = 0;
	
	gpEngine->pfnSetOrigin(bodyque_head, ent.origin);
	gpEngine->pfnSetSize(bodyque_head, ent.mins, ent.maxs);
	
	bodyque_head = bodyque_head.owner;
*/
};