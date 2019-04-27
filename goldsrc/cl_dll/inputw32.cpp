/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018-2019 BlackPhrase
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
/// @brief windows 95 mouse and joystick code

#include <cmath>

#include "const.h"
#include "cvardef.h"
#include "view.h"
#include "engine.h"
#include "keydefs.h"
#include "input.h"
#include "usercmd.h"

#ifdef _WIN32
#include <windows.h>
#endif

// 02/21/97 JCB Added extended DirectInput code to support external controllers.

typedef struct usercmd_s usercmd_t;

int mouse_buttons;
int mouse_oldbuttonstate;
POINT current_pos;
float mouse_x, mouse_y; // TODO: ints in gs
int old_mouse_x, old_mouse_y, mx_accum, my_accum;

static qboolean restore_spi;
static int originalmouseparms[3], newmouseparms[3] = { 0, 0, 1 };

qboolean mouseactive;
qboolean mouseinitialized;

static qboolean mouseparmsvalid;
static qboolean mouseshowtoggle = 1;

// joystick defines and variables
// where should defines be moved?
#define JOY_ABSOLUTE_AXIS 0x00000000 // control like a joystick
#define JOY_RELATIVE_AXIS 0x00000010 // control like a mouse, spinner, trackball
#define JOY_MAX_AXES 6               // X, Y, Z, R, U, V
#define JOY_AXIS_X 0
#define JOY_AXIS_Y 1
#define JOY_AXIS_Z 2
#define JOY_AXIS_R 3
#define JOY_AXIS_U 4
#define JOY_AXIS_V 5

enum _ControlList
{
	AxisNada = 0,
	AxisForward,
	AxisLook,
	AxisSide,
	AxisTurn
};

DWORD dwAxisFlags[JOY_MAX_AXES] =
{
	JOY_RETURNX,
	JOY_RETURNY,
	JOY_RETURNZ,
	JOY_RETURNR,
	JOY_RETURNU,
	JOY_RETURNV
};


DWORD dwAxisMap[JOY_MAX_AXES];
DWORD dwControlMap[JOY_MAX_AXES];
PDWORD pdwRawValue[JOY_MAX_AXES];

// mouse variables
cvar_t *m_filter;
cvar_t *sensitivity;

// none of these cvars are saved over a session
// this means that advanced controller configuration needs to be executed
// each time.  this avoids any problems with getting back to a default usage
// or when changing from one controller to another.  this way at least something
// works.
cvar_t *in_joystick;
cvar_t *joy_name;
cvar_t *joy_advanced;
cvar_t *joy_advaxisx;
cvar_t *joy_advaxisy;
cvar_t *joy_advaxisz;
cvar_t *joy_advaxisr;
cvar_t *joy_advaxisu;
cvar_t *joy_advaxisv;
cvar_t *joy_forwardthreshold;
cvar_t *joy_sidethreshold;
cvar_t *joy_pitchthreshold;
cvar_t *joy_yawthreshold;
cvar_t *joy_forwardsensitivity;
cvar_t *joy_sidesensitivity;
cvar_t *joy_pitchsensitivity;
cvar_t *joy_yawsensitivity;
cvar_t *joy_wwhack1;
cvar_t *joy_wwhack2;

qboolean joy_avail, joy_advancedinit, joy_haspov;
DWORD joy_oldbuttonstate, joy_oldpovstate;

int joy_id;
DWORD joy_flags;
DWORD joy_numbuttons;

static JOYINFOEX ji;

/*
===========
Force_CenterView_f
===========
*/
void Force_CenterView_f()
{
	vec3_t viewangles;
	
	//if(!iMouseInUse)
	{
		gpEngine->GetViewAngles((float*)viewangles);
		viewangles[PITCH] = 0;
		gpEngine->SetViewAngles((float*)viewangles);
	};
}

/*
===========
IN_ActivateMouse
===========
*/
void IN_ActivateMouse()
{
	if(mouseinitialized)
	{
#ifdef _WIN32
		if(mouseparmsvalid)
			restore_spi = SystemParametersInfo(SPI_SETMOUSE, 0, newmouseparms, 0);
#endif

		mouseactive = true;
	};
};

