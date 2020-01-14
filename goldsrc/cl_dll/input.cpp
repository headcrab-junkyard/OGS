/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018-2020 BlackPhrase
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

#include <cstdlib>
#include <cstring>

extern "C"
{
#include "mathlib.h"
};

#include "input.h"
#include "view.h"
#include "usercmd.h"
#include "cvardef.h"
#include "engine.h"
#include "in_buttons.h"

cvar_t *lookspring;
cvar_t *lookstrafe;

cvar_t *cl_upspeed;
cvar_t *cl_forwardspeed;
cvar_t *cl_backspeed;
cvar_t *cl_sidespeed;

cvar_t *cl_movespeedkey;

cvar_t *cl_yawspeed;
cvar_t *cl_pitchspeed;

cvar_t *cl_anglespeedkey;

cvar_t *m_pitch;
cvar_t *m_yaw;
cvar_t *m_forward;
cvar_t *m_side;

cvar_t *cl_pitchup;
cvar_t *cl_pitchdown;

/*
===============================================================================

KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as a parameter to the command so it can be matched up with
the release.

state bit 0 is the current state of the key
state bit 1 is edge triggered on the up to down transition
state bit 2 is edge triggered on the down to up transition

===============================================================================
*/

kbutton_t in_mlook;
kbutton_t in_klook;
kbutton_t in_jlook; // TODO
kbutton_t in_left;
kbutton_t in_right;
kbutton_t in_forward;
kbutton_t in_back;
kbutton_t in_lookup;
kbutton_t in_lookdown;
kbutton_t in_moveleft;
kbutton_t in_moveright;
kbutton_t in_strafe;
kbutton_t in_speed;
kbutton_t in_use;
kbutton_t in_jump;
kbutton_t in_attack;
kbutton_t in_attack2;
kbutton_t in_up;
kbutton_t in_down;
kbutton_t in_duck;
kbutton_t in_reload;
kbutton_t in_alt1;
kbutton_t in_score;
kbutton_t in_break;
kbutton_t in_graph;

int in_impulse;

void KeyDown(kbutton_t *b)
{
	int k;
	const char *c;

	c = gpEngine->Cmd_Argv(1);
	if(c[0])
		k = atoi(c);
	else
		k = -1; // typed manually at the console for continuous down

	if(k == b->down[0] || k == b->down[1])
		return; // repeating key

	if(!b->down[0])
		b->down[0] = k;
	else if(!b->down[1])
		b->down[1] = k;
	else
	{
		gpEngine->Con_Printf("Three keys down for a button!\n");
		return;
	}

	if(b->state & 1)
		return;        // still down
	
	// TODO: q2
	/*
	// save timestamp
	c = Cmd_Argv(2);
	b->downtime = atoi(c);
	if (!b->downtime)
		b->downtime = sys_frame_time - 100;
	*/
	
	b->state |= 1 + 2; // down + impulse down
}

void KeyUp(kbutton_t *b)
{
	int k;
	const char *c;

	c = gpEngine->Cmd_Argv(1);
	if(c[0])
		k = atoi(c);
	else
	{ // typed manually at the console, assume for unsticking, so clear all
		b->down[0] = b->down[1] = 0;
		b->state = 4; // impulse up
		return;
	}

	if(b->down[0] == k)
		b->down[0] = 0;
	else if(b->down[1] == k)
		b->down[1] = 0;
	else
		return; // key up without coresponding down (menu pass through)
	if(b->down[0] || b->down[1])
		return; // some other key is still holding it down

	if(!(b->state & 1))
		return;     // still up (this should not happen)
	b->state &= ~1; // now up
	b->state |= 4;  // impulse up
}

