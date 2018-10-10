/// @file

cvar_t lookspring = { "lookspring", "0", FCVAR_ARCHIVE };
cvar_t lookstrafe = { "lookstrafe", "0", FCVAR_ARCHIVE };

cvar_t cl_upspeed = { "cl_upspeed", "200" }; // TODO: FCVAR_ARCHIVE?
cvar_t cl_forwardspeed = { "cl_forwardspeed", "200", FCVAR_ARCHIVE };
cvar_t cl_backspeed = { "cl_backspeed", "200", FCVAR_ARCHIVE };
cvar_t cl_sidespeed = { "cl_sidespeed", "350" }; // TODO: FCVAR_ARCHIVE?

cvar_t cl_movespeedkey = { "cl_movespeedkey", "2.0" };

cvar_t cl_yawspeed = { "cl_yawspeed", "140" };
cvar_t cl_pitchspeed = { "cl_pitchspeed", "150" };

cvar_t cl_anglespeedkey = { "cl_anglespeedkey", "1.5" };

cvar_t m_pitch = { "m_pitch", "0.022", FCVAR_ARCHIVE };
cvar_t m_yaw = { "m_yaw", "0.022", FCVAR_ARCHIVE };
cvar_t m_forward = { "m_forward", "1", FCVAR_ARCHIVE };
cvar_t m_side = { "m_side", "0.8", FCVAR_ARCHIVE };

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

	c = Cmd_Argv(1);
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
		Con_Printf("Three keys down for a button!\n");
		return;
	}

	if(b->state & 1)
		return;        // still down
	b->state |= 1 + 2; // down + impulse down
}

void KeyUp(kbutton_t *b)
{
	int k;
	const char *c;

	c = Cmd_Argv(1);
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
	if(!(in_mlook.state & 1) && lookspring.value)
		V_StartPitchDrift();
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
	in_impulse = Q_atoi(Cmd_Argv(1));
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

	if(in_speed.state & 1)
		speed = frametime * cl_anglespeedkey.value;
	else
		speed = frametime;

	if(!(in_strafe.state & 1))
	{
		cl.viewangles[YAW] -= speed * cl_yawspeed.value * CL_KeyState(&in_right);
		cl.viewangles[YAW] += speed * cl_yawspeed.value * CL_KeyState(&in_left);
		cl.viewangles[YAW] = anglemod(cl.viewangles[YAW]);
	}
	if(in_klook.state & 1)
	{
		V_StopPitchDrift();
		cl.viewangles[PITCH] -= speed * cl_pitchspeed.value * CL_KeyState(&in_forward);
		cl.viewangles[PITCH] += speed * cl_pitchspeed.value * CL_KeyState(&in_back);
	}

	up = CL_KeyState(&in_lookup);
	down = CL_KeyState(&in_lookdown);

	cl.viewangles[PITCH] -= speed * cl_pitchspeed.value * up;
	cl.viewangles[PITCH] += speed * cl_pitchspeed.value * down;

	if(up || down)
		V_StopPitchDrift();

	if(cl.viewangles[PITCH] > 80)
		cl.viewangles[PITCH] = 80;
	if(cl.viewangles[PITCH] < -70)
		cl.viewangles[PITCH] = -70;

	if(cl.viewangles[ROLL] > 50)
		cl.viewangles[ROLL] = 50;
	if(cl.viewangles[ROLL] < -50)
		cl.viewangles[ROLL] = -50;
}

