/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/// @file

#pragma once

#define	MAX_ENT_LEAFS	48

typedef struct edict_s
{
	qboolean	free;
	link_t		area;				// linked to a division node or leaf
	
	int			num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	entity_state_t	baseline;
	
	float		freetime;			// sv.time when the object was freed
	entvars_t	v;					// C exported fields from progs
// other fields from progs come immediately after
} edict_t;