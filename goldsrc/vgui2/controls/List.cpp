/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2019 BlackPhrase

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

#include "vgui_controls/List.hpp"

void menulist_s::Menulist_DoEnter()
{
	int start;

	start = this->generic.y / 10 + 1;

	this->curvalue = this->parent->cursor - start;

	if ( this->callback )
		this->callback( this );
};

void menulist_s::MenuList_Draw()
{
	const char **n;
	int y = 0;

	Menu_DrawStringR2LDark( this->x + this->parent->x + LCOLUMN_OFFSET, this->y + this->parent->y, this->name );

	n = this->itemnames;

  	Draw_Fill( this->x - 112 + this->parent->x, this->parent->y + this->y + this->curvalue*10 + 10, 128, 10, 16 );
	while ( *n )
	{
		Menu_DrawStringR2LDark( this->x + this->parent->x + LCOLUMN_OFFSET, this->y + this->parent->y + y + 10, *n );

		n++;
		y += 10;
	};
};

void menulist_s::SpinControl_DoEnter()
{
	this->curvalue++;
	if ( this->itemnames[this->curvalue] == 0 )
		this->curvalue = 0;

	if ( this->callback )
		this->callback( this );
};

void menulist_s::SpinControl_DoSlide( int dir )
{
	this->curvalue += dir;

	if ( this->curvalue < 0 )
		this->curvalue = 0;
	else if ( this->itemnames[this->curvalue] == 0 )
		this->curvalue--;

	if ( this->callback )
		this->callback( this );
};

void menulist_s::SpinControl_Draw()
{
	char buffer[100];

	if ( this->generic.name )
	{
		Menu_DrawStringR2LDark( this->generic.x + this->generic.parent->x + LCOLUMN_OFFSET, 
							this->generic.y + this->generic.parent->y, 
							this->generic.name );
	};
	
	if ( !strchr( this->itemnames[this->curvalue], '\n' ) )
		Menu_DrawString( RCOLUMN_OFFSET + this->generic.x + this->generic.parent->x, this->generic.y + this->generic.parent->y, this->itemnames[this->curvalue] );
	else
	{
		strcpy( buffer, this->itemnames[this->curvalue] );
		*strchr( buffer, '\n' ) = 0;
		Menu_DrawString( RCOLUMN_OFFSET + this->generic.x + this->generic.parent->x, this->generic.y + this->generic.parent->y, buffer );
		strcpy( buffer, strchr( this->itemnames[this->curvalue], '\n' ) + 1 );
		Menu_DrawString( RCOLUMN_OFFSET + this->generic.x + this->generic.parent->x, this->generic.y + this->generic.parent->y + 10, buffer );
	};
};