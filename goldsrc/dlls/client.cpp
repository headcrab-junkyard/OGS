/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019 BlackPhrase
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
/// @brief client functions

#include "exports.h"
#include "custom.h" // TODO: temp?
#include "progdefs.h"
#include "mathlib.h"
#include "edict.h"
#include "Game.hpp"

void set_suicide_frame(entvars_t *self);

/*
===========
ClientConnect

called when a player connects to a server
============
*/
qboolean ClientConnect(edict_t *pEntity, const char *name, const char *adr, char sRejectReason[128])
{
	return gpGame->GetRules()->HandleClientConnect(ToBaseEntity(pEntity), name, adr, sRejectReason);
};

/*
===========
ClientDisconnect

called when a player disconnects from a server
============
*/
void ClientDisconnect(edict_t *self)
{
	gpGame->GetRules()->HandleClientDisconnect(ToBaseEntity(self));
};

// called by ClientKill and DeadThink
void respawn(entvars_t *self)
{
/*
	// make a copy of the dead body for appearances sake
	CopyToBodyQue (self);
	
	// set default spawn parms
	SetNewParms ();
	
	// respawn              
	ClientPutInServer (self);
*/
};

/*
============
ClientKill

Player entered the suicide command
============
*/
void ClientKill(edict_t *self)
{
/*
	bprint (PRINT_MEDIUM, self->v.netname);
	bprint (PRINT_MEDIUM, " suicides\n");
	
	set_suicide_frame (self->v);
	
	//self->v.modelindex = modelindex_player;
	//logfrag(self, self);
	//self->v.frags -= 2; // extra penalty
	//respawn(&self->v);
*/
};

/*
===========
ClientPutInServer

called each time a player enters a new level
============
*/
void ClientPutInServer(edict_t *client)
{
	edict_t *spot;
	//string s;

	//client->v.classname = "player";
	client->v.health = 100;
	client->v.takedamage = DAMAGE_AIM;
	client->v.solid = SOLID_SLIDEBOX;
	client->v.movetype = MOVETYPE_WALK;
	//client->v.show_hostile = 0;
	client->v.max_health = 100;
	client->v.flags = FL_CLIENT;
	//client->v.air_finished = gpGlobals->time + 12;
	//client->v.dmg = 2;                   // initial water damage
/*
	client->v.super_damage_finished = 0;
	client->v.radsuit_finished = 0;
	client->v.invisible_finished = 0;
	client->v.invincible_finished = 0;
*/
	client->v.effects = 0;
	//client->v.invincible_time = 0;

	//DecodeLevelParms ();
	
	//W_SetCurrentAmmo ();

	//client->v.attack_finished = gpGlobals->time;
	//client->v.th_pain = player_pain;
	//client->v.th_die = PlayerDie;
	
	client->v.deadflag = DEAD_NO;
// paustime is set by teleporters to keep the player from moving a while
	//client->v.pausetime = 0;
	
	//spot = SelectSpawnPoint ();

	VectorCopy(vec3_origin, client->v.origin);
	vec3_t vAdd{};
	vAdd[2] = 1;
	VectorAdd(client->v.origin, vAdd, client->v.origin);
	//client->v.origin = spot->origin + vec3_t{0, 0, 1};
	//client->v.angles = spot->angles;
	client->v.fixangle = 1;           // turn this way immediately

// oh, this is a hack!
	//setmodel (client, "models/eyes.mdl");
	//modelindex_eyes = client->v.modelindex;

	//setmodel (client, "models/player.mdl");
	//modelindex_player = client->v.modelindex;

	//setsize (client, VEC_HULL_MIN, VEC_HULL_MAX);
	
	vec3_t view_ofs{};
	view_ofs[2] = 22;
	VectorCopy(view_ofs, client->v.view_ofs);

// Mod - Xian (May.20.97)
// Bug where player would have velocity from their last kill

	VectorCopy(vec3_origin, client->v.velocity);

	//player_stand1 ();
	
	//makevectors(client->v.angles);
	//spawn_tfog (client->v.origin + v_forward*20);

	//spawn_tdeath (client->v.origin, client->v);

	// Set Rocket Jump Modifiers
	//if (stof(infokey(world, "rj")) != 0)
	{                
		//rj = stof(infokey(world, "rj"));
	}

/*
	if (deathmatch == 4)
	{
		client->v.ammo_shells = 0;
		if (stof(infokey(world, "axe")) == 0)
		{
			client->v.ammo_nails = 255;
			client->v.ammo_shells = 255;
			client->v.ammo_rockets = 255;
			client->v.ammo_cells = 255;
			client->v.items = client->v.items | IT_NAILGUN;
			client->v.items = client->v.items | IT_SUPER_NAILGUN;
			client->v.items = client->v.items | IT_SUPER_SHOTGUN;
			client->v.items = client->v.items | IT_ROCKET_LAUNCHER;
//		client->v.items = client->v.items | IT_GRENADE_LAUNCHER;
			client->v.items = client->v.items | IT_LIGHTNING;
		};
		client->v.items = client->v.items - (client->v.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3)) + IT_ARMOR3;
		client->v.armorvalue = 200;
		client->v.armortype = 0.8;
		client->v.health = 250;
		client->v.items = client->v.items | IT_INVULNERABILITY;
		client->v.invincible_time = 1;
		client->v.invincible_finished = time + 3;
	};

	if (deathmatch == 5)
	{
		client->v.ammo_nails = 80;
		client->v.ammo_shells = 30;
		client->v.ammo_rockets = 10;
		client->v.ammo_cells = 30;
		client->v.items = client->v.items | IT_NAILGUN;
		client->v.items = client->v.items | IT_SUPER_NAILGUN;
		client->v.items = client->v.items | IT_SUPER_SHOTGUN;
		client->v.items = client->v.items | IT_ROCKET_LAUNCHER;
		client->v.items = client->v.items | IT_GRENADE_LAUNCHER;
		client->v.items = client->v.items | IT_LIGHTNING;
		client->v.items = client->v.items - (client->v.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3)) + IT_ARMOR3;
		client->v.armorvalue = 200;
		client->v.armortype = 0.8;
		client->v.health = 200;
		client->v.items = client->v.items | IT_INVULNERABILITY;
		client->v.invincible_time = 1;
		client->v.invincible_finished = time + 3;
	};
*/
};

