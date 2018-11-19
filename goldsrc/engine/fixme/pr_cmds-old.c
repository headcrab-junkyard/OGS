



char *PF_VarString(int first)
{
	int i;
	static char out[256];

	out[0] = 0;
	for(i = first; i < pr_argc; i++)
	{
		strcat(out, G_STRING((OFS_PARM0 + i * 3)));
	}
	return out;
}

/*
=================
PF_errror

This is a TERMINAL error, which will kill off the entire server.
Dumps self.

error(value)
=================
*/
void PF_error(const char *s)
{
	edict_t *ed;

	Con_Printf("======SERVER ERROR in %s:\n%s\n", pr_strings + pr_xfunction->s_name, s);
	ed = PROG_TO_EDICT(gGlobalVariables.self);
	ED_Print(ed);

	Host_Error("Program error");
}

/*
=================
PF_objerror

Dumps out self, then an error message.  The program is aborted and self is
removed, but the level can continue.

objerror(value)
=================
*/
void PF_objerror(const char *s)
{
	edict_t *ed;

	Con_Printf("======OBJECT ERROR in %s:\n%s\n", pr_strings + pr_xfunction->s_name, s);
	ed = PROG_TO_EDICT(gGlobalVariables.self);
	ED_Print(ed);
	ED_Free(ed);

	Host_Error("Program error");
}

/*
=================
PF_bprint

broadcast print to everyone on server

bprint(value)
=================
*/
void PF_bprint(const char *s)
{
	SV_BroadcastPrintf("%s", s);
}

/*
=================
PF_normalize

vector normalize(vector)
=================
*/
void PF_normalize(float *value1)
{
	vec3_t newvalue;
	float fnew;

	fnew = value1[0] * value1[0] + value1[1] * value1[1] + value1[2] * value1[2];
	fnew = sqrt(fnew);

	if(fnew == 0)
		newvalue[0] = newvalue[1] = newvalue[2] = 0;
	else
	{
		fnew = 1 / fnew;
		newvalue[0] = value1[0] * fnew;
		newvalue[1] = value1[1] * fnew;
		newvalue[2] = value1[2] * fnew;
	}

	VectorCopy(newvalue, G_VECTOR(OFS_RETURN));
}

/*
=================
PF_vlen

scalar vlen(vector)
=================
*/
float PF_vlen(float *value1)
{
	float fnew;

	fnew = value1[0] * value1[0] + value1[1] * value1[1] + value1[2] * value1[2];
	fnew = sqrt(fnew);

	return fnew;
}

/*
=================
PF_Random

Returns a number from 0<= num < 1

random()
=================
*/
float PF_random()
{
	float num = (rand() & 0x7fff) / ((float)0x7fff);

	return num;
}

/*
=================
PF_traceline

Used for use tracing and shot targeting
Traces are blocked by bbox and exact bsp entities, and also slide box entities
if the tryents flag is set.

traceline (vector1, vector2, tryents)
=================
*/
void PF_traceline(float *v1, float *v2, int nomonsters, edict_t *ent)
{
	trace_t trace;

	trace = SV_Move(v1, vec3_origin, vec3_origin, v2, nomonsters, ent);

	gGlobalVariables.trace_allsolid = trace.allsolid;
	gGlobalVariables.trace_startsolid = trace.startsolid;
	gGlobalVariables.trace_fraction = trace.fraction;
	gGlobalVariables.trace_inwater = trace.inwater;
	gGlobalVariables.trace_inopen = trace.inopen;

	VectorCopy(trace.endpos, gGlobalVariables.trace_endpos);
	VectorCopy(trace.plane.normal, gGlobalVariables.trace_plane_normal);

	gGlobalVariables.trace_plane_dist = trace.plane.dist;

	if(trace.ent)
		gGlobalVariables.trace_ent = EDICT_TO_PROG(trace.ent);
	else
		gGlobalVariables.trace_ent = EDICT_TO_PROG(sv.edicts);
}

#ifdef QUAKE2
extern trace_t SV_Trace_Toss(edict_t *ent, edict_t *ignore);

void PF_TraceToss(edict_t *ent, edict_t *ignore)
{
	trace_t trace;

	trace = SV_Trace_Toss(ent, ignore);

	gGlobalVariables.trace_allsolid = trace.allsolid;
	gGlobalVariables.trace_startsolid = trace.startsolid;
	gGlobalVariables.trace_fraction = trace.fraction;
	gGlobalVariables.trace_inwater = trace.inwater;
	gGlobalVariables.trace_inopen = trace.inopen;

	VectorCopy(trace.endpos, gGlobalVariables.trace_endpos);
	VectorCopy(trace.plane.normal, gGlobalVariables.trace_plane_normal);

	gGlobalVariables.trace_plane_dist = trace.plane.dist;

	if(trace.ent)
		gGlobalVariables.trace_ent = EDICT_TO_PROG(trace.ent);
	else
		gGlobalVariables.trace_ent = EDICT_TO_PROG(sv.edicts);
}
#endif

