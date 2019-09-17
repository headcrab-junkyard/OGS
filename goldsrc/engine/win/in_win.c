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
/// @brief windows 95 mouse and joystick code

// 02/21/97 JCB Added extended DirectInput code to support external controllers.

#include <dinput.h>
#include "quakedef.h"
#include "winquake.h"
//#include "dosisms.h"

#define DINPUT_BUFFERSIZE 16
#define iDirectInputCreate(a, b, c, d) pDirectInputCreate(a, b, c, d)

HRESULT(WINAPI *pDirectInputCreate)(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUT *lplpDirectInput, LPUNKNOWN punkOuter);

unsigned int uiWheelMessage;

static qboolean mouseactivatetoggle;

static qboolean dinput_acquired;

static unsigned int mstate_di;

static LPDIRECTINPUT g_pdi;
static LPDIRECTINPUTDEVICE g_pMouse;

static HINSTANCE hInstDI;

static qboolean dinput;

typedef struct MYDATA
{
	LONG lX;       // X axis goes here
	LONG lY;       // Y axis goes here
	LONG lZ;       // Z axis goes here
	BYTE bButtonA; // One button goes here
	BYTE bButtonB; // Another button goes here
	BYTE bButtonC; // Another button goes here
	BYTE bButtonD; // Another button goes here
} MYDATA;

