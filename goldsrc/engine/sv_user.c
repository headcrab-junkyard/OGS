/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2021 BlackPhrase
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
/// @brief server code for moving users

#include "quakedef.h"

edict_t *sv_player;

extern cvar_t sv_friction;
cvar_t sv_edgefriction = { "edgefriction", "2", FCVAR_SERVER };
extern cvar_t sv_stopspeed;

extern playermove_t svpmove;

extern vec3_t player_mins;

//static vec3_t forward, right, up;

//vec3_t wishdir;
//float wishspeed;

// world
//float *angles;
//float *origin;
//float *velocity;

//int onground; // TODO: was qboolean here but marked as int in decl...

//usercmd_t cmd;

cvar_t sv_idealpitchscale = { "sv_idealpitchscale", "0.8" };

vec3_t pmove_mins, pmove_maxs;

const char *clc_strings[] =
{
	"clc_bad",
	"clc_nop",
	"clc_move",
	"clc_stringcmd",
	"clc_delta",
	"clc_resourcelist",
	"clc_tmove",
	"clc_fileconsistency",
	"clc_voicedata",
	"clc_hltv",
	"clc_cvarvalue",
	"clc_cvarvalue2",
	
	"End of List"
};

/*
====================
AddLinksToPmove

====================
*/
void AddLinksToPmove ( areanode_t *node )
{
	link_t		*l, *next;
	edict_t		*check;
	edict_t		*pl;
	int			i;
	physent_t	*pe;

	pl = sv_player;

	// touch linked edicts
	for (l = node->solid_edicts.next ; l != &node->solid_edicts ; l = next)
	{
		next = l->next;
		check = EDICT_FROM_AREA(l);

		if (check->v.owner == pl)
			continue;		// player's own missile
		if (check->v.solid == SOLID_BSP 
			|| check->v.solid == SOLID_BBOX 
			|| check->v.solid == SOLID_SLIDEBOX)
		{
			if (check == sv_player)
				continue;

			for (i=0 ; i<3 ; i++)
				if (check->v.absmin[i] > pmove_maxs[i]
				|| check->v.absmax[i] < pmove_mins[i])
					break;
			if (i != 3)
				continue;
			if (pmove->numphysent == MAX_PHYSENTS)
				return;
			pe = &pmove->physents[pmove->numphysent];
			pmove->numphysent++;

			VectorCopy (check->v.origin, pe->origin);
			pe->info = NUM_FOR_EDICT(check);
			if (check->v.solid == SOLID_BSP)
				pe->model = sv.models[(int)(check->v.modelindex)];
			else
			{
				pe->model = NULL;
				VectorCopy (check->v.mins, pe->mins);
				VectorCopy (check->v.maxs, pe->maxs);
			}
		}
	}
	
// recurse down both sides
	if (node->axis == -1)
		return;

	if ( pmove_maxs[node->axis] > node->dist )
		AddLinksToPmove ( node->children[0] );
	if ( pmove_mins[node->axis] < node->dist )
		AddLinksToPmove ( node->children[1] );
}

/*
===========
SV_PreRunCmd

Done before running a player command.  Clears the touch array
===========
*/
byte playertouch[(MAX_EDICTS + 7) / 8];

void SV_PreRunCmd()
{
	memset(playertouch, 0, sizeof(playertouch));

	//gEntityInterface.pfnCmdStart(); // TODO
}

