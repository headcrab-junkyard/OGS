

// prototypes
void W_WeaponFrame();
void W_SetCurrentAmmo();
void player_pain();
void player_stand1();
void spawn_tfog(vector org);
void spawn_tdeath(vector org, entity death_owner);

float   modelindex_eyes, modelindex_player;

/*
=============================================================================

				LEVEL CHANGING / INTERMISSION

=============================================================================
*/

string nextmap;

float   intermission_running;
float   intermission_exittime;

/*QUAKED info_intermission (1 0.5 0.5) (-16 -16 -16) (16 16 16)
This is the camera point for the intermission.
Use mangle instead of angle, so you can set pitch or roll as well as yaw.  'pitch roll yaw'
*/
void info_intermission(edict_t *self)
{
	self->v.angles = self->v.mangle;      // so C can get at it
};

void SetChangeParms(edict_t *self)
{
	if (self->v.health <= 0)
	{
		SetNewParms ();
		return;
	}
 
// remove items
	self->v.items = self->v.items - (self->v.items & 
	(IT_KEY1 | IT_KEY2 | IT_INVISIBILITY | IT_INVULNERABILITY | IT_SUIT | IT_QUAD) );
	
// cap super health
	if (self->v.health > 100)
		self->v.health = 100;
	if (self->v.health < 50)
		self->v.health = 50;
	parm1 = self->v.items;
	parm2 = self->v.health;
	parm3 = self->v.armorvalue;
	if (self->v.ammo_shells < 25)
		parm4 = 25;
	else
		parm4 = self->v.ammo_shells;
	parm5 = self->v.ammo_nails;
	parm6 = self->v.ammo_rockets;
	parm7 = self->v.ammo_cells;
	parm8 = self->v.weapon;
	parm9 = self->v.armortype * 100;
};

void SetNewParms()
{
	parm1 = IT_SHOTGUN | IT_AXE;
	parm2 = 100;
	parm3 = 0;
	parm4 = 25;
	parm5 = 0;
	parm6 = 0;
	parm7 = 0;
	parm8 = 1;
	parm9 = 0;
};

void DecodeLevelParms(edict_t *self)
{
	if (serverflags)
	{
		if (world->v.model == "maps/start.bsp")
			SetNewParms ();         // take away all stuff on starting new episode
	}
	
	self->v.items = parm1;
	self->v.health = parm2;
	self->v.armorvalue = parm3;
	self->v.ammo_shells = parm4;
	self->v.ammo_nails = parm5;
	self->v.ammo_rockets = parm6;
	self->v.ammo_cells = parm7;
	self->v.weapon = parm8;
	self->v.armortype = parm9 * 0.01;
};

/*
============
FindIntermission

Returns the entity to view from
============
*/
entity FindIntermission()
{
	entity spot;
	float cyc;

// look for info_intermission first
	spot = find (world, classname, "info_intermission");
	if (spot)
	{       // pick a random one
		cyc = random() * 4;
		while (cyc > 1)
		{
			spot = find (spot, classname, "info_intermission");
			if (!spot)
				spot = find (spot, classname, "info_intermission");
			cyc = cyc - 1;
		}
		return spot;
	}

// then look for the start position
	spot = find (world, classname, "info_player_start");
	if (spot)
		return spot;
	
	objerror ("FindIntermission: no spot");
};

void GotoNextMap()
{
	string newmap;

//ZOID: 12-13-96, samelevel is overloaded, only 1 works for same level

	//if (cvar("samelevel") == 1)     // if samelevel is set, stay on same level
		//changelevel (mapname);
	//else
	{
		// configurable map lists, see if the current map exists as a
		// serverinfo/localinfo var
		newmap = infokey(world, mapname);
		if (newmap != "")
			changelevel (newmap);
		else
			changelevel (nextmap);
	}
};

