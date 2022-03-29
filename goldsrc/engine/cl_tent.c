/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021 BlackPhrase
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
/// @brief client side temporary entities

#include "quakedef.h"

int num_temp_entities;
cl_entity_t cl_temp_entities[MAX_TEMP_ENTITIES];
beam_t cl_beams[MAX_BEAMS];

sfx_t *cl_sfx_tink1;
sfx_t *cl_sfx_ric1;
sfx_t *cl_sfx_ric2;
sfx_t *cl_sfx_ric3;
sfx_t *cl_sfx_r_exp3;

/*
=================
CL_ParseTEnt
=================
*/
void CL_InitTEnts()
{
	cl_sfx_tink1 = S_PrecacheSound("weapons/tink1.wav");
	cl_sfx_ric1 = S_PrecacheSound("weapons/ric1.wav");
	cl_sfx_ric2 = S_PrecacheSound("weapons/ric2.wav");
	cl_sfx_ric3 = S_PrecacheSound("weapons/ric3.wav");
	cl_sfx_r_exp3 = S_PrecacheSound("weapons/r_exp3.wav");
}

/*
=================
CL_ParseBeam
=================
*/
void CL_ParseBeam(model_t *m)
{
	int ent;
	vec3_t start, end;
	beam_t *b;
	int i;

	ent = MSG_ReadShort();

	start[0] = MSG_ReadCoord();
	start[1] = MSG_ReadCoord();
	start[2] = MSG_ReadCoord();

	end[0] = MSG_ReadCoord();
	end[1] = MSG_ReadCoord();
	end[2] = MSG_ReadCoord();

	// override any beam with the same entity
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
		if(b->entity == ent)
		{
			b->entity = ent;
			b->model = m;
			b->endtime = cl.time + 0.2;
			VectorCopy(start, b->start);
			VectorCopy(end, b->end);
			return;
		};

	// find a free beam
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
	{
		if(!b->model || b->endtime < cl.time)
		{
			b->entity = ent;
			b->model = m;
			b->endtime = cl.time + 0.2;
			VectorCopy(start, b->start);
			VectorCopy(end, b->end);
			return;
		};
	};
	Con_Printf("beam list overflow!\n");
};