/*
===========
SV_RunCmd
===========
*/
void SV_RunCmd(usercmd_t *ucmd)
{
	edict_t *ent;
	int i, n;
	int oldmsec;

	pmove->cmd = *ucmd;

	// chop up very long commands
	if(pmove->cmd.msec > 50)
	{
		oldmsec = ucmd->msec;
		pmove->cmd.msec = oldmsec / 2;
		SV_RunCmd(&pmove->cmd);
		pmove->cmd.msec = oldmsec / 2;
		pmove->cmd.impulse = 0;
		SV_RunCmd(&pmove->cmd);
		return;
	}

	if(!sv_player->v.fixangle)
		VectorCopy(ucmd->viewangles, sv_player->v.v_angle);

	sv_player->v.button = ucmd->buttons;
	if(ucmd->impulse)
		sv_player->v.impulse = ucmd->impulse;

	//
	// angles
	// show 1/3 the pitch angle and all the roll angle
	if(sv_player->v.health > 0)
	{
		if(!sv_player->v.fixangle)
		{
			sv_player->v.angles[PITCH] = -sv_player->v.v_angle[PITCH] / 3;
			sv_player->v.angles[YAW] = sv_player->v.v_angle[YAW];
		}
		sv_player->v.angles[ROLL] =
		V_CalcRoll(sv_player->v.angles, sv_player->v.velocity) * 4;
	}

	host_frametime = ucmd->msec * 0.001;
	if(host_frametime > 0.1)
		host_frametime = 0.1;

	//if(!host_client->spectator) // TODO
	{
		gGlobalVariables.frametime = host_frametime;

		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnPlayerPreThink(sv_player);

		SV_RunThink(sv_player);
		
		gEntityInterface.pfnPlayerPostThink(sv_player);
	}

	for(i = 0; i < 3; i++)
		pmove->origin[i] = sv_player->v.origin[i] + (sv_player->v.mins[i] - player_mins[i]);
	VectorCopy(sv_player->v.velocity, pmove->velocity);
	VectorCopy(sv_player->v.v_angle, pmove->angles);

	//pmove->spectator = host_client->spectator; // TODO
	pmove->waterjumptime = sv_player->v.teleport_time;
	pmove->numphysent = 1;
	pmove->physents[0].model = sv.worldmodel;
	pmove->cmd = *ucmd;
	pmove->dead = sv_player->v.health <= 0;
	//pmove->oldbuttons = host_client->oldbuttons; // TODO

	//movevars.entgravity = host_client->entgravity; // TODO
	//movevars.maxspeed = host_client->maxspeed; // TODO

	for(i = 0; i < 3; i++)
	{
		pmove_mins[i] = pmove->origin[i] - 256;
		pmove_maxs[i] = pmove->origin[i] + 256;
	}

#if 1
	AddLinksToPmove(sv_areanodes);
#else
	AddAllEntsToPmove();
#endif

	//{
	//int before = PM_TestPlayerPosition (pmove->origin);
	gEntityInterface.pfnPM_Move (pmove, true);
	//int after = PM_TestPlayerPosition (pmove->origin);

	//if (sv_player->v.health > 0 && before && !after )
		//Con_Printf ("player %s got stuck in playermove!!!!\n", host_client->name);
	//}

	//host_client->oldbuttons = pmove->oldbuttons; // TODO
	sv_player->v.teleport_time = pmove->waterjumptime;
	sv_player->v.waterlevel = pmove->waterlevel;
	sv_player->v.watertype = pmove->watertype;
	if(pmove->onground != -1) // TODO
	{
		sv_player->v.flags = (int)sv_player->v.flags | FL_ONGROUND;
		sv_player->v.groundentity = EDICT_NUM(pmove->physents[pmove->onground].info); // TODO
	}
	else
		sv_player->v.flags = (int)sv_player->v.flags & ~FL_ONGROUND;
	for(i = 0; i < 3; i++)
		sv_player->v.origin[i] = pmove->origin[i] - (sv_player->v.mins[i] - player_mins[i]);

#if 0
	// truncate velocity the same way the net protocol will
	for (i=0 ; i<3 ; i++)
		sv_player->v.velocity[i] = (int)pmove->velocity[i];
#else
	VectorCopy(pmove->velocity, sv_player->v.velocity);
#endif

	VectorCopy(pmove->angles, sv_player->v.v_angle);

	//if(!host_client->spectator) // TODO
	{
		// link into place and touch triggers
		SV_LinkEdict(sv_player, true);

		// touch other objects
		for(i = 0; i < pmove->numtouch; i++)
		{
			n = pmove->physents[pmove->touchindex[i].ent].info;
			ent = EDICT_NUM(n);
			if(!gEntityInterface.pfnTouch || (playertouch[n / 8] & (1 << (n % 8))))
				continue;
			gEntityInterface.pfnTouch(ent, sv_player);
			playertouch[n / 8] |= 1 << (n % 8);
		}
	}
}

