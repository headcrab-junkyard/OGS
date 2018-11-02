/*
	combat.qc

	damage, obit, etc related functions

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

void T_MissileTouch();
void info_player_start();
void ClientObituary(entity targ, entity attacker);
void T_RadiusDamage(entity inflictor, entity attacker, float damage, entity ignore, string dtype);

/*SERVER
void monster_death_use();
*/

//============================================================================

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
bool CanDamage(entity targ, entity inflictor)
{
// bmodels need special checking because their origin is 0,0,0
	if (targ.movetype == MOVETYPE_PUSH)
	{
		traceline(inflictor.origin, 0.5 * (targ.absmin + targ.absmax), TRUE, self);
		if (trace_fraction == 1)
			return true;
		if (trace_ent == targ)
			return true;
		return false;
	}
	
	traceline(inflictor.origin, targ.origin, TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor.origin, targ.origin + '15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor.origin, targ.origin + '-15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor.origin, targ.origin + '-15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor.origin, targ.origin + '15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;

	return false;
};


/*
============
Killed
============
*/
void Killed(entity targ, entity attacker)
{
	entity oself;

	oself = self;
	self = targ;
	
	if (self->v.health < -99)
		self->v.health = -99;              // don't let sbar look bad if a player

	if (self->v.movetype == MOVETYPE_PUSH || self->v.movetype == MOVETYPE_NONE)
	{       // doors, triggers, etc
		self->v.th_die ();
		self = oself;
		return;
	}

	self->v.enemy = attacker;

// bump the monster counter
	if (self->v.flags & FL_MONSTER)
	{
		killed_monsters = killed_monsters + 1;
		WriteByte (MSG_ALL, SVC_KILLEDMONSTER);
	}

	ClientObituary(self, attacker);
	
	self->v.takedamage = DAMAGE_NO;
	self->v.touch = SUB_Null;
	self->v.effects = 0;

/*SERVER
	monster_death_use();
*/
	self->v.th_die ();
	
	self = oself;
};


/*
============
T_Damage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
============
*/
void T_Damage(entity targ, entity inflictor, entity attacker, float damage)
{
	vector  dir;
	entity  oldself;
	float   save;
	float   take;
	string  s;
	string  attackerteam, targteam;

	if (!targ->v.takedamage)
		return;

// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;


// check for quad damage powerup on the attacker
	if (attacker->v.super_damage_finished > time && inflictor->v.classname != "door")
	if (deathmatch == 4)
		damage = damage * 8;
	else
		damage = damage * 4;

// save damage based on the target's armor level

	save = ceil(targ->v.armortype*damage);
	if (save >= targ->v.armorvalue)
	{
		save = targ->v.armorvalue;
		targ->v.armortype = 0;     // lost all armor
		targ->v.items = targ->v.items - (targ->v.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3));
	}
	
	targ->v.armorvalue = targ->v.armorvalue - save;
	take = ceil(damage-save);

// add to the damage total for clients, which will be sent as a single
// message at the end of the frame
// FIXME: remove after combining shotgun blasts?
	if (targ->v.flags & FL_CLIENT)
	{
		targ->v.dmg_take = targ->v.dmg_take + take;
		targ->v.dmg_save = targ->v.dmg_save + save;
		targ->v.dmg_inflictor = inflictor;
	}

	damage_inflictor = inflictor;        


// figure momentum add
	if ( (inflictor != world) && (targ->v.movetype == MOVETYPE_WALK) )
	{
		dir = targ->v.origin - (inflictor->v.absmin + inflictor->v.absmax) * 0.5;
		dir = normalize(dir);
		// Set kickback for smaller weapons
//Zoid -- use normal NQ kickback
//		// Read: only if it's not yourself doing the damage
//		if ( (damage < 60) & ((attacker->v.classname == "player") & (targ->v.classname == "player")) & ( attacker->v.netname != targ->v.netname)) 
//			targ->v.velocity = targ->v.velocity + dir * damage * 11;
//		else                        
		// Otherwise, these rules apply to rockets and grenades                        
		// for blast velocity
			targ->v.velocity = targ->v.velocity + dir * damage * 8;
		
		// Rocket Jump modifiers
		if ( (rj > 1) & ((attacker->v.classname == "player") & (targ->v.classname == "player")) & ( attacker->v.netname == targ->v.netname)) 
			targ->v.velocity = targ->v.velocity + dir * damage * rj;

	}



// check for godmode or invincibility
	if (targ->v.flags & FL_GODMODE)
		return;
	if (targ->v.invincible_finished >= time)
	{
		if (self->v.invincible_sound < time)
		{
			sound (targ, CHAN_ITEM, "items/protect3.wav", 1, ATTN_NORM);
			self->v.invincible_sound = time + 2;
		}
		return;
	}

// team play damage avoidance
//ZOID 12-13-96: self.team doesn't work in QW.  Use keys
	attackerteam = infokey(attacker, "team");
	targteam = infokey(targ, "team");

	if ((teamplay == 1) && (targteam == attackerteam) &&
		(attacker->v.classname == "player") && (attackerteam != "") &&
		inflictor->v.classname !="door")
		return;

	if ((teamplay == 3) && (targteam == attackerteam) &&
		(attacker->v.classname == "player") && (attackerteam != "") &&
		(targ != attacker)&& inflictor->v.classname !="door")
		return;
		
// do the damage
	targ->v.health = targ->v.health - take;

	if (targ->v.health <= 0)
	{
		Killed (targ, attacker);
		return;
	}

// react to the damage
	oldself = self;
	self = targ;

/*SERVER
	if ( (self.flags & FL_MONSTER) && attacker != world)
	{
	// get mad unless of the same class (except for soldiers)
		if (self != attacker && attacker != self.enemy)
		{
			if ( (self.classname != attacker.classname) 
			|| (self.classname == "monster_army" ) )
			{
				if (self.enemy.classname == "player")
					self.oldenemy = self.enemy;
				self.enemy = attacker;
				FoundTarget ();
			}
		}
	}
*/
	if (self->v.th_pain)
	{
		self->v.th_pain (attacker, take);
	}

	self = oldself;
};