/*
===========
IN_DeactivateMouse
===========
*/
void IN_DeactivateMouse()
{
	if(mouseinitialized)
	{
#ifdef _WIN32
		if(restore_spi)
			SystemParametersInfo(SPI_SETMOUSE, 0, originalmouseparms, 0);
#endif

		mouseactive = false;
	};
};

/*
===========
IN_StartupMouse
===========
*/
void IN_StartupMouse()
{
	if(gpEngine->CheckParm("-nomouse", nullptr))
		return;
	
	mouseinitialized = true;

#ifdef _WIN32
	mouseparmsvalid = SystemParametersInfo(SPI_GETMOUSE, 0, originalmouseparms, 0);

	if(mouseparmsvalid)
	{
		if(gpEngine->CheckParm("-noforcemspd", nullptr))
			newmouseparms[2] = originalmouseparms[2];

		if(gpEngine->CheckParm("-noforcemaccel", nullptr))
		{
			newmouseparms[0] = originalmouseparms[0];
			newmouseparms[1] = originalmouseparms[1];
		}

		if(gpEngine->CheckParm("-noforcemparms", nullptr))
		{
			newmouseparms[0] = originalmouseparms[0];
			newmouseparms[1] = originalmouseparms[1];
			newmouseparms[2] = originalmouseparms[2];
		}
	}
#endif

	mouse_buttons = 3; // TODO: MOUSE_BUTTON_COUNT
};

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown()
{
	IN_DeactivateMouse();
};

/*
===========
IN_GetMousePos
===========
*/
void IN_GetMousePos(int *mx, int *my)
{
	gpEngine->GetMousePosition(mx, my);
};

/*
===========
IN_ResetMouse
===========
*/
void IN_ResetMouse()
{
	SetCursorPos(gpEngine->GetWindowCenterX(), gpEngine->GetWindowCenterY());
};

/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent(int mstate)
{
	//if(iMouseInUse || g_iVisibleMouse)
		//return;

	// perform button actions
	for(int i = 0; i < mouse_buttons; i++)
	{
		if((mstate & (1 << i)) && !(mouse_oldbuttonstate & (1 << i)))
			gpEngine->Key_Event(K_MOUSE1 + i, true);

		if(!(mstate & (1 << i)) && (mouse_oldbuttonstate & (1 << i)))
			gpEngine->Key_Event(K_MOUSE1 + i, false);
	};
	
	mouse_oldbuttonstate = mstate;
};