/*
===========
SV_PostRunCmd

Done after running a player command.
===========
*/
void SV_PostRunCmd()
{
	// run post-think

	//if(!host_client->spectator) // TODO
	{
		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnPlayerPostThink(sv_player); // TODO: pfnCmdEnd?
		//SV_RunNewmis(); // TODO: unused
	}
	//else if(gEntityInterface.pfnSpectatorThink) // TODO
	{
		//gGlobalVariables.time = sv.time;
		//gEntityInterface.pfnSpectatorThink(sv_player);
	}
}

/*
===============
SV_SetIdealPitch
===============
*/
#define MAX_FORWARD 6
void SV_SetIdealPitch()
{
	float angleval, sinval, cosval;
	trace_t tr;
	vec3_t top, bottom;
	float z[MAX_FORWARD];
	int i, j;
	int step, dir, steps;

	if(!((int)sv_player->v.flags & FL_ONGROUND))
		return;

	angleval = sv_player->v.angles[YAW] * M_PI * 2 / 360;
	sinval = sin(angleval);
	cosval = cos(angleval);

	for(i = 0; i < MAX_FORWARD; i++)
	{
		top[0] = sv_player->v.origin[0] + cosval * (i + 3) * 12;
		top[1] = sv_player->v.origin[1] + sinval * (i + 3) * 12;
		top[2] = sv_player->v.origin[2] + sv_player->v.view_ofs[2];

		bottom[0] = top[0];
		bottom[1] = top[1];
		bottom[2] = top[2] - 160;

		tr = SV_Move(top, vec3_origin, vec3_origin, bottom, 1, sv_player);
		if(tr.allsolid)
			return; // looking at a wall, leave ideal the way is was

		if(tr.fraction == 1)
			return; // near a dropoff

		z[i] = top[2] + tr.fraction * (bottom[2] - top[2]);
	}

	dir = 0;
	steps = 0;
	for(j = 1; j < i; j++)
	{
		step = z[j] - z[j - 1];
		if(step > -ON_EPSILON && step < ON_EPSILON)
			continue;

		if(dir && (step - dir > ON_EPSILON || step - dir < -ON_EPSILON))
			return; // mixed changes

		steps++;
		dir = step;
	}

	if(!dir)
	{
		sv_player->v.idealpitch = 0;
		return;
	}

	if(steps < 2)
		return;
	sv_player->v.idealpitch = -dir * sv_idealpitchscale.value;
}

/*
==================
SV_UserFriction

==================
*/
/*
void SV_UserFriction()
{
	float *vel;
	float speed, newspeed, control;
	vec3_t start, stop;
	float friction;
	trace_t trace;

	vel = velocity;

	speed = sqrt(vel[0] * vel[0] + vel[1] * vel[1]);
	if(!speed)
		return;

	// if the leading edge is over a dropoff, increase friction
	start[0] = stop[0] = origin[0] + vel[0] / speed * 16;
	start[1] = stop[1] = origin[1] + vel[1] / speed * 16;
	start[2] = origin[2] + sv_player->v.mins[2];
	stop[2] = start[2] - 34;

	trace = SV_Move(start, vec3_origin, vec3_origin, stop, true, sv_player);

	if(trace.fraction == 1.0)
		friction = sv_friction.value * sv_edgefriction.value;
	else
		friction = sv_friction.value;

	// apply friction
	control = speed < sv_stopspeed.value ? sv_stopspeed.value : speed;
	newspeed = speed - host_frametime * control * friction;

	if(newspeed < 0)
		newspeed = 0;
	newspeed /= speed;

	vel[0] = vel[0] * newspeed;
	vel[1] = vel[1] * newspeed;
	vel[2] = vel[2] * newspeed;
}
*/