void ClientCommand(edict_t *pclent)
{
	// TODO
};

void ClientUserInfoChanged(edict_t *pclent, char *userinfo)
{
	// TODO
};

void ServerActivate(edict_t *edicts, int edictcount, int maxclients)
{
	// TODO
};

void ServerDeactivate()
{
	// TODO
};

/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink(edict_t *self)
{
	auto pBaseEntity{ToBaseEntity(self)};
	
	if(!pBaseEntity)
		return;
	
	auto pBasePlayer{dynamic_cast<CBasePlayer*>(pBaseEntity)};
	
	if(pBasePlayer)
		pBasePlayer->PreThink();
};

/*
================
PlayerPostThink

Called every frame after physics are run
================
*/
void PlayerPostThink(edict_t *self)
{
	auto pBaseEntity{ToBaseEntity(self)};
	
	if(!pBaseEntity)
		return;
	
	auto pBasePlayer{dynamic_cast<CBasePlayer*>(pBaseEntity)};
	
	if(pBasePlayer)
		pBasePlayer->PostThink();
};

void ParmsNewLevel() // TODO: SetNewParms?
{
	// TODO
};

void ParmsChangeLevel() // TODO: SetChangeParms?
{
	// TODO
};

void StartFrame()
{
	// TODO
};

const char *GetGameDescription()
{
	return "Stub (Null)";
};

void Sys_Error_Game(const char *error)
{
	// TODO
};

void PlayerCustomization(edict_t *pPlayer, customization_t *pCustom)
{
	// TODO
};

void SpectatorConnect(edict_t *pent)
{
	// TODO
};

void SpectatorDisconnect(edict_t *pent)
{
	// TODO
};

void SpectatorThink(edict_t *pent)
{
	// TODO
};

void SetupVisibility(edict_t *pViewEntity, edict_t *pClientEnt, unsigned char **pvs, unsigned char **pas){};

int AddToFullPack(struct entity_state_s *state, int e, edict_t *pent, edict_t *host_edict, int hostflags, int player, unsigned char *pSet)
{
	return 0;
};

void CreateBaseline(int player, int entindex, struct entity_state_s *baseline, edict_t *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs){};

void RegisterEncoders(){};

int GetWeaponData(edict_t *player, struct weapon_data_s *data)
{
	return 0;
};

void UpdateClientData(const edict_t *pent, int sendweapons, struct clientdata_s *pcd){};

void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed){};

void CmdEnd(const edict_t *player){};

int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *len)
{
	return 0;
};

int GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	return 0;
};

void CreateInstancedBaselines(){};

int InconsistentFile(const edict_t *player, const char *filename, char *disconnectmsg)
{
	return 0;
};

int AllowLagCompensation()
{
	return 0;
};

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 24)
The normal starting point for a level.
*/
C_EXPORT void info_player_start(entvars_t *self)
{
};