/*
=================
CL_ParseTEnt
=================
*/
void CL_ParseTEnt()
{
	int type;
	vec3_t pos;
	vec3_t endpos;
	dlight_t *dl;
	int rnd;
	int colorStart, colorLength;

	type = MSG_ReadByte();
	switch(type)
	{
	case TE_BEAMPOINTS:
		// TODO
		for(int i = 0; i < 3; ++i)
			pos[i] = MSG_ReadCoord();
		
		for(int i = 0; i < 3; ++i)
			endpos[i] = MSG_ReadCoord();
		
		MSG_ReadShort();
		
		MSG_ReadByte();
		MSG_ReadByte();
		MSG_ReadByte();
		MSG_ReadByte();
		MSG_ReadByte();
		
		for(int i = 0; i < 3; ++i)
			MSG_ReadByte();
		
		MSG_ReadByte();
		MSG_ReadByte();
		break;

	case TE_BEAMENTPOINT:
		// TODO
		MSG_ReadShort();
		
		for(int i = 0; i < 3; ++i)
			pos[i] = MSG_ReadCoord();
		
		MSG_ReadShort();
		
		MSG_ReadByte();
		MSG_ReadByte();
		MSG_ReadByte();
		MSG_ReadByte();
		MSG_ReadByte();
		
		for(int i = 0; i < 3; ++i)
			MSG_ReadByte();
		
		MSG_ReadByte();
		MSG_ReadByte();
		break;

/*
	case TE_SPIKE: // spike hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
#ifdef GLTEST
		Test_Spawn(pos);
#else
		R_RunParticleEffect(pos, vec3_origin, 0, 10);
#endif
		if(rand() % 5)
			S_StartDynamicSound(-1, 0, cl_sfx_tink1, pos, 1, 1, PITCH_NORM);
		else
		{
			rnd = rand() & 3;
			if(rnd == 1)
				S_StartDynamicSound(-1, 0, cl_sfx_ric1, pos, 1, 1, PITCH_NORM);
			else if(rnd == 2)
				S_StartDynamicSound(-1, 0, cl_sfx_ric2, pos, 1, 1, PITCH_NORM);
			else
				S_StartDynamicSound(-1, 0, cl_sfx_ric3, pos, 1, 1, PITCH_NORM);
		}
		break;
	case TE_SUPERSPIKE: // super spike hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 0, 20);

		if(rand() % 5)
			S_StartDynamicSound(-1, 0, cl_sfx_tink1, pos, 1, 1, PITCH_NORM);
		else
		{
			rnd = rand() & 3;
			if(rnd == 1)
				S_StartDynamicSound(-1, 0, cl_sfx_ric1, pos, 1, 1, PITCH_NORM);
			else if(rnd == 2)
				S_StartDynamicSound(-1, 0, cl_sfx_ric2, pos, 1, 1, PITCH_NORM);
			else
				S_StartDynamicSound(-1, 0, cl_sfx_ric3, pos, 1, 1, PITCH_NORM);
		}
		break;
*/
	case TE_GUNSHOT: // bullet hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 0, 20);
		break;

	case TE_EXPLOSION: // rocket explosion
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_ParticleExplosion(pos);
		dl = CL_AllocDlight(0);
		VectorCopy(pos, dl->origin);
		dl->radius = 350;
		dl->die = cl.time + 0.5;
		dl->decay = 300;
		S_StartDynamicSound(-1, 0, cl_sfx_r_exp3, pos, 1, 1, PITCH_NORM);
		break;

	case TE_TAREXPLOSION: // tarbaby explosion
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_BlobExplosion(pos);

		S_StartDynamicSound(-1, 0, cl_sfx_r_exp3, pos, 1, 1, PITCH_NORM);
		break;

	case TE_SMOKE:
		// TODO
		break;

	case TE_TRACER:
		// TODO
		break;

	case TE_LIGHTNING: // lightning bolts
		CL_ParseBeam(Mod_ForName("models/bolt.mdl", true));
		break;

	case TE_BEAMENTS:
		// TODO
		break;

	case TE_SPARKS:
		// TODO
		break;

/*
	case TE_LIGHTNING2: // lightning bolts
		CL_ParseBeam(Mod_ForName("progs/bolt2.mdl", true));
		break;

	case TE_LIGHTNING3: // lightning bolts
		CL_ParseBeam(Mod_ForName("progs/bolt3.mdl", true));
		break;
*/

	// PGM 01/21/97
	case TE_BEAM: // grappling hook beam
		CL_ParseBeam(Mod_ForName("progs/beam.mdl", true));
		break;
	// PGM 01/21/97

	case TE_LAVASPLASH:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_LavaSplash(pos);
		break;

	case TE_TELEPORT:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_TeleportSplash(pos);
		break;

	case TE_EXPLOSION2: // color mapped explosion
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		colorStart = MSG_ReadByte();
		colorLength = MSG_ReadByte();
		R_ParticleExplosion2(pos, colorStart, colorLength);
		dl = CL_AllocDlight(0);
		VectorCopy(pos, dl->origin);
		dl->radius = 350;
		dl->die = cl.time + 0.5;
		dl->decay = 300;
		S_StartDynamicSound(-1, 0, cl_sfx_r_exp3, pos, 1, 1, PITCH_NORM);
		break;

	case TE_BSPDECAL:
		// TODO
		break;

	case TE_IMPLOSION:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		S_StartDynamicSound(-1, 0, cl_sfx_imp, pos, 1, 1, PITCH_NORM);
		break;

	case TE_SPRITETRAIL:
		// TODO
		break;

/*
	case TE_RAILTRAIL:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();
		S_StartDynamicSound(-1, 0, cl_sfx_rail, pos, 1, 1, PITCH_NORM);
		S_StartDynamicSound(-1, 1, cl_sfx_r_exp3, endpos, 1, 1, PITCH_NORM);
		R_RocketTrail(pos, endpos, 0 + 128);
		R_ParticleExplosion(endpos);
		dl = CL_AllocDlight(-1);
		VectorCopy(endpos, dl->origin);
		dl->radius = 350;
		dl->die = cl.time + 0.5;
		dl->decay = 300;
		break;
*/

	case TE_BEAMSPRITE:
		// TODO
		break;

	case TE_BEAMTORUS:
		// TODO
		break;

	case TE_BEAMDISK:
		// TODO
		break;

	case TE_BEAMCYLINDER:
		// TODO
		break;

	case TE_BEAMFOLLOW:
		// TODO
		break;
	
	case TE_GLOWSPRITE:
		// TODO
		break;
	
	case TE_BEAMRING:
		// TODO
		break;
	
	case TE_STREAK_SPLASH:
		// TODO
		break;
	
	case TE_BEAMHOSE:
		// TODO
		break;
	
	case TE_DLIGHT:
		// TODO
		break;

	case TE_ELIGHT:
		// TODO
		break;
	
	case TE_TEXTMESSAGE:
		// TODO
		break;

	case TE_LINE:
		// TODO
		break;

	case TE_BOX:
		// TODO
		break;
	
	case TE_KILLBEAM:
		// TODO
		break;

	case TE_LARGEFUNNEL:
		// TODO
		break;

	case TE_BLOODSTREAM:
		// TODO
		break;

	case TE_SHOWLINE:
		// TODO
		break;

	case TE_BLOOD:
		// TODO
		break;

	case TE_DECAL:
		// TODO
		break;

	case TE_FIZZ:
		// TODO
		break;

	case TE_MODEL:
		// TODO
		break;

	case TE_EXPLODEMODEL:
		// TODO
		break;

	case TE_BREAKMODEL:
		// TODO
		break;

	case TE_GUNSHOTDECAL:
		// TODO
		break;

	case TE_SPRITE_SPRAY:
		// TODO
		break;

	case TE_ARMOR_RICOCHET:
		// TODO
		break;

	case TE_PLAYERDECAL:
		// TODO
		break;

	case TE_BUBBLES:
		// TODO
		break;

	case TE_BUBBLETRAIL:
		// TODO
		break;

	case TE_BLOODSPRITE:
		// TODO
		break;

	case TE_WORLDDECAL:
		// TODO
		break;

	case TE_WORLDDECALHIGH:
		// TODO
		break;

	case TE_DECALHIGH:
		// TODO
		break;

	case TE_PROJECTILE:
		// TODO
		break;

	case TE_SPRAY:
		// TODO
		break;

	case TE_PLAYERSPRITES:
		// TODO
		break;

	case TE_PARTICLEBURST:
		// TODO
		break;

	case TE_FIREFIELD:
		// TODO
		break;

	case TE_PLAYERATTACHMENT:
		// TODO
		break;

	case TE_KILLPLAYERATTACHMENTS:
		// TODO
		break;

	case TE_MULTIGUNSHOT:
		// TODO
		break;

	case TE_USERTRACER:
		// TODO
		break;

	default:
		Sys_Error("CL_ParseTEnt: bad type");
	};
};