/*
==============
SV_Accelerate
==============
*/
/*
cvar_t sv_maxspeed = { "sv_maxspeed", "320", false, true };
cvar_t sv_accelerate = { "sv_accelerate", "10" };
#if 0
void SV_Accelerate (vec3_t wishvel)
{
	int			i;
	float		addspeed, accelspeed;
	vec3_t		pushvec;

	if (wishspeed == 0)
		return;

	VectorSubtract (wishvel, velocity, pushvec);
	addspeed = VectorNormalize (pushvec);

	accelspeed = sv_accelerate.value*host_frametime*addspeed;
	if (accelspeed > addspeed)
		accelspeed = addspeed;
	
	for (i=0 ; i<3 ; i++)
		velocity[i] += accelspeed*pushvec[i];	
}
#endif
void SV_Accelerate()
{
	int i;
	float addspeed, accelspeed, currentspeed;

	currentspeed = DotProduct(velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if(addspeed <= 0)
		return;
	accelspeed = sv_accelerate.value * host_frametime * wishspeed;
	if(accelspeed > addspeed)
		accelspeed = addspeed;

	for(i = 0; i < 3; i++)
		velocity[i] += accelspeed * wishdir[i];
}
*/

/*
void SV_AirAccelerate(vec3_t wishveloc)
{
	int i;
	float addspeed, wishspd, accelspeed, currentspeed;

	wishspd = VectorNormalize(wishveloc);
	if(wishspd > 30)
		wishspd = 30;
	currentspeed = DotProduct(velocity, wishveloc);
	addspeed = wishspd - currentspeed;
	if(addspeed <= 0)
		return;
	//	accelspeed = sv_accelerate.value * host_frametime;
	accelspeed = sv_accelerate.value * wishspeed * host_frametime;
	if(accelspeed > addspeed)
		accelspeed = addspeed;

	for(i = 0; i < 3; i++)
		velocity[i] += accelspeed * wishveloc[i];
}
*/

/*
void DropPunchAngle()
{
	float len;

	len = VectorNormalize(sv_player->v.punchangle);

	len -= 10 * host_frametime;
	if(len < 0)
		len = 0;
	VectorScale(sv_player->v.punchangle, len, sv_player->v.punchangle);
}
*/

/*
===================
SV_WaterMove

===================
*/
// TODO: remove, unused
/*
void SV_WaterMove()
{
	float speed, newspeed, wishspeed, addspeed, accelspeed;

	//
	// user intentions
	//

	wishspeed = Length(wishvel);

	//
	// water friction
	//
	speed = Length(velocity);
	if(speed)
	{
		newspeed = speed - host_frametime * speed * sv_friction.value;
		if(newspeed < 0)
			newspeed = 0;
		VectorScale(velocity, newspeed / speed, velocity);
	}
	else
		newspeed = 0;

	//
	// water acceleration
	//
	if(!wishspeed)
		return;

	VectorNormalize(wishvel);
}
*/

/*
void SV_AirMove()
{

	

	// hack to not let you back into teleporter
	if(sv.time < sv_player->v.teleport_time && fmove < 0)
		fmove = 0;

	for(i = 0; i < 3; i++)
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	if((int)sv_player->v.movetype != MOVETYPE_WALK)
		wishvel[2] = cmd.upmove;
	else
		wishvel[2] = 0;

	if(sv_player->v.movetype == MOVETYPE_NOCLIP)
	{ // noclip
		VectorCopy(wishvel, velocity);
	}
	else if(onground)
	{
		SV_UserFriction();
		SV_Accelerate();
	}
	else
	{ // not on ground, so little effect on velocity
		SV_AirAccelerate(wishvel);
	}
}
*/

