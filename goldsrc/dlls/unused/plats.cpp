/*
	plats.qc

	platform functions

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

#include "GameEntity.hpp"

class CFuncPlat : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Use(CBaseEntity *apActivator, CBaseEntity *apCaller, UseType aeUseType, float afValue);
	
	void center_touch(CBaseEntity *apOther);
	void outside_touch(CBaseEntity *apOther);
	
	void trigger_use(CBaseEntity *apUser);
	
	void GoUp();
	void GoDown();
	
	void crush();
	
	void HitTop();
	void HitBottom();
};

const float PLAT_LOW_TRIGGER = 1;

void plat_spawn_inside_trigger()
{
	CBaseEntity *trigger;
	idVec3 tmin, tmax;

//
// middle trigger
//	
	trigger = gpEngine->pfnSpawn();
	trigger->SetTouchCallback(plat_center_touch);
	trigger->SetMoveType(MOVETYPE_NONE);
	trigger->SetSolidity(SOLID_TRIGGER);
	trigger->SetEnemy(self);
	
	tmin = self->v.mins + '25 25 0';
	tmax = self->v.maxs - '25 25 -8';
	
	tmin_z = tmax_z - (self->pos1_z - self->pos2_z + 8);
	
	if (self->v.spawnflags & PLAT_LOW_TRIGGER)
		tmax_z = tmin_z + 8;
	
	if (self->v.size_x <= 50)
	{
		tmin_x = (self->mins_x + self->maxs_x) / 2;
		tmax_x = tmin_x + 1;
	};
	
	if (self->size_y <= 50)
	{
		tmin_y = (self->mins_y + self->maxs_y) / 2;
		tmax_y = tmin_y + 1;
	};
	
	pEngine->pfnSetSize(trigger, tmin, tmax);
};

void CFuncPlat::HitTop()
{
	EmitSound(CHAN_NO_PHS_ADD + CHAN_VOICE, self->noise1, 1, ATTN_NORM);
	self->state = STATE_TOP;
	SetThinkCallback(CFuncPlat::GoDown);
	SetNextThink(self->ltime + 3);
};

void CFuncPlat::HitBottom()
{
	EmitSound(CHAN_NO_PHS_ADD + CHAN_VOICE, self->noise1, 1, ATTN_NORM);
	self->state = STATE_BOTTOM;
};

void CFuncPlat::GoDown()
{
	EmitSound(CHAN_VOICE, self->noise, 1, ATTN_NORM);
	self->state = STATE_DOWN;
	SetMoveDoneCallback(CFuncPlat::HitBottom);
	LinearMove(self->pos2, self->speed);
};

void CPlatform::GoUp()
{
	EmitSound(CHAN_VOICE, self->noise, 1, ATTN_NORM);
	self->state = STATE_UP;
	SetMoveDoneCallback(CFuncPlat::HitTop);
	LinearMove(self->pos1, self->speed);
};

void CPlatform::center_touch(CBaseEntity *other)
{
	if (other->GetClassName() != "player")
		return;
		
	if (other->GetHealth() <= 0)
		return;

	self = self->GetEnemy();
	if (self->state == STATE_BOTTOM)
		go_up();
	else if (self->state == STATE_TOP)
		SetNextThink(self->ltime + 1); // delay going down
};

void CPlatform::outside_touch(CBaseEntity *other)
{
	if (other->GetClassName() != "player")
		return;

	if (other->GetHealth() <= 0)
		return;
		
//dprint ("plat_outside_touch\n");
	self = self->enemy;
	if (self->state == STATE_TOP)
		go_down();
};

void CPlatform::trigger_use(CBaseEntity *other) // TODO: , CBaseEntity *activator; Use_Plat in q2
{
	if (self->think)
		return;		// already activated/down
	go_down();
};

void CPlatform::crush(CBaseEntity *other)
{
//dprint ("plat_crush\n");

	other->deathtype = "squish";
	other->TakeDamage(self, self, 1);
	
	if (self->state == STATE_UP)
		go_down();
	else if (self->state == STATE_DOWN)
		go_up();
	else
		objerror("plat_crush: bad self.state\n");
};

void CFuncPlat::Use(CBaseEntity *apActivator, CBaseEntity *apCaller, UseType aeUseType, float afValue)
{
	if(IsTogglePlat())
	{
		// TODO
	}
	else
	{
		SetUseCallback(SUB_Null);
		if(self->state != STATE_UP)
			objerror ("plat_use: not in up state");
		GoDown();
	};
};

/*QUAKED func_plat (0 .5 .8) ? PLAT_LOW_TRIGGER
speed	default 150

Plats are always drawn in the extended position, so they will light correctly.

If the plat is the target of another trigger or button, it will start out disabled in the extended position until it is trigger, when it will lower and become a normal plat.

If the "height" key is set, that will determine the amount the plat moves, instead of being implicitly determined by the model's height.

Set "sounds" to one of the following:
1) base fast
2) chain slow
*/

