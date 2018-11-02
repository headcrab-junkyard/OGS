/*
	spectate.qc

	spectator functions

	PURPOSE

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

// Added Aug11'97 by Zoid <zoid@idsoftware.com>
//
// These functions are called from the server if they exist.
// Note that Spectators only have one think since they movement code doesn't
// track them much.  Impulse commands work as usual, but don't call
// the regular ImpulseCommand handler in weapons.qc since Spectators don't
// have any weapons and things can explode.
//
//   --- Zoid.

/*
===========
SpectatorConnect

called when a spectator connects to a server
============
*/
void SpectatorConnect(edict_t *self)
{
	bprint (PRINT_MEDIUM, "Spectator ");
	bprint (PRINT_MEDIUM, self->v.netname);
	bprint (PRINT_MEDIUM, " entered the game\n");

	self->v.goalentity = world; // used for impulse 1 below
};

/*
===========
SpectatorDisconnect

called when a spectator disconnects from a server
============
*/
void SpectatorDisconnect(edict_t *self)
{
	bprint (PRINT_MEDIUM, "Spectator ");
	bprint (PRINT_MEDIUM, self->v.netname);
	bprint (PRINT_MEDIUM, " left the game\n");
};

/*
================
SpectatorImpulseCommand

Called by SpectatorThink if the spectator entered an impulse
================
*/
void SpectatorImpulseCommand(edict_t *self)
{
	if (self->v.impulse == 1)
	{
		// teleport the spectator to the next spawn point
		// note that if the spectator is tracking, this doesn't do
		// much
		self->v.goalentity = find(self->v.goalentity, classname, "info_player_deathmatch");
		if (self->v.goalentity == world)
			self->v.goalentity = find(self->v.goalentity, classname, "info_player_deathmatch");
		if (self->v.goalentity != world)
		{
			setorigin(self, self->v.goalentity.origin);
			self->v.angles = self->v.goalentity.angles;
			self->v.fixangle = TRUE;           // turn this way immediately
		}
	}

	self->v.impulse = 0;
};

/*
================
SpectatorThink

Called every frame after physics are run
================
*/
void SpectatorThink(edict_t *self)
{
	// self.origin, etc contains spectator position, so you could
	// do some neat stuff here

	if(self->v.impulse)
		SpectatorImpulseCommand(self);
};