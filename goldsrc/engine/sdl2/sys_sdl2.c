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

extern void ImGui_ImplSDL2_ProcessEvent(SDL_Event *pEvent);

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
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			Key_Event(SDL2KeyToInternalKey(), true);
			break;
		case SDL_CONTROLLERBUTTONUP:
			Key_Event(SDL2KeyToInternalKey(), false);
			break;
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMAPPED:
			break;
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
			break;
		case SDL_KEYDOWN:
			Key_Event(SDL2KeyToInternalKey(), true);
			break;
		case SDL_KEYUP:
			Key_Event(SDL2KeyToInternalKey(), false);
			break;
		case SDL_JOYAXISMOTION:
			break;
		case SDL_JOYBALLMOTION:
			break;
		case SDL_JOYHATMOTION:
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			break;
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
			break;
		case SDL_MOUSEMOTION:
			IN_MouseEvent(); // TODO
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			IN_MouseEvent(); // TODO
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
			break;
		//case WM_ACTIVATE: // TODO
		//case WM_DESTROY: // TODO
		};
	};
};