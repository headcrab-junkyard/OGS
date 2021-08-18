

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

void SetChangeParms(edict_t *self)
{
	if(self->GetHealth() <= 0)
	{
		SetNewParms(self);
		return;
	};
 
	// remove items
	self->v.items = self->v.items - (self->v.items & (IT_KEY1 | IT_KEY2 | IT_INVISIBILITY | IT_INVULNERABILITY | IT_SUIT | IT_QUAD) );
	
	// cap super health
	if (self->GetHealth() > 100)
		self->SetHealth(100);
	if (self->GetHealth() < 50)
		self->SetHealth(50);
	gpGlobals->parm1 = self->v.items;
	gpGlobals->parm2 = self->GetHealth();
	gpGlobals->parm3 = self->GetArmorValue();
	if (self->v.ammo_shells < 25)
		gpGlobals->parm4 = 25;
	else
		gpGlobals->parm4 = self->v.ammo_shells;
	gpGlobals->parm5 = self->v.ammo_nails;
	gpGlobals->parm6 = self->v.ammo_rockets;
	gpGlobals->parm7 = self->v.ammo_cells;
	gpGlobals->parm8 = self->v.weapon;
	gpGlobals->parm9 = self->GetArmorType() * 100;
};

void SetNewParms()
{
	gpGlobals->parm1 = IT_SHOTGUN | IT_AXE;
	gpGlobals->parm2 = 100;
	gpGlobals->parm3 = 0;
	gpGlobals->parm4 = 25;
	gpGlobals->parm5 = 0;
	gpGlobals->parm6 = 0;
	gpGlobals->parm7 = 0;
	gpGlobals->parm8 = 1;
	gpGlobals->parm9 = 0;
};

void DecodeLevelParms(edict_t *self)
{
	if(serverflags)
	{
		if(world->GetModel() == "maps/start.bsp")
			SetNewParms(); // take away all stuff on starting new episode
	};
	
	self->v.items = parm1;
	self->SetHealth(parm2);
	self->SetArmorValue(parm3);
	self->v.ammo_shells = parm4;
	self->v.ammo_nails = parm5;
	self->v.ammo_rockets = parm6;
	self->v.ammo_cells = parm7;
	self->v.weapon = parm8;
	self->SetArmorType(parm9 * 0.01);
};

/*
============
FindIntermission

Returns the entity to view from
============
*/
CBaseEntity *FindIntermission()
{
	// look for info_intermission first
	CBaseEntity *spot{mpWorld->FindEntityByString(world, "classname", "info_intermission")};
	if(spot)
	{
		// pick a random one
		float cyc{random() * 4};
		while(cyc > 1)
		{
			spot = mpWorld->FindEntityByString(spot, "classname", "info_intermission");
			if(!spot)
				spot = mpWorld->FindEntityByString(spot, "classname", "info_intermission");
			--cyc;
		};
		return spot;
	};

	// then look for the start position
	spot = mpWorld->FindEntityByString(world, "classname", "info_player_start");
	if(spot)
		return spot;
	
	objerror("FindIntermission: no spot");
	//return nullptr;
};

void GotoNextMap()
{
	//ZOID: 12-13-96, samelevel is overloaded, only 1 works for same level
	//if (cvar("samelevel") == 1)     // if samelevel is set, stay on same level
		//mpWorld->ChangeLevel(gpGlobals->mapname);
	//else
	{
		// configurable map lists, see if the current map exists as a
		// serverinfo/localinfo var
		const char *newmap = gpEngine->pfnInfoKeyValue(gpEngine->pfnGetInfoBuffer(world), gpGlobals->mapname);
		if (newmap != "")
			mpWorld->ChangeLevel(newmap);
		else
			mpWorld->ChangeLevel(nextmap);
	}
};

/*
============
IntermissionThink

When the player presses attack or jump, change to the next level
============
*/
void CBasePlayer::IntermissionThink()
{
	if (gpGlobals->time < intermission_exittime)
		return;

	if (!self->button & IN_ATTACK && !self->button & IN_ATTACK2 && !self->button & IN_JUMP)
		return;
	
	GotoNextMap ();
};

