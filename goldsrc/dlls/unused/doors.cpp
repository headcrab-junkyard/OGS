/*
	doors.qc

	door functions

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

const float DOOR_START_OPEN = 1;
const float DOOR_DONT_LINK = 4;
const float DOOR_GOLD_KEY = 8;
const float DOOR_SILVER_KEY = 16;
const float DOOR_TOGGLE = 32;

/*

Doors are similar to buttons, but can spawn a fat trigger field around them
to open without a touch, and they link together to form simultanious
double/quad doors.
 
Door.owner is the master door.  If there is only one door, it points to itself.
If multiple doors, all will point to a single one.

Door.enemy chains from the master door through all doors linked in the chain.

*/

/*
=============================================================================

THINK FUNCTIONS

=============================================================================
*/

void CDoor::go_down();
void CDoor::go_up();

void CDoor::Blocked(CBaseEntity *other)
{
	other->deathtype = "squish";
	other->TakeDamage (self, self->goalentity, self->dmg);

// if a door has a negative wait, it would never come back if blocked,
// so let it just squash the object to death real fast
	if (self->wait >= 0)
	{
		if (self->state == STATE_DOWN)
			this->go_up ();
		else
			this->go_down ();
	};
};

void CDoor::hit_top()
{
	self->EmitSound (CHAN_NO_PHS_ADD+CHAN_VOICE, self->noise1, 1, ATTN_NORM);
	self->state = STATE_TOP;
	if (self->spawnflags & DOOR_TOGGLE)
		return;         // don't come down automatically
	self->SetThinkCallback(this->go_down);
	self->SetNextThink(self->ltime + self->wait);
};

void CDoor::hit_bottom()
{
	gpEngine->pfnEmitSound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self.noise1, 1, ATTN_NORM);
	self.state = STATE_BOTTOM;
};

void CDoor::go_down()
{
	self->EmitSound (CHAN_VOICE, self->noise2, 1, ATTN_NORM);
	if (self->GetMaxHealth())
	{
		self->takedamage = DAMAGE_YES;
		self->SetHealth(self->GetMaxHealth());
	};
	
	self->state = STATE_DOWN;
	SUB_CalcMove (self->pos1, self->speed, this->hit_bottom);
};

void CDoor::go_up()
{
	if (self.state == STATE_UP)
		return;         // already going up

	if (self.state == STATE_TOP)
	{       // reset top wait time
		self.nextthink = self.ltime + self.wait;
		return;
	};
	
	gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise2, 1, ATTN_NORM);
	self.state = STATE_UP;
	SUB_CalcMove (self.pos2, self.speed, this->hit_top);

	SUB_UseTargets();
};


/*
=============================================================================

ACTIVATION FUNCTIONS

=============================================================================
*/

void CDoor::Fire(CBaseEntity *activator)
{
	entity    oself;
	entity    starte;

	if (self.owner != self)
		objerror ("door_fire: self.owner != self");

// play use key sound

	if (self.items)
		gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise4, 1, ATTN_NORM);

	self.message = string_null;             // no more message
	oself = self;

	if (self.spawnflags & DOOR_TOGGLE)
	{
		if (self.state == STATE_UP || self.state == STATE_TOP)
		{
			starte = self;
			do
			{
				this->go_down ();
				self = self.enemy;
			} while ( (self != starte) && (self != world) );
			self = oself;
			return;
		}
	}
	
// trigger all paired doors
	starte = self;
	
	do
	{
		self.goalentity = activator;		// Who fired us
		this->go_up ();
		self = self.enemy;
	} while ( (self != starte) && (self != world) );
	self = oself;
};

void CDoor::Use(CBaseEntity *other)
{
	entity oself;

	self.message = "";                      // door message are for touch only
	self.owner.message = "";        
	self.enemy.message = "";

	oself = self;
	self = self.owner;
	Fire(other);
	self = oself;
};

void CDoor::trigger_touch(CBaseEntity *other)
{
	if (other.health <= 0)
		return;

	if (time < self.attack_finished)
		return;
	self.attack_finished = time + 1;

	activator = other;

	self = self.owner;
	Use ();
};

void CDoor::Killed()
{
	entvars_t *oself = self;
	self = self->GetOwner();
	self->SetHealth(self->GetMaxHealth());
	self->takedamage = DAMAGE_NO;    // wil be reset upon return
	Use();
	self = oself;
};