/*
============
IntermissionThink

When the player presses attack or jump, change to the next level
============
*/
void IntermissionThink(edict_t *self)
{
	if (time < intermission_exittime)
		return;

	if (!self->v.button0 && !self->v.button1 && !self->v.button2)
		return;
	
	GotoNextMap ();
};

/*
============
execute_changelevel

The global "nextmap" has been set previously.
Take the players to the intermission spot
============
*/
void execute_changelevel()
{
	entity    pos;

	intermission_running = 1;
	
// enforce a wait time before allowing changelevel
	intermission_exittime = time + 5;

	pos = FindIntermission ();

// play intermission music
	WriteByte (MSG_ALL, SVC_CDTRACK);
	WriteByte (MSG_ALL, 3);

	WriteByte (MSG_ALL, SVC_INTERMISSION);
	WriteCoord (MSG_ALL, pos.origin_x);
	WriteCoord (MSG_ALL, pos.origin_y);
	WriteCoord (MSG_ALL, pos.origin_z);
	WriteAngle (MSG_ALL, pos.mangle_x);
	WriteAngle (MSG_ALL, pos.mangle_y);
	WriteAngle (MSG_ALL, pos.mangle_z);
	
	other = find (world, classname, "player");
	while (other != world)
	{
		other->v.takedamage = DAMAGE_NO;
		other->v.solid = SOLID_NOT;
		other->v.movetype = MOVETYPE_NONE;
		other->v.modelindex = 0;
		other = find (other, classname, "player");
	};
};

void changelevel_touch()
{
	entity    pos;

	if (other->v.classname != "player")
		return;

// if "noexit" is set, blow up the player trying to leave
//ZOID, 12-13-96, noexit isn't supported in QW.  Overload samelevel
//      if ((cvar("noexit") == 1) || ((cvar("noexit") == 2) && (mapname != "start")))
	if ((cvar("samelevel") == 2) || ((cvar("samelevel") == 3) && (mapname != "start")))
	{
		T_Damage (other, self, self, 50000);
		return;
	}

	bprint (PRINT_HIGH, other->v.netname);
	bprint (PRINT_HIGH," exited the level\n");
	
	nextmap = self->v.map;

	SUB_UseTargets ();

	self->v.touch = SUB_Null;

// we can't move people right now, because touch functions are called
// in the middle of C movement code, so set a think time to do it
	self->v.think = execute_changelevel;
	self->v.nextthink = time + 0.1;
};

/*QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION
When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
*/
void trigger_changelevel(edict_t *self)
{
	if (!self->v.map)
		objerror ("chagnelevel trigger doesn't have map");
	
	InitTrigger ();
	self->v.touch = changelevel_touch;
};


/*
=============================================================================

				PLAYER GAME EDGE FUNCTIONS

=============================================================================
*/

float CheckSpawnPoint(vector v)
{
	return FALSE;
};

/*
============
SelectSpawnPoint

Returns the entity to spawn at
============
*/
entity SelectSpawnPoint()
{
	entity spot, newspot, thing;
	float   numspots, totalspots;
	float   rnum, pcount;
	float   rs;
	entity spots;

	numspots = 0;
	totalspots = 0;

// testinfo_player_start is only found in regioned levels
	spot = find (world, classname, "testplayerstart");
	if (spot)
		return spot;
		
// choose a info_player_deathmatch point

// ok, find all spots that don't have players nearby

	spots = world;
	spot = find (world, classname, "info_player_deathmatch");       
	while (spot)
	{
		totalspots = totalspots + 1;

		thing=findradius(spot->v.origin, 84);
		pcount=0;               
		while (thing)
		{
			if (thing->v.classname == "player")
				pcount=pcount + 1;                      
			thing=thing->v.chain;      
		}
		if (pcount == 0) {
			spot->v.goalentity = spots;
			spots = spot;
			numspots = numspots + 1;
		}

		// Get the next spot in the chain
		spot = find (spot, classname, "info_player_deathmatch");                
	}
	totalspots=totalspots - 1;
	if (!numspots) {
		// ack, they are all full, just pick one at random
//		bprint (PRINT_HIGH, "Ackk! All spots are full. Selecting random spawn spot\n");
		totalspots = rint((random() * totalspots));
		spot = find (world, classname, "info_player_deathmatch");       
		while (totalspots > 0) {
			totalspots = totalspots - 1;
			spot = find (spot, classname, "info_player_deathmatch");
		}
		return spot;
	}
		
// We now have the number of spots available on the map in numspots

	// Generate a random number between 1 and numspots

	numspots = numspots - 1;
	
	numspots = rint((random() * numspots ) );

	spot = spots;
	while (numspots > 0) {
		spot = spot->v.goalentity;
		numspots = numspots - 1;
	}
	return spot;

};

