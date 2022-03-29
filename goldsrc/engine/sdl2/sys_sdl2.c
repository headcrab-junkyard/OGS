/*
 * This file is part of OGS Engine
 * Copyright (C) 2019 BlackPhrase
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

#include "quakedef.h"

#include <SDL2/SDL.h>

extern void ImGui_ImplSDL2_ProcessEvent(SDL_Event *pEvent);

int SDL2KeyToInternalKey(int anSDLKey)
{
	// TODO
	return 0;
};

void HandleSDLEvents()
{
	SDL_Event Event;
	while(SDL_PollEvent(&Event))
	{
#ifdef OGS_USE_IMGUI
		ImGui_ImplSDL2_ProcessEvent(&Event);
#endif

		switch(Event.type)
		{
		case SDL_CONTROLLERAXISMOTION:
			// TODO
			break;
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			Key_Event(SDL2KeyToInternalKey(Event.cbutton.button), Event.cbutton.state);
			break;
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMAPPED:
			// TODO
			break;
		case SDL_FINGERMOTION:
			// TODO
			break;
		//case SDL_FINGERDOWN:
		//case SDL_FINGERUP:
			//IN_MouseEvent(SDL2KeyToInternalKey(Event.finger.button)); // TODO
			//break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			Key_Event(SDL2KeyToInternalKey(Event.key.keysym), Event.key.state);
			break;
		case SDL_JOYAXISMOTION:
			// TODO
			break;
		case SDL_JOYBALLMOTION:
			// TODO
			break;
		case SDL_JOYHATMOTION:
			// TODO
			break;
		//case SDL_JOYBUTTONDOWN:
		//case SDL_JOYBUTTONUP:
			// NOTE: handled by client dll's IN_Commands func
			//break;
		case SDL_JOYDEVICEADDED:
			Con_DPrintf("A new joystick device attached!"); // TODO
			break;
		case SDL_JOYDEVICEREMOVED:
			Con_DPrintf("Joystick device detached!"); // TODO
			break;
		case SDL_MOUSEMOTION:
			// TODO
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			IN_MouseEvent(SDL2KeyToInternalKey(Event.button.button));
			break;
		case SDL_MOUSEWHEEL:
			if(Event.wheel.y > 0)
			{
				Key_Event(K_MWHEELUP, true);
				Key_Event(K_MWHEELUP, false);
			}
			else
			{
				Key_Event(K_MWHEELDOWN, true);
				Key_Event(K_MWHEELDOWN, false);
			};
			break;
		case SDL_WINDOWEVENT:
			// TODO: killfocus, move
			switch(Event.window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				Sys_Quit();
				break;
			};
			break;
		case SDL_QUIT:
			CL_Disconnect();
			Host_ShutdownServer(false);
			Sys_Quit();
			break;
		//case WM_ACTIVATE: // TODO
		//case WM_DESTROY: // TODO
		default:
			break;
		};
	};
};

void Sys_Printf(const char *fmt, ...)
{
	SDL_Log(); // TODO
};

void Sys_Sleep()
{
	SDL_Delay(1); // TODO: unused in gs
};