void IN_KLookDown()
{
	KeyDown(&in_klook);
}
void IN_KLookUp()
{
	KeyUp(&in_klook);
}
void IN_MLookDown()
{
	KeyDown(&in_mlook);
}
void IN_MLookUp()
{
	KeyUp(&in_mlook);
	if(!(in_mlook.state & 1) && lookspring->value)
		V_StartPitchDrift();
}
void IN_JLookDown()
{
	KeyDown(&in_jlook);
}
void IN_JLookUp()
{
	KeyUp(&in_jlook);
}
void IN_UpDown()
{
	KeyDown(&in_up);
}
void IN_UpUp()
{
	KeyUp(&in_up);
}
void IN_DownDown()
{
	KeyDown(&in_down);
}
void IN_DownUp()
{
	KeyUp(&in_down);
}
void IN_LeftDown()
{
	KeyDown(&in_left);
}
void IN_LeftUp()
{
	KeyUp(&in_left);
}
void IN_RightDown()
{
	KeyDown(&in_right);
}
void IN_RightUp()
{
	KeyUp(&in_right);
}
void IN_ForwardDown()
{
	KeyDown(&in_forward);
}
void IN_ForwardUp()
{
	KeyUp(&in_forward);
}
void IN_BackDown()
{
	KeyDown(&in_back);
}
void IN_BackUp()
{
	KeyUp(&in_back);
}
void IN_LookupDown()
{
	KeyDown(&in_lookup);
}
void IN_LookupUp()
{
	KeyUp(&in_lookup);
}
void IN_LookdownDown()
{
	KeyDown(&in_lookdown);
}
void IN_LookdownUp()
{
	KeyUp(&in_lookdown);
}
void IN_MoveleftDown()
{
	KeyDown(&in_moveleft);
}
void IN_MoveleftUp()
{
	KeyUp(&in_moveleft);
}
void IN_MoverightDown()
{
	KeyDown(&in_moveright);
}
void IN_MoverightUp()
{
	KeyUp(&in_moveright);
}

void IN_SpeedDown()
{
	KeyDown(&in_speed);
}
void IN_SpeedUp()
{
	KeyUp(&in_speed);
}
void IN_StrafeDown()
{
	KeyDown(&in_strafe);
}
void IN_StrafeUp()
{
	KeyUp(&in_strafe);
}

void IN_AttackDown()
{
	KeyDown(&in_attack);
}
void IN_AttackUp()
{
	KeyUp(&in_attack);
}

void IN_Attack2Down()
{
	KeyDown(&in_attack2);
}
void IN_Attack2Up()
{
	KeyUp(&in_attack2);
}

void IN_UseDown()
{
	KeyDown(&in_use);
}
void IN_UseUp()
{
	KeyUp(&in_use);
}
void IN_JumpDown()
{
	KeyDown(&in_jump);
}
void IN_JumpUp()
{
	KeyUp(&in_jump);
}

void IN_Impulse()
{
	in_impulse = atoi(gpEngine->Cmd_Argv(1)); // TODO: Q_atoi
}

/*
===============
CL_KeyState

Returns 0.25 if a key was pressed and released during the frame,
0.5 if it was pressed and held
0 if held then released, and
1.0 if held for the entire time
===============
*/
float CL_KeyState(kbutton_t *key)
{
	float val;
	qboolean impulsedown, impulseup, down;

	impulsedown = key->state & 2;
	impulseup = key->state & 4;
	down = key->state & 1;
	val = 0;

	if(impulsedown && !impulseup)
		if(down)
			val = 0.5; // pressed and held this frame
		else
			val = 0; //	I_Error ();
	if(impulseup && !impulsedown)
		if(down)
			val = 0; //	I_Error ();
		else
			val = 0; // released this frame
	if(!impulsedown && !impulseup)
		if(down)
			val = 1.0; // held the entire frame
		else
			val = 0; // up the entire frame
	if(impulsedown && impulseup)
		if(down)
			val = 0.75; // released and re-pressed this frame
		else
			val = 0.25; // pressed and released this frame

	key->state &= 1; // clear impulses

	return val;
}

/*
================
CL_AdjustAngles

Moves the local angle positions
================
*/
void CL_AdjustAngles(float frametime)
{
	float speed;
	float up, down;
	vec3_t cl_viewangles{};

	if(in_speed.state & 1)
		speed = frametime * cl_anglespeedkey->value;
	else
		speed = frametime;

	gpEngine->GetViewAngles((float*)cl_viewangles);

	if(!(in_strafe.state & 1))
	{
		cl_viewangles[YAW] -= speed * cl_yawspeed->value * CL_KeyState(&in_right);
		cl_viewangles[YAW] += speed * cl_yawspeed->value * CL_KeyState(&in_left);
		cl_viewangles[YAW] = anglemod(cl_viewangles[YAW]);
	}
	if(in_klook.state & 1)
	{
		V_StopPitchDrift();
		cl_viewangles[PITCH] -= speed * cl_pitchspeed->value * CL_KeyState(&in_forward);
		cl_viewangles[PITCH] += speed * cl_pitchspeed->value * CL_KeyState(&in_back);
	}

	up = CL_KeyState(&in_lookup);
	down = CL_KeyState(&in_lookdown);

	cl_viewangles[PITCH] -= speed * cl_pitchspeed->value * up;
	cl_viewangles[PITCH] += speed * cl_pitchspeed->value * down;

	if(up || down)
		V_StopPitchDrift();

	if(cl_viewangles[PITCH] > 80)
		cl_viewangles[PITCH] = 80;
	if(cl_viewangles[PITCH] < -70)
		cl_viewangles[PITCH] = -70;

	if(cl_viewangles[ROLL] > 50)
		cl_viewangles[ROLL] = 50;
	if(cl_viewangles[ROLL] < -50)
		cl_viewangles[ROLL] = -50;
	
	gpEngine->SetViewAngles((float*)cl_viewangles);
}