/*
===========
IN_MouseMove
===========
*/
void IN_MouseMove(float frametime, usercmd_t *cmd)
{
	int mx, my;
	vec3_t viewangles;
	
	gpEngine->GetViewAngles((float*)viewangles);
	
	if(in_mlook.state & 1)
		V_StopPitchDrift();

	//if(dinput)
		// engine handles that
	//else
	{
		GetCursorPos(&current_pos);
		
		mx = current_pos.x - gpEngine->GetWindowCenterX() + mx_accum;
		my = current_pos.y - gpEngine->GetWindowCenterY() + my_accum;
		
		mx_accum = 0;
		my_accum = 0;
	}

	// TODO: temp
	if(mx ||  my)
		gpEngine->Con_DPrintf("mx=%d, my=%d\n", mx, my);

	if(m_filter->value)
	{
		mouse_x = (mx + old_mouse_x) * 0.5;
		mouse_y = (my + old_mouse_y) * 0.5;
	}
	else
	{
		mouse_x = mx;
		mouse_y = my;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	//if(gHUD.GetSensitivity() != 0)
	{
		// TODO
	}
	//else
	{
		mouse_x *= sensitivity->value;
		mouse_y *= sensitivity->value;
	};

	// add mouse X/Y movement to cmd
	if((in_strafe.state & 1) || (lookstrafe->value && (in_mlook.state & 1)))
		cmd->sidemove += m_side->value * mouse_x;
	else
		viewangles[YAW] -= m_yaw->value * mouse_x;

	if((in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		viewangles[PITCH] += m_pitch->value * mouse_y;
		if(viewangles[PITCH] > 80)
			viewangles[PITCH] = 80;
		if(viewangles[PITCH] < -70)
			viewangles[PITCH] = -70;
	}
	else
	{
		if((in_strafe.state & 1) && gpEngine->IsNoClipping())
			cmd->upmove -= m_forward->value * mouse_y;
		else
			cmd->forwardmove -= m_forward->value * mouse_y;
	}

	// if the mouse has moved, force it to the center, so there's room to move
	if(mx || my)
	{
		IN_ResetMouse();
	}

	gpEngine->SetViewAngles((float*)viewangles);
};

/*
===========
IN_Accumulate
===========
*/
void IN_Accumulate()
{
	if(mouseactive)
	{
#ifdef _WIN32
		//if(!rawinput)
		{
			//if(!mousethread)
			{
				GetCursorPos(&current_pos);

				mx_accum += current_pos.x - gpEngine->GetWindowCenterX();
				my_accum += current_pos.y - gpEngine->GetWindowCenterY();
				
				// force the mouse to the center, so there's room to move
				IN_ResetMouse();
			};
		}
		//else
		{
		};
	};
#endif
};

/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates()
{
	if(mouseactive)
	{
		mx_accum = 0;
		my_accum = 0;
		mouse_oldbuttonstate = 0;
	};
};

/* 
=============== 
IN_StartupJoystick 
=============== 
*/
void IN_StartupJoystick()
{
/*
	int i, numdevs;
	JOYCAPS jc;
	MMRESULT mmr;

	// assume no joystick
	joy_avail = false;

	// abort startup if user requests no joystick
	if(COM_CheckParm("-nojoy"))
		return;

	// verify joystick driver is present
	if((numdevs = joyGetNumDevs()) == 0)
	{
		Con_Printf("\njoystick not found -- driver not present\n\n");
		return;
	}

	// cycle through the joystick ids for the first valid one
	for(joy_id = 0; joy_id < numdevs; joy_id++)
	{
		memset(&ji, 0, sizeof(ji));
		ji.dwSize = sizeof(ji);
		ji.dwFlags = JOY_RETURNCENTERED;

		if((mmr = joyGetPosEx(joy_id, &ji)) == JOYERR_NOERROR)
			break;
	}

	// abort startup if we didn't find a valid joystick
	if(mmr != JOYERR_NOERROR)
	{
		Con_Printf("\njoystick not found -- no valid joysticks (%x)\n\n", mmr);
		return;
	}

	// get the capabilities of the selected joystick
	// abort startup if command fails
	memset(&jc, 0, sizeof(jc));
	if((mmr = joyGetDevCaps(joy_id, &jc, sizeof(jc))) != JOYERR_NOERROR)
	{
		Con_Printf("\njoystick not found -- invalid joystick capabilities (%x)\n\n", mmr);
		return;
	}

	// save the joystick's number of buttons and POV status
	joy_numbuttons = jc.wNumButtons;
	joy_haspov = jc.wCaps & JOYCAPS_HASPOV;

	// old button and POV states default to no buttons pressed
	joy_oldbuttonstate = joy_oldpovstate = 0;

	// mark the joystick as available and advanced initialization not completed
	// this is needed as cvars are not available during initialization

	joy_avail = true;
	joy_advancedinit = false;

	Con_Printf("\njoystick detected\n\n");
*/
};

/*
===========
RawValuePointer
===========
*/
/*
PDWORD RawValuePointer(int axis)
{
	switch(axis)
	{
	case JOY_AXIS_X:
		return &ji.dwXpos;
	case JOY_AXIS_Y:
		return &ji.dwYpos;
	case JOY_AXIS_Z:
		return &ji.dwZpos;
	case JOY_AXIS_R:
		return &ji.dwRpos;
	case JOY_AXIS_U:
		return &ji.dwUpos;
	case JOY_AXIS_V:
		return &ji.dwVpos;
	};
};
*/

/*
===========
Joy_AdvancedUpdate_f
===========
*/
void Joy_AdvancedUpdate_f()
{
/*
	// called once by IN_ReadJoystick and by user whenever an update is needed
	// cvars are now available
	int i;
	DWORD dwTemp;

	// initialize all the maps
	for(i = 0; i < JOY_MAX_AXES; i++)
	{
		dwAxisMap[i] = AxisNada;
		dwControlMap[i] = JOY_ABSOLUTE_AXIS;
		pdwRawValue[i] = RawValuePointer(i);
	}

	if(joy_advanced.value == 0.0)
	{
		// default joystick initialization
		// 2 axes only with joystick control
		dwAxisMap[JOY_AXIS_X] = AxisTurn;
		// dwControlMap[JOY_AXIS_X] = JOY_ABSOLUTE_AXIS;
		dwAxisMap[JOY_AXIS_Y] = AxisForward;
		// dwControlMap[JOY_AXIS_Y] = JOY_ABSOLUTE_AXIS;
	}
	else
	{
		if(Q_strcmp(joy_name.string, "joystick") != 0)
		{
			// notify user of advanced controller
			Con_Printf("\n%s configured\n\n", joy_name.string);
		}

		// advanced initialization here
		// data supplied by user via joy_axisn cvars
		dwTemp = (DWORD)joy_advaxisx.value;
		dwAxisMap[JOY_AXIS_X] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_X] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisy.value;
		dwAxisMap[JOY_AXIS_Y] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Y] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisz.value;
		dwAxisMap[JOY_AXIS_Z] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Z] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisr.value;
		dwAxisMap[JOY_AXIS_R] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_R] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisu.value;
		dwAxisMap[JOY_AXIS_U] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_U] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisv.value;
		dwAxisMap[JOY_AXIS_V] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_V] = dwTemp & JOY_RELATIVE_AXIS;
	}

	// compute the axes to collect from DirectInput
	joy_flags = JOY_RETURNCENTERED | JOY_RETURNBUTTONS | JOY_RETURNPOV;
	for(i = 0; i < JOY_MAX_AXES; i++)
	{
		if(dwAxisMap[i] != AxisNada)
		{
			joy_flags |= dwAxisFlags[i];
		}
	}
*/
}

/*
===========
IN_Commands
===========
*/
void IN_Commands()
{
/*
	int i, key_index;
	DWORD buttonstate, povstate;

	if(!joy_avail)
	{
		return;
	}

	// loop through the joystick buttons
	// key a joystick event or auxillary event for higher number buttons for each state change
	buttonstate = ji.dwButtons;
	for(i = 0; i < joy_numbuttons; i++)
	{
		if((buttonstate & (1 << i)) && !(joy_oldbuttonstate & (1 << i)))
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event(key_index + i, true);
		}

		if(!(buttonstate & (1 << i)) && (joy_oldbuttonstate & (1 << i)))
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event(key_index + i, false);
		}
	}
	joy_oldbuttonstate = buttonstate;

	if(joy_haspov)
	{
		// convert POV information into 4 bits of state information
		// this avoids any potential problems related to moving from one
		// direction to another without going through the center position
		povstate = 0;
		if(ji.dwPOV != JOY_POVCENTERED)
		{
			if(ji.dwPOV == JOY_POVFORWARD)
				povstate |= 0x01;
			if(ji.dwPOV == JOY_POVRIGHT)
				povstate |= 0x02;
			if(ji.dwPOV == JOY_POVBACKWARD)
				povstate |= 0x04;
			if(ji.dwPOV == JOY_POVLEFT)
				povstate |= 0x08;
		}
		// determine which bits have changed and key an auxillary event for each change
		for(i = 0; i < 4; i++)
		{
			if((povstate & (1 << i)) && !(joy_oldpovstate & (1 << i)))
			{
				Key_Event(K_AUX29 + i, true);
			}

			if(!(povstate & (1 << i)) && (joy_oldpovstate & (1 << i)))
			{
				Key_Event(K_AUX29 + i, false);
			}
		}
		joy_oldpovstate = povstate;
	}
*/
};

/* 
=============== 
IN_ReadJoystick
=============== 
*/
qboolean IN_ReadJoystick()
{
	memset(&ji, 0, sizeof(ji));
	ji.dwSize = sizeof(ji);
	ji.dwFlags = joy_flags;

	if(joyGetPosEx(joy_id, &ji) == JOYERR_NOERROR)
	{
		// this is a hack -- there is a bug in the Logitech WingMan Warrior DirectInput Driver
		// rather than having 32768 be the zero point, they have the zero point at 32668
		// go figure -- anyway, now we get the full resolution out of the device
		if(joy_wwhack1->value != 0.0)
		{
			ji.dwUpos += 100;
		}
		return true;
	}
	else
	{
		// read error occurred
		// turning off the joystick seems too harsh for 1 read error,\
		// but what should be done?
		// Con_Printf ("IN_ReadJoystick: no response\n");
		// joy_avail = false;
		return false;
	}
};

/*
===========
IN_JoyMove
===========
*/
void IN_JoyMove(float frametime, usercmd_t *cmd)
{
	float speed, aspeed;
	float fAxisValue, fTemp;
	int i;
	vec3_t viewangles;
	
	gpEngine->GetViewAngles((float*)viewangles);

	// complete initialization if first time in
	// this is needed as cvars are not available at initialization time
	if(joy_advancedinit != true)
	{
		Joy_AdvancedUpdate_f();
		joy_advancedinit = true;
	}

	// verify joystick is available and that the user wants to use it
	if(!joy_avail || !in_joystick->value)
	{
		return;
	}

	// collect the joystick data, if possible
	if(IN_ReadJoystick() != true)
	{
		return;
	}

	if(in_speed.state & 1)
		speed = cl_movespeedkey->value;
	else
		speed = 1;
	
	aspeed = speed * frametime; // TODO: was host_frametime

	// loop through the axes
	for(i = 0; i < JOY_MAX_AXES; i++)
	{
		// get the floating point zero-centered, potentially-inverted data for the current axis
		fAxisValue = (float)*pdwRawValue[i];
		// move centerpoint to zero
		fAxisValue -= 32768.0;

		if(joy_wwhack2->value != 0.0)
		{
			if(dwAxisMap[i] == AxisTurn)
			{
				// this is a special formula for the Logitech WingMan Warrior
				// y=ax^b; where a = 300 and b = 1.3
				// also x values are in increments of 800 (so this is factored out)
				// then bounds check result to level out excessively high spin rates
				fTemp = 300.0 * pow(abs(fAxisValue) / 800.0, 1.3);
				if(fTemp > 14000.0)
					fTemp = 14000.0;
				// restore direction information
				fAxisValue = (fAxisValue > 0.0) ? fTemp : -fTemp;
			}
		}

		// convert range from -32768..32767 to -1..1
		fAxisValue /= 32768.0;

		switch(dwAxisMap[i])
		{
		case AxisForward:
			if((joy_advanced->value == 0.0) && (in_jlook.state & 1)) // TODO: was in_mlook
			{
				// user wants forward control to become look control
				if(fabs(fAxisValue) > joy_pitchthreshold->value)
				{
					// if mouse invert is on, invert the joystick pitch value
					// only absolute control support here (joy_advanced is false)
					if(m_pitch->value < 0.0)
					{
						viewangles[PITCH] -= (fAxisValue * joy_pitchsensitivity->value) * aspeed * cl_pitchspeed->value;
					}
					else
					{
						viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity->value) * aspeed * cl_pitchspeed->value;
					}
					V_StopPitchDrift();
				}
				else
				{
					// no pitch movement
					// disable pitch return-to-center unless requested by user
					// *** this code can be removed when the lookspring bug is fixed
					// *** the bug always has the lookspring feature on
					if(lookspring->value == 0.0)
						V_StopPitchDrift();
				}
			}
			else
			{
				// user wants forward control to be forward control
				if(fabs(fAxisValue) > joy_forwardthreshold->value)
				{
					cmd->forwardmove += (fAxisValue * joy_forwardsensitivity->value) * speed * cl_forwardspeed->value;
				}
			}
			break;

		case AxisSide:
			if(fabs(fAxisValue) > joy_sidethreshold->value)
			{
				cmd->sidemove += (fAxisValue * joy_sidesensitivity->value) * speed * cl_sidespeed->value;
			}
			break;

		case AxisTurn:
			if((in_strafe.state & 1) || (lookstrafe->value && (in_jlook.state & 1))) // TODO: was in_mlook
			{
				// user wants turn control to become side control
				if(fabs(fAxisValue) > joy_sidethreshold->value)
				{
					cmd->sidemove -= (fAxisValue * joy_sidesensitivity->value) * speed * cl_sidespeed->value;
				}
			}
			else
			{
				// user wants turn control to be turn control
				if(fabs(fAxisValue) > joy_yawthreshold->value)
				{
					if(dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						viewangles[YAW] += (fAxisValue * joy_yawsensitivity->value) * aspeed * cl_yawspeed->value;
					}
					else
					{
						viewangles[YAW] += (fAxisValue * joy_yawsensitivity->value) * speed * 180.0;
					}
				}
			}
			break;

		case AxisLook:
			if(in_jlook.state & 1) // TODO: was in_mlook
			{
				if(fabs(fAxisValue) > joy_pitchthreshold->value)
				{
					// pitch movement detected and pitch movement desired by user
					if(dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity->value) * aspeed * cl_pitchspeed->value;
					}
					else
					{
						viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity->value) * speed * 180.0;
					}
					V_StopPitchDrift();
				}
				else
				{
					// no pitch movement
					// disable pitch return-to-center unless requested by user
					// *** this code can be removed when the lookspring bug is fixed
					// *** the bug always has the lookspring feature on
					if(lookspring->value == 0.0)
						V_StopPitchDrift();
				}
			}
			break;

		default:
			break;
		}
	}

	// bounds check pitch
	if(viewangles[PITCH] > cl_pitchdown->value) // TODO: was 80.0
		viewangles[PITCH] = cl_pitchdown->value; // TODO: was 80.0
	if(viewangles[PITCH] < -cl_pitchup->value) // TODO: was -70.0
		viewangles[PITCH] = -cl_pitchup->value; // TODO: was -70.0

	gpEngine->SetViewAngles((float*)viewangles);
};