/*
================
door_touch

Prints messages and opens key doors
================
*/
void CDoor::Touch(CBaseEntity *other)
{
	if (other.classname != "player")
		return;
	if (self.owner.attack_finished > time)
		return;

	self.owner.attack_finished = time + 2;

	if (self.owner.message != "")
	{
		centerprint (other, self.owner.message);
		gpEngine->pfnEmitSound (other, CHAN_VOICE, "misc/talk.wav", 1, ATTN_NORM);
	};
	
// key door stuff
	if (!self.items)
		return;

// FIXME: blink key on player's status bar
	if ( (self.items & other.items) != self.items )
	{
		if (self.owner.items == IT_KEY1)
		{
			if (world.worldtype == 2)
			{
				centerprint (other, "You need the silver keycard");
				gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
			}
			else if (world.worldtype == 1)
			{
				centerprint (other, "You need the silver runekey");
				gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
			}
			else if (world.worldtype == 0)
			{
				centerprint (other, "You need the silver key");
				gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
			};
		}
		else
		{
			if (world.worldtype == 2)
			{
				centerprint (other, "You need the gold keycard");
				gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
			}
			else if (world.worldtype == 1)
			{
				centerprint (other, "You need the gold runekey");
				gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);                    
			}
			else if (world.worldtype == 0)
			{
				centerprint (other, "You need the gold key");
				gpEngine->pfnEmitSound (self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
			};
		};
		return;
	};

	other.items = other.items - self.items;
	self.touch = SUB_Null;
	if (self.enemy)
		self.enemy.touch = SUB_Null;    // get paired door
	Use ();
};

/*
=============================================================================

SPAWNING FUNCTIONS

=============================================================================
*/

entity spawn_field(const idVec3 &fmins, const idVec3 &fmaxs)
{
	entity    trigger;
	idVec3 t1, t2;

	trigger = spawn();
	trigger.movetype = MOVETYPE_NONE;
	trigger.solid = SOLID_TRIGGER;
	trigger.owner = self;
	trigger.touch = door_trigger_touch;

	t1 = fmins;
	t2 = fmaxs;
	gpEngine->pfnSetSize (trigger, t1 - '60 60 8', t2 + '60 60 8');
	return (trigger);
};

bool EntitiesTouching(edict_t *e1, edict_t *e2)
{
	if (e1->v.mins_x > e2->v.maxs_x)
		return false;
	if (e1->v.mins_y > e2->v.maxs_y)
		return false;
	if (e1->v.mins_z > e2->v.maxs_z)
		return false;
	if (e1->v.maxs_x < e2->v.mins_x)
		return false;
	if (e1->v.maxs_y < e2->v.mins_y)
		return false;
	if (e1->v.maxs_z < e2->v.mins_z)
		return false;
	return true;
};


/*
=============
LinkDoors


=============
*/
void LinkDoors()
{
	entvars_t t, starte;
	idVec3 cmins, cmaxs;

	if (self.enemy)
		return;         // already linked by another door
	if (self.spawnflags & 4)
	{
		self.owner = self.enemy = self;
		return;         // don't want to link this door
	};

	cmins = self.mins;
	cmaxs = self.maxs;
	
	starte = self;
	t = self;
	
	do
	{
		self.owner = starte;                    // master door

		if (self.health)
			starte.health = self.health;
		if (self.targetname)
			starte.targetname = self.targetname;
		if (self.message != "")
			starte.message = self.message;

		t = find (t, classname, self.classname);        
		if (!t)
		{
			self.enemy = starte;            // make the chain a loop

		// shootable, fired, or key doors just needed the owner/enemy links,
		// they don't spawn a field
	
			self = self.owner;

			if (self.health)
				return;
			if (self.targetname)
				return;
			if (self.items)
				return;

			self.owner.trigger_field = spawn_field(cmins, cmaxs);

			return;
		};

		if (EntitiesTouching(self,t))
		{
			if (t.enemy)
				objerror ("cross connected doors");
			
			self.enemy = t;
			self = t;

			if (t.mins_x < cmins_x)
				cmins_x = t.mins_x;
			if (t.mins_y < cmins_y)
				cmins_y = t.mins_y;
			if (t.mins_z < cmins_z)
				cmins_z = t.mins_z;
			if (t.maxs_x > cmaxs_x)
				cmaxs_x = t.maxs_x;
			if (t.maxs_y > cmaxs_y)
				cmaxs_y = t.maxs_y;
			if (t.maxs_z > cmaxs_z)
				cmaxs_z = t.maxs_z;
		};
	}
	while(1);
};

