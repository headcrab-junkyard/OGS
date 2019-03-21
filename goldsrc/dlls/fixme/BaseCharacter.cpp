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

#include "BaseCharacter.hpp"

void CBaseCharacter::TakeDamage()
{
};

void CBaseCharacter::Pain()
{
	if (self->weaponframe)
		return;

	// eyes don't have pain frames
	if (self->invisible_finished > time)
		return;

	if (self->GetCurrentWeapon() == IT_AXE)
		player_axpain1 ();
	else
		player_pain1 ();
};

void CBaseCharacter::DeathSound()
{
	// water death sounds
	if (self->GetWaterLevel() == 3)
	{
		DeathBubbles(5);
		EmitSound(CHAN_VOICE, "player/h2odeath.wav", 1, ATTN_NONE);
		return;
	};
	
	float rs = rint ((random() * 4) + 1);
	if (rs == 1)
		self->noise = "player/death1.wav";
	if (rs == 2)
		self->noise = "player/death2.wav";
	if (rs == 3)
		self->noise = "player/death3.wav";
	if (rs == 4)
		self->noise = "player/death4.wav";
	if (rs == 5)
		self->noise = "player/death5.wav";

	EmitSound(CHAN_VOICE, self->noise, 1, ATTN_NONE);
	return;
};

void CBaseCharacter::Die()
{
	float   i;
	string	s;
	
	self.items = self.items - (self.items & IT_INVISIBILITY);

	if ((stof(infokey(world,"dq"))) != 0)
	{
		if (self.super_damage_finished > 0)
		{
			DropQuad (self.super_damage_finished - time);
			bprint (PRINT_LOW, self.netname);
			if (deathmatch == 4)
				bprint (PRINT_LOW, " lost an OctaPower with ");
			else
				bprint (PRINT_LOW, " lost a quad with ");
			s = ftos(rint(self.super_damage_finished - time));
			bprint (PRINT_LOW, s);
			bprint (PRINT_LOW, " seconds remaining\n");
		};
	};

	if ((stof(infokey(world,"dr"))) != 0)
	{
		if (self.invisible_finished > 0)
		{
			bprint (PRINT_LOW, self.netname);
			bprint (PRINT_LOW, " lost a ring with ");
			s = ftos(rint(self.invisible_finished - time));
			bprint (PRINT_LOW, s);
			bprint (PRINT_LOW, " seconds remaining\n");
			DropRing (self.invisible_finished - time);
		};
	};

	self.invisible_finished = 0;    // don't die as eyes
	self.invincible_finished = 0;
	self.super_damage_finished = 0;
	self.radsuit_finished = 0;
	self.modelindex = modelindex_player;    // don't use eyes

	DropBackpack();
	
	self->weaponmodel = "";
	self->view_ofs = '0 0 -8';
	self->deadflag = DEAD_DYING;
	self->solid = SOLID_NOT;
	self->flags = self.flags - (self.flags & FL_ONGROUND);
	self->SetMoveType(MOVETYPE_TOSS);

	if (self.velocity_z < 10)
		self.velocity_z = self.velocity_z + random()*300;

	if (self->GetHealth() < -40)
	{
		GibPlayer ();
		return;
	};

	DeathSound();
	
	self.angles_x = 0;
	self.angles_z = 0;
	
	if (self->GetCurrentWeapon() == IT_AXE)
	{
		player_die_ax1 ();
		return;
	};
	
	i = cvar("temp1");
	if (!i)
		i = 1 + floor(random()*6);
	
	if (i == 1)
		player_diea1();
	else if (i == 2)
		player_dieb1();
	else if (i == 3)
		player_diec1();
	else if (i == 4)
		player_died1();
	else
		player_diee1();
};