/*
===================
SV_ClientThink

the move fields specify an intended velocity in pix/sec
the angle fields specify an exact angular motion in degrees
===================
*/
/*
void SV_ClientThink()
{
	vec3_t v_angle;

	onground = (int)sv_player->v.flags & FL_ONGROUND;

	DropPunchAngle();

	//
	// if dead, behave differently
	//
	if(sv_player->v.health <= 0)
		return;

	//
	// angles
	// show 1/3 the pitch angle and all the roll angle
	//cmd = host_client->cmd; // TODO
	angles = sv_player->v.angles;

	VectorAdd(sv_player->v.v_angle, sv_player->v.punchangle, v_angle);
	angles[ROLL] = V_CalcRoll(sv_player->v.angles, sv_player->v.velocity) * 4;
	if(!sv_player->v.fixangle)
	{
		angles[PITCH] = -v_angle[PITCH] / 3;
		angles[YAW] = v_angle[YAW];
	}

	//
	// walk
	//
	if((sv_player->v.waterlevel >= 2) && (sv_player->v.movetype != MOVETYPE_NOCLIP))
	{
		SV_WaterMove();
		return;
	}
}
*/

/*
===================
SV_ReadClientMove
===================
*/
void SV_ReadClientMove(client_t *host_client, usercmd_t *move)
{
	int i;
	vec3_t angle;
	int bits;

	MSG_ReadByte();
	
	// read lossage percentage
	MSG_ReadByte();
	
	// read ping time
	host_client->ping_times[host_client->num_pings % NUM_PING_TIMES] = sv.time - MSG_ReadFloat();
	host_client->num_pings++;

	// read current angles
	for(i = 0; i < 3; i++)
		angle[i] = MSG_ReadAngle();

	VectorCopy(angle, host_client->edict->v.v_angle);
	
	// read movement
	move->forwardmove = MSG_ReadShort();
	move->sidemove = MSG_ReadShort();
	move->upmove = MSG_ReadShort();

	// read light level
	host_client->edict->v.light_level = MSG_ReadByte();
	
	// read buttons
	bits = MSG_ReadShort();
	host_client->edict->v.button = bits;

	// read impulse
	i = MSG_ReadByte();
	if(i)
		host_client->edict->v.impulse = i;

	// read weapon selection
	MSG_ReadByte(); // TODO: handle
}

/*
===================
SV_ReadClientMessage

Returns false if the client should be killed
===================
*/
/*
qboolean SV_ReadClientMessage ()
{
	int		ret;
	int		cmd;
	char		*s;
	
	do
	{
nextmsg:
		ret = NET_GetPacket(NS_SERVER, &net_from, &host_client->netchan.message); // TODO
		if (ret == -1)
		{
			Sys_Printf ("SV_ReadClientMessage: NET_GetMessage failed\n");
			return false;
		}
		if (!ret)
			return true;
					
		MSG_BeginReading ();
		
		while (1)
		{
			if (!host_client->active)
				return false;	// a command caused an error

			if (msg_badread)
			{
				Sys_Printf ("SV_ReadClientMessage: badread\n");
				return false;
			}	
	
			cmd = MSG_ReadChar ();
			
			switch (cmd)
			{
			case -1:
				goto nextmsg;		// end of message
				
			default:
				Sys_Printf ("SV_ReadClientMessage: unknown command char\n");
				return false;
							
			case clc_nop:
//				Sys_Printf ("clc_nop\n");
				break;
				
			case clc_stringcmd:	
				s = MSG_ReadString ();
				//if (host_client->privileged) // TODO
					ret = 2;
				//else
					//ret = 0;
				if (Q_strncasecmp(s, "status", 6) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "god", 3) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "notarget", 8) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "fly", 3) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "name", 4) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "noclip", 6) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "say", 3) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "say_team", 8) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "tell", 4) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "color", 5) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "kill", 4) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "pause", 5) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "spawn", 5) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "begin", 5) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "prespawn", 8) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "kick", 4) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "ping", 4) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "give", 4) == 0)
					ret = 1;
				else if (Q_strncasecmp(s, "ban", 3) == 0)
					ret = 1;
				if (ret == 2)
					Cbuf_InsertText (s);
				else if (ret == 1)
					Cmd_ExecuteString (s, src_client);
				else
					Con_DPrintf("%s tried to %s\n", host_client->name, s);
				break;
				
			case clc_disconnect:
//				Sys_Printf ("SV_ReadClientMessage: client disconnected\n");
				return false;
			}
		}
	} while (ret == 1);
	
	return true;
}
*/

