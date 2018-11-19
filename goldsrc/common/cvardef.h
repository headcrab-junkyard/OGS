/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018 BlackPhrase
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

#pragma once

#define	FCVAR_ARCHIVE	1	///< set to cause it to be saved to vars.rc
#define	FCVAR_USERINFO	2	///< added to userinfo  when changed
#define	FCVAR_SERVER	4	///< notifies players when changed
//#define	FCVAR_SERVERINFO	4	///< added to serverinfo when changed
//#define	FCVAR_NOSET		8	///< don't allow change from console at all,
							/// but can be set from the command line
//#define	FCVAR_LATCH		16	///< save changes until server restart

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s
{
	const char	*name;
	char	*string;
	int			flags;
	//qboolean archive;		// set to true to cause it to be saved to vars.rc
	//qboolean server;		// notifies players when changed // TODO: qboolean info in QW (either userinfo or serverinfo)
	float	value;
	struct cvar_s *next;
} cvar_t;