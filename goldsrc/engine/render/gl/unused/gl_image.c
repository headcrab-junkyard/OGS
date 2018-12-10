/*
 * This file is part of OGS Engine
 * Copyright (C) 1997-2001 Id Software, Inc.
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

/// @file

#include "quakedef.h"

void GL_SetTexturePalette(int texnum, uint palette[256])
{
	byte temptable[768];

	for (int i = 0; i < 256; i++ )
	{
		temptable[i*3+0] = ( palette[i] >> 0 ) & 0xff;
		temptable[i*3+1] = ( palette[i] >> 8 ) & 0xff;
		temptable[i*3+2] = ( palette[i] >> 16 ) & 0xff;
	};

	if ( qglColorTableEXT /*&& gl_ext_palettedtexture->value*/ )
	{
		GL_Bind(texnum);
		qglColorTableEXT( GL_TEXTURE_2D, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, temptable );
	};
};