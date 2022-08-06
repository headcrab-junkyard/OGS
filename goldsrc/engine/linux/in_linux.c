/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2022 BlackPhrase
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

#include "quakdef.h"

cvar_t _windowed_mouse = { "_windowed_mouse", "0", true };

float old_windowed_mouse;

qboolean mouse_avail;
int mouse_buttonstate;

void mousehandler(int buttonstate, int dx, int dy)
{
	mouse_buttonstate = buttonstate;
	mx += dx;
	my += dy;
}

void IN_Init()
{
#ifdef SVGALIB
	if(UseMouse)
	{
		Cvar_RegisterVariable(&mouse_button_commands[0]);
		Cvar_RegisterVariable(&mouse_button_commands[1]);
		Cvar_RegisterVariable(&mouse_button_commands[2]);

		mouse_buttons = 3;

		int mtype = vga_getmousetype();

		char *mousedev = "/dev/mouse";
		if(getenv("MOUSEDEV"))
			mousedev = getenv("MOUSEDEV");
		if(COM_CheckParm("-mdev"))
			mousedev = com_argv[COM_CheckParm("-mdev") + 1];

		int mouserate = 1200;
		if(getenv("MOUSERATE"))
			mouserate = atoi(getenv("MOUSERATE"));
		if(COM_CheckParm("-mrate"))
			mouserate = atoi(com_argv[COM_CheckParm("-mrate") + 1]);

		//		printf("Mouse: dev=%s,type=%s,speed=%d\n",
		//			mousedev, mice[mtype].name, mouserate);
		if(mouse_init(mousedev, mtype, mouserate))
		{
			Con_Printf("No mouse found\n");
			UseMouse = 0;
		}
		else
			mouse_seteventhandler(mousehandler);
	}
#elif VIDX
	Cvar_RegisterVariable(&_windowed_mouse);
	
	mouse_x = mouse_y = 0.0;
	mouse_avail = 1;
#enidf
}

void IN_Shutdown()
{
#ifdef SVGALIB
	if(UseMouse)
		mouse_close();
#elif VIDX
	mouse_avail = 0;
#enidf
}

/*
===========
IN_Commands
===========
*/
void IN_Commands()
{
#ifdef SVGALIB
	if(UseMouse && cls.state != ca_dedicated)
	{
		// poll mouse values
		while(mouse_update())
			;

		// perform button actions
		if((mouse_buttonstate & MOUSE_LEFTBUTTON) &&
		   !(mouse_oldbuttonstate & MOUSE_LEFTBUTTON))
			Key_Event(K_MOUSE1, true);
		else if(!(mouse_buttonstate & MOUSE_LEFTBUTTON) &&
		        (mouse_oldbuttonstate & MOUSE_LEFTBUTTON))
			Key_Event(K_MOUSE1, false);

		if((mouse_buttonstate & MOUSE_RIGHTBUTTON) &&
		   !(mouse_oldbuttonstate & MOUSE_RIGHTBUTTON))
			Key_Event(K_MOUSE2, true);
		else if(!(mouse_buttonstate & MOUSE_RIGHTBUTTON) &&
		        (mouse_oldbuttonstate & MOUSE_RIGHTBUTTON))
			Key_Event(K_MOUSE2, false);

		if((mouse_buttonstate & MOUSE_MIDDLEBUTTON) &&
		   !(mouse_oldbuttonstate & MOUSE_MIDDLEBUTTON))
			Key_Event(K_MOUSE3, true);
		else if(!(mouse_buttonstate & MOUSE_MIDDLEBUTTON) &&
		        (mouse_oldbuttonstate & MOUSE_MIDDLEBUTTON))
			Key_Event(K_MOUSE3, false);

		mouse_oldbuttonstate = mouse_buttonstate;
	}
#elif VIDX
	int i;

	if(!mouse_avail)
		return;

	for(i = 0; i < mouse_buttons; i++)
	{
		if((mouse_buttonstate & (1 << i)) && !(mouse_oldbuttonstate & (1 << i)))
			Key_Event(K_MOUSE1 + i, true);

		if(!(mouse_buttonstate & (1 << i)) && (mouse_oldbuttonstate & (1 << i)))
			Key_Event(K_MOUSE1 + i, false);
	}
	mouse_oldbuttonstate = mouse_buttonstate;
#enidf
}

/*
===========
IN_Move
===========
*/
void IN_MouseMove(usercmd_t *cmd)
{
#ifdef SVGALIB
	mx = my = 0; // clear for next update

	// add mouse X/Y movement to cmd
	if((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;

	if(in_mlook.state & 1)
		V_StopPitchDrift();

	if((in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if(cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if(cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}
#elif VIDX
	if(m_filter.value)
	{
		mouse_x = (mouse_x + old_mouse_x) * 0.5;
		mouse_y = (mouse_y + old_mouse_y) * 0.5;
	}

	if((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;
	
	if(in_mlook.state & 1)
		V_StopPitchDrift();

	if((in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if(cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if(cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	
	mouse_x = mouse_y = 0.0;
#endif
}

void IN_Move(usercmd_t *cmd)
{
	IN_MouseMove(cmd);
}