/*QUAKED func_door (0 .5 .8) ? START_OPEN x DOOR_DONT_LINK GOLD_KEY SILVER_KEY TOGGLE
if two doors touch, they are assumed to be connected and operate as a unit.

TOGGLE causes the door to wait in both the start and end states for a trigger event.

START_OPEN causes the door to move to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not usefull for touch or takedamage doors).

Key doors are allways wait -1.

"message"       is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"angle"         determines the opening direction
"targetname" if set, no touch field will be spawned and a remote button or trigger field activates the door.
"health"        if set, door must be shot open
"speed"         movement speed (100 default)
"wait"          wait before returning (3 default, -1 = never return)
"lip"           lip remaining at end of move (8 default)
"dmg"           damage to inflict when blocked (2 default)
"sounds"
0)      no sound
1)      stone
2)      base
3)      stone chain
4)      screechy metal
*/

void func_door(edict_t *self)
{
	if (world->v.worldtype == 0)
	{
		gpEngine->pfnPrecacheSound ("doors/medtry.wav");
		gpEngine->pfnPrecacheSound ("doors/meduse.wav");
		self->v.noise3 = "doors/medtry.wav";
		self->v.noise4 = "doors/meduse.wav";
	}
	else if (world->v.worldtype == 1)
	{
		gpEngine->pfnPrecacheSound ("doors/runetry.wav");
		gpEngine->pfnPrecacheSound ("doors/runeuse.wav");
		self->v.noise3 = "doors/runetry.wav";
		self->v.noise4 = "doors/runeuse.wav";
	}
	else if (world->v.worldtype == 2)
	{
		gpEngine->pfnPrecacheSound ("doors/basetry.wav");
		gpEngine->pfnPrecacheSound ("doors/baseuse.wav");
		self->v.noise3 = "doors/basetry.wav";
		self->v.noise4 = "doors/baseuse.wav";
	}
	else
		dprint ("no worldtype set!\n");
	
	if (self->v.sounds == 0)
	{
		gpEngine->pfnPrecacheSound ("misc/null.wav");
		self->v.noise1 = "misc/null.wav";
		self->v.noise2 = "misc/null.wav";
	};
	
	if (self->v.sounds == 1)
	{
		gpEngine->pfnPrecacheSound ("doors/drclos4.wav");
		gpEngine->pfnPrecacheSound ("doors/doormv1.wav");
		self->v.noise1 = "doors/drclos4.wav";
		self->v.noise2 = "doors/doormv1.wav";
	};
	
	if (self->v.sounds == 2)
	{
		gpEngine->pfnPrecacheSound ("doors/hydro1.wav");
		gpEngine->pfnPrecacheSound ("doors/hydro2.wav");
		self->v.noise2 = "doors/hydro1.wav";
		self->v.noise1 = "doors/hydro2.wav";
	};
	
	if (self->v.sounds == 3)
	{
		gpEngine->pfnPrecacheSound ("doors/stndr1.wav");
		gpEngine->pfnPrecacheSound ("doors/stndr2.wav");
		self->v.noise2 = "doors/stndr1.wav";
		self->v.noise1 = "doors/stndr2.wav";
	};
	
	if (self->v.sounds == 4)
	{
		gpEngine->pfnPrecacheSound ("doors/ddoor1.wav");
		gpEngine->pfnPrecacheSound ("doors/ddoor2.wav");
		self->v.noise1 = "doors/ddoor2.wav";
		self->v.noise2 = "doors/ddoor1.wav";
	};

	SetMovedir ();

	self->v.max_health = self->v.health;
	self->v.solid = SOLID_BSP;
	self->v.movetype = MOVETYPE_PUSH;
	
	gpEngine->pfnSetOrigin (self, self->v.origin);  
	gpEngine->pfnSetModel (self, self->v.model);
	
	self->v.classname = "door";

	self->v.blocked = door_blocked;
	self->v.use = door_use;
	
	if (self->v.spawnflags & DOOR_SILVER_KEY)
		self->v.items = IT_KEY1;
	if (self->v.spawnflags & DOOR_GOLD_KEY)
		self->v.items = IT_KEY2;
	
	if (!self->v.speed)
		self->v.speed = 100;
	if (!self->v.wait)
		self->v.wait = 3;
	if (!self->v.lip)
		self->v.lip = 8;
	if (!self->v.dmg)
		self->v.dmg = 2;

	self->v.pos1 = self->v.origin;
	self->v.pos2 = self->v.pos1 + self->v.movedir*(fabs(self->v.movedir*self->v.size) - self->v.lip);

// DOOR_START_OPEN is to allow an entity to be lighted in the closed position
// but spawn in the open position
	if (self->v.spawnflags & DOOR_START_OPEN)
	{
		gpEngine->pfnSetOrigin (self, self->v.pos2);
		self->v.pos2 = self->v.pos1;
		self->v.pos1 = self->v.origin;
	};

	self->v.state = STATE_BOTTOM;

	if (self->v.health)
	{
		self->v.takedamage = DAMAGE_YES;
		self->v.th_die = door_killed;
	};
	
	if (self->v.items)
		self->v.wait = -1;
	
	self->v.touch = door_touch;

// LinkDoors can't be done until all of the doors have been spawned, so
// the sizes can be detected properly.
	self->v.think = LinkDoors;
	self->v.nextthink = self->v.ltime + 0.1;
};