/*
==================
SV_RunClients
==================
*/
/*
void SV_RunClients ()
{
	int				i;
	
	for (i=0, host_client = svs.clients ; i<svs.maxclients ; i++, host_client++)
	{
		if (!host_client->active)
			continue;
	
		sv_player = host_client->edict;

		if (!SV_ReadClientMessage ())
		{
			SV_DropClient (host_client, false, "Bye!");	// client misbehaved... // TODO
			continue;
		}

		if (!host_client->spawned)
		{
		// clear client movement until a new packet is received
			memset (&host_client->cmd, 0, sizeof(host_client->cmd));
			continue;
		}

// always pause in single player if in console or menus
		if (!sv.paused && (svs.maxclients > 1 || key_dest == key_game) )
			SV_ClientThink ();
	}
}
*/


void SV_ParseDelta(client_t *cl)
{
	cl->delta_sequence = MSG_ReadByte();
};

void SV_ParseConsistencyResponse(client_t *cl)
{
	// TODO
};

void SV_ParseVoiceData(client_t *cl)
{
	int nPlayerIndex = MSG_ReadByte();
	int nSize = MSG_ReadWord();
	
	byte *pData = (byte*)Mem_Alloc(nSize);
	
	for(int i = 0; i < nSize; i++)
		pData[i] = MSG_ReadByte();
	
	// TODO: do something with the data (send to other clients)
	
	Mem_Free(pData);
};

void SV_ParseStringCommand(client_t *cl)
{
	char *s = MSG_ReadString();
	
	Cmd_TokenizeString(s);
	
	host_client = cl; // TODO: hack to let SV_New_f work properly
	sv_player = cl->edict;
	
	if(Cmd_Exists(Cmd_Argv(0)))
		Cmd_ExecuteString(s, src_client); // TODO: this allows players to call any cmd on the server??????????????
	else
		gEntityInterface.pfnClientCommand(sv_player);
};

void SV_ParseResourceList(client_t *cl)
{
	// TODO
};

/*
==================
SV_ExecuteUserCommand
==================
*/
void SV_ExecuteUserCommand(const char *s)
{
	//ucmd_t	*u;
	
	Cmd_TokenizeString (s /*, true*/); // TODO: q2
	sv_player = host_client->edict; // TODO: = sv_client->edict in q2

	SV_BeginRedirect (RD_CLIENT); // TODO: commented out in q2

	// TODO: Cmd_ExecuteString here instead of this
	/*
	for (u=ucmds ; u->name ; u++)
		if (!strcmp (Cmd_Argv(0), u->name) )
		{
			u->func ();
			break;
		};
	*/
	
	
	//if(!u->name /*&& sv.state == ss_game*/) // TODO
		gEntityInterface.pfnClientCommand(sv_player);
	
	// TODO: qw
	//if(!u->name)
		//Con_Printf("Bad user command: %s\n", Cmd_Argv(0));

	SV_EndRedirect(); // TODO: commented out in q2
}

