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