/*
=============================================================================

SECRET DOORS

=============================================================================
*/

void fd_secret_move1();
void fd_secret_move2();
void fd_secret_move3();
void fd_secret_move4();
void fd_secret_move5();
void fd_secret_move6();
void fd_secret_done();

const float SECRET_OPEN_ONCE = 1;             // stays open
const float SECRET_1ST_LEFT = 2;              // 1st move is left of arrow
const float SECRET_1ST_DOWN = 4;              // 1st move is down from arrow
const float SECRET_NO_SHOOT = 8;              // only opened by trigger
const float SECRET_YES_SHOOT = 16;    // shootable even if targeted

// Wait after first movement...
void fd_secret_move1()
{
	self->v.nextthink = self->v.ltime + 1.0;
	self->v.think = fd_secret_move2;
	gpEngine->pfnEmitSound(self, CHAN_VOICE, self->v.noise3, 1, ATTN_NORM);
};

// Start moving sideways w/sound...
void fd_secret_move2()
{
	gpEngine->pfnEmitSound(self, CHAN_VOICE, self.noise2, 1, ATTN_NORM);
	SUB_CalcMove(self.dest2, self.speed, fd_secret_move3);
};

// Wait here until time to go back...
void fd_secret_move3()
{
	gpEngine->pfnEmitSound(self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
	if (!(self.spawnflags & SECRET_OPEN_ONCE))
	{
		self.nextthink = self.ltime + self.wait;
		self.think = fd_secret_move4;
	};
};

// Move backward...
void fd_secret_move4()
{
	gpEngine->pfnEmitSound(self, CHAN_VOICE, self.noise2, 1, ATTN_NORM);
	SUB_CalcMove(self.dest1, self.speed, fd_secret_move5);          
};

// Wait 1 second...
void fd_secret_move5()
{
	self.nextthink = self.ltime + 1.0;
	self.think = fd_secret_move6;
	gpEngine->pfnEmitSound(self, CHAN_VOICE, self.noise3, 1, ATTN_NORM);
};

void fd_secret_move6()
{
	gpEngine->pfnEmitSound(self, CHAN_VOICE, self.noise2, 1, ATTN_NORM);
	SUB_CalcMove(self.oldorigin, self.speed, fd_secret_done);
};

void fd_secret_done()
{
	if (!self.targetname || self.spawnflags&SECRET_YES_SHOOT)
	{
		self.health = 10000;
		self.takedamage = DAMAGE_YES;
		self.th_pain = fd_secret_use;   
		self.th_die = fd_secret_use;
	};
	gpEngine->pfnEmitSound(self, CHAN_NO_PHS_ADD+CHAN_VOICE, self.noise3, 1, ATTN_NORM);
};

/*QUAKED func_door_secret (0 .5 .8) ? open_once 1st_left 1st_down no_shoot always_shoot
Basic secret door. Slides back, then to the side. Angle determines direction.
wait  = # of seconds before coming back
1st_left = 1st move is left of arrow
1st_down = 1st move is down from arrow
always_shoot = even if targeted, keep shootable
t_width = override WIDTH to move back (or height if going down)
t_length = override LENGTH to move sideways
"dmg"           damage to inflict when blocked (2 default)

If a secret door has a targetname, it will only be opened by it's botton or trigger, not by damage.
"sounds"
1) medieval
2) metal
3) base
*/
class CFuncDoorSecret : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Use(CBaseEntity *other) override;
	void Touch(CBaseEntity *other) override;
	void Think() override;
};

LINK_ENTITY_TO_CLASS(func_door_secret, CFuncDoorSecret)