/*
=================
PF_checkpos

Returns true if the given entity can move to the given position from it's
current position by walking or rolling.
FIXME: make work...
scalar checkpos (entity, vector)
=================
*/
qboolean PF_checkpos(edict_t *ed, float *v)
{
	return false;
}

//============================================================================

byte checkpvs[MAX_MAP_LEAFS / 8];

int PF_newcheckclient(int check)
{
	int i;
	byte *pvs;
	edict_t *ent;
	mleaf_t *leaf;
	vec3_t org;

	// cycle to the next one

	if(check < 1)
		check = 1;
	if(check > svs.maxclients)
		check = svs.maxclients;

	if(check == svs.maxclients)
		i = 1;
	else
		i = check + 1;

	for(;; i++)
	{
		if(i == svs.maxclients + 1)
			i = 1;

		ent = EDICT_NUM(i);

		if(i == check)
			break; // didn't find anything else

		if(ent->free)
			continue;
		if(ent->v.health <= 0)
			continue;
		if((int)ent->v.flags & FL_NOTARGET)
			continue;

		// anything that is a client, or has a client as an enemy
		break;
	}

	// get the PVS for the entity
	VectorAdd(ent->v.origin, ent->v.view_ofs, org);
	leaf = Mod_PointInLeaf(org, sv.worldmodel);
	pvs = Mod_LeafPVS(leaf, sv.worldmodel);
	memcpy(checkpvs, pvs, (sv.worldmodel->numleafs + 7) >> 3);

	return i;
}

/*
=================
PF_checkclient

Returns a client (or object that has a client enemy) that would be a
valid target.

If there are more than one valid options, they are cycled each frame

If (self.origin + self.viewofs) is not in the PVS of the current target,
it is not returned at all.

name checkclient ()
=================
*/
#define MAX_CHECK 16
int c_invis, c_notvis;
edict_t *PF_checkclient()
{
	edict_t *ent, *self;
	mleaf_t *leaf;
	int l;
	vec3_t view;

	// find a new check if on a new frame
	if(sv.time - sv.lastchecktime >= 0.1)
	{
		sv.lastcheck = PF_newcheckclient(sv.lastcheck);
		sv.lastchecktime = sv.time;
	}

	// return check if it might be visible
	ent = EDICT_NUM(sv.lastcheck);
	if(ent->free || ent->v.health <= 0)
	{
		return sv.edicts;
	}

	// if current entity can't possibly see the check entity, return 0
	self = PROG_TO_EDICT(gGlobalVariables.self);
	VectorAdd(self->v.origin, self->v.view_ofs, view);
	leaf = Mod_PointInLeaf(view, sv.worldmodel);
	l = (leaf - sv.worldmodel->leafs) - 1;
	if((l < 0) || !(checkpvs[l >> 3] & (1 << (l & 7))))
	{
		c_notvis++;
		return sv.edicts;
	}

	// might be able to see it
	c_invis++;
	return ent;
}

//============================================================================

/*
=================
PF_findradius

Returns a chain of entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
edict_t *PF_findradius(float *org, float rad)
{
	edict_t *ent, *chain;
	vec3_t eorg;
	int i, j;

	chain = (edict_t *)sv.edicts;

	ent = NEXT_EDICT(sv.edicts);
	for(i = 1; i < sv.num_edicts; i++, ent = NEXT_EDICT(ent))
	{
		if(ent->free)
			continue;
		if(ent->v.solid == SOLID_NOT)
			continue;
		for(j = 0; j < 3; j++)
			eorg[j] = org[j] - (ent->v.origin[j] + (ent->v.mins[j] + ent->v.maxs[j]) * 0.5);
		if(Length(eorg) > rad)
			continue;

		ent->v.chain = EDICT_TO_PROG(chain);
		chain = ent;
	}

	return chain;
}

/*
=========
PF_dprint
=========
*/
void PF_dprint(const char *s)
{
	Con_DPrintf("%s", s);
}

char pr_string_temp[128];

int PF_ftos(float v)
{
	if(v == (int)v)
		sprintf(pr_string_temp, "%d", (int)v);
	else
		sprintf(pr_string_temp, "%5.1f", v);
	return /*PR_GetString()*/ pr_string_temp - pr_strings;
}

int PF_vtos(vec3_t vec)
{
	sprintf(pr_string_temp, "'%5.1f %5.1f %5.1f'", vec[0], vec[1], vec[2]);
	return pr_string_temp - pr_strings;
}

