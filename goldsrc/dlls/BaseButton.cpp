/*
	buttons.qc

	button and multiple button

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

#include "BaseButton.hpp"

void CButton::Spawn()
{
	if (self->sounds == 0)
	{
		gpEngine->pfnPrecacheSound ("buttons/airbut1.wav");
		self->noise = "buttons/airbut1.wav";
	};
	if (self->sounds == 1)
	{
		gpEngine->pfnPrecacheSound ("buttons/switch21.wav");
		self->noise = "buttons/switch21.wav";
	};
	if (self->sounds == 2)
	{
		gpEngine->pfnPrecacheSound ("buttons/switch02.wav");
		self->noise = "buttons/switch02.wav";
	};
	if (self->sounds == 3)
	{
		gpEngine->pfnPrecacheSound ("buttons/switch04.wav");
		self->noise = "buttons/switch04.wav";
	};
	
	SetMovedir ();

	SetMoveType(MOVETYPE_PUSH);
	SetSolidity(SOLID_BSP);
	SetModel(self->GetModel());

	SetBlockedCallback(CButton::Blocked);
	SetUseCallback(CButton::Use);

	if (GetHealth())
	{
		SetMaxHealth(GetHealth());
		self->th_die = CButton::Killed;
		self->takedamage = DAMAGE_YES;
	}
	else
		SetTouchCallback(CButton::Touch);

	if (!self->speed)
		self->speed = 40;
	if (!self->wait)
		self->wait = 1;
	if (!self->lip)
		self->lip = 4;

	state = STATE_BOTTOM;

	self->pos1 = GetOrigin();
	self->pos2 = self->pos1 + self->movedir * (fabs(self->movedir * self->size) - self->lip);
};

void CButton::Use(CBaseEntity *other)
{
	self->SetEnemy(activator);
	Fire();
};

void CButton::Touch(CBaseEntity *other)
{
	if (other->GetClassName() != "player")
		return;
	
	mpActivator = other;
	
	SetEnemy(other);
	
	Fire();
};

void CButton::Blocked(CBaseEntity *other)
{
	// do nothing, just don't ome all the way back out
};

void CButton::Wait()
{
	self->SetState(STATE_TOP);
	
	{
		SetNextThink(self->ltime + self->wait);
		SetThinkCallback(CButton::Return);
	};
	
	SUB_UseTargets(GetEnemy(), USE_TOGGLE, 0);
	
	self->frame = 1; // use alternate textures
};

void CButton::Done()
{
	self->SetState(STATE_BOTTOM);
};

void CButton::Return()
{
	self->SetState(STATE_DOWN);
	SUB_CalcMove(self->pos1, self->speed, CButton::Done);
	self->frame = 0; // use normal textures
	if (GetHealth())
		SetDamageable(DAMAGE_YES); // can be shot again
};

void CButton::Fire()
{
	if (self->GetState() == STATE_UP || self->GetState() == STATE_TOP)
		return;

	EmitSound(CHAN_VOICE, self->noise, 1, ATTN_NORM);

	self->SetState(STATE_UP);
	SUB_CalcMove (self->pos2, self->speed, button_wait);
};

void CButton::Killed(CBaseEntity *attacker)
{
	SetEnemy(damage_attacker);
	SetHealth(GetMaxHealth());
	SetDamageable(DAMAGE_NO); // wil be reset upon return
	Fire ();
};

/*QUAKED func_button (0 .5 .8) ?
When a button is touched, it moves some distance in the direction of it's angle, triggers all of it's targets, waits some time, then returns to it's original position where it can be triggered again.

"angle"		determines the opening direction
"target"	all entities with a matching targetname will be used
"speed"		override the default 40 speed
"wait"		override the default 1 second wait (-1 = never return)
"lip"		override the default 4 pixel lip remaining at end of move
"health"	if set, the button must be killed instead of touched
"sounds"
0) steam metal
1) wooden clunk
2) metallic click
3) in-out
*/
LINK_ENTITY_TO_CLASS(func_button, CButton)