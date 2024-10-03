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

#include "vgui_controls/TextField.hpp"

bool Field::DoEnter()
{
	if ( this->callback )
	{
		this->callback( this );
		return true;
	};
	return false;
};

void Field::Draw()
{
	int i;
	char tempbuffer[128] = "";

	if ( this->name )
		Menu_DrawStringR2LDark( this->x + this->parent->x + LCOLUMN_OFFSET, this->y + this->parent->y, this->name );

	strncpy( tempbuffer, this->buffer + this->visible_offset, this->visible_length );

	Draw_Char( this->x + this->parent->x + 16, this->y + this->parent->y - 4, 18 );
	Draw_Char( this->x + this->parent->x + 16, this->y + this->parent->y + 4, 24 );

	Draw_Char( this->x + this->parent->x + 24 + this->visible_length * 8, this->y + this->parent->y - 4, 20 );
	Draw_Char( this->x + this->parent->x + 24 + this->visible_length * 8, this->y + this->parent->y + 4, 26 );

	for ( i = 0; i < this->visible_length; i++ )
	{
		Draw_Char( this->x + this->parent->x + 24 + i * 8, this->y + this->parent->y - 4, 19 );
		Draw_Char( this->x + this->parent->x + 24 + i * 8, this->y + this->parent->y + 4, 25 );
	};

	Menu_DrawString( this->x + this->parent->x + 24, this->y + this->parent->y, tempbuffer );

	if ( Menu_ItemAtCursor( this->parent ) == this )
	{
		int offset;

		if ( this->visible_offset )
			offset = this->visible_length;
		else
			offset = this->cursor;

		if ( ( ( int ) ( Sys_Milliseconds() / 250 ) ) & 1 )
		{
			Draw_Char( this->x + this->parent->x + ( offset + 2 ) * 8 + 8,
					   this->y + this->parent->y,
					   11 );
		}
		else
		{
			Draw_Char( this->x + this->parent->x + ( offset + 2 ) * 8 + 8,
					   this->y + this->parent->y,
					   ' ' );
		};
	};
};

bool Field::HandleKey(int key)
{
	extern int keydown[];

	switch ( key )
	{
	case K_KP_SLASH:
		key = '/';
		break;
	case K_KP_MINUS:
		key = '-';
		break;
	case K_KP_PLUS:
		key = '+';
		break;
	case K_KP_HOME:
		key = '7';
		break;
	case K_KP_UPARROW:
		key = '8';
		break;
	case K_KP_PGUP:
		key = '9';
		break;
	case K_KP_LEFTARROW:
		key = '4';
		break;
	case K_KP_5:
		key = '5';
		break;
	case K_KP_RIGHTARROW:
		key = '6';
		break;
	case K_KP_END:
		key = '1';
		break;
	case K_KP_DOWNARROW:
		key = '2';
		break;
	case K_KP_PGDN:
		key = '3';
		break;
	case K_KP_INS:
		key = '0';
		break;
	case K_KP_DEL:
		key = '.';
		break;
	};

	if ( key > 127 )
	{
		switch ( key )
		{
		case K_DEL:
		default:
			return false;
		};
	};

	/*
	** support pasting from the clipboard
	*/
	if ( ( toupper( key ) == 'V' && keydown[K_CTRL] ) ||
		 ( ( ( key == K_INS ) || ( key == K_KP_INS ) ) && keydown[K_SHIFT] ) )
	{
		char *cbd;
		
		if ( ( cbd = Sys_GetClipboardData() ) != 0 )
		{
			strtok( cbd, "\n\r\b" );

			strncpy( this->buffer, cbd, this->length - 1 );
			this->cursor = strlen( this->buffer );
			this->visible_offset = this->cursor - this->visible_length;
			if ( this->visible_offset < 0 )
				this->visible_offset = 0;

			free( cbd );
		};
		return true;
	};

	switch ( key )
	{
	case K_KP_LEFTARROW:
	case K_LEFTARROW:
	case K_BACKSPACE:
		if ( this->cursor > 0 )
		{
			memmove( &this->buffer[this->cursor-1], &this->buffer[this->cursor], strlen( &this->buffer[this->cursor] ) + 1 );
			this->cursor--;

			if ( this->visible_offset )
			{
				this->visible_offset--;
			};
		};
		break;

	case K_KP_DEL:
	case K_DEL:
		memmove( &this->buffer[this->cursor], &this->buffer[this->cursor+1], strlen( &this->buffer[this->cursor+1] ) + 1 );
		break;

	case K_KP_ENTER:
	case K_ENTER:
	case K_ESCAPE:
	case K_TAB:
		return false;

	case K_SPACE:
	default:
		if ( !isdigit( key ) && ( this->generic.flags & QMF_NUMBERSONLY ) )
			return false;

		if ( this->cursor < this->length )
		{
			this->buffer[this->cursor++] = key;
			this->buffer[this->cursor] = 0;

			if ( this->cursor > this->visible_length )
			{
				this->visible_offset++;
			};
		};
	};

	return true;
};