/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2019 BlackPhrase
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

#include "r_studioint.h"

engine_studio_api_t gStudio;

int StudioDrawModel(int flags)
{
	// TODO
	return 0;
};

int StudioDrawPlayer(int flags, struct entity_state_s *pPlayer)
{
	// TODO
	return 0;
};

r_studio_interface_t gStudioAPI =
{
	STUDIO_INTERFACE_VERSION,
	
	StudioDrawModel,
	StudioDrawPlayer
};

//r_studio_interface_t *pStudioAPI = &gStudioAPI;

int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppInterface, struct engine_studio_api_s *pStudio)
{
	if(version != STUDIO_INTERFACE_VERSION)
		return 0;
	
	*ppInterface = &gStudioAPI;
	
	memcpy(&gStudio, pStudio, sizeof(engine_studio_api_t));
	
	return 1;
};