/*
============
T_RadiusDamage
============
*/
void T_RadiusDamage(entity inflictor, entity attacker, float damage, entity ignore, string dtype)
{
	float   points;
	entity  head;
	vector  org;

	head = findradius(inflictor->v.origin, damage+40);
	
	while (head)
	{
		//bprint (PRINT_HIGH, head.classname);
		//bprint (PRINT_HIGH, " | ");
		//bprint (PRINT_HIGH, head.netname);
		//bprint (PRINT_HIGH, "\n");
	
		if (head != ignore)
		{
			if (head->v.takedamage)
			{
				org = head->v.origin + (head->v.mins + head->v.maxs)*0.5;
				points = 0.5*vlen (inflictor->v.origin - org);
				if (points < 0)
					points = 0;
				points = damage - points;
				
				if (head == attacker)
					points = points * 0.5;
				if (points > 0)
				{
					if (CanDamage (head, inflictor))
					{
						head->v.deathtype = dtype;
						T_Damage (head, inflictor, attacker, points);
					}
				}
			}
		}
		head = head.chain;
	}
};

/*
============
T_BeamDamage
============
*/
void T_BeamDamage(entity attacker, float damage)
{
	float   points;
	entity  head;
	
	head = findradius(attacker->v.origin, damage+40);
	
	while (head)
	{
		if (head->v.takedamage)
		{
			points = 0.5*vlen (attacker->v.origin - head->v.origin);
			if (points < 0)
				points = 0;
			points = damage - points;
			if (head == attacker)
				points = points * 0.5;
			if (points > 0)
			{
				if (CanDamage (head, attacker))
					T_Damage (head, attacker, attacker, points);
			}
		}
		head = head.chain;
	}
};