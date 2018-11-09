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

typedef struct usermsg_s
{
	int index;
	int size; ///< Byte size of message, or -1 for variable sized
	const char *name;
	pfnUserMsgHook mfnHook; ///< Client only dispatch function for message
} usermsg_t;

// TODO: add cl_messages concmd

// SERVER ONLY

usermsg_t gSvUserMsgs[MAX_USERMSGS];
int num_svusermsgs = 0;

int gnLastIndex = 64; // Start counting from 64

int RegUserMsg(const char *szMsgName, int size)
{
	// Check if already registered
	for(int i = 0; i < MAX_USERMSGS; i++)
		if(!strcmp(gSvUserMsgs[num_svusermsgs].name, szMsgName))
			return 0;
	
	if(num_svusermsgs >= MAX_USERMSGS)
		return 0;
	
	gSvUserMsgs[num_svusermsgs].index = gnLastIndex;
	//strcpy(gSvUserMsgs[num_svusermsgs].name, szMsgName);
	gSvUserMsgs[num_svusermsgs].name = szMsgName;
	gSvUserMsgs[num_svusermsgs].size = size;
	
	num_svusermsgs++;
	
	return gnLastIndex++;
};

// CLIENT ONLY

usermsg_t gClUserMsgs[MAX_USERMSGS];
int num_clusermsgs = 0;

void RegClUserMsg(int index, int size, const char *szMsgName)
{
	// Check if already registered
	for(int i = 0; i < MAX_USERMSGS; i++)
		if(!strcmp(gClUserMsgs[num_clusermsgs].name, szMsgName))
			return;
	
	if(num_clusermsgs >= MAX_USERMSGS)
		return;
	
	gClUserMsgs[num_clusermsgs].index = index;
	//strcpy(gClUserMsgs[num_clusermsgs].name, szMsgName);
	gClUserMsgs[num_clusermsgs].name = szMsgName;
	gClUserMsgs[num_clusermsgs].size = size;
	
	num_clusermsgs++;
};

int HookUserMsg(const char *szMsgName, pfnUserMsgHook afnHook)
{
	for(int i = 0; i < MAX_USERMSGS; i++)
		if(!Q_strcmp(gClUserMsgs[i].name, szMsgName))
		{
			if(gClUserMsgs[i].mfnHook)
				return 0; // TODO: already hooked
			
			gClUserMsgs[i].mfnHook = afnHook;
			return 1;
		};
	
	return 0;
};

void DispatchUserMsg(int id)
{
	if(!gClUserMsgs[id].mfnHook)
		Sys_Error("UserMsg: No pfn %s %d", gClUserMsgs[id].name, id);
	
	if(id < 0 || id > MAX_USERMSGS)
		Sys_Error("DispatchUserMsg:  Illegal User Msg %d", id);
	
	gClUserMsgs[id].mfnHook(gClUserMsgs[id].name, net_message.cursize, net_message.data);
};