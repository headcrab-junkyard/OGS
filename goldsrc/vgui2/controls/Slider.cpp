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

#include "vgui_controls/Slider.hpp"

#define SLIDER_RANGE 10

CSlider::CSlider(float afMinValue, float afMaxValue) : minvalue(afMinValue), maxvalue(afMaxValue)
{
};

CSlider::~CSlider() = default;

void CSlider::Draw()
{
	int	i;

	Menu_DrawStringR2LDark( this->generic.x + this->generic.parent->x + LCOLUMN_OFFSET,
		                this->generic.y + this->generic.parent->y, 
						this->generic.name );

	this->range = ( this->curvalue - this->minvalue ) / ( float ) ( this->maxvalue - this->minvalue );

	if ( this->range < 0)
		this->range = 0;
	if ( this->range > 1)
		this->range = 1;
	Draw_Char( this->generic.x + this->generic.parent->x + RCOLUMN_OFFSET, this->generic.y + this->generic.parent->y, 128);
	for ( i = 0; i < SLIDER_RANGE; i++ )
		Draw_Char( RCOLUMN_OFFSET + this->generic.x + i*8 + this->generic.parent->x + 8, this->generic.y + this->generic.parent->y, 129);
	Draw_Char( RCOLUMN_OFFSET + this->generic.x + i*8 + this->generic.parent->x + 8, this->generic.y + this->generic.parent->y, 130);
	Draw_Char( ( int ) ( 8 + RCOLUMN_OFFSET + this->generic.parent->x + this->generic.x + (SLIDER_RANGE-1)*8 * this->range ), this->generic.y + this->generic.parent->y, 131);
};

void CSlider::DoSlide( int dir )
{
	this->curvalue += dir;

	if ( this->curvalue > this->maxvalue )
		this->curvalue = this->maxvalue;
	else if ( this->curvalue < this->minvalue )
		this->curvalue = this->minvalue;

	if ( this->callback )
		this->callback( this );
};