/*
=============================================================================

				PLAYER GAME EDGE FUNCTIONS

=============================================================================
*/

bool CheckSpawnPoint(const idVec3 &v)
{
	return false;
};

/*
============
SelectSpawnPoint

Returns the entity to spawn at
============
*/
CBaseEntity *SelectSpawnPoint()
{
	int   numspots = 0, totalspots = 0;
	CBaseEntity *world = ToBaseEntity(gpWorld->GetEntity(0));

// testinfo_player_start is only found in regioned levels
	CBaseEntity *spot = gpWorld->FindEntityByString(world, "classname", "testplayerstart");
	if (spot)
		return spot;
		
// choose a info_player_deathmatch point

// ok, find all spots that don't have players nearby

	CBaseEntity *spots = world;
	spot = gpWorld->FindEntityByString(world, "classname", "info_player_deathmatch");       
	while (spot)
	{
		totalspots++;

		CBaseEntity *thing = nullptr;
		thing = gpWorld->FindEntityInSphere(thing, spot->GetOrigin(), 84);
		int pcount=0;               
		while (thing)
		{
			if (thing->GetClassName() == "player")
				pcount++;                      
			thing=gpWorld->FindEntityInSphere(thing, spot->GetOrigin(), 84);
		}
		if (pcount == 0) {
			spot->SetGoal(spots);
			spots = spot;
			numspots++;
		}

		// Get the next spot in the chain
		spot = gpWorld->FindEntityByString(spot, "classname", "info_player_deathmatch");                
	}
	totalspots--;
	if (!numspots) {
		// ack, they are all full, just pick one at random
//		bprint (PRINT_HIGH, "Ackk! All spots are full. Selecting random spawn spot\n");
		totalspots = rint((random() * totalspots));
		spot = gpWorld->FindEntityByString(world, "classname", "info_player_deathmatch");       
		while (totalspots > 0) {
			totalspots--;
			spot = gpWorld->FindEntityByString(spot, "classname", "info_player_deathmatch");
		}
		return spot;
	}
		
// We now have the number of spots available on the map in numspots

	// Generate a random number between 1 and numspots

	numspots--;
	
	numspots = rint((random() * numspots ) );

	spot = spots;
	while (numspots > 0) {
		spot = spot->GetGoalEntity();
		numspots--;
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
float ValidateUser(edict_t *e)
{
/*
	string_t    s;
	string_t    userclan;
	float     rank, rankmin, rankmax;

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
	edict_t *o;
	string_t newmap;

	if (nextmap != "")
		return; // already done

	if (gpGlobals->mapname == "start")
	{
		o = gpEngine->pfnCreateEntity();
		o->v.map = gpGlobals->mapname;
	}
	else
	{
		// find a trigger changelevel
		o = find(world, classname, "trigger_changelevel");
		if (!o || gpGlobals->mapname == "start")
		{
			// go back to same map if no trigger_changelevel
			o = gpEngine->pfnCreateEntity();
			o->v.map = gpGlobals->mapname;
		};
	};

	nextmap = o->v.map;

	if (o->GetNextThink() < gpGlobals->time)
	{
		o->SetThinkCallback(execute_changelevel);
		o->SetNextThink(gpGlobals->time + 0.1);
	};
};

/*
============
CheckRules

Exit deathmatch games upon conditions
============
*/
void CheckRules(CBasePlayer *self)
{
	if (timelimit && gpGlobals->time >= timelimit)
		NextLevel ();
	
	if (fraglimit && self->GetFrags() >= fraglimit)
		NextLevel ();
};

//============================================================================

/*
===========
ClientObituary

called when a player dies
============
*/
void ClientObituary(CBaseEntity *targ, CBaseEntity *attacker)
{
	float rnum;
	string deathstring, deathstring2;
	string s;
	string  attackerteam, targteam;

	rnum = random();
	//ZOID 12-13-96: self.team doesn't work in QW.  Use keys
	attackerteam = infokey(attacker, "team");
	targteam = infokey(targ, "team");

	if (targ->GetClassName() == "player")
	{
		if (deathmatch > 3)	
		{
			if (targ->v.deathtype == "selfwater")
			{
				bprint (PRINT_MEDIUM, targ->v.netname);
				bprint (PRINT_MEDIUM," electrocutes himself.\n ");
				targ->AddFrags(-1);
				return;
			};
		};

		if (attacker->GetClassName() == "teledeath")
		{
			bprint (PRINT_MEDIUM,targ->v.netname);
			bprint (PRINT_MEDIUM," was telefragged by ");
			bprint (PRINT_MEDIUM,attacker->GetOwner()->netname);
			bprint (PRINT_MEDIUM,"\n");
			logfrag (attacker->GetOwner(), targ);

			attacker->GetOwner()->AddFrags(1);
			return;
		};

		if (attacker->GetClassName() == "teledeath2")
		{
			bprint (PRINT_MEDIUM,"Satan's power deflects ");
			bprint (PRINT_MEDIUM,targ->v.netname);
			bprint (PRINT_MEDIUM,"'s telefrag\n");

			targ->AddFrags(-1);
			logfrag (targ, targ);
			return;
		};

		// double 666 telefrag (can happen often in deathmatch 4)
		if (attacker->GetClassName() == "teledeath3") 
		{
			bprint (PRINT_MEDIUM,targ->v.netname);
			bprint (PRINT_MEDIUM," was telefragged by ");
			bprint (PRINT_MEDIUM,attacker->GetOwner()->v.netname);
			bprint (PRINT_MEDIUM, "'s Satan's power\n");
			targ->AddFrags(-1);
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
			else if (attacker->GetClassName() == "player" && attacker != targ)
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
				targ->AddFrags(-1);            // killed self
				bprint (PRINT_MEDIUM,targ->v.netname);
				bprint (PRINT_MEDIUM," was squished\n");
				return;
			};
		};

		if (attacker->GetClassName() == "player")
		{
			if (targ == attacker)
			{
				// killed self
				logfrag (attacker, attacker);
				attacker->AddFrags(-1);
				bprint (PRINT_MEDIUM,targ->v.netname);
				if (targ->v.deathtype == "grenade")
					bprint (PRINT_MEDIUM," tries to put the pin back in\n");
				else if (targ->v.deathtype == "rocket")
					bprint (PRINT_MEDIUM," becomes bored with life\n");
				else if (targ->v.weapon == 64 && targ->GetWaterLevel() > 1)
				{
					if (targ->GetWaterType() == CONTENT_SLIME)
						bprint (PRINT_MEDIUM," discharges into the slime\n");
					else if (targ->GetWaterType() == CONTENT_LAVA)
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
				attacker->AddFrags(-1);
				//ZOID 12-13-96:  killing a teammate logs as suicide
				logfrag (attacker, attacker);
				return;
			}
			else
			{
				logfrag (attacker, targ);
				attacker->AddFrags(1);

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
					if (targ->GetHealth() < -40)
					{
						deathstring = " was gibbed by ";
						deathstring2 = "'s grenade\n";
					}
				}
				else if (targ->v.deathtype == "rocket")
				{
					if (attacker->v.super_damage_finished > 0 && targ->GetHealth() < -40)
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
						if (targ->GetHealth() < -40)
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
					if (attacker->GetWaterLevel() > 1)
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
			targ->AddFrags(-1);            // killed self
			rnum = targ->GetWaterType();

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
				if (targ->GetHealth() < -15)
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

			if (attacker->GetClassName() == "explo_box")
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
			if (attacker->GetClassName() == "fireball")
			{
				bprint (PRINT_MEDIUM," ate a lavaball\n");
				return;
			}
			if (attacker->GetClassName() == "trigger_changelevel")
			{
				bprint (PRINT_MEDIUM," tried to leave\n");
				return;
			}

			bprint (PRINT_MEDIUM," died\n");
		}
	}
};