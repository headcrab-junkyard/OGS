/*
Copyright (C) 1997-2001 Id Software, Inc.

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

#include <cstring>
#include <cctype>

#include "client.h"
#include "qmenu.h"

static void  Menu_DrawStatusBar( const char *string );

#define RCOLUMN_OFFSET  16
#define LCOLUMN_OFFSET -16

extern refexport_t re;
extern viddef_t viddef;

#define VID_WIDTH viddef.width
#define VID_HEIGHT viddef.height

#define Draw_Char re.DrawChar
#define Draw_Fill re.DrawFill

void menuaction_s::Action_DoEnter()
{
	if ( this->callback )
		this->callback( this );
};

void menuaction_s::Action_Draw()
{
	if ( this->generic.flags & QMF_LEFT_JUSTIFY )
	{
		if ( this->generic.flags & QMF_GRAYED )
			Menu_DrawStringDark( this->generic.x + this->generic.parent->x + LCOLUMN_OFFSET, this->generic.y + this->generic.parent->y, this->generic.name );
		else
			Menu_DrawString( this->generic.x + this->generic.parent->x + LCOLUMN_OFFSET, this->generic.y + this->generic.parent->y, this->generic.name );
	}
	else
	{
		if ( this->generic.flags & QMF_GRAYED )
			Menu_DrawStringR2LDark( this->generic.x + this->generic.parent->x + LCOLUMN_OFFSET, this->generic.y + this->generic.parent->y, this->generic.name );
		else
			Menu_DrawStringR2L( this->generic.x + this->generic.parent->x + LCOLUMN_OFFSET, this->generic.y + this->generic.parent->y, this->generic.name );
	};
	
	if ( this->generic.ownerdraw )
		this->generic.ownerdraw( this );
};

bool menufield_s::Field_DoEnter( menufield_s *f )
{
	if ( f->generic.callback )
	{
		f->generic.callback( f );
		return true;
	};
	return false;
};

void menufield_s::Field_Draw( menufield_s *f )
{
	int i;
	char tempbuffer[128]="";

	if ( f->generic.name )
		Menu_DrawStringR2LDark( f->generic.x + f->generic.parent->x + LCOLUMN_OFFSET, f->generic.y + f->generic.parent->y, f->generic.name );

	strncpy( tempbuffer, f->buffer + f->visible_offset, f->visible_length );

	Draw_Char( f->generic.x + f->generic.parent->x + 16, f->generic.y + f->generic.parent->y - 4, 18 );
	Draw_Char( f->generic.x + f->generic.parent->x + 16, f->generic.y + f->generic.parent->y + 4, 24 );

	Draw_Char( f->generic.x + f->generic.parent->x + 24 + f->visible_length * 8, f->generic.y + f->generic.parent->y - 4, 20 );
	Draw_Char( f->generic.x + f->generic.parent->x + 24 + f->visible_length * 8, f->generic.y + f->generic.parent->y + 4, 26 );

	for ( i = 0; i < f->visible_length; i++ )
	{
		Draw_Char( f->generic.x + f->generic.parent->x + 24 + i * 8, f->generic.y + f->generic.parent->y - 4, 19 );
		Draw_Char( f->generic.x + f->generic.parent->x + 24 + i * 8, f->generic.y + f->generic.parent->y + 4, 25 );
	};

	Menu_DrawString( f->generic.x + f->generic.parent->x + 24, f->generic.y + f->generic.parent->y, tempbuffer );

	if ( Menu_ItemAtCursor( f->generic.parent ) == f )
	{
		int offset;

		if ( f->visible_offset )
			offset = f->visible_length;
		else
			offset = f->cursor;

		if ( ( ( int ) ( Sys_Milliseconds() / 250 ) ) & 1 )
		{
			Draw_Char( f->generic.x + f->generic.parent->x + ( offset + 2 ) * 8 + 8,
					   f->generic.y + f->generic.parent->y,
					   11 );
		}
		else
		{
			Draw_Char( f->generic.x + f->generic.parent->x + ( offset + 2 ) * 8 + 8,
					   f->generic.y + f->generic.parent->y,
					   ' ' );
		};
	};
};

bool menufield_s::Field_Key( menufield_s *f, int key )
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

			strncpy( f->buffer, cbd, f->length - 1 );
			f->cursor = strlen( f->buffer );
			f->visible_offset = f->cursor - f->visible_length;
			if ( f->visible_offset < 0 )
				f->visible_offset = 0;

			free( cbd );
		};
		return true;
	};

	switch ( key )
	{
	case K_KP_LEFTARROW:
	case K_LEFTARROW:
	case K_BACKSPACE:
		if ( f->cursor > 0 )
		{
			memmove( &f->buffer[f->cursor-1], &f->buffer[f->cursor], strlen( &f->buffer[f->cursor] ) + 1 );
			f->cursor--;

			if ( f->visible_offset )
			{
				f->visible_offset--;
			};
		};
		break;

	case K_KP_DEL:
	case K_DEL:
		memmove( &f->buffer[f->cursor], &f->buffer[f->cursor+1], strlen( &f->buffer[f->cursor+1] ) + 1 );
		break;

	case K_KP_ENTER:
	case K_ENTER:
	case K_ESCAPE:
	case K_TAB:
		return false;

	case K_SPACE:
	default:
		if ( !isdigit( key ) && ( f->generic.flags & QMF_NUMBERSONLY ) )
			return false;

		if ( f->cursor < f->length )
		{
			f->buffer[f->cursor++] = key;
			f->buffer[f->cursor] = 0;

			if ( f->cursor > f->visible_length )
			{
				f->visible_offset++;
			};
		};
	};

	return true;
};

void menuframework_s::Menu_AddItem( void *item )
{
	if ( this->nitems == 0 )
		this->nslots = 0;

	if ( this->nitems < MAXMENUITEMS )
	{
		this->items[this->nitems] = item;
		( ( menucommon_s * ) this->items[this->nitems] )->parent = this;
		this->nitems++;
	};

	this->nslots = this->Menu_TallySlots();
};

/*
** Menu_AdjustCursor
**
** This function takes the given menu, the direction, and attempts
** to adjust the menu's cursor so that it's at the next available
** slot.
*/
void menuframework_s::Menu_AdjustCursor( menuframework_s *m, int dir )
{
	menucommon_s *citem;

	/*
	** see if it's in a valid spot
	*/
	if ( m->cursor >= 0 && m->cursor < m->nitems )
	{
		if ( ( citem = Menu_ItemAtCursor( m ) ) != 0 )
		{
			if ( citem->type != MTYPE_SEPARATOR )
				return;
		};
	};

	/*
	** it's not in a valid spot, so crawl in the direction indicated until we
	** find a valid spot
	*/
	if ( dir == 1 )
	{
		while ( 1 )
		{
			citem = Menu_ItemAtCursor( m );
			if ( citem )
				if ( citem->type != MTYPE_SEPARATOR )
					break;
			m->cursor += dir;
			if ( m->cursor >= m->nitems )
				m->cursor = 0;
		};
	}
	else
	{
		while ( 1 )
		{
			citem = Menu_ItemAtCursor( m );
			if ( citem )
				if ( citem->type != MTYPE_SEPARATOR )
					break;
			m->cursor += dir;
			if ( m->cursor < 0 )
				m->cursor = m->nitems - 1;
		};
	};
};

