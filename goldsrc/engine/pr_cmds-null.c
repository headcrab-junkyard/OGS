/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"
#include "delta.h"

cvar_t sv_aim = { "sv_aim", "0.93" };

/*
===============================================================================

						BUILT-IN FUNCTIONS

===============================================================================
*/

void PR_CheckEmptyString(const char *s)
{
	if(s[0] <= ' ')
		Host_Error("Bad string");
};

int PF_precache_model_I(const char *s)
{
	int i;

	if(sv.state != ss_loading)
		Host_Error("PF_Precache_*: Precache can only be done in spawn functions");

	PR_CheckEmptyString(s);

	for(i = 0; i < MAX_MODELS; i++)
	{
		if(!sv.model_precache[i])
		{
			sv.model_precache[i] = s;
			sv.models[i] = Mod_ForName(s, true);
			return i;
		};
		
		if(!strcmp(sv.model_precache[i], s))
			return i;
	};
	
	Host_Error("PF_precache_model: overflow");
};

int PF_precache_sound_I(const char *s)
{
	int i;

	if(sv.state != ss_loading)
		Host_Error("PF_Precache_*: Precache can only be done in spawn functions");

	PR_CheckEmptyString(s);

	for(i = 0; i < MAX_SOUNDS; i++)
	{
		if(!sv.sound_precache[i])
		{
			sv.sound_precache[i] = s;
			return i;
		};
		
		if(!strcmp(sv.sound_precache[i], s))
			return i;
	};
	
	Host_Error("PF_precache_sound: overflow");
};

void SetMinMaxSize(edict_t *e, float *min, float *max, qboolean rotate)
{
	float *angles;
	vec3_t rmin, rmax;
	float bounds[2][3];
	float xvector[2], yvector[2];
	float a;
	vec3_t base, transformed;
	int i, j, k, l;

	for(i = 0; i < 3; i++)
		if(min[i] > max[i])
			Host_Error("backwards mins/maxs");

	rotate = false; // FIXME: implement rotation properly again

	if(!rotate)
	{
		VectorCopy(min, rmin);
		VectorCopy(max, rmax);
	}
	else
	{
		// find min / max for rotations
		angles = e->v.angles;

		a = angles[1] / 180 * M_PI;

		xvector[0] = cos(a);
		xvector[1] = sin(a);
		yvector[0] = -sin(a);
		yvector[1] = cos(a);

		VectorCopy(min, bounds[0]);
		VectorCopy(max, bounds[1]);

		rmin[0] = rmin[1] = rmin[2] = 9999;
		rmax[0] = rmax[1] = rmax[2] = -9999;

		for(i = 0; i <= 1; i++)
		{
			base[0] = bounds[i][0];
			for(j = 0; j <= 1; j++)
			{
				base[1] = bounds[j][1];
				for(k = 0; k <= 1; k++)
				{
					base[2] = bounds[k][2];

					// transform the point
					transformed[0] = xvector[0] * base[0] + yvector[0] * base[1];
					transformed[1] = xvector[1] * base[0] + yvector[1] * base[1];
					transformed[2] = base[2];

					for(l = 0; l < 3; l++)
					{
						if(transformed[l] < rmin[l])
							rmin[l] = transformed[l];
						if(transformed[l] > rmax[l])
							rmax[l] = transformed[l];
					};
				};
			};
		};
	};

	// set derived values
	VectorCopy(rmin, e->v.mins);
	VectorCopy(rmax, e->v.maxs);
	VectorSubtract(max, min, e->v.size);

	SV_LinkEdict(e, false);
};

/*
=================
PF_setmodel

setmodel(entity, model)
=================
*/
void PF_setmodel_I(edict_t *e, const char *m)
{
	char **check;
	model_t *mod;
	int i;

	// check to see if model was properly precached
	for(i = 0, check = sv.model_precache; *check; i++, check++)
		if(!strcmp(*check, m))
			break;

	if(!*check)
		Host_Error("no precache: %s\n", m);

	e->v.model = m - pr_strings;
	e->v.modelindex = i; //SV_ModelIndex (m);

	mod = sv.models[(int)e->v.modelindex]; // Mod_ForName (m, true);

	if(mod)
		SetMinMaxSize(e, mod->mins, mod->maxs, true);
	else
		SetMinMaxSize(e, vec3_origin, vec3_origin, true);
};

int PF_modelindex(const char *name)
{
	return SV_ModelIndex(name);
};

int ModelFrames(int modelindex)
{
	return 0; // TODO
};

/*
=================
PF_setsize

the size box is rotated by the current angle

setsize (entity, minvector, maxvector)
=================
*/
void PF_setsize_I(edict_t *e, const float *min, const float *max)
{
	SetMinMaxSize(e, min, max, false);
};

/*
==============
PF_changelevel
==============
*/
void PF_changelevel_I(const char *s1, const char *s2)
{
	// make sure we don't issue two changelevels
	if(svs.changelevel_issued)
		return;
	svs.changelevel_issued = true;

	if(s2 && *s2)
		//if ((int)gGlobalVariables.serverflags & (SFL_NEW_UNIT | SFL_NEW_EPISODE))
			//Cbuf_AddText (va("changelevel %s %s\n",s1, s2));
		//else
			Cbuf_AddText(va("changelevel2 %s %s\n", s1, s2));
	else
		Cbuf_AddText(va("changelevel %s\n", s1));
};

void PF_SaveSpawnParms(edict_t *ent)
{
	// TODO
};

void PF_changeyaw()
{
	// TODO
};

enginefuncs_t gEngineFuncs[] =
{
	PF_precache_model_I,
	PF_precache_sound_I,
	
	PF_setmodel_I,
	PF_modelindex,
	ModelFrames,
	
	PF_setsize_I,
	
	PF_changelevel_I,
	
	NULL, // PF_GetSpawnParms
	PF_SaveSpawnParms
};