/*
	items.qc

	item functions

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

void W_SetCurrentAmmo();
/* ALL LIGHTS SHOULD BE 0 1 0 IN COLOR ALL OTHER ITEMS SHOULD
BE .8 .3 .4 IN COLOR */


void SUB_regen(entvars_t *self)
{
	self.model = self.mdl;          // restore original model
	self.solid = SOLID_TRIGGER;     // allow it to be touched again
	gpEngine->pfnEmitSound (self, CHAN_VOICE, "items/itembk2.wav", 1, ATTN_NORM);    // play respawn sound
	gpEngine->pfnSetOrigin (self, self.origin);
};



/*QUAKED noclass (0 0 0) (-8 -8 -8) (8 8 8)
prints a warning message when spawned
*/
void noclass(entvars_t *self)
{
	dprint ("noclass spawned at");
	dprint (vtos(self.origin));
	dprint ("\n");
	remove (self);
};

void q_touch()
{
	entity    stemp;
	float     best;
	string    s;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	self.mdl = self.model;

	gpEngine->pfnEmitSound (other, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	other.items = other.items | IT_QUAD;
	self.model = string_null;
		if (deathmatch == 4)
		{
			other.armortype = 0;
			other.armorvalue = 0 * 0.01;
			other.ammo_cells = 0;
		}

// do the apropriate action
	other.super_time = 1;
	other.super_damage_finished = self.cnt;

	s=ftos(rint(other.super_damage_finished - time));

	bprint (PRINT_LOW, other.netname);
	if (deathmatch == 4)
		bprint (PRINT_LOW, " recovered an OctaPower with ");
	else 
		bprint (PRINT_LOW, " recovered a Quad with ");
	bprint (PRINT_LOW, s);
	bprint (PRINT_LOW, " seconds remaining!\n");

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


void DropQuad(float timeleft)
{
	entity    item;

	item = spawn();
	item.origin = self.origin;
	
	item.velocity_z = 300;
	item.velocity_x = -100 + (random() * 200);
	item.velocity_y = -100 + (random() * 200);
	
	item.flags = FL_ITEM;
	item.solid = SOLID_TRIGGER;
	item.movetype = MOVETYPE_TOSS;
	item.noise = "items/damage.wav";
	setmodel (item, "models/quaddama.mdl");
	setsize (item, '-16 -16 -24', '16 16 32');
	item.cnt = time + timeleft;
	item.touch = q_touch;
	item.nextthink = time + timeleft;    // remove it with the time left on it
	item.think = SUB_Remove;
};

void r_touch()
{
	entity    stemp;
	float     best;
	string    s;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	self.mdl = self.model;

	gpEngine->pfnEmitSound (other, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	other.items = other.items | IT_INVISIBILITY;
	self.model = string_null;

// do the apropriate action
	other.invisible_time = 1;
	other.invisible_finished = self.cnt;
	s=ftos(rint(other.invisible_finished - time));
	bprint (PRINT_LOW, other.netname);
	bprint (PRINT_LOW, " recovered a Ring with ");
	bprint (PRINT_LOW, s);
	bprint (PRINT_LOW, " seconds remaining!\n");
      

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


void DropRing(float timeleft)
{
	entity    item;

	item = spawn();
	item.origin = self.origin;
	
	item.velocity_z = 300;
	item.velocity_x = -100 + (random() * 200);
	item.velocity_y = -100 + (random() * 200);
	
	item.flags = FL_ITEM;
	item.solid = SOLID_TRIGGER;
	item.movetype = MOVETYPE_TOSS;
	item.noise = "items/inv1.wav";
	setmodel (item, "models/invisibl.mdl");
	setsize (item, '-16 -16 -24', '16 16 32');
	item.cnt = time + timeleft;
	item.touch = r_touch;
	item.nextthink = time + timeleft;    // remove after 30 seconds
	item.think = SUB_Remove;
};

/*
============
PlaceItem

plants the object on the floor
============
*/
void PlaceItem()
{
	float     oldz;

	self.mdl = self.model;          // so it can be restored on respawn
	self.flags = FL_ITEM;           // make extra wide
	self.solid = SOLID_TRIGGER;
	self.movetype = MOVETYPE_TOSS;  
	self.velocity = '0 0 0';
	self.origin_z = self.origin_z + 6;
	oldz = self.origin_z;
	if (!droptofloor())
	{
		dprint ("Bonus item fell out of level at ");
		dprint (vtos(self.origin));
		dprint ("\n");
		remove(self);
		return;
	}
};

/*
============
StartItem

Sets the clipping size and plants the object on the floor
============
*/
void StartItem()
{
	self.nextthink = time + 0.2;    // items start after other solids
	self.think = PlaceItem;
};

/*
=========================================================================

HEALTH BOX

=========================================================================
*/
//
// T_Heal: add health to an entity, limiting health to max_health
// "ignore" will ignore max_health limit
//
float T_Heal(entity e, float healamount, float ignore)
{
	if (e.health <= 0)
		return 0;
	if ((!ignore) && (e.health >= other.max_health))
		return 0;
	healamount = ceil(healamount);

	e.health = e.health + healamount;
	if ((!ignore) && (e.health >= other.max_health))
		e.health = other.max_health;
		
	if (e.health > 250)
		e.health = 250;
	return 1;
};

/*QUAKED item_health (.3 .3 1) (0 0 0) (32 32 32) rotten megahealth
Health box. Normally gives 25 points.
Rotten box heals 5-10 points,
megahealth will add 100 health, then 
rot you down to your maximum health limit, 
one point per second.
*/

const float   H_ROTTEN = 1;
const float   H_MEGA = 2;

.float  healamount, healtype;

void health_touch();
void item_megahealth_rot();

void item_health(entvars_t *self)
{       
	self.touch = health_touch;

	if (self.spawnflags & H_ROTTEN)
	{
		gpEngine->pfnPrecacheModel("maps/b_bh10.bsp");

		gpEngine->pfnPrecacheSound("items/r_item1.wav");
		gpEngine->pfnSetModel(self, "maps/b_bh10.bsp");
		self.noise = "items/r_item1.wav";
		self.healamount = 15;
		self.healtype = 0;
	}
	else
	if (self.spawnflags & H_MEGA)
	{
		gpEngine->pfnPrecacheModel("maps/b_bh100.bsp");
		gpEngine->pfnPrecacheSound("items/r_item2.wav");
		gpEngine->pfnSetModel(self, "maps/b_bh100.bsp");
		self.noise = "items/r_item2.wav";
		self.healamount = 100;
		self.healtype = 2;
	}
	else
	{
		gpEngine->pfnPrecacheModel("maps/b_bh25.bsp");
		gpEngine->pfnPrecacheSound("items/health1.wav");
		gpEngine->pfnSetModel(self, "maps/b_bh25.bsp");
		self.noise = "items/health1.wav";
		self.healamount = 25;
		self.healtype = 1;
	}
	gpEngine->pfnSetSize (self, '0 0 0', '32 32 56');
	StartItem ();
};


void health_touch(entvars_t *self, entvars_t *other)
{
	float amount;
	string  s;
	
	if (deathmatch == 4)
		if (other.invincible_time > 0)
			return;

	if (other.classname != "player")
		return;
	
	if (self.healtype == 2) // Megahealth?  Ignore max_health...
	{
		if (other.health >= 250)
			return;
		if (!T_Heal(other, self.healamount, 1))
			return;
	}
	else
	{
		if (!T_Heal(other, self.healamount, 0))
			return;
	}
	
	sprint(other, PRINT_LOW, "You receive ");
	s = ftos(self.healamount);
	sprint(other, PRINT_LOW, s);
	sprint(other, PRINT_LOW, " health\n");
	
// health touch sound
	gpEngine->pfnEmitSound(other, CHAN_ITEM, self.noise, 1, ATTN_NORM);

	stuffcmd (other, "bf\n");
	
	self.model = string_null;
	self.solid = SOLID_NOT;

	// Megahealth = rot down the player's super health
	if (self.healtype == 2)
	{
		other.items = other.items | IT_SUPERHEALTH;
		if (deathmatch != 4)
		{
			self.nextthink = time + 5;
			self.think = item_megahealth_rot;
		}
		self.owner = other;
	}
	else
	{
		if (deathmatch != 2)            // deathmatch 2 is the silly old rules
		{
			self.nextthink = time + 20;
			self.think = SUB_regen;
		}
	}
	
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};      

void item_megahealth_rot()
{
	other = self.owner;
	
	if (other.health > other.max_health)
	{
		other.health = other.health - 1;
		self.nextthink = time + 1;
		return;
	}

// it is possible for a player to die and respawn between rots, so don't
// just blindly subtract the flag off
	other.items = other.items - (other.items & IT_SUPERHEALTH);
	
	if (deathmatch != 2)    // deathmatch 2 is silly old rules
	{
		self.nextthink = time + 20;
		self.think = SUB_regen;
	}
};

/*
===============================================================================

ARMOR

===============================================================================
*/

void armor_touch(entvars_t *self, entvars_t *other)
{
	float   type, value, bit;
	
	if (other.health <= 0)
		return;
	if (other.classname != "player")
		return;

	if (deathmatch == 4)
		if (other.invincible_time > 0)
			return;

	if (self.classname == "item_armor1")
	{
		type = 0.3;
		value = 100;
		bit = IT_ARMOR1;
	}
	if (self.classname == "item_armor2")
	{
		type = 0.6;
		value = 150;
		bit = IT_ARMOR2;
	}
	if (self.classname == "item_armorInv")
	{
		type = 0.8;
		value = 200;
		bit = IT_ARMOR3;
	}
	if (other.armortype*other.armorvalue >= type*value)
		return;
		
	other.armortype = type;
	other.armorvalue = value;
	other.items = other.items - (other.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3)) + bit;

	self.solid = SOLID_NOT;
	self.model = string_null;
	if (deathmatch != 2)
		self.nextthink = time + 20;
	self.think = SUB_regen;

	sprint(other, PRINT_LOW, "You got armor\n");
// armor touch sound
	gpEngine->pfnEmitSound(other, CHAN_ITEM, "items/armor1.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


/*QUAKED item_armor1 (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void item_armor1(entvars_t *self)
{
	self.touch = armor_touch;
	gpEngine->pfnPrecacheModel ("models/armor.mdl");
	gpEngine->pfnSetModel (self, "models/armor.mdl");
	self.skin = 0;
	gpEngine->pfnSetSize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED item_armor2 (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void item_armor2()
{
	self.touch = armor_touch;
	precache_model ("models/armor.mdl");
	setmodel (self, "models/armor.mdl");
	self.skin = 1;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED item_armorInv (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void item_armorInv()
{
	self.touch = armor_touch;
	precache_model ("models/armor.mdl");
	setmodel (self, "models/armor.mdl");
	self.skin = 2;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*
===============================================================================

WEAPONS

===============================================================================
*/

void bound_other_ammo()
{
	if (other.ammo_shells > 100)
		other.ammo_shells = 100;
	if (other.ammo_nails > 200)
		other.ammo_nails = 200;
	if (other.ammo_rockets > 100)
		other.ammo_rockets = 100;               
	if (other.ammo_cells > 100)
		other.ammo_cells = 100;         
};


float RankForWeapon(float w)
{
	if (w == IT_LIGHTNING)
		return 1;
	if (w == IT_ROCKET_LAUNCHER)
		return 2;
	if (w == IT_SUPER_NAILGUN)
		return 3;
	if (w == IT_GRENADE_LAUNCHER)
		return 4;
	if (w == IT_SUPER_SHOTGUN)
		return 5;
	if (w == IT_NAILGUN)
		return 6;
	return 7;
};

float WeaponCode(float w)
{
	if (w == IT_SUPER_SHOTGUN)
		return 3;
	if (w == IT_NAILGUN)
		return 4;
	if (w == IT_SUPER_NAILGUN)
		return 5;
	if (w == IT_GRENADE_LAUNCHER)
		return 6;
	if (w == IT_ROCKET_LAUNCHER)
		return 7;
	if (w == IT_LIGHTNING)
		return 8;
	return 1;
};

/*
=============
Deathmatch_Weapon

Deathmatch weapon change rules for picking up a weapon

.float          ammo_shells, ammo_nails, ammo_rockets, ammo_cells;
=============
*/
void Deathmatch_Weapon(float old, float fnew)
{
	float or, nr;

// change self.weapon if desired
	or = RankForWeapon (self.weapon);
	nr = RankForWeapon (fnew);
	if ( nr < or )
		self.weapon = fnew;
};

/*
=============
weapon_touch
=============
*/
float W_BestWeapon();

void weapon_touch()
{
	float   hadammo, best, fnew, old;
	entity  stemp;
	float   leave;

	// For client weapon_switch
	float   w_switch;

	if (!(other.flags & FL_CLIENT))
		return;

	if ((stof(infokey(other,"w_switch"))) == 0)
		w_switch = 8;
	else
		w_switch = stof(infokey(other,"w_switch"));
	
// if the player was using his best weapon, change up to the new one if better          
	stemp = self;
	self = other;
	best = W_BestWeapon();
	self = stemp;

	if (deathmatch == 2 || deathmatch == 3 || deathmatch == 5)
		leave = 1;
	else
		leave = 0;
	
	if (self.classname == "weapon_nailgun")
	{
		if (leave && (other.items & IT_NAILGUN) )
			return;
		hadammo = other.ammo_nails;                     
		fnew = IT_NAILGUN;
		other.ammo_nails = other.ammo_nails + 30;
	}
	else if (self.classname == "weapon_supernailgun")
	{
		if (leave && (other.items & IT_SUPER_NAILGUN) )
			return;
		hadammo = other.ammo_rockets;                   
		fnew = IT_SUPER_NAILGUN;
		other.ammo_nails = other.ammo_nails + 30;
	}
	else if (self.classname == "weapon_supershotgun")
	{
		if (leave && (other.items & IT_SUPER_SHOTGUN) )
			return;
		hadammo = other.ammo_rockets;                   
		fnew = IT_SUPER_SHOTGUN;
		other.ammo_shells = other.ammo_shells + 5;
	}
	else if (self.classname == "weapon_rocketlauncher")
	{
		if (leave && (other.items & IT_ROCKET_LAUNCHER) )
			return;
		hadammo = other.ammo_rockets;                   
		fnew = IT_ROCKET_LAUNCHER;
		other.ammo_rockets = other.ammo_rockets + 5;
	}
	else if (self.classname == "weapon_grenadelauncher")
	{
		if (leave && (other.items & IT_GRENADE_LAUNCHER) )
			return;
		hadammo = other.ammo_rockets;                   
		fnew = IT_GRENADE_LAUNCHER;
		other.ammo_rockets = other.ammo_rockets + 5;
	}
	else if (self.classname == "weapon_lightning")
	{
		if (leave && (other.items & IT_LIGHTNING))
			return;
		hadammo = other.ammo_rockets;                   
		fnew = IT_LIGHTNING;
		other.ammo_cells = other.ammo_cells + 15;
	}
	else
		objerror ("weapon_touch: unknown classname");

	sprint (other, PRINT_LOW, "You got the ");
	sprint (other, PRINT_LOW, self.netname);
	sprint (other, PRINT_LOW, "\n");
// weapon touch sound
	sound (other, CHAN_ITEM, "weapons/pkup.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");

	bound_other_ammo ();

// change to the weapon
	old = other.items;
	other.items = other.items | fnew;
	
	stemp = self;
	self = other;

	if ( WeaponCode(fnew) <= w_switch )
	{
		if (self.flags & FL_INWATER)
		{
			if (fnew != IT_LIGHTNING)
			{
				Deathmatch_Weapon (old, fnew);
			}
		}
		else
		{                
			Deathmatch_Weapon (old, fnew);
		}
	}

	W_SetCurrentAmmo();

	self = stemp;

	if (leave)
		return;

	if (deathmatch!=3 || deathmatch !=5)
	{
	// remove it in single player, or setup for respawning in deathmatch
		self.model = string_null;
		self.solid = SOLID_NOT;
		if (deathmatch != 2)
			self.nextthink = time + 30;
		self.think = SUB_regen;
	}
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


/*QUAKED weapon_supershotgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void weapon_supershotgun()
{
	if (deathmatch <= 3)
	{
		precache_model ("models/g_shot.mdl");
		setmodel (self, "models/g_shot.mdl");
		self.weapon = IT_SUPER_SHOTGUN;
		self.netname = "Double-barrelled Shotgun";
		self.touch = weapon_touch;
		setsize (self, '-16 -16 0', '16 16 56');
		StartItem ();
	}
};

/*QUAKED weapon_nailgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void weapon_nailgun()
{
	if (deathmatch <= 3)
	{
		precache_model ("models/g_nail.mdl");
		setmodel (self, "models/g_nail.mdl");
		self.weapon = IT_NAILGUN;
		self.netname = "nailgun";
		self.touch = weapon_touch;
		setsize (self, '-16 -16 0', '16 16 56');
		StartItem ();
	}
};

/*QUAKED weapon_supernailgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void weapon_supernailgun()
{
	if (deathmatch <= 3)
	{
		precache_model ("models/g_nail2.mdl");
		setmodel (self, "models/g_nail2.mdl");
		self.weapon = IT_SUPER_NAILGUN;
		self.netname = "Super Nailgun";
		self.touch = weapon_touch;
		setsize (self, '-16 -16 0', '16 16 56');
		StartItem ();
	}
};

/*QUAKED weapon_grenadelauncher (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void weapon_grenadelauncher()
{
	if (deathmatch <= 3)
	{
		precache_model ("models/g_rock.mdl");
		setmodel (self, "models/g_rock.mdl");
		self.weapon = 3;
		self.netname = "Grenade Launcher";
		self.touch = weapon_touch;
		setsize (self, '-16 -16 0', '16 16 56');
		StartItem ();
	}
};

/*QUAKED weapon_rocketlauncher (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void weapon_rocketlauncher()
{
	if (deathmatch <= 3)
	{
		precache_model ("models/g_rock2.mdl");
		setmodel (self, "models/g_rock2.mdl");
		self.weapon = 3;
		self.netname = "Rocket Launcher";
		self.touch = weapon_touch;
		setsize (self, '-16 -16 0', '16 16 56');
		StartItem ();
	}
};


/*QUAKED weapon_lightning (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void weapon_lightning()
{
	if (deathmatch <= 3)
	{
		precache_model ("models/g_light.mdl");
		setmodel (self, "models/g_light.mdl");
		self.weapon = 3;
		self.netname = "Thunderbolt";
		self.touch = weapon_touch;
		setsize (self, '-16 -16 0', '16 16 56');
		StartItem ();
	}
};


/*
===============================================================================

AMMO

===============================================================================
*/

void ammo_touch()
{
	entity    stemp;
	float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

// if the player was using his best weapon, change up to the new one if better          
	stemp = self;
	self = other;
	best = W_BestWeapon();
	self = stemp;


// shotgun
	if (self.weapon == 1)
	{
		if (other.ammo_shells >= 100)
			return;
		other.ammo_shells = other.ammo_shells + self.aflag;
	}

// spikes
	if (self.weapon == 2)
	{
		if (other.ammo_nails >= 200)
			return;
		other.ammo_nails = other.ammo_nails + self.aflag;
	}

//      rockets
	if (self.weapon == 3)
	{
		if (other.ammo_rockets >= 100)
			return;
		other.ammo_rockets = other.ammo_rockets + self.aflag;
	}

//      cells
	if (self.weapon == 4)
	{
		if (other.ammo_cells >= 100)
			return;
		other.ammo_cells = other.ammo_cells + self.aflag;
	}

	bound_other_ammo ();
	
	sprint (other, PRINT_LOW, "You got the ");
	sprint (other, PRINT_LOW, self.netname);
	sprint (other, PRINT_LOW, "\n");
// ammo touch sound
	sound (other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");

// change to a better weapon if appropriate

	if ( other.weapon == best )
	{
		stemp = self;
		self = other;
		self.weapon = W_BestWeapon();
		W_SetCurrentAmmo ();
		self = stemp;
	}

// if changed current ammo, update it
	stemp = self;
	self = other;
	W_SetCurrentAmmo();
	self = stemp;

// remove it in single player, or setup for respawning in deathmatch
	self.model = string_null;
	self.solid = SOLID_NOT;
	if (deathmatch != 2)
		self.nextthink = time + 30;

// Xian -- If playing in DM 3.0 mode, halve the time ammo respawns        

	if (deathmatch == 3 || deathmatch == 5)        
		self.nextthink = time + 15;

	self.think = SUB_regen;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};

const float WEAPON_BIG2 = 1;

/*QUAKED item_shells (0 .5 .8) (0 0 0) (32 32 32) big
*/

void item_shells()
{
	if (deathmatch == 4)
		return;

	self.touch = ammo_touch;

	if (self.spawnflags & WEAPON_BIG2)
	{
		precache_model ("maps/b_shell1.bsp");
		setmodel (self, "maps/b_shell1.bsp");
		self.aflag = 40;
	}
	else
	{
		precache_model ("maps/b_shell0.bsp");
		setmodel (self, "maps/b_shell0.bsp");
		self.aflag = 20;
	}
	self.weapon = 1;
	self.netname = "shells";
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();
};

/*QUAKED item_spikes (0 .5 .8) (0 0 0) (32 32 32) big
*/

void item_spikes()
{
	if (deathmatch == 4)
		return;

	self.touch = ammo_touch;

	if (self.spawnflags & WEAPON_BIG2)
	{
		precache_model ("maps/b_nail1.bsp");
		setmodel (self, "maps/b_nail1.bsp");
		self.aflag = 50;
	}
	else
	{
		precache_model ("maps/b_nail0.bsp");
		setmodel (self, "maps/b_nail0.bsp");
		self.aflag = 25;
	}
	self.weapon = 2;
	self.netname = "nails";
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();

};

/*QUAKED item_rockets (0 .5 .8) (0 0 0) (32 32 32) big
*/

void item_rockets()
{
	if (deathmatch == 4)
		return;

	self.touch = ammo_touch;

	if (self.spawnflags & WEAPON_BIG2)
	{
		gpEngine->pfnPrecacheModel ("maps/b_rock1.bsp");
		setmodel (self, "maps/b_rock1.bsp");
		self.aflag = 10;
	}
	else
	{
		gpEngine->pfnPrecacheModel ("maps/b_rock0.bsp");
		setmodel (self, "maps/b_rock0.bsp");
		self.aflag = 5;
	};
	
	self.weapon = 3;
	self.netname = "rockets";
	
	setsize (self, '0 0 0', '32 32 56');
	
	StartItem ();
};

/*QUAKED item_cells (0 .5 .8) (0 0 0) (32 32 32) big
*/

void item_cells()
{
	if (deathmatch == 4)
		return;

	self.touch = ammo_touch;

	if (self.spawnflags & WEAPON_BIG2)
	{
		gpEngine->pfnPrecacheModel ("maps/b_batt1.bsp");
		setmodel (self, "maps/b_batt1.bsp");
		self.aflag = 12;
	}
	else
	{
		gpEngine->pfnPrecacheModel ("maps/b_batt0.bsp");
		setmodel (self, "maps/b_batt0.bsp");
		self.aflag = 6;
	};
	
	self.weapon = 4;
	self.netname = "cells";
	
	setsize (self, '0 0 0', '32 32 56');
	
	StartItem ();
};

/*QUAKED item_weapon (0 .5 .8) (0 0 0) (32 32 32) shotgun rocket spikes big
DO NOT USE THIS!!!! IT WILL BE REMOVED!
*/

const float WEAPON_SHOTGUN = 1;
const float WEAPON_ROCKET = 2;
const float WEAPON_SPIKES = 4;
const float WEAPON_BIG = 8;

void item_weapon()
{
	self.touch = ammo_touch;

	if (self.spawnflags & WEAPON_SHOTGUN)
	{
		if (self.spawnflags & WEAPON_BIG)
		{
			gpEngine->pfnPrecacheModel ("maps/b_shell1.bsp");
			setmodel (self, "maps/b_shell1.bsp");
			self.aflag = 40;
		}
		else
		{
			gpEngine->pfnPrecacheModel ("maps/b_shell0.bsp");
			setmodel (self, "maps/b_shell0.bsp");
			self.aflag = 20;
		};
		
		self.weapon = 1;
		self.netname = "shells";
	};

	if (self.spawnflags & WEAPON_SPIKES)
	{
		if (self.spawnflags & WEAPON_BIG)
		{
			gpEngine->pfnPrecacheModel ("maps/b_nail1.bsp");
			setmodel (self, "maps/b_nail1.bsp");
			self.aflag = 40;
		}
		else
		{
			gpEngine->pfnPrecacheModel ("maps/b_nail0.bsp");
			setmodel (self, "maps/b_nail0.bsp");
			self.aflag = 20;
		};
		
		self.weapon = 2;
		self.netname = "spikes";
	};

	if (self.spawnflags & WEAPON_ROCKET)
	{
		if (self.spawnflags & WEAPON_BIG)
		{
			gpEngine->pfnPrecacheModel ("maps/b_rock1.bsp");
			setmodel (self, "maps/b_rock1.bsp");
			self.aflag = 10;
		}
		else
		{
			gpEngine->pfnPrecacheModel ("maps/b_rock0.bsp");
			setmodel (self, "maps/b_rock0.bsp");
			self.aflag = 5;
		};
		
		self.weapon = 3;
		self.netname = "rockets";
	};
	
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();
};

/*
===============================================================================

KEYS

===============================================================================
*/

void key_touch()
{
	entity    stemp;
	float             best;

	if (other.classname != "player")
		return;
	
	if (other.health <= 0)
		return;
	
	if (other.items & self.items)
		return;

	sprint (other, PRINT_LOW, "You got the ");
	sprint (other, PRINT_LOW, self.netname);
	sprint (other,PRINT_LOW, "\n");

	sound (other, CHAN_ITEM, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	other.items = other.items | self.items;

	self.solid = SOLID_NOT;
	self.model = string_null;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};

void key_setsounds()
{
	if (world.worldtype == 0)
	{
		gpEngine->pfnPrecacheSound ("misc/medkey.wav");
		self.noise = "misc/medkey.wav";
	};
	
	if (world.worldtype == 1)
	{
		gpEngine->pfnPrecacheSound ("misc/runekey.wav");
		self.noise = "misc/runekey.wav";
	};
	
	if (world.worldtype == 2)
	{
		gpEngine->pfnPrecacheSound ("misc/basekey.wav");
		self.noise = "misc/basekey.wav";
	};
};

/*QUAKED item_key1 (0 .5 .8) (-16 -16 -24) (16 16 32)
SILVER key
In order for keys to work
you MUST set your maps
worldtype to one of the
following:
0: medieval
1: metal
2: base
*/

void item_key1()
{
	if (world.worldtype == 0)
	{
		gpEngine->pfnPrecacheModel ("models/w_s_key.mdl");
		gpEngine->pfnSetModel (self, "models/w_s_key.mdl");
		self.netname = "silver key";
	}
	else if (world.worldtype == 1)
	{
		gpEngine->pfnPrecacheModel ("models/m_s_key.mdl");
		gpEngine->pfnSetModel (self, "models/m_s_key.mdl");
		self.netname = "silver runekey";
	}
	else if (world.worldtype == 2)
	{
		gpEngine->pfnPrecacheModel ("models/b_s_key.mdl");
		gpEngine->pfnSetModel (self, "models/b_s_key.mdl");
		self.netname = "silver keycard";
	};
	
	key_setsounds();
	
	self.touch = key_touch;
	self.items = IT_KEY1;
	
	gpEngine->pfnSetSize(self, '-16 -16 -24', '16 16 32');
	
	StartItem ();
};

/*QUAKED item_key2 (0 .5 .8) (-16 -16 -24) (16 16 32)
GOLD key
In order for keys to work
you MUST set your maps
worldtype to one of the
following:
0: medieval
1: metal
2: base
*/

void item_key2()
{
	if (world.worldtype == 0)
	{
		gpEngine->pfnPrecacheModel("models/w_g_key.mdl");
		gpEngine->pfnSetModel(self, "models/w_g_key.mdl");
		self.netname = "gold key";
	};
	
	if (world.worldtype == 1)
	{
		gpEngine->pfnPrecacheModel("models/m_g_key.mdl");
		gpEngine->pfnSetModel(self, "models/m_g_key.mdl");
		self.netname = "gold runekey";
	};
	
	if (world.worldtype == 2)
	{
		gpEngine->pfnPrecacheModel("models/b_g_key.mdl");
		gpEngine->pfnSetModel(self, "models/b_g_key.mdl");
		self.netname = "gold keycard";
	};
	
	key_setsounds();
	
	self.touch = key_touch;
	self.items = IT_KEY2;
	
	gpEngine->pfnSetSize(self, '-16 -16 -24', '16 16 32');
	
	StartItem ();
};

/*
===============================================================================

END OF LEVEL RUNES

===============================================================================
*/

void sigil_touch()
{
	entity    stemp;
	float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	centerprint (other, "You got the rune!");

	sound (other, CHAN_ITEM, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	self.model = string_null;
	serverflags = serverflags | (self.spawnflags & 15);
	self.classname = "";            // so rune doors won't find it
	
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};

/*QUAKED item_sigil (0 .5 .8) (-16 -16 -24) (16 16 32) E1 E2 E3 E4
End of level sigil, pick up to end episode and return to jrstart.
*/

void item_sigil()
{
	if (!self.spawnflags)
		objerror ("no spawnflags");

	gpEngine->pfnPrecacheSound ("misc/runekey.wav");
	
	self.noise = "misc/runekey.wav";

	if (self.spawnflags & 1)
	{
		gpEngine->pfnPrecacheModel("models/end1.mdl");
		gpEngine->pfnSetModel(self, "models/end1.mdl");
	};
	
	if (self.spawnflags & 2)
	{
		gpEngine->pfnPrecacheModel("models/end2.mdl");
		gpEngine->pfnSetModel(self, "models/end2.mdl");
	};
	
	if (self.spawnflags & 4)
	{
		gpEngine->pfnPrecacheModel("models/end3.mdl");
		gpEngine->pfnSetModel(self, "models/end3.mdl");
	};
	
	if (self.spawnflags & 8)
	{
		gpEngine->pfnPrecacheModel("models/end4.mdl");
		gpEngine->pfnSetModel(self, "models/end4.mdl");
	};
	
	self.touch = sigil_touch;
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};

/*
===============================================================================

POWERUPS

===============================================================================
*/

void CPowerUp::Touch()
{
	entity    stemp;
	float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	sprint (other, PRINT_LOW, "You got the ");
	sprint (other,PRINT_LOW,  self.netname);
	sprint (other,PRINT_LOW, "\n");

	self.mdl = self.model;
	
	if ((self.classname == "item_artifact_invulnerability") ||
		(self.classname == "item_artifact_invisibility"))
		self.nextthink = time + 60*5;
	else
		self.nextthink = time + 60;
	
	self.think = SUB_regen;

	gpEngine->pfnEmitSound(other, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	other.items = other.items | self.items;
	self.model = string_null;

// do the apropriate action
	if (self.classname == "item_artifact_envirosuit")
	{
		other.rad_time = 1;
		other.radsuit_finished = time + 30;
	};
	
	if (self.classname == "item_artifact_invulnerability")
	{
		other.invincible_time = 1;
		other.invincible_finished = time + 30;
	};
	
	if (self.classname == "item_artifact_invisibility")
	{
		other.invisible_time = 1;
		other.invisible_finished = time + 30;
	};

	if (self.classname == "item_artifact_super_damage")
	{
		if (deathmatch == 4)
		{
			other.armortype = 0;
			other.armorvalue = 0 * 0.01;
			other.ammo_cells = 0;
		};
		
		other.super_time = 1;
		other.super_damage_finished = time + 30;
	};      

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};

/*QUAKED item_artifact_invulnerability (0 .5 .8) (-16 -16 -24) (16 16 32)
Player is invulnerable for 30 seconds
*/
void item_artifact_invulnerability()
{
	self.touch = powerup_touch;

	gpEngine->pfnPrecacheModel ("models/invulner.mdl");
	gpEngine->pfnPrecacheSound ("items/protect.wav");
	gpEngine->pfnPrecacheSound ("items/protect2.wav");
	gpEngine->pfnPrecacheSound ("items/protect3.wav");
	
	self.noise = "items/protect.wav";
	
	gpEngine->pfnSetModel (self, "models/invulner.mdl");
	
	self.netname = "Pentagram of Protection";
	self.effects |= EF_RED;
	self.items = IT_INVULNERABILITY;
	
	setsize (self, '-16 -16 -24', '16 16 32');
	
	StartItem ();
};

/*QUAKED item_artifact_envirosuit (0 .5 .8) (-16 -16 -24) (16 16 32)
Player takes no damage from water or slime for 30 seconds
*/
void item_artifact_envirosuit()
{
	self.touch = powerup_touch;

	gpEngine->pfnPrecacheModel ("models/suit.mdl");
	gpEngine->pfnPrecacheSound ("items/suit.wav");
	gpEngine->pfnPrecacheSound ("items/suit2.wav");
	
	self.noise = "items/suit.wav";
	
	gpEngine->pfnSetModel (self, "models/suit.mdl");
	
	self.netname = "Biosuit";
	self.items = IT_SUIT;
	
	setsize (self, '-16 -16 -24', '16 16 32');
	
	StartItem ();
};


/*QUAKED item_artifact_invisibility (0 .5 .8) (-16 -16 -24) (16 16 32)
Player is invisible for 30 seconds
*/
void item_artifact_invisibility()
{
	self.touch = powerup_touch;

	gpEngine->pfnPrecacheModel ("models/invisibl.mdl");
	gpEngine->pfnPrecacheSound ("items/inv1.wav");
	gpEngine->pfnPrecacheSound ("items/inv2.wav");
	gpEngine->pfnPrecacheSound ("items/inv3.wav");
	
	self.noise = "items/inv1.wav";
	
	gpEngine->pfnSetModel (self, "models/invisibl.mdl");
	
	self.netname = "Ring of Shadows";
	self.items = IT_INVISIBILITY;
	
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};


/*QUAKED item_artifact_super_damage (0 .5 .8) (-16 -16 -24) (16 16 32)
The next attack from the player will do 4x damage
*/
void item_artifact_super_damage()
{
	self.touch = powerup_touch;

	gpEngine->pfnPrecacheModel ("models/quaddama.mdl");
	gpEngine->pfnPrecacheSound ("items/damage.wav");
	gpEngine->pfnPrecacheSound ("items/damage2.wav");
	gpEngine->pfnPrecacheSound ("items/damage3.wav");
	
	self.noise = "items/damage.wav";
	
	gpEngine->pfnSetModel (self, "models/quaddama.mdl");
	
	if (deathmatch == 4)
		self.netname = "OctaPower";	
	else
		self.netname = "Quad Damage";
	
	self.items = IT_QUAD;
	self.effects = self.effects | EF_BLUE;
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};

/*
===============================================================================

PLAYER BACKPACKS

===============================================================================
*/

void CBackpack::Touch()
{
	string    s;
	float   best, old, new;
	entity  stemp;
	float   acount;
	float   b_switch;

	if (deathmatch == 4)
		if (other.invincible_time > 0)
			return;

	if ((stof(infokey(other,"b_switch"))) == 0)
		b_switch = 8;
	else
		b_switch = stof(infokey(other,"b_switch"));
	

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;
		
	acount = 0;
	sprint (other, PRINT_LOW, "You get ");
 
	if (deathmatch == 4)
	{       
		other.health = other.health + 10;
		sprint (other, PRINT_LOW, "10 additional health\n");
		if ((other.health > 250) && (other.health < 300))
			sound (other, CHAN_ITEM, "items/protect3.wav", 1, ATTN_NORM);
		else
			sound (other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM);
		stuffcmd (other, "bf\n");
		remove(self);

		if (other.health >299)
		{               
			if (other.invincible_time != 1)
			{                       
				other.invincible_time = 1;
				other.invincible_finished = time + 30;
				other.items = other.items | IT_INVULNERABILITY;
				
				other.super_time = 1;
				other.super_damage_finished = time + 30;
				other.items = other.items | IT_QUAD;

				other.ammo_cells = 0;		
		
	
				sound (other, CHAN_VOICE, "boss1/sight1.wav", 1, ATTN_NORM);
				stuffcmd (other, "bf\n");               
				bprint (PRINT_HIGH, other.netname);
				bprint (PRINT_HIGH, " attains bonus powers!!!\n");
			}
		}
		self = other;
		return;
	}
	if (self.items)
		if ((other.items & self.items) == 0)
		{
			acount = 1;
			sprint (other, PRINT_LOW, "the ");
			sprint (other, PRINT_LOW, self.netname);
		}
 
// if the player was using his best weapon, change up to the new one if better          
	stemp = self;
	self = other;
	best = W_BestWeapon();
	self = stemp;

// change weapons
	other.ammo_shells = other.ammo_shells + self.ammo_shells;
	other.ammo_nails = other.ammo_nails + self.ammo_nails;
	other.ammo_rockets = other.ammo_rockets + self.ammo_rockets;
	other.ammo_cells = other.ammo_cells + self.ammo_cells;

	new = self.items;
	if (!new)
		new = other.weapon;
	old = other.items;
	other.items = other.items | self.items;
	
	bound_other_ammo ();

	if (self.ammo_shells)
	{
		if (acount)
			sprint(other, PRINT_LOW, ", ");
		acount = 1;
		s = ftos(self.ammo_shells);
		sprint (other, PRINT_LOW, s);
		sprint (other, PRINT_LOW, " shells");
	}
	if (self.ammo_nails)
	{
		if (acount)
			sprint(other, PRINT_LOW, ", ");
		acount = 1;
		s = ftos(self.ammo_nails);
		sprint (other, PRINT_LOW, s);
		sprint (other, PRINT_LOW, " nails");
	}
	if (self.ammo_rockets)
	{
		if (acount)
			sprint(other, PRINT_LOW, ", ");
		acount = 1;
		s = ftos(self.ammo_rockets);
		sprint (other, PRINT_LOW, s);
		sprint (other, PRINT_LOW, " rockets");
	}
	if (self.ammo_cells)
	{
		if (acount)
			sprint(other, PRINT_LOW, ", ");
		acount = 1;
		s = ftos(self.ammo_cells);
		sprint (other, PRINT_LOW, s);
		sprint (other,PRINT_LOW, " cells");
	}
	
	if ( (deathmatch==3 || deathmatch == 5) & ( (WeaponCode(new)==6) || (WeaponCode(new)==7) ) & (other.ammo_rockets < 5) )
		other.ammo_rockets = 5;

	sprint (other, PRINT_LOW, "\n");
// backpack touch sound
	sound (other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");

	remove(self);
	self = other;
	
// change to the weapon
	
	if ( WeaponCode(new) <= b_switch )
	{
		if (self.flags & FL_INWATER)
		{
			if (new != IT_LIGHTNING)
				Deathmatch_Weapon (old, new);
		}
		else  
			Deathmatch_Weapon (old, new);
	};

	W_SetCurrentAmmo ();
};