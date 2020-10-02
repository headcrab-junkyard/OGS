/*  Copyright (C) 1996-1997  Id Software, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

    See file, 'COPYING', for details.
*/

/// @file

#include "BaseMonster.hpp"

/* ALL MONSTERS SHOULD BE 1 0 0 IN COLOR */

// name =[framenum,	nexttime, nextthink] {code}
// expands to:
// name ()
// {
//		self.frame=framenum;
//		self.nextthink = time + nexttime;
//		self.think = nextthink
//		<code>
// };

/*
================
monster_use

Using a monster makes it angry at the current activator
================
*/
void CBaseMonster::Use(CBaseEntity *activator)
{
	if (GetEnemy() || GetHealth() <= 0)
		return;
	
	if (activator->GetItems() & IT_INVISIBILITY)
		return;
	if (activator->GetFlags() & FL_NOTARGET)
		return;
	if (activator->GetClassName() != "player")
		return;
	
// delay reaction so if the monster is teleported, its sound is still heard
	SetEnemy(activator);
	SetNextThink(gpGlobals->time + 0.1);
	SetThinkCallback(FoundTarget);
};

/*
================
monster_death_use

When a monster dies, it fires all of its targets with the current
enemy as activator.
================
*/
void CBaseMonster::DeathUse()
{
// fall to ground
	if (GetFlags() & FL_FLY)
		self->flags -= FL_FLY;

	if (GetFlags() & FL_SWIM)
		self->flags -= FL_SWIM;

	if (!self->target)
		return;

	SUB_UseTargets (GetEnemy());
};