/*
===================
SV_ExecuteClientMessage

The current net_message is parsed for the given client
===================
*/
void SV_ExecuteClientMessage(client_t *cl)
{
	int c;
	char *s;
	usercmd_t oldest, oldcmd, newcmd;
	client_frame_t *frame;
	vec3_t o;
	qboolean move_issued = false; //only allow one move command
	int checksumIndex;
	byte checksum, calculatedChecksum;
	int seq_hash;

	// calc ping time
	frame = &cl->frames[cl->netchan.incoming_acknowledged & UPDATE_MASK];
	frame->ping_time = realtime - frame->senttime;

	// make sure the reply sequence number matches the incoming
	// sequence number
	if(cl->netchan.incoming_sequence >= cl->netchan.outgoing_sequence)
		cl->netchan.outgoing_sequence = cl->netchan.incoming_sequence;
	else
		cl->send_message = false; // don't reply, sequences have slipped

	// save time for ping calculations
	cl->frames[cl->netchan.outgoing_sequence & UPDATE_MASK].senttime = realtime;
	cl->frames[cl->netchan.outgoing_sequence & UPDATE_MASK].ping_time = -1;

	host_client = cl;
	sv_player = host_client->edict;
	pmove = &svpmove;

	//	seq_hash = (cl->netchan.incoming_sequence & 0xffff) ; // ^ QW_CHECK_HASH;
	seq_hash = cl->netchan.incoming_sequence;

	// mark time so clients will know how much to predict
	// other players
	cl->localtime = sv.time;
	cl->delta_sequence = -1; // no delta unless requested
	while(1)
	{
		if(msg_badread)
		{
			Con_Printf("SV_ReadClientMessage: badread\n");
			SV_DropClient(cl, false, "badread");
			return;
		}

		c = MSG_ReadByte();
		if(c == -1)
			break;

		switch(c)
		{
		default:
			Con_Printf("SV_ReadClientMessage: unknown command char (%d)\n", c);
			//SV_DropClient(cl, false, "unknown command char"); // TODO: just ignore the message for now
			return;

		case clc_nop:
			break;

		case clc_move:
			SV_ReadClientMove(cl, &cl->lastcmd); // TODO
			// TODO: qw
			/*
			if(move_issued)
				return; // someone is trying to cheat...

			move_issued = true;

			//checksumIndex = MSG_GetReadCount(); // TODO
			checksum = (byte)MSG_ReadByte();

			// read loss percentage
			//cl->lossage = MSG_ReadByte(); // TODO

			//MSG_ReadDeltaUsercmd(&nullcmd, &oldest); // TODO
			MSG_ReadDeltaUsercmd(&oldest, &oldcmd);
			MSG_ReadDeltaUsercmd(&oldcmd, &newcmd);
			*/
			
			if(!cl->spawned)
				break;

			/*
			// if the checksum fails, ignore the rest of the packet
			//calculatedChecksum = COM_BlockSequenceCRCByte( // TODO
			//net_message.data + checksumIndex + 1, MSG_GetReadCount() - checksumIndex - 1, seq_hash);

			if(calculatedChecksum != checksum)
			{
				Con_DPrintf("Failed command checksum for %s(%d) (%d != %d)\n",
				            cl->name, cl->netchan.incoming_sequence, checksum, calculatedChecksum);
				return;
			}
			*/
			
			if(!sv.paused)
			{
				SV_PreRunCmd(cl->edict, &cl->lastcmd, 0); // TODO: gen random seed

				if(net_drop < 20)
				{
					while(net_drop > 2)
					{
						SV_RunCmd(&cl->lastcmd);
						net_drop--;
					}
					if(net_drop > 1)
						SV_RunCmd(&oldest);
					if(net_drop > 0)
						SV_RunCmd(&oldcmd);
				};
				SV_RunCmd(&newcmd);

				SV_PostRunCmd();
			};

			cl->lastcmd = newcmd;
			cl->lastcmd.buttons = 0; // avoid multiple fires on lag
			
			break;

		case clc_stringcmd:
			SV_ParseStringCommand(cl);
			//s = MSG_ReadString();
			//SV_ExecuteUserCommand(s);
			break;
		
		case clc_delta:
			SV_ParseDelta(cl);
			break;
		
		case clc_resourcelist:
			SV_ParseResourceList(cl);
			break;

		case clc_tmove:
		// TODO: unused?
		/*
			o[0] = MSG_ReadCoord();
			o[1] = MSG_ReadCoord();
			o[2] = MSG_ReadCoord();
			// only allowed by spectators
			//if(host_client->spectator)
			{
				VectorCopy(o, sv_player->v.origin);
				SV_LinkEdict(sv_player, false);
			}
		*/
			break;
		
		case clc_fileconsistency:
			SV_ParseConsistencyResponse(cl);
			break;
		
		case clc_voicedata:
			SV_ParseVoiceData(cl);
			break;
		
		case clc_hltv:
			// TODO
			break;
		
		case clc_cvarvalue:
			// TODO
			break;
		
		case clc_cvarvalue2:
			// TODO
			break;
		
		// TODO: unused
		/*
		case clc_upload:
			SV_NextUpload();
			break;
		*/
		}
	}
}