/*
================
CL_BaseMove

Send the intended movement message to the server
================
*/
void CL_CreateMove(float frametime, usercmd_t *cmd, int active)
{
	int bits;
	
	if(active)
	{
		CL_AdjustAngles(frametime);

		memset(cmd, 0, sizeof(*cmd));

		//VectorCopy(cl_viewangles, cmd->viewangles); // TODO: qw
		
		if(in_strafe.state & 1)
		{
			cmd->sidemove += cl_sidespeed->value * CL_KeyState(&in_right);
			cmd->sidemove -= cl_sidespeed->value * CL_KeyState(&in_left);
		}

		cmd->sidemove += cl_sidespeed->value * CL_KeyState(&in_moveright);
		cmd->sidemove -= cl_sidespeed->value * CL_KeyState(&in_moveleft);

		cmd->upmove += cl_upspeed->value * CL_KeyState(&in_up);
		cmd->upmove -= cl_upspeed->value * CL_KeyState(&in_down);

		if(!(in_klook.state & 1))
		{
			cmd->forwardmove += cl_forwardspeed->value * CL_KeyState(&in_forward);
			cmd->forwardmove -= cl_backspeed->value * CL_KeyState(&in_back);
		};

		//
		// adjust for speed key
		//
		if(in_speed.state & 1)
		{
			cmd->forwardmove *= cl_movespeedkey->value;
			cmd->sidemove *= cl_movespeedkey->value;
			cmd->upmove *= cl_movespeedkey->value;
		}

		// allow mice or other external controllers to add to the move
		IN_Move(frametime, cmd);
	};
	
	cmd->impulse = in_impulse;
	in_impulse = 0;
	
	cmd->weaponselect = 0; // TODO
	
	//
	// send button bits
	//
	bits = 0;

	if(in_attack.state & 3)
		bits |= IN_ATTACK;
	in_attack.state &= ~2;

	if(in_attack2.state & 3)
		bits |= IN_ATTACK2;
	in_attack2.state &= ~2;
	
	if(in_reload.state & 3)
		bits |= IN_RELOAD;
	in_reload.state &= ~2;
	
	if(in_duck.state & 3)
		bits |= IN_DUCK;
	in_duck.state &= ~2;
	
	if(in_jump.state & 3)
		bits |= IN_JUMP;
	in_jump.state &= ~2;
	
	if(in_use.state & 3)
		bits |= IN_USE;
	in_use.state &= ~2;
	
	if(in_forward.state & 3)
		bits |= IN_FORWARD;
	in_forward.state &= ~2;
	
	if(in_back.state & 3)
		bits |= IN_BACK;
	in_back.state &= ~2;
	
	if(in_left.state & 3)
		bits |= IN_LEFT;
	in_left.state &= ~2;
	
	if(in_right.state & 3)
		bits |= IN_RIGHT;
	in_right.state &= ~2;
	
	if(in_moveleft.state & 3)
		bits |= IN_MOVELEFT;
	in_moveleft.state &= ~2;
	
	if(in_moveright.state & 3)
		bits |= IN_MOVERIGHT;
	in_moveright.state &= ~2;
	
	cmd->buttons = bits;
	
	vec3_t cl_viewangles;
	gpEngine->GetViewAngles((float*)cl_viewangles);
	VectorCopy(cl_viewangles, cmd->viewangles);
	
	// TODO: CBaseCombatWeapon::CreateMove
}