void DecodeLevelParms();
void PlayerDie();

/*
===========
ValidateUser


============
*/
float ValidateUser(entity e)
{
/*
	local string    s;
	local string    userclan;
	local float     rank, rankmin, rankmax;

//
// if the server has set "clan1" and "clan2", then it
// is a clan match that will allow only those two clans in
//
	s = serverinfo("clan1");
	if (s)
	{
		userclan = masterinfo(e,"clan");
		if (s == userclan)
			return true;
		s = serverinfo("clan2");
		if (s == userclan)
			return true;
		return false;
	}

//
// if the server has set "rankmin" and/or "rankmax" then
// the users rank must be between those two values
//
	s = masterinfo (e, "rank");
	rank = stof (s);

	s = serverinfo("rankmin");
	if (s)
	{
		rankmin = stof (s);
		if (rank < rankmin)
			return false;
	}
	s = serverinfo("rankmax");
	if (s)
	{
		rankmax = stof (s);
		if (rankmax < rank)
			return false;
	}

	return true;
*/
};


/*
=============================================================================

				QUAKED FUNCTIONS

=============================================================================
*/


/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 24)
The normal starting point for a level.
*/
void info_player_start()
{
};


/*QUAKED info_player_start2 (1 0 0) (-16 -16 -24) (16 16 24)
Only used on start map for the return point from an episode.
*/
void info_player_start2()
{
};

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 24)
potential spawning position for deathmatch games
*/
void info_player_deathmatch()
{
};

/*QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 24)
potential spawning position for coop games
*/
void info_player_coop()
{
};

/*
===============================================================================

RULES

===============================================================================
*/

/*
go to the next level for deathmatch
*/
void NextLevel()
{
	entity o;
	string newmap;

	if (nextmap != "")
		return; // already done

	if (mapname == "start")
	{
		o = spawn();
		o->v.map = mapname;
	}
	else
	{
		// find a trigger changelevel
		o = find(world, classname, "trigger_changelevel");
		if (!o || mapname == "start")
		{       // go back to same map if no trigger_changelevel
			o = spawn();
			o->v.map = mapname;
		}
	}

	nextmap = o->v.map;

	if (o->v.nextthink < time)
	{
		o->v.think = execute_changelevel;
		o->v.nextthink = time + 0.1;
	}
};

/*
============
CheckRules

Exit deathmatch games upon conditions
============
*/
void CheckRules()
{       
	if (timelimit && time >= timelimit)
		NextLevel ();
	
	if (fraglimit && self.frags >= fraglimit)
		NextLevel ();
};

//============================================================================

void PlayerDeathThink(edict_t *self)
{
	entity    old_self;
	float             forward;

	if ((self->v.flags & FL_ONGROUND))
	{
		forward = vlen (self->v.velocity);
		forward = forward - 20;
		if (forward <= 0)
			self->v.velocity = '0 0 0';
		else    
			self->v.velocity = forward * normalize(self->v.velocity);
	}

// wait for all buttons released
	if (self->v.deadflag == DEAD_DEAD)
	{
		if (self->v.button2 || self->v.button1 || self->v.button0)
			return;
		self->v.deadflag = DEAD_RESPAWNABLE;
		return;
	}

// wait for any button down
	if (!self->v.button2 && !self->v.button1 && !self->v.button0)
		return;

	self->v.button0 = 0;
	self->v.button1 = 0;
	self->v.button2 = 0;
	respawn(self);
};

