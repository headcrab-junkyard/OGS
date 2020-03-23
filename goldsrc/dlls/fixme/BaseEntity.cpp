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

#include "BaseEntity.hpp"

/*
==============================================================================
BULLETS
==============================================================================
*/

/*
================
TraceAttack
================
*/
void CBaseEntity::TraceAttack(float damage, vec3_t dir)
{
	vec3_t  vel, org;
	
	vel = normalize(dir + v_up*crandom() + v_right*crandom());
	vel = vel + 2*trace_plane_normal;
	vel = vel * 200;

	org = trace_endpos - dir*4;

	if (trace_ent.takedamage)
	{
		blood_count = blood_count + 1;
		blood_org = org;
		AddMultiDamage (trace_ent, damage);
	}
	else
		puff_count = puff_count + 1;
};

/*
============
T_Damage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
============
*/
void CBaseEntity::TakeDamage(CBaseEntity *inflictor, CBaseEntity *attacker, float damage)
{
	vec3_t  dir;
	CBaseEntity *oldself;
	float   save;
	float   take;
	string  s;
	string  attackerteam, targteam;

	if (!self->v.takedamage)
		return;

// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;

// check for quad damage powerup on the attacker
	if (attacker->v.super_damage_finished > time && inflictor->GetClassName() != "door")
	if (deathmatch == 4)
		damage *= 8;
	else
		damage *= 4;

// save damage based on the target's armor level

	save = ceil(self->GetArmorType() * damage);
	if (save >= self->GetArmorValue())
	{
		save = self->GetArmorValue();
		self->SetArmorType(0);     // lost all armor
		self->v.items = self->v.items - (self->v.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3));
	}
	
	self->v.armorvalue = self->GetArmorValue() - save;
	take = ceil(damage-save);

// add to the damage total for clients, which will be sent as a single
// message at the end of the frame
// FIXME: remove after combining shotgun blasts?
	if (self->v.flags & FL_CLIENT)
	{
		self->v.dmg_take = self->v.dmg_take + take;
		self->v.dmg_save = self->v.dmg_save + save;
		self->v.dmg_inflictor = inflictor;
	};

	damage_inflictor = inflictor;

// figure momentum add
	if ( (inflictor != world) && (self->GetMoveType() == MOVETYPE_WALK) )
	{
		dir = self->GetOrigin() - (inflictor->v.absmin + inflictor->v.absmax) * 0.5;
		dir = normalize(dir);
		// Set kickback for smaller weapons
//Zoid -- use normal NQ kickback
//		// Read: only if it's not yourself doing the damage
//		if ( (damage < 60) & ((attacker->v.classname == "player") & (self->v.classname == "player")) & ( attacker->v.netname != self->v.netname)) 
//			self->v.velocity = self->v.velocity + dir * damage * 11;
//		else                        
		// Otherwise, these rules apply to rockets and grenades                        
		// for blast velocity
			self->v.velocity = self->GetVelocity() + dir * damage * 8;
		
		// Rocket Jump modifiers
		if ( (rj > 1) & ((attacker->GetClassName() == "player") & (self->GetClassName() == "player")) & ( attacker->v.netname == self->v.netname)) 
			self->v.velocity = self->GetVelocity() + dir * damage * rj;

	}

// check for godmode or invincibility
	if (self->GetFlags() & FL_GODMODE)
		return;
	if (self->v.invincible_finished >= gpGlobals->time)
	{
		if (self->v.invincible_sound < gpGlobals->time)
		{
			EmitSound (CHAN_ITEM, "items/protect3.wav", 1, ATTN_NORM);
			self->v.invincible_sound = gpGlobals->time + 2;
		};
		return;
	};

// team play damage avoidance
//ZOID 12-13-96: self.team doesn't work in QW.  Use keys
	attackerteam = infokey(attacker, "team");
	targteam = infokey(self, "team");

	if ((teamplay == 1) && (targteam == attackerteam) &&
		(attacker->GetClassName() == "player") && (attackerteam != "") &&
		inflictor->GetClassName() !="door")
		return;

	if ((teamplay == 3) && (targteam == attackerteam) &&
		(attacker->GetClassName() == "player") && (attackerteam != "") &&
		(self != attacker)&& inflictor->GetClassName() !="door")
		return;
		
// do the damage
	self->v.health = self->GetHealth() - take;

	if (self->GetHealth() <= 0)
	{
		Killed (attacker);
		return;
	};

// react to the damage
	oldself = self;

/*SERVER
	if ( (self.flags & FL_MONSTER) && attacker != world)
	{
	// get mad unless of the same class (except for soldiers)
		if (self != attacker && attacker != self->GetEnemy())
		{
			if ( (self.classname != attacker.classname) 
			|| (self.classname == "monster_army" ) )
			{
				if (self.enemy->GetClassName() == "player")
					self.oldenemy = self->GetEnemy();
				self.enemy = attacker;
				FoundTarget ();
			}
		}
	}
*/
	if (self->v.th_pain)
		self->v.th_pain (attacker, take);

	self = oldself;
};


/*
============
Killed
============
*/
void CBaseEntity::Killed(CBaseEntity *attacker)
{
	CBaseEntity *oself = self;
	
	if (GetHealth() < -99)
		SetHealth(-99);              // don't let sbar look bad if a player

	if (GetMoveType() == MOVETYPE_PUSH || GetMoveType() == MOVETYPE_NONE)
	{
		// doors, triggers, etc
		self->v.th_die ();
		self = oself;
		return;
	};

	self->SetEnemy(attacker);

	// bump the monster counter
	if (self->GetFlags() & FL_MONSTER)
	{
		killed_monsters++;
		WriteByte (MSG_ALL, SVC_KILLEDMONSTER);
	};

	ClientObituary(self, attacker);
	
	self->v.takedamage = DAMAGE_NO;
	self->v.touch = SUB_Null;
	self->SetEffects(0);

/*SERVER
	monster_death_use();
*/
	self->v.th_die ();
	
	self = oself;
};

/*
================
FireBullets

Used by shotgun, super shotgun, and enemy soldier firing
Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void CBaseEntity::FireBullets(float shotcount, vec3_t dir, vec3_t spread)
{
	vec3_t direction;
	vec3_t src;
	
	gpEngine->pfnMakeVectors(self.v_angle);

	src = self->GetOrigin() + v_forward*10;
	src_z = self.absmin_z + self.size_z * 0.7;

	ClearMultiDamage ();

	traceline (src, src + dir*2048, FALSE, self);
	puff_org = trace_endpos - dir*4;

	while (shotcount > 0)
	{
		direction = dir + crandom()*spread_x*v_right + crandom()*spread_y*v_up;
		traceline (src, src + direction*2048, FALSE, self);
		if (trace_fraction != 1.0)
			TraceAttack (4, direction);

		shotcount--;
	};
	ApplyMultiDamage ();
	Multi_Finish ();
};