#ifdef QUAKE2
int PF_etos(edict_t *ent)
{
	sprintf(pr_string_temp, "entity %i", EDICT_NUM(ent));
	return pr_string_temp - pr_strings;
}
#endif

// entity (entity start, .string field, string match) find = #5;
void PF_Find(edict_t *start, int f, const char *s)
#ifdef QUAKE2
{
	int e;
	char *t;
	edict_t *ed;
	edict_t *first;
	edict_t *second;
	edict_t *last;

	first = second = last = (edict_t *)sv.edicts;
	e = EDICT_NUM(start);
	if(!s)
		Host_Error("PF_Find: bad search string");

	for(e++; e < sv.num_edicts; e++)
	{
		ed = EDICT_NUM(e);
		if(ed->free)
			continue;
		t = E_STRING(ed, f);
		if(!t)
			continue;
		if(!strcmp(t, s))
		{
			if(first == (edict_t *)sv.edicts)
				first = ed;
			else if(second == (edict_t *)sv.edicts)
				second = ed;
			ed->v.chain = EDICT_TO_PROG(last);
			last = ed;
		}
	}

	if(first != last)
	{
		if(last != second)
			first->v.chain = last->v.chain;
		else
			first->v.chain = EDICT_TO_PROG(last);
		last->v.chain = EDICT_TO_PROG((edict_t *)sv.edicts);
		if(second && second != last)
			second->v.chain = EDICT_TO_PROG(last);
	}
	return first;
}
#else
{
	int e;
	int f;
	char *s, *t;
	edict_t *ed;

	e = EDICT_NUM(OFS_PARM0);
	f = G_INT(OFS_PARM1);
	s = G_STRING(OFS_PARM2);
	if(!s)
		Host_Error("PF_Find: bad search string");

	for(e++; e < sv.num_edicts; e++)
	{
		ed = EDICT_NUM(e);
		if(ed->free)
			continue;
		t = E_STRING(ed, f);
		if(!t)
			continue;
		if(!strcmp(t, s))
		{
			return ed;
		}
	}

	return sv.edicts;
}
#endif

void PF_coredump()
{
	ED_PrintEdicts();
}

void PF_eprint(edict_t *ent)
{
	ED_PrintNum(EDICT_NUM(ent));
}

int PF_rint(float f)
{
	if(f > 0)
		return (int)(f + 0.5);
	else
		return (int)(f - 0.5);
}

/*
=============
PF_nextent

entity nextent(entity)
=============
*/
edict_t *PF_nextent(edict_t *ent)
{
	int i;
	edict_t *ent;

	i = EDICT_NUM(ent);
	while(1)
	{
		i++;
		if(i == sv.num_edicts)
		{
			return sv.edicts;
			return;
		}
		ent = EDICT_NUM(i);
		if(!ent->free)
		{
			return ent;
		}
	}
}



//=============================================================================

//=============================================================================

#ifdef QUAKE2