void PlayerJump(edict_t *self)
{
	vector start, end;

	if (self->v.flags & FL_WATERJUMP)
		return;
	
	if (self->v.waterlevel >= 2)
	{
// play swiming sound
		if (self->v.swim_flag < time)
		{
			self->v.swim_flag = time + 1;
			if (random() < 0.5)
				gpEngine->pfnEmitSound(self, CHAN_BODY, "misc/water1.wav", 1, ATTN_NORM);
			else
				gpEngine->pfnEmitSound(self, CHAN_BODY, "misc/water2.wav", 1, ATTN_NORM);
		}

		return;
	}

	if (!(self->v.flags & FL_ONGROUND))
		return;

	if ( !(self->v.flags & FL_JUMPRELEASED) )
		return;         // don't pogo stick

	self->v.flags = self->v.flags - (self->v.flags & FL_JUMPRELEASED);       
	self->v.button2 = 0;

// player jumping sound
	gpEngine->pfnEmitSound(self, CHAN_BODY, "player/plyrjmp8.wav", 1, ATTN_NORM);
};


/*
===========
WaterMove

============
*/
.float  dmgtime;

void WaterMove(edict_t *self)
{
//dprint (ftos(self.waterlevel));
	if (self->v.movetype == MOVETYPE_NOCLIP)
		return;
	if (self->v.health < 0)
		return;

	if (self->v.waterlevel != 3)
	{
		if (self->v.air_finished < time)
			gpEngine->pfnEmitSound(self, CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM);
		else if (self->v.air_finished < time + 9)
			gpEngine->pfnEmitSound(self, CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM);
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
			T_Damage (self, world, world, self->v.dmg);
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

void CheckWaterJump(edict_t *self)
{
	vector start, end;

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
void CheckPowerups(edict_t *self)
{
	if (self->v.health <= 0)
		return;

// invisibility
	if (self->v.invisible_finished)
	{
// sound and screen flash when items starts to run out
		if (self->v.invisible_sound < time)
		{
			gpEngine->pfnEmitSound(self, CHAN_AUTO, "items/inv3.wav", 0.5, ATTN_IDLE);
			self->v.invisible_sound = time + ((random() * 3) + 1);
		}


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

/*
===========
ClientObituary

called when a player dies
============
*/
void ClientObituary(entity targ, entity attacker)
{
	float rnum;
	string deathstring, deathstring2;
	string s;
	string  attackerteam, targteam;

	rnum = random();
	//ZOID 12-13-96: self.team doesn't work in QW.  Use keys
	attackerteam = infokey(attacker, "team");
	targteam = infokey(targ, "team");

	if (targ->v.classname == "player")
	{
		if (deathmatch > 3)	
		{
			if (targ->v.deathtype == "selfwater")
			{
				bprint (PRINT_MEDIUM, targ->v.netname);
				bprint (PRINT_MEDIUM," electrocutes himself.\n ");
				targ->v.frags -= 1;
				return;
			};
		};

		if (attacker->v.classname == "teledeath")
		{
			bprint (PRINT_MEDIUM,targ->v.netname);
			bprint (PRINT_MEDIUM," was telefragged by ");
			bprint (PRINT_MEDIUM,attacker->v.owner.netname);
			bprint (PRINT_MEDIUM,"\n");
			logfrag (attacker->v.owner, targ);

			attacker->v.owner.frags = attacker->v.owner.frags + 1;
			return;
		};

		if (attacker->v.classname == "teledeath2")
		{
			bprint (PRINT_MEDIUM,"Satan's power deflects ");
			bprint (PRINT_MEDIUM,targ->v.netname);
			bprint (PRINT_MEDIUM,"'s telefrag\n");

			targ->v.frags -= 1;
			logfrag (targ, targ);
			return;
		};

		// double 666 telefrag (can happen often in deathmatch 4)
		if (attacker->v.classname == "teledeath3") 
		{
			bprint (PRINT_MEDIUM,targ->v.netname);
			bprint (PRINT_MEDIUM," was telefragged by ");
			bprint (PRINT_MEDIUM,attacker->v.owner->v.netname);
			bprint (PRINT_MEDIUM, "'s Satan's power\n");
			targ->v.frags = targ->v.frags - 1;
			logfrag (targ, targ);
			return;
		};

		if (targ->v.deathtype == "squish")
		{
			if (teamplay && targteam == attackerteam && attackerteam != "" && targ != attacker)
			{
				logfrag (attacker, attacker);
				attacker->v.frags = attacker->v.frags - 1; 
				bprint (PRINT_MEDIUM,attacker->v.netname);
				bprint (PRINT_MEDIUM," squished a teammate\n");
				return;
			}
			else if (attacker->v.classname == "player" && attacker != targ)
			{
				bprint (PRINT_MEDIUM, attacker->v.netname);
				bprint (PRINT_MEDIUM," squishes ");
				bprint (PRINT_MEDIUM,targ->v.netname);
				bprint (PRINT_MEDIUM,"\n");
				logfrag (attacker, targ);
				attacker->v.frags = attacker->v.frags + 1;
				return;
			}
			else
			{
				logfrag (targ, targ);
				targ->v.frags -= 1;            // killed self
				bprint (PRINT_MEDIUM,targ->v.netname);
				bprint (PRINT_MEDIUM," was squished\n");
				return;
			};
		};

		if (attacker->v.classname == "player")
		{
			if (targ == attacker)
			{
				// killed self
				logfrag (attacker, attacker);
				attacker->v.frags = attacker->v.frags - 1;
				bprint (PRINT_MEDIUM,targ->v.netname);
				if (targ->v.deathtype == "grenade")
					bprint (PRINT_MEDIUM," tries to put the pin back in\n");
				else if (targ->v.deathtype == "rocket")
					bprint (PRINT_MEDIUM," becomes bored with life\n");
				else if (targ->v.weapon == 64 && targ->v.waterlevel > 1)
				{
					if (targ->v.watertype == CONTENT_SLIME)
						bprint (PRINT_MEDIUM," discharges into the slime\n");
					else if (targ->v.watertype == CONTENT_LAVA)
						bprint (PRINT_MEDIUM," discharges into the lava\n");
					else
						bprint (PRINT_MEDIUM," discharges into the water.\n");
				}
				else
					bprint (PRINT_MEDIUM," becomes bored with life\n");
				return;
			}
			else if ( (teamplay == 2) && (targteam == attackerteam) &&
				(attackerteam != "") )
			{
				if (rnum < 0.25)
					deathstring = " mows down a teammate\n";
				else if (rnum < 0.50)
					deathstring = " checks his glasses\n";
				else if (rnum < 0.75)
					deathstring = " gets a frag for the other team\n";
				else
					deathstring = " loses another friend\n";
				bprint (PRINT_MEDIUM, attacker->v.netname);
				bprint (PRINT_MEDIUM, deathstring);
				attacker->v.frags -= 1;
				//ZOID 12-13-96:  killing a teammate logs as suicide
				logfrag (attacker, attacker);
				return;
			}
			else
			{
				logfrag (attacker, targ);
				attacker->v.frags += 1;

				rnum = attacker->v.weapon;
				if (targ->v.deathtype == "nail")
				{
					deathstring = " was nailed by ";
					deathstring2 = "\n";
				}
				else if (targ->v.deathtype == "supernail")
				{
					deathstring = " was punctured by ";
					deathstring2 = "\n";
				}
				else if (targ->v.deathtype == "grenade")
				{
					deathstring = " eats ";
					deathstring2 = "'s pineapple\n";
					if (targ->v.health < -40)
					{
						deathstring = " was gibbed by ";
						deathstring2 = "'s grenade\n";
					}
				}
				else if (targ->v.deathtype == "rocket")
				{
					if (attacker->v.super_damage_finished > 0 && targ->v.health < -40)
					{
						rnum = random();
						if (rnum < 0.3)
							deathstring = " was brutalized by ";
						else if (rnum < 0.6)
							deathstring = " was smeared by ";
						else
						{
							bprint (PRINT_MEDIUM, attacker->v.netname);
							bprint (PRINT_MEDIUM, " rips ");
							bprint (PRINT_MEDIUM, targ->v.netname);
							bprint (PRINT_MEDIUM, " a new one\n");
							return;
						}
						deathstring2 = "'s quad rocket\n";
					}
					else
					{
						deathstring = " rides ";
						deathstring2 = "'s rocket\n";
						if (targ->v.health < -40)
						{
							deathstring = " was gibbed by ";
							deathstring2 = "'s rocket\n" ;
						}
					}
				}
				else if (rnum == IT_AXE)
				{
					deathstring = " was ax-murdered by ";
					deathstring2 = "\n";
				}
				else if (rnum == IT_SHOTGUN)
				{
					deathstring = " chewed on ";
					deathstring2 = "'s boomstick\n";
				}
				else if (rnum == IT_SUPER_SHOTGUN)
				{
					deathstring = " ate 2 loads of ";
					deathstring2 = "'s buckshot\n";
				}
				else if (rnum == IT_LIGHTNING)
				{
					deathstring = " accepts ";
					if (attacker->v.waterlevel > 1)
						deathstring2 = "'s discharge\n";
					else
						deathstring2 = "'s shaft\n";
				}
				bprint (PRINT_MEDIUM,targ->v.netname);
				bprint (PRINT_MEDIUM,deathstring);
				bprint (PRINT_MEDIUM,attacker->v.netname);
				bprint (PRINT_MEDIUM,deathstring2);
			}
			return;
		}
		else
		{
			logfrag (targ, targ);
			targ->v.frags -= 1;            // killed self
			rnum = targ->v.watertype;

			bprint (PRINT_MEDIUM,targ->v.netname);
			if (rnum == -3)
			{
				if (random() < 0.5)
					bprint (PRINT_MEDIUM," sleeps with the fishes\n");
				else
					bprint (PRINT_MEDIUM," sucks it down\n");
				return;
			}
			else if (rnum == -4)
			{
				if (random() < 0.5)
					bprint (PRINT_MEDIUM," gulped a load of slime\n");
				else
					bprint (PRINT_MEDIUM," can't exist on slime alone\n");
				return;
			}
			else if (rnum == -5)
			{
				if (targ->v.health < -15)
				{
					bprint (PRINT_MEDIUM," burst into flames\n");
					return;
				}
				if (random() < 0.5)
					bprint (PRINT_MEDIUM," turned into hot slag\n");
				else
					bprint (PRINT_MEDIUM," visits the Volcano God\n");
				return;
			}

			if (attacker->v.classname == "explo_box")
			{
				bprint (PRINT_MEDIUM," blew up\n");
				return;
			}
			if (targ->v.deathtype == "falling")
			{
				bprint (PRINT_MEDIUM," fell to his death\n");
				return;
			}
			if (targ->v.deathtype == "nail" || targ->v.deathtype == "supernail")
			{
				bprint (PRINT_MEDIUM," was spiked\n");
				return;
			}
			if (targ->v.deathtype == "laser")
			{
				bprint (PRINT_MEDIUM," was zapped\n");
				return;
			}
			if (attacker->v.classname == "fireball")
			{
				bprint (PRINT_MEDIUM," ate a lavaball\n");
				return;
			}
			if (attacker->v.classname == "trigger_changelevel")
			{
				bprint (PRINT_MEDIUM," tried to leave\n");
				return;
			}

			bprint (PRINT_MEDIUM," died\n");
		}
	}
};