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
// d_clear: clears a specified rectangle to the specified color

#include "quakedef.h"


/*
================
D_FillRect
================
*/
void D_FillRect (vrect_t *rect, int color)
{
	int				rx, ry, rwidth, rheight;
	unsigned char	*dest;
	unsigned		*ldest;

	rx = rect->x;
	ry = rect->y;
	rwidth = rect->width;
	rheight = rect->height;

	if (rx < 0)
	{
		rwidth += rx;
		rx = 0;
	}
	if (ry < 0)
	{
		rheight += ry;
		ry = 0;
	}
	if (rx+rwidth > vid.width)
		rwidth = vid.width - rx;
	if (ry+rheight > vid.height)
		rheight = vid.height - rx;
		
	if (rwidth < 1 || rheight < 1)
		return;

	dest = ((byte *)vid.buffer + ry*vid.rowbytes + rx);

	if (((rwidth & 0x03) == 0) && (((long)dest & 0x03) == 0))
	{
	// faster aligned dword clear
		ldest = (unsigned *)dest;
		color += color << 16;

		rwidth >>= 2;
		color += color << 8;

		for (ry=0 ; ry<rheight ; ry++)
		{
			for (rx=0 ; rx<rwidth ; rx++)
				ldest[rx] = color;
			ldest = (unsigned *)((byte*)ldest + vid.rowbytes);
		}
	}
	else
	{
	// slower byte-by-byte clear for unaligned cases
		for (ry=0 ; ry<rheight ; ry++)
		{
			for (rx=0 ; rx<rwidth ; rx++)
				dest[rx] = color;
			dest += vid.rowbytes;
		}
	}
}