void menuframework_s::Menu_Center()
{
	int height;

	height = ( ( menucommon_s * ) this->items[this->nitems-1])->y;
	height += 10;

	this->y = ( VID_HEIGHT - height ) / 2;
};

void menuframework_s::Menu_Draw()
{
	int i;
	menucommon_s *item;

	/*
	** draw contents
	*/
	for ( i = 0; i < this->nitems; i++ )
	{
		switch ( ( ( menucommon_s * ) this->items[i] )->type )
		{
		case MTYPE_FIELD:
			Field_Draw( ( menufield_s * ) this->items[i] );
			break;
		case MTYPE_SLIDER:
			Slider_Draw( ( menuslider_s * ) this->items[i] );
			break;
		case MTYPE_LIST:
			MenuList_Draw( ( menulist_s * ) this->items[i] );
			break;
		case MTYPE_SPINCONTROL:
			SpinControl_Draw( ( menulist_s * ) this->items[i] );
			break;
		case MTYPE_ACTION:
			this->( menuaction_s * )items[i]->Action_Draw();
			break;
		case MTYPE_SEPARATOR:
			this->( menuseparator_s * )items[i]->Separator_Draw();
			break;
		};
	};

	item = this->Menu_ItemAtCursor();

	if ( item && item->cursordraw )
	{
		item->cursordraw( item );
	}
	else if ( this->cursordraw )
	{
		this->cursordraw( this );
	}
	else if ( item && item->type != MTYPE_FIELD )
	{
		if ( item->flags & QMF_LEFT_JUSTIFY )
		{
			Draw_Char( this->x + item->x - 24 + item->cursor_offset, this->y + item->y, 12 + ( ( int ) ( Sys_Milliseconds()/250 ) & 1 ) );
		}
		else
		{
			Draw_Char( this->x + item->cursor_offset, this->y + item->y, 12 + ( ( int ) ( Sys_Milliseconds()/250 ) & 1 ) );
		};
	};

	if ( item )
	{
		if ( item->statusbarfunc )
			item->statusbarfunc( ( void * ) item );
		else if ( item->statusbar )
			Menu_DrawStatusBar( item->statusbar );
		else
			Menu_DrawStatusBar( this->statusbar );

	}
	else
	{
		Menu_DrawStatusBar( this->statusbar );
	};
};