/*
===============
DropBackpack
===============
*/
void CBaseCharacter::DropBackpack()
{
	if (!(self.ammo_shells + self.ammo_nails + self.ammo_rockets + self.ammo_cells))
		return; // nothing in it

	CBaseEntity *item = gpEngine->pfnSpawn();
	item->SetOrigin(self->GetOrigin() - Vec3(0, 0, 24));
	
	item.items = self->GetCurrentWeapon();
	item.netname = "";
	
	if (item.items == IT_AXE)
		item.netname = "Axe";
	else if (item.items == IT_SHOTGUN)
		item.netname = "Shotgun";
	else if (item.items == IT_SUPER_SHOTGUN)
		item.netname = "Double-barrelled Shotgun";
	else if (item.items == IT_NAILGUN)
		item.netname = "Nailgun";
	else if (item.items == IT_SUPER_NAILGUN)
		item.netname = "Super Nailgun";
	else if (item.items == IT_GRENADE_LAUNCHER)
		item.netname = "Grenade Launcher";
	else if (item.items == IT_ROCKET_LAUNCHER)
		item.netname = "Rocket Launcher";
	else if (item.items == IT_LIGHTNING)
		item.netname = "Thunderbolt";

	item.ammo_shells = self.ammo_shells;
	item.ammo_nails = self.ammo_nails;
	item.ammo_rockets = self.ammo_rockets;
	item.ammo_cells = self.ammo_cells;

	item.velocity_z = 300;
	item.velocity_x = -100 + (random() * 200);
	item.velocity_y = -100 + (random() * 200);
	
	item->SetFlags(FL_ITEM);
	item->solid = SOLID_TRIGGER;
	item->SetMoveType(MOVETYPE_TOSS);

	item->SetModel("models/backpack.mdl");
	item->SetSize('-16 -16 0', '16 16 56');

	item->SetTouchCallback(BackpackTouch);
	
	item->SetNextThink(gpGlobals->time + 120); // remove after 2 minutes
	item->SetThinkCallback(SUB_Remove);
};

int CBaseCharacter::GetBestWeapon() const
{
	int it = self.items;

	if (self.waterlevel <= 1 && self.ammo_cells >= 1 && (it & IT_LIGHTNING) )
		return IT_LIGHTNING;
	else if(self.ammo_nails >= 2 && (it & IT_SUPER_NAILGUN) )
		return IT_SUPER_NAILGUN;
	else if(self.ammo_shells >= 2 && (it & IT_SUPER_SHOTGUN) )
		return IT_SUPER_SHOTGUN;
	else if(self.ammo_nails >= 1 && (it & IT_NAILGUN) )
		return IT_NAILGUN;
	else if(self.ammo_shells >= 1 && (it & IT_SHOTGUN)  )
		return IT_SHOTGUN;
		
/*
	if(self.ammo_rockets >= 1 && (it & IT_ROCKET_LAUNCHER) )
		return IT_ROCKET_LAUNCHER;
	else if(self.ammo_rockets >= 1 && (it & IT_GRENADE_LAUNCHER) )
		return IT_GRENADE_LAUNCHER;

*/

	return IT_AXE;
};

bool CBaseCharacter::CheckNoAmmo()
{
	if(self->GetCurrentWeapon()->GetCurrentAmmo() > 0)
		return true;

	if(self->GetCurrentWeapon() == IT_AXE)
		return true;
	
	self->SetCurrentWeapon(GetBestWeapon());

	W_SetCurrentAmmo();
	
	// drop the weapon down
	return false;
};

/*
===========
WaterMove

============
*/
.float  dmgtime;

