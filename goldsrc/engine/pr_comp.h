/*
Copyright (C) 1996-1997 Id Software, Inc.

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

// this file is shared by quake and qcc



typedef struct statement_s
{
	unsigned short	op;
	short	a,b,c;
} dstatement_t;

typedef struct
{
	unsigned short	type;		// if DEF_SAVEGLOBGAL bit is set
								// the variable needs to be saved in savegames
	unsigned short	ofs;
	int			s_name;
} ddef_t;
#define	DEF_SAVEGLOBAL	(1<<15)

#define	MAX_PARMS	8

typedef struct
{
	int		first_statement;	// negative numbers are builtins
	int		parm_start;
	int		locals;				// total ints of parms + locals
	
	int		profile;		// runtime
	
	int		s_name;
	int		s_file;			// source file defined in
	
	int		numparms;
	byte	parm_size[MAX_PARMS];
} dfunction_t;


#define	PROG_VERSION	6
typedef struct
{
	int		version;
	int		crc;			// check of header file
	
	int		ofs_statements;
	int		numstatements;	// statement 0 is an error

	int		ofs_globaldefs;
	int		numglobaldefs;
	
	int		ofs_fielddefs;
	int		numfielddefs;
	
	int		ofs_functions;
	int		numfunctions;	// function 0 is an empty
	
	int		ofs_strings;
	int		numstrings;		// first string is a null string

	int		ofs_globals;
	int		numglobals;
	
	int		entityfields;
} dprograms_t;