void Menu_DrawStatusBar( const char *string )
{
	if ( string )
	{
		int l = strlen( string );
		int maxrow = VID_HEIGHT / 8;
		int maxcol = VID_WIDTH / 8;
		int col = maxcol / 2 - l / 2;

		Draw_Fill( 0, VID_HEIGHT-8, VID_WIDTH, 8, 4 );
		Menu_DrawString( col*8, VID_HEIGHT - 8, string );
	}
	else
	{
		Draw_Fill( 0, VID_HEIGHT-8, VID_WIDTH, 8, 0 );
	};
};

void Menu_DrawString( int x, int y, const char *string )
{
	unsigned i;

	for ( i = 0; i < strlen( string ); i++ )
	{
		Draw_Char( ( x + i*8 ), y, string[i] );
	};
};

void Menu_DrawStringDark( int x, int y, const char *string )
{
	unsigned i;

	for ( i = 0; i < strlen( string ); i++ )
	{
		Draw_Char( ( x + i*8 ), y, string[i] + 128 );
	};
};

void Menu_DrawStringR2L( int x, int y, const char *string )
{
	unsigned i;

	for ( i = 0; i < strlen( string ); i++ )
	{
		Draw_Char( ( x - i*8 ), y, string[strlen(string)-i-1] );
	};
};

void Menu_DrawStringR2LDark( int x, int y, const char *string )
{
	unsigned i;

	for ( i = 0; i < strlen( string ); i++ )
	{
		Draw_Char( ( x - i*8 ), y, string[strlen(string)-i-1]+128 );
	};
};

void *menuframework_s::Menu_ItemAtCursor()
{
	if ( this->cursor < 0 || this->cursor >= this->nitems )
		return 0;

	return this->items[this->cursor];
};

qboolean menuframework_s::Menu_SelectItem()
{
	menucommon_s *item = ( menucommon_s * )this->Menu_ItemAtCursor();

	if ( item )
	{
		switch ( item->type )
		{
		case MTYPE_FIELD:
			return Field_DoEnter( ( menufield_s * ) item ) ;
		case MTYPE_ACTION:
			( menuaction_s * )item->Action_DoEnter();
			return true;
		case MTYPE_LIST:
//			( menulist_s * ) item->Menulist_DoEnter();
			return false;
		case MTYPE_SPINCONTROL:
//			( menulist_s * ) item->SpinControl_DoEnter();
			return false;
		};
	};
	return false;
};

void menuframework_s::Menu_SetStatusBar(const char *string )
{
	this->statusbar = string;
};

void menuframework_s::Menu_SlideItem(int dir )
{
	menucommon_s *item = ( menucommon_s * ) this->Menu_ItemAtCursor();

	if ( item )
	{
		switch ( item->type )
		{
		case MTYPE_SLIDER:
			( menuslider_s * ) item->Slider_DoSlide(dir );
			break;
		case MTYPE_SPINCONTROL:
			( menulist_s * ) item->SpinControl_DoSlide( dir );
			break;
		};
	};
};

int menuframework_s::Menu_TallySlots()
{
	int i;
	int total = 0;

	for ( i = 0; i < this->nitems; i++ )
	{
		if ( ( ( menucommon_s * ) this->items[i] )->type == MTYPE_LIST )
		{
			int nitems = 0;
			const char **n = ( ( menulist_s * )this->items[i] )->itemnames;

			while (*n)
				nitems++, n++;

			total += nitems;
		}
		else
		{
			total++;
		};
	};

	return total;
};

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

void menuseparator_s::Separator_Draw()
{
	if ( this->name )
		Menu_DrawStringR2LDark( this->x + this->parent->x, this->y + this->parent->y, this->name );
};

void menuslider_s::Slider_DoSlide( int dir )
{
	this->curvalue += dir;

	if ( this->curvalue > this->maxvalue )
		this->curvalue = this->maxvalue;
	else if ( this->curvalue < this->minvalue )
		this->curvalue = this->minvalue;

	if ( this->callback )
		this->callback( this );
};

#define SLIDER_RANGE 10

void menuslider_s::Slider_Draw()
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