void CBaseCharacter::WaterMove()
{
//dprint (ftos(self.waterlevel));
	if (self->GetMoveType() == MOVETYPE_NOCLIP)
		return;
	if (self->GetHealth() < 0)
		return;

	if (self->GetWaterLevel() != 3)
	{
		if (self->v.air_finished < time)
			EmitSound(CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM);
		else if (self->v.air_finished < time + 9)
			EmitSound(CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM);
		self->v.air_finished = time + 12;
		self->v.dmg = 2;
	}
	else if (self->v.air_finished < time)
	{       // drown!
		if (self->v.pain_finished < time)
		{
			self->v.dmg = self->v.dmg + 2;
			if (self->v.dmg > 15)
				self->v.dmg = 10;
			TakeDamage (self, world, world, self->v.dmg);
			self->v.pain_finished = time + 1;
		}
	}
	
	if (!self->v.waterlevel)
	{
		if (self->v.flags & FL_INWATER)
		{       
			// play leave water sound
			gpEngine->pfnEmitSound (self, CHAN_BODY, "misc/outwater.wav", 1, ATTN_NORM);
			self->v.flags = self->v.flags - FL_INWATER;
		}
		return;
	}

	if (self->v.watertype == CONTENT_LAVA)
	{       // do damage
		if (self->v.dmgtime < time)
		{
			if (self->v.radsuit_finished > time)
				self->v.dmgtime = time + 1;
			else
				self->v.dmgtime = time + 0.2;

			T_Damage (self, world, world, 10*self->v.waterlevel);
		}
	}
	else if (self->v.watertype == CONTENT_SLIME)
	{       // do damage
		if (self->v.dmgtime < time && self->v.radsuit_finished < time)
		{
			self->v.dmgtime = time + 1;
			T_Damage (self, world, world, 4*self->v.waterlevel);
		}
	}
	
	if ( !(self->v.flags & FL_INWATER) )
	{       

// player enter water sound

		if (self->v.watertype == CONTENT_LAVA)
			gpEngine->pfnEmitSound(self, CHAN_BODY, "player/inlava.wav", 1, ATTN_NORM);
		if (self->v.watertype == CONTENT_WATER)
			gpEngine->pfnEmitSound(self, CHAN_BODY, "player/inh2o.wav", 1, ATTN_NORM);
		if (self->v.watertype == CONTENT_SLIME)
			gpEngine->pfnEmitSound(self, CHAN_BODY, "player/slimbrn2.wav", 1, ATTN_NORM);

		self->v.flags += FL_INWATER;
		self->v.dmgtime = 0;
	}       
};

void CBaseCharacter::CheckWaterJump()
{
	vec3_t start, end;

// check for a jump-out-of-water
	makevectors (self->v.angles);
	start = self->v.origin;
	start_z = start_z + 8; 
	v_forward_z = 0;
	normalize(v_forward);
	end = start + v_forward*24;
	traceline (start, end, TRUE, self);
	if (trace_fraction < 1)
	{       // solid at waist
		start_z = start_z + self->v.maxs_z - 8;
		end = start + v_forward*24;
		self->v.movedir = trace_plane_normal * -50;
		traceline (start, end, TRUE, self);
		if (trace_fraction == 1)
		{       // open at eye level
			self->v.flags |= FL_WATERJUMP;
			self->v.velocity_z = 225;
			self->v.flags = self->v.flags - (self->v.flags & FL_JUMPRELEASED);
			self->v.teleport_time = time + 2;  // safety net
			return;
		};
	};
};

