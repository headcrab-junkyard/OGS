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

#pragma once

/*
=============
SUB_CalcMove

calculate self.velocity and self.nextthink to reach dest from
self.origin traveling at speed
===============
*/
void CBaseToggle::SUB_CalcMoveEnt(edict_t *ent, vector tdest, float tspeed, void (*func)())
{
	edict_t stemp;
	stemp = *self;
	self = ent;

	SUB_CalcMove (tdest, tspeed, func);
	self = stemp;
};

void CBaseToggle::SUB_CalcMove(idVec3 tdest, float tspeed, void (*func)())
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
void CBaseToggle::SUB_CalcMoveDone() // TODO: LinearMoveDone in gs?
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
void CBaseToggle::SUB_CalcAngleMoveEnt(entity ent, vector destangle, float tspeed, void() func)
{
	entity		stemp;
	stemp = self;
	self = ent;
	SUB_CalcAngleMove (destangle, tspeed, func);
	self = stemp;
};

void CBaseToggle::SUB_CalcAngleMove(vector destangle, float tspeed, void() func)
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
void CBaseToggle::SUB_CalcAngleMoveDone() // TODO: AngularMoveDone in gs?
{
	SetAngles(self->finalangle);
	SetAngularVelocity(idVec3::Origin);
	SetNextThink(-1);
	if (self->think1)
		self->think1();
};