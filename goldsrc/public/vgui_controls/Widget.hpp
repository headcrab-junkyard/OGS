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

#pragma once

class menucommon_s
{
public:
	CWidget(menuframework_s *apParent, const char *asName);
	~CWidget();
	
	enum class Type : int
	{
		MTYPE_INVALID = -1,
		MTYPE_SLIDER = 0,
		MTYPE_LIST,
		MTYPE_ACTION,
		MTYPE_SPINCONTROL,
		MTYPE_SEPARATOR,
		MTYPE_FIELD
	};

	Type type{Type::MTYPE_INVALID};
	const char *name{""};
	int x{0}, y{0};
	menuframework_s *parent{nullptr};
	int cursor_offset{0};
	int	localdata[4]{};
	uint flags{0};

	const char *statusbar{""};

	void (*callback)( void *self );
	void (*statusbarfunc)( void *self );
	void (*ownerdraw)( void *self );
	void (*cursordraw)( void *self );
};