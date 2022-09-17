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