/*
	subs.qc

	sub-functions, mostly movement related

	Copyright (C) 1996-1997  Id Software, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

*/

/// @file

void CBaseEntity::SUB_Null()
{
};

void CBaseEntity::SUB_Remove()
{
	gpEngine->pfnRemove(ToEdict());
};

/*
QuakeEd only writes a single float for angles (bad idea), so up and down are
just constant angles.
*/
void CBaseEntity::SetMovedir()
{
	if (GetAngles() == idVec3(0, -1, 0)) // TODO: -idVec3::Up
		SetMoveDir(idVec3(0, 0, 1));
	else if (GetAngles() == idVec3(0, -2, 0))
		SetMoveDir(idVec3(0, 0, -1));
	else
	{
		gpEngine->pfnMakeVectors(GetAngles());
		SetMoveDir(gpGlobals->v_forward);
	};
	
	SetAngles(idVec3::Origin);
};