static DIOBJECTDATAFORMAT rgodf[] = {
	{&GUID_XAxis, FIELD_OFFSET(MYDATA, lX), DIDFT_AXIS | DIDFT_ANYINSTANCE, 0,},
	{&GUID_YAxis, FIELD_OFFSET(MYDATA, lY), DIDFT_AXIS | DIDFT_ANYINSTANCE, 0,},
	{&GUID_ZAxis, FIELD_OFFSET(MYDATA, lZ), 0x80000000 | DIDFT_AXIS | DIDFT_ANYINSTANCE, 0,},
	{0, FIELD_OFFSET(MYDATA, bButtonA), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
	{0, FIELD_OFFSET(MYDATA, bButtonB), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
	{0, FIELD_OFFSET(MYDATA, bButtonC), 0x80000000 | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
	{0, FIELD_OFFSET(MYDATA, bButtonD), 0x80000000 | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
};

#define NUM_OBJECTS (sizeof(rgodf) / sizeof(rgodf[0]))

static DIDATAFORMAT df = {
	sizeof(DIDATAFORMAT),       // this structure
	sizeof(DIOBJECTDATAFORMAT), // size of object data format
	DIDF_RELAXIS,               // absolute axis coordinates
	sizeof(MYDATA),             // device data size
	NUM_OBJECTS,                // number of objects
	rgodf,                      // and here they are
};

// forward-referenced functions
void IN_StartupJoystick();
void Joy_AdvancedUpdate_f();
void IN_JoyMove(usercmd_t *cmd);



/*
===========
IN_UpdateClipCursor
===========
*/
void IN_UpdateClipCursor()
{
	//if(mouseinitialized && mouseactive && !dinput)
	{
		ClipCursor(&window_rect);
	}
}

/*
===========
IN_ShowMouse
===========
*/
void IN_ShowMouse()
{
	//if(!mouseshowtoggle)
	{
		ShowCursor(TRUE);
		//mouseshowtoggle = 1;
	}
}

/*
===========
IN_HideMouse
===========
*/
void IN_HideMouse()
{
	//if(mouseshowtoggle)
	{
		ShowCursor(FALSE);
		//mouseshowtoggle = 0;
	}
}

/*
===========
IN_ActivateMouse
===========
*/
void IN_ActivateMouse()
{
	mouseactivatetoggle = true;

	//if(mouseinitialized)
	{
		if(dinput)
		{
			if(g_pMouse)
			{
				if(!dinput_acquired)
				{
					IDirectInputDevice_Acquire(g_pMouse);
					dinput_acquired = true;
				};
			}
			else
				return;
		}
		else
		{
			SetCursorPos(window_center_x, window_center_y);
			SetCapture(mainwindow);
			ClipCursor(&window_rect);
			
			//ClientDLL_ActivateMouse(); // TODO: here or below???
		};
	};
	
	ClientDLL_ActivateMouse();
};

/*
===========
IN_SetQuakeMouseState
===========
*/
void IN_SetQuakeMouseState()
{
	if(mouseactivatetoggle)
		IN_ActivateMouse();
}

/*
===========
IN_DeactivateMouse
===========
*/
void IN_DeactivateMouse()
{
	mouseactivatetoggle = false;

	//if(mouseinitialized)
	{
		if(dinput)
		{
			if(g_pMouse)
			{
				if(dinput_acquired)
				{
					IDirectInputDevice_Unacquire(g_pMouse);
					dinput_acquired = false;
				};
			};
		}
		else
		{
			ClipCursor(NULL);
			ReleaseCapture();
			
			//ClientDLL_DeactivateMouse(); // TODO: here or below???
		};
	};
	
	ClientDLL_DeactivateMouse();
};

/*
===========
IN_RestoreOriginalMouseState
===========
*/
void IN_RestoreOriginalMouseState()
{
	if(mouseactivatetoggle)
	{
		IN_DeactivateMouse();
		mouseactivatetoggle = true;
	}

	// try to redraw the cursor so it gets reinitialized, because sometimes it
	// has garbage after the mode switch
	ShowCursor(TRUE);
	ShowCursor(FALSE);
}

/*
===========
IN_InitDInput
===========
*/
qboolean IN_InitDInput()
{
	HRESULT hr;
	DIPROPDWORD dipdw = {
		{
		sizeof(DIPROPDWORD),  // diph.dwSize
		sizeof(DIPROPHEADER), // diph.dwHeaderSize
		0,                    // diph.dwObj
		DIPH_DEVICE,          // diph.dwHow
		},
		DINPUT_BUFFERSIZE, // dwData
	};

	if(!hInstDI)
	{
		hInstDI = LoadLibrary("dinput.dll");

		if(hInstDI == NULL)
		{
			Con_SafePrintf("Couldn't load dinput.dll\n");
			return false;
		}
	}

	if(!pDirectInputCreate)
	{
		pDirectInputCreate = (void *)GetProcAddress(hInstDI, "DirectInputCreateA");

		if(!pDirectInputCreate)
		{
			Con_SafePrintf("Couldn't get DI proc addr\n");
			return false;
		}
	}

	// register with DirectInput and get an IDirectInput to play with.
	hr = iDirectInputCreate(0 /*global_hInstance*/, DIRECTINPUT_VERSION, &g_pdi, NULL); // TODO

	if(FAILED(hr))
	{
		return false;
	}

	// obtain an interface to the system mouse device.
	hr = IDirectInput_CreateDevice(g_pdi, &GUID_SysMouse, &g_pMouse, NULL);

	if(FAILED(hr))
	{
		Con_SafePrintf("Couldn't open DI mouse device\n");
		return false;
	}

	// set the data format to "mouse format".
	hr = IDirectInputDevice_SetDataFormat(g_pMouse, &df);

	if(FAILED(hr))
	{
		Con_SafePrintf("Couldn't set DI mouse format\n");
		return false;
	}

	// set the cooperativity level.
	hr = IDirectInputDevice_SetCooperativeLevel(g_pMouse, mainwindow,
	                                            DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	if(FAILED(hr))
	{
		Con_SafePrintf("Couldn't set DI coop level\n");
		return false;
	}

	// set the buffer size to DINPUT_BUFFERSIZE elements.
	// the buffer size is a DWORD property associated with the device
	hr = IDirectInputDevice_SetProperty(g_pMouse, DIPROP_BUFFERSIZE, &dipdw.diph);

	if(FAILED(hr))
	{
		Con_SafePrintf("Couldn't set DI buffersize\n");
		return false;
	}

	return true;
}

/*
===========
IN_StartupMouse
===========
*/
void IN_StartupMouse()
{
/*
	dinput = IN_InitDInput();

	if(!dinput)
	{
		// TODO: moved to client dll
	}

	// if a fullscreen video mode was set before the mouse was initialized,
	// set the mouse state appropriately
	if(mouseactivatetoggle)
		IN_ActivateMouse();
*/
}

/*
===========
IN_Init
===========
*/
void IN_Init()
{
	uiWheelMessage = RegisterWindowMessage("MSWHEEL_ROLLMSG");
}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown()
{
	IN_ShowMouse();

	if(g_pMouse)
	{
		IDirectInputDevice_Release(g_pMouse);
		g_pMouse = NULL;
	}

	if(g_pdi)
	{
		IDirectInput_Release(g_pdi);
		g_pdi = NULL;
	}
}

/*
===========
IN_MouseMove
===========
*/
void IN_MouseMove(usercmd_t *cmd)
{
	HDC hdc;
	int i;
	DIDEVICEOBJECTDATA od;
	DWORD dwElements;
	HRESULT hr;

	// TODO
	//if(!mouseactive)
		//return;
	
	if(dinput)
	{
		// TODO
/*
		// TODO
		//mx = 0;
		//my = 0;

		for(;;)
		{
			dwElements = 1;

			hr = IDirectInputDevice_GetDeviceData(g_pMouse,
			                                      sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);

			if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			{
				dinput_acquired = true;
				IDirectInputDevice_Acquire(g_pMouse);
				break;
			}

			// Unable to read data or no data available
			if(FAILED(hr) || dwElements == 0)
			{
				break;
			}

			// Look at the element to see what happened

			switch(od.dwOfs)
			{
			case DIMOFS_X:
				mx += od.dwData;
				break;

			case DIMOFS_Y:
				my += od.dwData;
				break;

			case DIMOFS_BUTTON0:
				if(od.dwData & 0x80)
					mstate_di |= 1;
				else
					mstate_di &= ~1;
				break;

			case DIMOFS_BUTTON1:
				if(od.dwData & 0x80)
					mstate_di |= (1 << 1);
				else
					mstate_di &= ~(1 << 1);
				break;

			case DIMOFS_BUTTON2:
				if(od.dwData & 0x80)
					mstate_di |= (1 << 2);
				else
					mstate_di &= ~(1 << 2);
				break;
			}
		}

		// perform button actions
		for(i = 0; i < mouse_buttons; i++)
		{
			if((mstate_di & (1 << i)) && !(mouse_oldbuttonstate & (1 << i)))
			{
				Key_Event(K_MOUSE1 + i, true);
			}

			if(!(mstate_di & (1 << i)) &&
			   (mouse_oldbuttonstate & (1 << i)))
			{
				Key_Event(K_MOUSE1 + i, false);
			}
		}

		mouse_oldbuttonstate = mstate_di;
*/
	}
	else
	{
	};
}

void IN_Move(usercmd_t *cmd)
{
	if(ActiveApp && !Minimized)
	{
	};
}

/* 
=============== 
IN_StartupJoystick 
=============== 
*/
void IN_StartupJoystick()
{
}

/*
===========
IN_Commands
===========
*/
void IN_Commands()
{
}