float PF_WaterMove(edict_t *self)
{
	int flags;
	int waterlevel;
	int watertype;
	float drownlevel;
	float damage = 0.0;

	//self = PROG_TO_EDICT(gGlobalVariables.self);

	if(self->v.movetype == MOVETYPE_NOCLIP)
	{
		self->v.air_finished = sv.time + 12;
		return damage;
	}

	if(self->v.health < 0)
	{
		return damage;
	}

	if(self->v.deadflag == DEAD_NO)
		drownlevel = 3;
	else
		drownlevel = 1;

	flags = (int)self->v.flags;
	waterlevel = (int)self->v.waterlevel;
	watertype = (int)self->v.watertype;

	if(!(flags & (FL_IMMUNE_WATER + FL_GODMODE)))
		if(((flags & FL_SWIM) && (waterlevel < drownlevel)) || (waterlevel >= drownlevel))
		{
			if(self->v.air_finished < sv.time)
				if(self->v.pain_finished < sv.time)
				{
					self->v.dmg = self->v.dmg + 2;
					if(self->v.dmg > 15)
						self->v.dmg = 10;
					//					T_Damage (self, world, world, self.dmg, 0, FALSE);
					damage = self->v.dmg;
					self->v.pain_finished = sv.time + 1.0;
				}
		}
		else
		{
			if(self->v.air_finished < sv.time)
				//				sound (self, CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM);
				SV_StartSound(self, CHAN_VOICE, "player/gasp2.wav", 255, ATTN_NORM);
			else if(self->v.air_finished < sv.time + 9)
				//				sound (self, CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM);
				SV_StartSound(self, CHAN_VOICE, "player/gasp1.wav", 255, ATTN_NORM);
			self->v.air_finished = sv.time + 12.0;
			self->v.dmg = 2;
		}

	if(!waterlevel)
	{
		if(flags & FL_INWATER)
		{
			// play leave water sound
			//			sound (self, CHAN_BODY, "misc/outwater.wav", 1, ATTN_NORM);
			SV_StartSound(self, CHAN_BODY, "misc/outwater.wav", 255, ATTN_NORM);
			self->v.flags = (float)(flags & ~FL_INWATER);
		}
		self->v.air_finished = sv.time + 12.0;
		return damage;
	}

	if(watertype == CONTENT_LAVA)
	{ // do damage
		if(!(flags & (FL_IMMUNE_LAVA + FL_GODMODE)))
			if(self->v.dmgtime < sv.time)
			{
				if(self->v.radsuit_finished < sv.time)
					self->v.dmgtime = sv.time + 0.2;
				else
					self->v.dmgtime = sv.time + 1.0;
				//				T_Damage (self, world, world, 10*self.waterlevel, 0, TRUE);
				damage = (float)(10 * waterlevel);
			}
	}
	else if(watertype == CONTENT_SLIME)
	{ // do damage
		if(!(flags & (FL_IMMUNE_SLIME + FL_GODMODE)))
			if(self->v.dmgtime < sv.time && self->v.radsuit_finished < sv.time)
			{
				self->v.dmgtime = sv.time + 1.0;
				//				T_Damage (self, world, world, 4*self.waterlevel, 0, TRUE);
				damage = (float)(4 * waterlevel);
			}
	}

	if(!(flags & FL_INWATER))
	{
		// player enter water sound
		if(watertype == CONTENT_LAVA)
			//			sound (self, CHAN_BODY, "player/inlava.wav", 1, ATTN_NORM);
			SV_StartSound(self, CHAN_BODY, "player/inlava.wav", 255, ATTN_NORM);
		if(watertype == CONTENT_WATER)
			//			sound (self, CHAN_BODY, "player/inh2o.wav", 1, ATTN_NORM);
			SV_StartSound(self, CHAN_BODY, "player/inh2o.wav", 255, ATTN_NORM);
		if(watertype == CONTENT_SLIME)
			//			sound (self, CHAN_BODY, "player/slimbrn2.wav", 1, ATTN_NORM);
			SV_StartSound(self, CHAN_BODY, "player/slimbrn2.wav", 255, ATTN_NORM);

		self->v.flags = (float)(flags | FL_INWATER);
		self->v.dmgtime = 0;
	}

	if(!(flags & FL_WATERJUMP))
	{
		//		self.velocity = self.velocity - 0.8*self.waterlevel*frametime*self.velocity;
		VectorMA(self->v.velocity, -0.8 * self->v.waterlevel * host_frametime, self->v.velocity, self->v.velocity);
	}

	return damage;
}
#endif

enginefuncs_t gEngineFuncs[] =
{
  PF_makevectors,    // void(entity e)	makevectors 		= #1;
  PF_setorigin,      // void(entity e, vector o) setorigin	= #2;
  PF_setmodel,       // void(entity e, string m) setmodel	= #3;
  PF_setsize,        // void(entity e, vector min, vector max) setsize = #4;
  PF_Fixme,          // void(entity e, vector min, vector max) setabssize = #5;
  PF_break,          // void() break						= #6;
  PF_random,         // float() random						= #7;
  PF_sound,          // void(entity e, float chan, string samp) sound = #8;
  PF_normalize,      // vector(vector v) normalize			= #9;
  PF_error,          // void(string e) error				= #10;
  PF_objerror,       // void(string e) objerror				= #11;
  PF_vlen,           // float(vector v) vlen				= #12;
  PF_vectoyaw,       // float(vector v) vectoyaw		= #13;
  PF_Spawn,          // entity() spawn						= #14;
  PF_Remove,         // void(entity e) remove				= #15;
  PF_traceline,      // float(vector v1, vector v2, float tryents) traceline = #16;
  PF_checkclient,    // entity() clientlist					= #17;
  PF_Find,           // entity(entity start, .string fld, string match) find = #18;
  PF_precache_sound, // void(string s) precache_sound		= #19;
  PF_precache_model, // void(string s) precache_model		= #20;
  PF_stuffcmd,       // void(entity client, string s)stuffcmd = #21;
  PF_findradius,     // entity(vector org, float rad) findradius = #22;
  PF_bprint,         // void(string s) bprint				= #23;
  PF_sprint,         // void(entity client, string s) sprint = #24;
  PF_dprint,         // void(string s) dprint				= #25;
  PF_ftos,           // void(string s) ftos				= #26;
  PF_vtos,           // void(string s) vtos				= #27;
  PF_eprint,   // void(entity e) debug print an entire entity
  PF_walkmove, // float(float yaw, float dist) walkmove
  PF_Fixme,    // float(float yaw, float dist) walkmove
};