void CFuncDoorSecret::Spawn()
{
	if (self->sounds == 0)
		self->sounds = 3;
	if (self->sounds == 1)
	{
		gpEngine->pfnPrecacheSound ("doors/latch2.wav");
		gpEngine->pfnPrecacheSound ("doors/winch2.wav");
		gpEngine->pfnPrecacheSound ("doors/drclos4.wav");
		self->noise1 = "doors/latch2.wav";
		self->noise2 = "doors/winch2.wav";
		self->noise3 = "doors/drclos4.wav";
	};
	if (self->sounds == 2)
	{
		gpEngine->pfnPrecacheSound ("doors/airdoor1.wav");
		gpEngine->pfnPrecacheSound ("doors/airdoor2.wav");
		self->noise2 = "doors/airdoor1.wav";
		self->noise1 = "doors/airdoor2.wav";
		self->noise3 = "doors/airdoor2.wav";
	};
	if (self->sounds == 3)
	{
		gpEngine->pfnPrecacheSound ("doors/basesec1.wav");
		gpEngine->pfnPrecacheSound ("doors/basesec2.wav");
		self->noise2 = "doors/basesec1.wav";
		self->noise1 = "doors/basesec2.wav";
		self->noise3 = "doors/basesec2.wav";
	};

	if (!self->dmg)
		self->dmg = 2;
		
	// Magic formula...
	self->mangle = self->angles;
	self->SetAngles(idVec3(0));
	self->SetSolidity(SOLID_BSP);
	self->SetMoveType(MOVETYPE_PUSH);
	self->SetClassName("door");
	
	self->SetModel(self->GetModel());
	self->SetOrigin(self->GetOrigin());  
	
	self->SetTouchCallback(secret_touch);
	self->SetBlockedCallback(secret_blocked);
	self->SetSpeed(50);
	self->SetUseCallback(fd_secret_use);
	if ( !self->targetname || self->spawnflags & SECRET_YES_SHOOT)
	{
		self->SetHealth(10000);
		self->takedamage = DAMAGE_YES;
		self->th_pain = fd_secret_use;
	};
	self->oldorigin = self->GetOrigin();
	if (!self->wait)
		self->wait = 5;          // 5 seconds before closing
};

void CFuncDoorSecret::fd_secret_use(CBaseEntity *other)
{
	float temp;
	
	self->SetHealth(10000);

	// exit if still moving around...
	if (self->GetOrigin() != self->oldorigin)
		return;
	
	self->message = string_null;             // no more message

	SUB_UseTargets();                               // fire all targets / killtargets
	
	if (!(self->spawnflags & SECRET_NO_SHOOT))
	{
		self->th_pain = SUB_Null;
		self->takedamage = DAMAGE_NO;
	};
	
	self->SetVelocity(idVec3(0));

	// Make a sound, wait a little...
	
	self->EmitSound(CHAN_VOICE, self->noise1, 1, ATTN_NORM);
	self->SetNextThink(self->ltime + 0.1);

	temp = 1 - (self->spawnflags & SECRET_1ST_LEFT); // 1 or -1
	makevectors(self->mangle);
	
	if (!self->t_width)
	{
		if (self->spawnflags & SECRET_1ST_DOWN)
			self->t_width = fabs(v_up * self->size);
		else
			self->t_width = fabs(v_right * self->size);
	};
		
	if (!self->t_length)
		self->t_length = fabs(v_forward * self->size);

	if (self->spawnflags & SECRET_1ST_DOWN)
		self->dest1 = self->origin - v_up * self->t_width;
	else
		self->dest1 = self->origin + v_right * (self->t_width * temp);
		
	self->dest2 = self->dest1 + v_forward * self->t_length;
	SUB_CalcMove(self->dest1, self->speed, fd_secret_move1);
	self->EmitSound(CHAN_VOICE, self->noise2, 1, ATTN_NORM);
};

void CFuncDoorSecret::secret_blocked(CBaseEntity *other)
{
	if (gpGlobals->time < self->attack_finished)
		return;
	self->attack_finished = gpGlobals->time + 0.5;
	other->deathtype = "squish";
	other->TakeDamage(self, self, self->dmg);
};

/*
================
secret_touch

Prints messages
================
*/
void CFuncDoorSecret::secret_touch(CBaseEntity *other)
{
	if (other->GetClassName() != "player")
		return;
	if (self->attack_finished > gpGlobals->time)
		return;

	self->attack_finished = gpGlobals->time + 2;
	
	if (self->message)
	{
		other->CenterPrint (self->message);
		other->EmitSound(CHAN_BODY, "misc/talk.wav", 1, ATTN_NORM);
	};
};