/*
=================
CL_NewTempEntity
=================
*/
cl_entity_t *CL_NewTempEntity()
{
	cl_entity_t *ent;

	if(cl_numvisedicts == MAX_VISEDICTS)
		return NULL;
	if(num_temp_entities == MAX_TEMP_ENTITIES)
		return NULL;
	ent = &cl_temp_entities[num_temp_entities];
	memset(ent, 0, sizeof(*ent));
	num_temp_entities++;
	cl_visedicts[cl_numvisedicts] = ent;
	cl_numvisedicts++;

	ent->colormap = vid.colormap;
	return ent;
};

/*
=================
CL_UpdateTEnts
=================
*/
void CL_UpdateTEnts()
{
	int i;
	beam_t *b;
	vec3_t dist, org;
	float d;
	cl_entity_t *ent;
	float yaw, pitch;
	float forward;

	num_temp_entities = 0;

	// update lightning
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
	{
		if(!b->model || b->endtime < cl.time)
			continue;

		// if coming from the player, update the start position
		if(b->entity == cl.viewentity)
			VectorCopy(cl_entities[cl.viewentity].origin, b->start);

		// calculate pitch and yaw
		VectorSubtract(b->end, b->start, dist);

		if(dist[1] == 0 && dist[0] == 0)
		{
			yaw = 0;
			if(dist[2] > 0)
				pitch = 90;
			else
				pitch = 270;
		}
		else
		{
			yaw = (int)(atan2(dist[1], dist[0]) * 180 / M_PI);
			if(yaw < 0)
				yaw += 360;

			forward = sqrt(dist[0] * dist[0] + dist[1] * dist[1]);
			pitch = (int)(atan2(dist[2], forward) * 180 / M_PI);
			if(pitch < 0)
				pitch += 360;
		};

		// add new entities for the lightning
		VectorCopy(b->start, org);
		d = VectorNormalize(dist);
		while(d > 0)
		{
			ent = CL_NewTempEntity();
			if(!ent)
				return;
			VectorCopy(org, ent->origin);
			ent->model = b->model;
			ent->angles[0] = pitch;
			ent->angles[1] = yaw;
			ent->angles[2] = rand() % 360;

			for(i = 0; i < 3; i++)
				org[i] += dist[i] * 30;
			d -= 30;
		};
	};
};