C_EXPORT void func_plat(entvars_t *self)
{
	CFuncPlat::Spawn();
};

void CFuncPlat::Spawn()
{
// TODO: unused by q2
	if (!self->v.t_length)
		self->v.t_length = 80;
	if (!self->v.t_width)
		self->v.t_width = 10;

	if (self->v.sounds == 0)
		self->v.sounds = 2;
// FIX THIS TO LOAD A GENERIC PLAT SOUND

	if (self->sounds == 1)
	{
		pEngine->pfnPrecacheSound ("plats/plat1.wav");
		pEngine->pfnPrecacheSound ("plats/plat2.wav");
		self->noise = "plats/plat1.wav";
		self->noise1 = "plats/plat2.wav";
	};

	if (self->sounds == 2)
	{
		pEngine->pfnPrecacheSound ("plats/medplat1.wav");
		pEngine->pfnPrecacheSound ("plats/medplat2.wav");
		self->noise = "plats/medplat1.wav";
		self->noise1 = "plats/medplat2.wav";
	};
// end unused by q2

	self->mangle = self->v.angles; // TODO: remove?
	self->angles = '0 0 0'; // VectorClear

	SetClassName("plat"); // TODO: should we do this here?
	SetSolidity(SOLID_BSP);
	SetMoveType(MOVETYPE_PUSH);
	
	SetOrigin(GetOrigin()); // TODO: q2 doesn't do that
	SetModel(GetModel());
	SetSize(GetSize().mins , GetSize().maxs); // TODO: q2 doesn't do that

	SetBlockedCallback(CPlatform::crush); // TODO: plat_blocked in q2
	
	if (!self->speed)
		self->speed = 150;

// pos1 is the top position, pos2 is the bottom
	self->pos1 = GetOrigin();
	self->pos2 = GetOrigin();
	if (self->height)
		self->pos2_z = self->v.origin_z - self->v.height;
	else
		self->pos2_z = self->v.origin_z - self->v.size_z + 8;

	SetUseCallback(CPlatform::trigger_use); // TODO: Use_Plat in q2

	plat_spawn_inside_trigger (self);	// the "start moving" trigger	

	if (self->targetname)
	{
		self->SetState(STATE_UP); // TODO: self->moveinfo.state in q2
		SetUseCallback(CPlatform::Use); // TODO: not present in q2
	}
	else
	{
		SetOrigin(self->v.pos2);
		self->SetState(STATE_BOTTOM); // TODO: self->moveinfo.state in q2
	};
};

//============================================================================

void train_next();
void func_train_find();

class CTrain : public CBaseEntity
{
public:
	CTrain();
	~CTrain();
	
	void Blocked(CBaseEntity *other);
	void Use(CBaseEntity *other);
	void Wait();
private:
};

CTrain::CTrain() = default;
CTrain::~CTrain() = default;

void CTrain::Blocked(edict_t *other)
{
	if (time < self->v.attack_finished)
		return;
	self->v.attack_finished = time + 0.5;
	other->v.deathtype = "squish";
	other->TakeDamage(self, self, self->v.dmg);
};