/*
================
CL_BaseMove

Send the intended movement message to the server
================
*/
void CL_CreateMove(float frametime, usercmd_t *cmd, int active)
{
	if(active)
	{
		CL_AdjustAngles(frametime);

		memset(cmd, 0, sizeof(*cmd));

		if(in_strafe.state & 1)
		{
			cmd->sidemove += cl_sidespeed.value * CL_KeyState(&in_right);
			cmd->sidemove -= cl_sidespeed.value * CL_KeyState(&in_left);
		}

		cmd->sidemove += cl_sidespeed.value * CL_KeyState(&in_moveright);
		cmd->sidemove -= cl_sidespeed.value * CL_KeyState(&in_moveleft);

		cmd->upmove += cl_upspeed.value * CL_KeyState(&in_up);
		cmd->upmove -= cl_upspeed.value * CL_KeyState(&in_down);

		if(!(in_klook.state & 1))
		{
			cmd->forwardmove += cl_forwardspeed.value * CL_KeyState(&in_forward);
			cmd->forwardmove -= cl_backspeed.value * CL_KeyState(&in_back);
		};

		//
		// adjust for speed key
		//
		if(in_speed.state & 1)
		{
			cmd->forwardmove *= cl_movespeedkey.value;
			cmd->sidemove *= cl_movespeedkey.value;
			cmd->upmove *= cl_movespeedkey.value;
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
		bits |= 1;
	in_attack.state &= ~2;

	if(in_jump.state & 3)
		bits |= 2;
	in_jump.state &= ~2;
	
	cmd->buttons = bits;
}

/*
============
CL_InitInput
============
*/
void InitInput()
{
	Cmd_AddCommand("+moveup", IN_UpDown);
	Cmd_AddCommand("-moveup", IN_UpUp);
	Cmd_AddCommand("+movedown", IN_DownDown);
	Cmd_AddCommand("-movedown", IN_DownUp);
	Cmd_AddCommand("+left", IN_LeftDown);
	Cmd_AddCommand("-left", IN_LeftUp);
	Cmd_AddCommand("+right", IN_RightDown);
	Cmd_AddCommand("-right", IN_RightUp);
	Cmd_AddCommand("+forward", IN_ForwardDown);
	Cmd_AddCommand("-forward", IN_ForwardUp);
	Cmd_AddCommand("+back", IN_BackDown);
	Cmd_AddCommand("-back", IN_BackUp);
	Cmd_AddCommand("+lookup", IN_LookupDown);
	Cmd_AddCommand("-lookup", IN_LookupUp);
	Cmd_AddCommand("+lookdown", IN_LookdownDown);
	Cmd_AddCommand("-lookdown", IN_LookdownUp);
	Cmd_AddCommand("+strafe", IN_StrafeDown);
	Cmd_AddCommand("-strafe", IN_StrafeUp);
	Cmd_AddCommand("+moveleft", IN_MoveleftDown);
	Cmd_AddCommand("-moveleft", IN_MoveleftUp);
	Cmd_AddCommand("+moveright", IN_MoverightDown);
	Cmd_AddCommand("-moveright", IN_MoverightUp);
	Cmd_AddCommand("+speed", IN_SpeedDown);
	Cmd_AddCommand("-speed", IN_SpeedUp);
	Cmd_AddCommand("+attack", IN_AttackDown);
	Cmd_AddCommand("-attack", IN_AttackUp);
	Cmd_AddCommand("+use", IN_UseDown);
	Cmd_AddCommand("-use", IN_UseUp);
	Cmd_AddCommand("+jump", IN_JumpDown);
	Cmd_AddCommand("-jump", IN_JumpUp);
	Cmd_AddCommand("impulse", IN_Impulse);
	Cmd_AddCommand("+klook", IN_KLookDown);
	Cmd_AddCommand("-klook", IN_KLookUp);
	Cmd_AddCommand("+mlook", IN_MLookDown);
	Cmd_AddCommand("-mlook", IN_MLookUp);
	
	Cvar_RegisterVariable(&lookspring);
	Cvar_RegisterVariable(&lookstrafe);
	
	Cvar_RegisterVariable(&cl_upspeed);
	Cvar_RegisterVariable(&cl_forwardspeed);
	Cvar_RegisterVariable(&cl_backspeed);
	Cvar_RegisterVariable(&cl_sidespeed);
	
	Cvar_RegisterVariable(&cl_movespeedkey);
	Cvar_RegisterVariable(&cl_yawspeed);
	Cvar_RegisterVariable(&cl_pitchspeed);
	Cvar_RegisterVariable(&cl_anglespeedkey);
	
	Cvar_RegisterVariable(&m_pitch);
	Cvar_RegisterVariable(&m_yaw);
	Cvar_RegisterVariable(&m_forward);
	Cvar_RegisterVariable(&m_side);
};