/*
===========
IN_Move
===========
*/
void IN_Move(float frametime, usercmd_t *cmd)
{
	if(/*!iMouseInUse && */ mouseactive)
		IN_MouseMove(frametime, cmd);
	
	IN_JoyMove(frametime, cmd);
};

/*
===========
IN_Init
===========
*/
void IN_Init()
{
	// mouse variables
	m_filter = gpEngine->pfnRegisterVariable("m_filter", "0", FCVAR_ARCHIVE);
	sensitivity = gpEngine->pfnRegisterVariable("sensitivity", "3", FCVAR_ARCHIVE);

	// joystick variables
	in_joystick = gpEngine->pfnRegisterVariable("joystick", "0", FCVAR_ARCHIVE);
	joy_name = gpEngine->pfnRegisterVariable("joyname", "joystick", 0);
	joy_advanced = gpEngine->pfnRegisterVariable("joyadvanced", "0", 0);
	joy_advaxisx = gpEngine->pfnRegisterVariable("joyadvaxisx", "0", 0);
	joy_advaxisy = gpEngine->pfnRegisterVariable("joyadvaxisy", "0", 0);
	joy_advaxisz = gpEngine->pfnRegisterVariable("joyadvaxisz", "0", 0);
	joy_advaxisr = gpEngine->pfnRegisterVariable("joyadvaxisr", "0", 0);
	joy_advaxisu = gpEngine->pfnRegisterVariable("joyadvaxisu", "0", 0);
	joy_advaxisv = gpEngine->pfnRegisterVariable("joyadvaxisv", "0", 0);
	joy_forwardthreshold = gpEngine->pfnRegisterVariable("joyforwardthreshold", "0.15", 0);
	joy_sidethreshold = gpEngine->pfnRegisterVariable("joysidethreshold", "0.15", 0);
	joy_pitchthreshold = gpEngine->pfnRegisterVariable("joypitchthreshold", "0.15", 0);
	joy_yawthreshold = gpEngine->pfnRegisterVariable("joyyawthreshold", "0.15", 0);
	joy_forwardsensitivity = gpEngine->pfnRegisterVariable("joyforwardsensitivity", "-1.0", 0);
	joy_sidesensitivity = gpEngine->pfnRegisterVariable("joysidesensitivity", "-1.0", 0);
	joy_pitchsensitivity = gpEngine->pfnRegisterVariable("joypitchsensitivity", "1.0", 0);
	joy_yawsensitivity = gpEngine->pfnRegisterVariable("joyyawsensitivity", "-1.0", 0);
	joy_wwhack1 = gpEngine->pfnRegisterVariable("joywwhack1", "0.0", 0);
	joy_wwhack2 = gpEngine->pfnRegisterVariable("joywwhack2", "0.0", 0);
	
	gpEngine->pfnAddCommand("force_centerview", Force_CenterView_f);
	gpEngine->pfnAddCommand("joyadvancedupdate", Joy_AdvancedUpdate_f);
	
	IN_StartupMouse();
	IN_StartupJoystick();
};