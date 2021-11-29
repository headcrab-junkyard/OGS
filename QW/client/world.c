int			sv_numareanodes;

int SV_PointContents (vec3_t p)
{
	return SV_HullPointContents (&sv.worldmodel->hulls[0], 0, p);
}

//===========================================================================

/*
============
SV_TestEntityPosition

A small wrapper around SV_BoxInSolidEntity that never clips against the
supplied entity.
============
*/

qboolean SV_RecursiveHullCheck (hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace)
{


#ifdef PARANOID
	if (SV_HullPointContents (sv_hullmodel, mid, node->children[side]) == CONTENTS_SOLID)
	{
		Con_Printf ("mid PointInHullSolid\n");
		return false;
	}
#endif
	
	if (SV_HullPointContents (hull, node->children[side^1], mid) != CONTENTS_SOLID)
// go past the node
		return SV_RecursiveHullCheck (hull, node->children[side^1], midf, p2f, mid, p2, trace);
	
	if (trace->allsolid)
		return false;		// never got out of the solid area
		
//==================
// the other side of the node is solid, this is the impact point
//==================
	if (!side)
	{
		VectorCopy (plane->normal, trace->plane.normal);
		trace->plane.dist = plane->dist;
	}
	else
	{
		VectorSubtract (vec3_origin, plane->normal, trace->plane.normal);
		trace->plane.dist = -plane->dist;
	}

	while (SV_HullPointContents (hull, hull->firstclipnode, mid) == CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy (mid, trace->endpos);
			Con_Printf ("backup past 0\n");
			return false;
		}
		midf = p1f + (p2f - p1f)*frac;
		for (i=0 ; i<3 ; i++)
			mid[i] = p1[i] + frac*(p2[i] - p1[i]);
	}

	trace->fraction = midf;
	VectorCopy (mid, trace->endpos);

	return false;
}

/*
==================
SV_Move
==================
*/
trace_t SV_Move (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type, edict_t *passedict)
{
}

//=============================================================================

/*
============
SV_TestPlayerPosition

============
*/
edict_t	*SV_TestPlayerPosition (edict_t *ent, vec3_t origin)
{
	hull_t	*hull;
	edict_t	*check;
	vec3_t	boxmins, boxmaxs;
	vec3_t	offset;
	int		e;
	
// check world first
	hull = &sv.worldmodel->hulls[1];
	if ( SV_HullPointContents (hull, hull->firstclipnode, origin) != CONTENTS_EMPTY )
		return sv.edicts;

// check all entities
	VectorAdd (origin, ent->v.mins, boxmins);
	VectorAdd (origin, ent->v.maxs, boxmaxs);
	
	check = NEXT_EDICT(sv.edicts);
	for (e=1 ; e<sv.num_edicts ; e++, check = NEXT_EDICT(check))
	{
		if (check->free)
			continue;
		if (check->v.solid != SOLID_BSP &&
			check->v.solid != SOLID_BBOX &&
			check->v.solid != SOLID_SLIDEBOX)
			continue;

		if (boxmins[0] > check->v.absmax[0]
		|| boxmins[1] > check->v.absmax[1]
		|| boxmins[2] > check->v.absmax[2]
		|| boxmaxs[0] < check->v.absmin[0]
		|| boxmaxs[1] < check->v.absmin[1]
		|| boxmaxs[2] < check->v.absmin[2] )
			continue;

		if (check == ent)
			continue;

	// get the clipping hull
		hull = SV_HullForEntity (check, ent->v.mins, ent->v.maxs, offset);
	
		VectorSubtract (origin, offset, offset);
	
	// test the point
		if ( SV_HullPointContents (hull, hull->firstclipnode, offset) != CONTENTS_EMPTY )
			return check;
	}

	return NULL;
}


