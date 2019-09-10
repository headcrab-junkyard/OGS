/*
	subs.qc

	sub-functions, mostly movement related

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

void CBaseEntity::SUB_Null()
{
};

void CBaseEntity::SUB_Remove()
{
	gpEngine->pfnRemove(ToEdict());
};

/*
QuakeEd only writes a single float for angles (bad idea), so up and down are
just constant angles.
*/
void CBaseEntity::SetMovedir()
{
	if (GetAngles() == idVec3(0, -1, 0)) // TODO: -idVec3::Up
		SetMoveDir(idVec3(0, 0, 1));
	else if (GetAngles() == idVec3(0, -2, 0))
		SetMoveDir(idVec3(0, 0, -1));
	else
	{
		gpEngine->pfnMakeVectors(GetAngles());
		SetMoveDir(gpGlobals->v_forward);
	};
	
	SetAngles(idVec3::Origin);
};

/*
================
InitTrigger
================
*/
void InitTrigger()
{
	// trigger angles are used for one-way touches.  An angle of 0 is assumed
	// to mean no restrictions, so use a yaw of 360 instead.
	if (self->GetAngles() != '0 0 0')
		SetMovedir (self);
	self->SetSolidity(SOLID_TRIGGER);
	self->SetModel(self->GetModel()); // set size and link into world
	self->SetMoveType(MOVETYPE_NONE);
	self->v.modelindex = 0;
	self->SetModel("");
};

/*
=============
SUB_CalcMove

calculate self.velocity and self.nextthink to reach dest from
self.origin traveling at speed
===============
*/
void SUB_CalcMoveEnt(edict_t *ent, vector tdest, float tspeed, void (*func)())
{
	edict_t stemp;
	stemp = *self;
	self = ent;

	SUB_CalcMove (tdest, tspeed, func);
	self = stemp;
};

void SUB_CalcMove(idVec3 tdest, float tspeed, void (*func)())
{
	idVec3	vdestdelta;
	float		len, traveltime;

	if (!tspeed)
		objerror("No speed is defined!");

	self->think1 = func;
	self->finaldest = tdest;
	self->think = SUB_CalcMoveDone;

	if (tdest == self->GetOrigin())
	{
		self->velocity = '0 0 0';
		self->nextthink = self->ltime + 0.1;
		return;
	};
		
// set destdelta to the vector needed to move
	vdestdelta = tdest - self->GetOrigin();
	
// calculate length of vector
	len = vlen (vdestdelta);
	
// divide by speed to get time to reach dest
	traveltime = len / tspeed;

	if (traveltime < 0.03)
		traveltime = 0.03;
	
// set nextthink to trigger a think when dest is reached
	self->nextthink = self->ltime + traveltime;

// scale the destdelta vector by the time spent traveling to get velocity
	self->velocity = vdestdelta * (1/traveltime);	// qcc won't take vec/float	
};

/*
============
After moving, set origin to exact final destination
============
*/
void CBaseEntity::SUB_CalcMoveDone() // TODO: CBaseToggle::LinearMoveDone in gs?
{
	SetOrigin(self->finaldest);
	SetVelocity(idVec3::Origin);
	SetNextThink(-1);
	if (self->think1)
		self->think1();
};

/*
=============
SUB_CalcAngleMove

calculate self.avelocity and self.nextthink to reach destangle from
self.angles rotating 

The calling function should make sure self.think is valid
===============
*/
void SUB_CalcAngleMoveEnt(entity ent, vector destangle, float tspeed, void() func)
{
	entity		stemp;
	stemp = self;
	self = ent;
	SUB_CalcAngleMove (destangle, tspeed, func);
	self = stemp;
};

void SUB_CalcAngleMove(vector destangle, float tspeed, void() func)
{
	vector	destdelta;
	float		len, traveltime;

	if (!tspeed)
		objerror("No speed is defined!");
		
// set destdelta to the vector needed to move
	destdelta = destangle - self.angles;
	
// calculate length of vector
	len = vlen (destdelta);
	
// divide by speed to get time to reach dest
	traveltime = len / tspeed;

// set nextthink to trigger a think when dest is reached
	self.nextthink = self.ltime + traveltime;

// scale the destdelta vector by the time spent traveling to get velocity
	self.avelocity = destdelta * (1 / traveltime);
	
	self.think1 = func;
	self.finalangle = destangle;
	self.think = SUB_CalcAngleMoveDone;
};

/*
============
After rotating, set angle to exact final angle
============
*/
void CBaseEntity::SUB_CalcAngleMoveDone() // TODO: CBaseToggle::AngularMoveDone in gs?
{
	SetAngles(self->finalangle);
	SetAngularVelocity(idVec3::Origin);
	SetNextThink(-1);
	if (self->think1)
		self->think1();
};

//=============================================================================

void CBaseEntity::DelayThink() // TODO: CBaseDelay
{
	SUB_UseTargets (GetEnemy());
	gpEngine->pfnRemove(ToEdict());
};

/*
==============================
SUB_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Removes all entities with a targetname that match self.killtarget,
and removes them, so some events can remove other triggers.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void CBaseEntity::SUB_UseTargets(CBaseEntity *activator) // TODO: CBaseDelay
{
	CBaseEntity *stemp, *otemp, *act;

	//
	// check for a delay
	//
	if (self->delay)
	{
		// create a temp object to fire at a later time
		CBaseEntity *t = mpWorld->SpawnEntity();
		t->SetClassName("DelayedUse");
		t->SetNextThink(gpGlobals->time + self->delay);
		t->SetThinkCallback(CBaseEntity::DelayThink);
		t->SetEnemy(activator);
		t->message = self->message;
		t->killtarget = self->killtarget;
		t->SetTarget(self->target);
		return;
	};
	
	//
	// print the message
	//
	if (activator->GetClassName() == "player" && self.message != "")
	{
		gpEngine->pfnCenterPrint (activator, self->message);
		if (!self->noise)
			activator->EmitSound(CHAN_VOICE, "misc/talk.wav", 1, ATTN_NORM);
	};

	//
	// kill the killtagets
	//
	if (self.killtarget)
	{
		t = world;
		do
		{
			t = find (t, targetname, self.killtarget);
			if (!t)
				return;
			gpEngine->pfnRemove (t);
		}
		while ( 1 );
	};
	
	//
	// fire targets
	//
	if (self.target)
	{
		act = activator;
		t = world;
		do
		{
			t = find (t, targetname, self.target);
			if (!t)
				return;
			stemp = self;
			otemp = other;
			self = t;
			other = stemp;
			if (self.use != SUB_Null)
			{
				self->Use();
			};
			self = stemp;
			other = otemp;
			activator = act;
		}
		while ( 1 );
	};
};