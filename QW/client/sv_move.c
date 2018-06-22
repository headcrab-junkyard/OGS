/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/



void PF_changeyaw (void);




void SV_MoveToGoal (void)
{
	edict_t		*ent, *goal;
	float		dist;
	
	ent = PROG_TO_EDICT(pr_global_struct->self);
	goal = PROG_TO_EDICT(ent->v.goalentity);
	dist = G_FLOAT(OFS_PARM0);

	if ( !( (int)ent->v.flags & (FL_ONGROUND|FL_FLY|FL_SWIM) ) )
	{
		G_FLOAT(OFS_RETURN) = 0;
		return;
	}

// if the next step hits the enemy, return immediately
	if ( PROG_TO_EDICT(ent->v.enemy) != sv.edicts &&  SV_CloseEnough (ent, goal, dist) )
		return;

// bump around...
	if ( (rand()&3)==1 ||
	!SV_StepDirection (ent, ent->v.ideal_yaw, dist))
	{
		SV_NewChaseDir (ent, goal, dist);
	}
}
