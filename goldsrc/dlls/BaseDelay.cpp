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

#include "BaseDelay.hpp"

//=============================================================================

bool CBaseDelay::HandleKeyValue(KeyValueData *apKVData)
{
	// TODO
	return false;
};

void CBaseDelay::DelayThink()
{
	SUB_UseTargets(GetEnemy());
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
void CBaseDelay::SUB_UseTargets(CBaseEntity *activator, USE_TYPE aeUseType, float afValue)
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
		t->SetThinkCallback(CBaseDelay::DelayThink);
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
			activator->EmitSound(CHAN_VOICE, "misc/talk.wav", 1, ATTN_NORM, PITCH_NORM);
	};

	//
	// kill the killtagets
	//
	if (self.killtarget)
	{
		t = world;
		do
		{
			t = gpEngine->pfnFind (t, targetname, self.killtarget);
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
			t = gpEngine->pfnFind (t, targetname, self.target);
			if (!t)
				return;
			stemp = self;
			otemp = other;
			self = t;
			other = stemp;
			if (self->GetUseCallback() != SUB_Null)
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