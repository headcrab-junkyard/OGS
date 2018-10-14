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

enum eMenuType
{
	MTYPE_INVALID = -1,
	MTYPE_SLIDER = 0,
	MTYPE_LIST,
	MTYPE_ACTION,
	MTYPE_SPINCONTROL,
	MTYPE_SEPARATOR,
	MTYPE_FIELD
};

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

struct menucommon_s
{
	eMenuType type{MTYPE_INVALID};
	const char *name{""};
	int x{0}, y{0};
	menuframework_s *parent{nullptr};
	int cursor_offset{0};
	int	localdata[4]{};
	unsigned int flags{0};

	const char *statusbar{""};

	void (*callback)( void *self );
	void (*statusbarfunc)( void *self );
	void (*ownerdraw)( void *self );
	void (*cursordraw)( void *self );
};

struct menufield_s : public menucommon_s
{
	bool Field_DoEnter( menufield_s *f );
	void Field_Draw( menufield_s *f );
	bool Field_Key( menufield_s *field, int key );
	
	char buffer[80]{};
	int cursor{0};
	int length{0};
	int visible_length{0};
	int visible_offset{0};
};

struct menuslider_s : public menucommon_s
{
	void Slider_DoSlide( int dir );
	void Slider_Draw();

	float minvalue{0.0f};
	float maxvalue{0.0f};
	float curvalue{0.0f};

	float range{0.0f};
};

struct menulist_s : public menucommon_s
{
	void Menulist_DoEnter();
	void MenuList_Draw();

	void SpinControl_DoEnter();
	void SpinControl_Draw();
	void SpinControl_DoSlide( int dir );

	int curvalue{0};

	const char **itemnames{""};
};

struct menuaction_s : public menucommon_s
{
	void Action_DoEnter();
	void Action_Draw();
};

struct menuseparator_s : public menucommon_s
{
	void Separator_Draw();
};

void	 Menu_DrawString( int, int, const char * );
void	 Menu_DrawStringDark( int, int, const char * );
void	 Menu_DrawStringR2L( int, int, const char * );
void	 Menu_DrawStringR2LDark( int, int, const char * );