/*
================
CheckPowerups

Check for turning off powerups
================
*/
void CBaseCharacter::CheckPowerups()
{
	if (self->GetHealth() <= 0)
		return;

// invisibility
	if (self->v.invisible_finished)
	{
// sound and screen flash when items starts to run out
		if (self->v.invisible_sound < time)
		{
			gpEngine->pfnEmitSound(self, CHAN_AUTO, "items/inv3.wav", 0.5, ATTN_IDLE);
			self->v.invisible_sound = time + ((random() * 3) + 1);
		};

		if (self->v.invisible_finished < time + 3)
		{
			if (self->v.invisible_time == 1)
			{
				sprint (self, PRINT_HIGH, "Ring of Shadows magic is fading\n");
				stuffcmd (self, "bf\n");
				gpEngine->pfnEmitSound(self, CHAN_AUTO, "items/inv2.wav", 1, ATTN_NORM);
				self->v.invisible_time = time + 1;
			}
			
			if (self->v.invisible_time < time)
			{
				self->v.invisible_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self->v.invisible_finished < time)
		{       // just stopped
			self->v.items = self->v.items - IT_INVISIBILITY;
			self->v.invisible_finished = 0;
			self->v.invisible_time = 0;
		}
		
	// use the eyes
		self->v.frame = 0;
		self->v.modelindex = modelindex_eyes;
	}
	else
		self->v.modelindex = modelindex_player;    // don't use eyes

// invincibility
	if (self->v.invincible_finished)
	{
// sound and screen flash when items starts to run out
		if (self->v.invincible_finished < time + 3)
		{
			if (self->v.invincible_time == 1)
			{
				sprint (self, PRINT_HIGH, "Protection is almost burned out\n");
				stuffcmd (self, "bf\n");
				gpEngine->pfnEmitSound(self, CHAN_AUTO, "items/protect2.wav", 1, ATTN_NORM);
				self->v.invincible_time = time + 1;
			}
			
			if (self->v.invincible_time < time)
			{
				self->v.invincible_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}
		
		if (self->v.invincible_finished < time)
		{       // just stopped
			self->v.items = self->v.items - IT_INVULNERABILITY;
			self->v.invincible_time = 0;
			self->v.invincible_finished = 0;
		}
		if (self->v.invincible_finished > time)
		{
			self->v.effects |= EF_DIMLIGHT;
			self->v.effects |= EF_RED;
		}
		else
		{
			self->v.effects = self->v.effects - (self->v.effects & EF_DIMLIGHT);
			self->v.effects = self->v.effects - (self->v.effects & EF_RED);
		}
	}

// super damage
	if (self->v.super_damage_finished)
	{

// sound and screen flash when items starts to run out

		if (self->v.super_damage_finished < time + 3)
		{
			if (self->v.super_time == 1)
			{
				if (deathmatch == 4)
					sprint (self, PRINT_HIGH, "OctaPower is wearing off\n");
				else
					sprint (self, PRINT_HIGH, "Quad Damage is wearing off\n");
				stuffcmd (self, "bf\n");
				gpEngine->pfnEmitSound(self, CHAN_AUTO, "items/damage2.wav", 1, ATTN_NORM);
				self->v.super_time = time + 1;
			}         
			
			if (self->v.super_time < time)
			{
				self->v.super_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self->v.super_damage_finished < time)
		{       // just stopped
			self->v.items = self->v.items - IT_QUAD;
			if (deathmatch == 4)
			{
				self->v.ammo_cells = 255;
				self->v.armorvalue = 1;
				self->v.armortype = 0.8;
				self->v.health = 100;
			};
			
			self->v.super_damage_finished = 0;
			self->v.super_time = 0;
		}
		if (self->v.super_damage_finished > time)
		{
			self->v.effects |= EF_DIMLIGHT;
			self->v.effects |= EF_BLUE;
		}
		else
		{
			self->v.effects = self->v.effects - (self->v.effects & EF_DIMLIGHT);
			self->v.effects = self->v.effects - (self->v.effects & EF_BLUE);
		};
	};  

// suit 
	if (self->v.radsuit_finished)
	{
		self->v.air_finished = time + 12;          // don't drown

// sound and screen flash when items starts to run out
		if (self->v.radsuit_finished < time + 3)
		{
			if (self->v.rad_time == 1)
			{
				sprint (self, PRINT_HIGH, "Air supply in Biosuit expiring\n");
				stuffcmd (self, "bf\n");
				gpEngine->pfnEmitSound(self, CHAN_AUTO, "items/suit2.wav", 1, ATTN_NORM);
				self->v.rad_time = time + 1;
			}
			
			if (self->v.rad_time < time)
			{
				self->v.rad_time = time + 1;
				stuffcmd (self, "bf\n");
			};
		};

		if (self->v.radsuit_finished < time)
		{       // just stopped
			self->v.items = self->v.items - IT_SUIT;
			self->v.rad_time = 0;
			self->v.radsuit_finished = 0;
		};
	};
};