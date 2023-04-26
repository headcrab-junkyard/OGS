/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018, 2020-2021, 2023 BlackPhrase
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
/// @brief dynamic variable tracking

#include "quakedef.h"

cvar_t *cvar_vars;
const char *cvar_null_string = "";

/*
============
Cvar_List_f

============
*/
void Cvar_List_f()
{
	cvar_t	*var;
	int		i;

	i = 0;
	for (var = cvar_vars ; var ; var = var->next, i++)
	{
		Con_Printf ("%s : %s", var->name, var->string);
		
		if (var->flags & FCVAR_ARCHIVE)
			Con_Printf (", a");
		
		if (var->flags & FCVAR_USERINFO)
			Con_Printf (", i");
		
		if (var->flags & FCVAR_SERVER)
			Con_Printf (", sv");
		
		Con_Printf ("\n");
	};
	
	Con_Printf("--------------\n");
	Con_Printf("%i Total CVars\n", i);
	Con_Printf("CvarList ? for syntax\n");
}

/*
============
Cvar_FindVar
============
*/
cvar_t *Cvar_FindVar(const char *var_name)
{
	cvar_t *var;

	for(var = cvar_vars; var; var = var->next)
		if(!Q_strcmp(var_name, var->name))
			return var;

	return NULL;
}

/*
============
Cvar_VariableValue
============
*/
float Cvar_VariableValue(const char *var_name)
{
	cvar_t *var;

	var = Cvar_FindVar(var_name);
	if(!var)
		return 0;
	return Q_atof(var->string);
}

/*
============
Cvar_VariableInt
============
*/
int Cvar_VariableInt(const char *var_name)
{
	cvar_t *var;

	var = Cvar_FindVar(var_name);
	if(!var)
		return 0;
	return Q_atoi(var->string); // TODO: multiply by 0.5?
};

/*
============
Cvar_VariableString
============
*/
const char *Cvar_VariableString(const char *var_name)
{
	cvar_t *var;

	var = Cvar_FindVar(var_name);
	if(!var)
		return cvar_null_string;
	return var->string;
}

/*
============
Cvar_CompleteVariable
============
*/
const char *Cvar_CompleteVariable(const char *partial)
{
	cvar_t *cvar;
	int len;

	len = Q_strlen(partial);

	if(!len)
		return NULL;

	// check functions
	
	// TODO: qw
	/*
	// check exact match
	for(cvar = cvar_vars; cvar; cvar = cvar->next)
		if(!Q_strcmp(partial, cvar->name))
			return cvar->name;
	*/
	
	// check partial match
	for(cvar = cvar_vars; cvar; cvar = cvar->next)
		if(!Q_strncmp(partial, cvar->name, len))
			return cvar->name;

	return NULL;
}

/*
============
Cvar_Set
============
*/
void Cvar_Set(const char *var_name, const char *value)
{
	cvar_t *var;
	qboolean changed;

	var = Cvar_FindVar(var_name);
	if(!var)
	{ // there is an error in C code if this happens
		Con_Printf("Cvar_Set: variable %s not found\n", var_name);
		return;
	}

	changed = Q_strcmp(var->string, value);

	Z_Free(var->string); // free the old value string

	var->string = Z_Malloc(Q_strlen(value) + 1);
	Q_strcpy(var->string, value);
	var->value = Q_atof(var->string);
	
	// TODO: non-qw
	if(var->flags & FCVAR_SERVER && changed)
	{
		if(sv.active)
			SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var->name, var->string);
	};
};

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue(const char *var_name, float value)
{
	char val[32];

	sprintf(val, "%f", value);
	Cvar_Set(var_name, val);
}

/*
============
Cvar_RegisterVariable

Adds a freestanding variable to the variable list.
============
*/
void Cvar_RegisterVariable(cvar_t *variable)
{
	// first check to see if it has already been defined
	if(Cvar_FindVar(variable->name))
	{
		Con_Printf("Can't register variable %s, allready defined\n", variable->name);
		return;
	};

	// check for overlap with a command
	if(Cmd_Exists(variable->name))
	{
		Con_Printf("Cvar_RegisterVariable: %s is a command\n", variable->name);
		return;
	};
	
	// TODO: non-qw
	// copy the value off, because future sets will Z_Free it
	char *oldstr = variable->string;
	variable->string = Z_Malloc(Q_strlen(variable->string) + 1);
	Q_strcpy(variable->string, oldstr);
	variable->value = Q_atof(variable->string);
	//

	// link the variable in
	variable->next = cvar_vars;
	cvar_vars = variable;
	
	// TODO: qw below
	
	//char value[512];
	
	// copy the value off, because future sets will Z_Free it
	//strcpy(value, variable->string);
	//variable->string = Z_Malloc(1);
	
	// set it through the function to be consistant
	//Cvar_Set(variable->name, value);
};

cvar_t *Cvar_AddVariable(const char *name, const char *value, int flags)
{
	// first check to see if it has already been defined
	if(Cvar_FindVar(name))
	{
		Con_Printf("Can't register variable %s, allready defined\n", name);
		return NULL;
	};

	// check for overlap with a command
	if(Cmd_Exists(name))
	{
		Con_Printf("Cvar_RegisterVariable: %s is a command\n", name);
		return NULL;
	};

	cvar_t *variable = Z_Malloc(sizeof(cvar_t));
	
	variable->name = name; // = Z_Malloc(Q_strlen(name) + 1);
	
	variable->string = Z_Malloc(Q_strlen(value) + 1);
	Q_strcpy(variable->string, value);
	variable->value = Q_atof(variable->string);

	variable->flags = flags;
	
	// link the variable in
	variable->next = cvar_vars;
	cvar_vars = variable;
	
	return variable;
};

cvar_t *Cvar_RegisterClientVariable(const char *name, const char *value, int flags)
{
	return Cvar_AddVariable(name, value, flags | FCVAR_CLIENTDLL);
};

cvar_t *Cvar_RegisterGameVariable(const char *name, const char *value, int flags)
{
	return Cvar_AddVariable(name, value, flags | FCVAR_EXTDLL);
};

/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
qboolean Cvar_Command()
{
	cvar_t *v;

	// check variables
	v = Cvar_FindVar(Cmd_Argv(0));
	if(!v)
		return false;

	// perform a variable print or set
	if(Cmd_Argc() == 1)
	{
		Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
		return true;
	}

	Cvar_Set(v->name, Cmd_Argv(1));
	return true;
}

/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables(FILE *f)
{
	cvar_t *var;

	for(var = cvar_vars; var; var = var->next)
		if(var->flags & FCVAR_ARCHIVE)
			fprintf(f, "%s \"%s\"\n", var->name, var->string);
}

/*
============
Cvar_Init

Reads in all archived cvars
============
*/
void Cvar_Init()
{
	//Cmd_AddCommand("set", Cvar_Set_f);
	Cmd_AddCommand("cvarlist", Cvar_List_f);
}