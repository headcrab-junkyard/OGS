/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"
#include "usermsg.h"

#define MAX_USERMSGS 128 // TODO: ???

/// Client dispatch function
typedef int (*pfnUserMsgHook)(const char *sName, int nSize, void *pBuf);

typedef struct usermsg_s
{
	int size; ///< Byte size of message, or -1 for variable sized
	const char *name;
	pfnUserMsgHook mfnHook; ///< Client only dispatch function for message
} usermsg_t;

usermsg_t cl_messages[MAX_USERMSGS];

// SERVER ONLY

void RegisterUserMsg(char *szMsgName, int size)
{
	// TODO
};

// CLIENT ONLY

int HookUserMsg(char *szMsgName, pfnUserMsgHook afnHook)
{
	for(int i = 0; i < MAX_USERMSGS; i++)
		if(!Q_strcmp(cl_messages[i].name, szMsgName))
		{
			if(cl_messages[i].mfnHook)
				return 0; // TODO: already hooked
			
			cl_messages[i].mfnHook = afnHook;
			return 1;
		};
	
	return 0;
};

void DispatchUserMsg(int id)
{
	if(!cl_messages[id].mfnHook)
		Sys_Error("UserMsg: No pfn %s %d", cl_messages[id].name, id);
	
	if(id < 0 || i > MAX_USERMSGS)
		Sys_Error("DispatchUserMsg:  Illegal User Msg %d", i);
	
	cl_messages[id].mfnHook(cl_messages[id].name, net_message.cursize, net_message.data);
};