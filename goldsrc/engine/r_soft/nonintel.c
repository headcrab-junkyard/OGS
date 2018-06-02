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
//
// nonintel.c: code for non-Intel processors only
//

#include "quakedef.h"

#if	!id386

/*
================
R_Surf8Patch
================
*/
void R_Surf8Patch ()
{
	// we only patch code on Intel
}


/*
================
R_Surf16Patch
================
*/
void R_Surf16Patch ()
{
	// we only patch code on Intel
}


/*
================
R_SurfacePatch
================
*/
void R_SurfacePatch (void)
{
	// we only patch code on Intel
}


#endif	// !id386

