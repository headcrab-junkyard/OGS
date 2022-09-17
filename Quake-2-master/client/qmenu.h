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

#pragma once

#define MAXMENUITEMS	64

#define	K_TAB			9
#define	K_ENTER			13
#define	K_ESCAPE		27
#define	K_SPACE			32

// normal keys should be passed as lowercased ascii

#define	K_BACKSPACE		127
#define	K_UPARROW		128
#define	K_DOWNARROW		129
#define	K_LEFTARROW		130
#define	K_RIGHTARROW	131

#define QMF_LEFT_JUSTIFY	0x00000001
#define QMF_GRAYED			0x00000002
#define QMF_NUMBERSONLY		0x00000004

struct menuframework_s
{
	void	Menu_AddItem( void *item );
	void	Menu_AdjustCursor( menuframework_s *menu, int dir );
	void	Menu_Center();
	void	Menu_Draw();
	void	*Menu_ItemAtCursor();
	qboolean Menu_SelectItem();
	void	Menu_SetStatusBar( const char *string );
	void	Menu_SlideItem( int dir );
	int		Menu_TallySlots();

	int x, y;
	int	cursor;

	int	nitems;
	int nslots;
	void *items[64];

	const char *statusbar;

	void (*cursordraw)( struct _tag_menuframework *m );
	
};

void	 Menu_DrawString( int, int, const char * );
void	 Menu_DrawStringDark( int, int, const char * );
void	 Menu_DrawStringR2L( int, int, const char * );
void	 Menu_DrawStringR2LDark( int, int, const char * );