void CTrain::Use()
{
	if (self->think != func_train_find)
		return;		// already activated
	train_next();
};

void CTrain::Wait()
{
	if (self->v.wait)
	{
		self->SetNextThink(self->v.ltime + self->v.wait);
		EmitSound(CHAN_NO_PHS_ADD+CHAN_VOICE, self->v.noise, 1, ATTN_NORM);
	}
	else
		SetNextThink(self->v.ltime + 0.1);
	
	SetThinkCallback(train_next);
};

void train_next()
{
	entity	targ;

	targ = gpEngine->pfnFindEntityByString(world, targetname, self.target);
	self.target = targ.target;
	if (!self.target)
		objerror ("train_next: no next target");
	if (targ.wait)
		self.wait = targ.wait;
	else
		self.wait = 0;
	pEngine->pfnEmitSound (self, CHAN_VOICE, self.noise1, 1, ATTN_NORM);
	SUB_CalcMove (targ.origin - self.mins, self.speed, train_wait);
};

void func_train_find()
{
	entity	targ;

	targ = find (world, targetname, self.target);
	self.target = targ.target;
	self->SetOrigin(targ.origin - self.mins);
	if (!self.targetname)
	{	// not triggered, so start immediately
		self.nextthink = self.ltime + 0.1;
		self.think = train_next;
	};
};

/*QUAKED func_train (0 .5 .8) ?
Trains are moving platforms that players can ride.
The targets origin specifies the min point of the train at each corner.
The train spawns at the first target it is pointing at.
If the train is the target of a button or trigger, it will not begin moving until activated.
speed	default 100
dmg		default	2
sounds
1) ratchet metal

*/
C_EXPORT void func_train(entvars_t *self)
{
	CFuncTrain::Spawn();
};

void CFuncTrain::Spawn()
{	
	if (!self.speed)
		self.speed = 100;
	if (!self.target)
		objerror ("func_train without a target");
	if (!self.dmg)
		self.dmg = 2;

	if (self.sounds == 0)
	{
		self.noise = ("misc/null.wav");
		pEngine->precache_sound ("misc/null.wav");
		self.noise1 = ("misc/null.wav");
		pEngine->precache_sound ("misc/null.wav");
	};

	if (self.sounds == 1)
	{
		self.noise = ("plats/train2.wav");
		precache_sound ("plats/train2.wav");
		self.noise1 = ("plats/train1.wav");
		precache_sound ("plats/train1.wav");
	};

	self.cnt = 1;
	self.solid = SOLID_BSP;
	self.movetype = MOVETYPE_PUSH;
	self.blocked = train_blocked;
	self.use = train_use;
	self.classname = "train";

	pEngine->setmodel (self, self.model);
	pEngine->setsize (self, self.mins , self.maxs);
	pEngine->setorigin (self, self.origin);

// start trains on the second frame, to make sure their targets have had
// a chance to spawn
	self.nextthink = self.ltime + 0.1;
	self.think = func_train_find;
};

/*QUAKED misc_teleporttrain (0 .5 .8) (-8 -8 -8) (8 8 8)
This is used for the final bos
*/
void misc_teleporttrain()
{
	if (!self.speed)
		self.speed = 100;
	if (!self.target)
		objerror ("func_train without a target");

	self.cnt = 1;
	self.solid = SOLID_NOT;
	self.movetype = MOVETYPE_PUSH;
	self.blocked = train_blocked;
	self.use = train_use;
	self.avelocity = '100 200 300';

	self.noise = ("misc/null.wav");
	pEngine->precache_sound ("misc/null.wav");
	self.noise1 = ("misc/null.wav");
	pEngine->precache_sound ("misc/null.wav");

	precache_model2 ("models/teleport.mdl");
	pEngine->setmodel (self, "models/teleport.mdl");
	pEngine->setsize (self, self.mins , self.maxs);
	pEngine->setorigin (self, self.origin);

// start trains on the second frame, to make sure their targets have had
// a chance to spawn
	self.nextthink = self.ltime + 0.1;
	self.think = func_train_find;
};