/*
============
CL_InitInput
============
*/
void InitInput()
{
	gpEngine->pfnAddCommand("+moveup", IN_UpDown);
	gpEngine->pfnAddCommand("-moveup", IN_UpUp);
	gpEngine->pfnAddCommand("+movedown", IN_DownDown);
	gpEngine->pfnAddCommand("-movedown", IN_DownUp);
	gpEngine->pfnAddCommand("+left", IN_LeftDown);
	gpEngine->pfnAddCommand("-left", IN_LeftUp);
	gpEngine->pfnAddCommand("+right", IN_RightDown);
	gpEngine->pfnAddCommand("-right", IN_RightUp);
	gpEngine->pfnAddCommand("+forward", IN_ForwardDown);
	gpEngine->pfnAddCommand("-forward", IN_ForwardUp);
	gpEngine->pfnAddCommand("+back", IN_BackDown);
	gpEngine->pfnAddCommand("-back", IN_BackUp);
	gpEngine->pfnAddCommand("+lookup", IN_LookupDown);
	gpEngine->pfnAddCommand("-lookup", IN_LookupUp);
	gpEngine->pfnAddCommand("+lookdown", IN_LookdownDown);
	gpEngine->pfnAddCommand("-lookdown", IN_LookdownUp);
	gpEngine->pfnAddCommand("+strafe", IN_StrafeDown);
	gpEngine->pfnAddCommand("-strafe", IN_StrafeUp);
	gpEngine->pfnAddCommand("+moveleft", IN_MoveleftDown);
	gpEngine->pfnAddCommand("-moveleft", IN_MoveleftUp);
	gpEngine->pfnAddCommand("+moveright", IN_MoverightDown);
	gpEngine->pfnAddCommand("-moveright", IN_MoverightUp);
	gpEngine->pfnAddCommand("+speed", IN_SpeedDown);
	gpEngine->pfnAddCommand("-speed", IN_SpeedUp);
	gpEngine->pfnAddCommand("+attack", IN_AttackDown);
	gpEngine->pfnAddCommand("-attack", IN_AttackUp);
	gpEngine->pfnAddCommand("+attack2", IN_Attack2Down);
	gpEngine->pfnAddCommand("-attack2", IN_Attack2Up);
	gpEngine->pfnAddCommand("+use", IN_UseDown);
	gpEngine->pfnAddCommand("-use", IN_UseUp);
	gpEngine->pfnAddCommand("+jump", IN_JumpDown);
	gpEngine->pfnAddCommand("-jump", IN_JumpUp);
	gpEngine->pfnAddCommand("impulse", IN_Impulse);
	gpEngine->pfnAddCommand("+klook", IN_KLookDown);
	gpEngine->pfnAddCommand("-klook", IN_KLookUp);
	gpEngine->pfnAddCommand("+mlook", IN_MLookDown);
	gpEngine->pfnAddCommand("-mlook", IN_MLookUp);
	
	lookspring = gpEngine->pfnRegisterVariable("lookspring", "0", FCVAR_ARCHIVE);
	lookstrafe = gpEngine->pfnRegisterVariable("lookstrafe", "0", FCVAR_ARCHIVE);
	
	cl_upspeed = gpEngine->pfnRegisterVariable("cl_upspeed", "200", 0); // TODO: FCVAR_ARCHIVE?
	cl_forwardspeed = gpEngine->pfnRegisterVariable("cl_forwardspeed", "200", FCVAR_ARCHIVE);
	cl_backspeed = gpEngine->pfnRegisterVariable("cl_backspeed", "200", FCVAR_ARCHIVE);
	cl_sidespeed = gpEngine->pfnRegisterVariable("cl_sidespeed", "350", 0); // TODO: FCVAR_ARCHIVE?
	
	cl_movespeedkey = gpEngine->pfnRegisterVariable("cl_movespeedkey", "2.0", 0);
	cl_yawspeed = gpEngine->pfnRegisterVariable("cl_yawspeed", "140", 0);
	cl_pitchspeed = gpEngine->pfnRegisterVariable("cl_pitchspeed", "150", 0);
	cl_anglespeedkey = gpEngine->pfnRegisterVariable("cl_anglespeedkey", "1.5", 0);
	
	m_pitch = gpEngine->pfnRegisterVariable("m_pitch", "0.022", FCVAR_ARCHIVE);
	m_yaw = gpEngine->pfnRegisterVariable("m_yaw", "0.022", FCVAR_ARCHIVE);
	m_forward = gpEngine->pfnRegisterVariable("m_forward", "1", FCVAR_ARCHIVE);
	m_side = gpEngine->pfnRegisterVariable("m_side", "0.8", FCVAR_ARCHIVE);
	
	cl_pitchup = gpEngine->pfnRegisterVariable("cl_pitchup", "-70.0", 0);
	cl_pitchdown = gpEngine->pfnRegisterVariable("cl_pitchdown", "80.0", 0);
	
	IN_Init();
	V_Init();
};

void ShutdownInput()
{
	IN_Shutdown();
};