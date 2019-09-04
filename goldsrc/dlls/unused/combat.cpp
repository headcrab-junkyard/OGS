/*
	combat.qc

	damage, obit, etc related functions

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

void T_MissileTouch();
void info_player_start();
void ClientObituary(entity targ, entity attacker);

/*SERVER
void monster_death_use();
*/

//============================================================================

/*
============
T_BeamDamage
============
*/
void T_BeamDamage(CBaseEntity *attacker, float damage)
{
	float   points;
	CBaseEntity *head;
	
	head = findradius(attacker->GetOrigin(), damage+40);
	
	while (head)
	{
		if (head->v.takedamage)
		{
			points = 0.5*vlen (attacker->GetOrigin() - head->GetOrigin());
			if (points < 0)
				points = 0;
			points = damage - points;
			if (head == attacker)
				points = points * 0.5;
			if (points > 0)
			{
				if (IGameRules::CanDamage (head, attacker))
					head->TakeDamage (attacker, attacker, points);
			}
		}
		head = head.chain;
	}
};