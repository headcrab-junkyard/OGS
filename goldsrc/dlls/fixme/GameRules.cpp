/*
 * This file is part of OGS Engine
 * Copyright (C) 2019 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "GameRules.hpp"

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
bool CGameRules::CanDamage(CBaseEntity *targ, CBaseEntity *inflictor)
{
	// bmodels need special checking because their origin is 0,0,0
	if (targ->GetMoveType() == MOVETYPE_PUSH)
	{
		traceline(inflictor->GetOrigin(), 0.5 * (targ.absmin + targ.absmax), TRUE, self);
		if (trace_fraction == 1)
			return true;
		if (trace_ent == targ)
			return true;
		return false;
	};
	
	traceline(inflictor->GetOrigin(), targ->GetOrigin(), TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor->GetOrigin(), targ->GetOrigin() + '15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor->GetOrigin(), targ->GetOrigin() + '-15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor->GetOrigin(), targ->GetOrigin() + '-15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;
	traceline(inflictor->GetOrigin(), targ->GetOrigin() + '15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return true;

	return false;
};

bool CGameRules::HandleClientConnect(CBaseEntity *apEntity, const char *asName, const char *asAdr, char sRejectReason[128])
{
/*
	bprint(PRINT_HIGH, pEntity->v.netname);
	bprint(PRINT_HIGH, " entered the game\n");
*/

	// a client connecting during an intermission can cause problems
	//if(intermission_running)
		//GotoNextMap();
	
	return true;
};

void CGameRules::HandleClientDisconnect(CBaseEntity *apEntity)
{
/*
	// let everyone else know
	bprint (PRINT_HIGH, self->v.netname);
		bprint (PRINT_HIGH, " left the game with ");
		bprint (PRINT_HIGH, ftos(self->GetFrags()));
		bprint (PRINT_HIGH, " frags\n");
	apEntity->EmitSound(CHAN_BODY, "player/tornoff2.wav", 1, ATTN_NONE);
	set_suicide_frame(self->v);
*/
};