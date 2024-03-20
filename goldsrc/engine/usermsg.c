/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2020, 2022 BlackPhrase
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

#include "quakedef.h"
#include "usermsg.h"

typedef struct usermsg_s
{
	int size; ///< Byte size of message, or -1 for variable sized
	char name[16]; // BP: looks like only 16 chars are allowed
	pfnUserMsgHook mfnHook; ///< Client only dispatch function for message
} usermsg_t;

// TODO: add cl_messages concmd

// SERVER ONLY

void SV_BroadcastNewUserMsg(int nIndex, int nSize, const char *sName)
{
	client_t *pClient = svs.clients;
	
	for(int i = 0; i < svs.maxclients; ++i, ++pClient)
	{
		if(!pClient->active)
			continue;
		
		MSG_WriteByte(&pClient->netchan.message, svc_newusermsg);
		MSG_WriteByte(&pClient->netchan.message, nIndex);
		MSG_WriteByte(&pClient->netchan.message, nSize);
		MSG_WriteString(&pClient->netchan.message, sName);
	};
};

int SV_RegUserMsg(const char *szMsgName, int size)
{
	static int gnLastIndex = 64; // Start counting from 64 // TODO: why?

	// Check if already registered
	for(int i = 0; i < MAX_USER_MESSAGES; ++i)
		if(!Q_strcmp(sv.usermsgs[gnLastIndex]->name, szMsgName))
			return i;
	
	if(gnLastIndex - 64 >= MAX_USER_MESSAGES)
		return 0;
	
	strncpy(sv.usermsgs[gnLastIndex]->name, szMsgName, sizeof(sv.usermsgs[gnLastIndex]->name) - 1);
	sv.usermsgs[gnLastIndex]->size = size;
	
	SV_BroadcastNewUserMsg(gnLastIndex, size, szMsgName);
	
	return gnLastIndex++;
};

// CLIENT ONLY

void CL_RegUserMsg(int index, int size, const char *szMsgName)
{
	static int gnLastIndex = 64; // TODO: Start counting from 64 because user messages are using the same functions 
								 // as standard protocol entries, so we should register them right after the last 
								 // entry of the standard protocol to prevent them from overlapping each other
	
	// Check if already registered
	for(int i = 0; i < MAX_USER_MESSAGES; ++i)
		if(!Q_strcmp(cl.messages[gnLastIndex]->name, szMsgName))
			return;
	
	if(gnLastIndex - 64 >= MAX_USER_MESSAGES)
		return;
	
	Q_strcpy(cl.messages[gnLastIndex]->name, szMsgName);
	cl.messages[gnLastIndex]->size = size;
	
	++gnLastIndex;
};

int pfnHookUserMsg(const char *szMsgName, pfnUserMsgHook afnHook)
{
	for(int i = 0; i < MAX_USER_MESSAGES; i++)
		if(!Q_strcmp(cl.messages[i]->name, szMsgName))
		{
			if(cl.messages[i]->mfnHook)
				return 0; // TODO: already hooked
			
			cl.messages[i]->mfnHook = afnHook;
			return 1;
		};
	
	return 0;
};

void DispatchUserMsg(int id)
{
	if(!cl.messages[id]->mfnHook)
		Sys_Error("UserMsg: No pfn %s %d", cl.messages[id]->name, id);
	
	if(id < 0 || id > MAX_USER_MESSAGES)
		Sys_Error("DispatchUserMsg:  Illegal User Msg %d", id);
	
	cl.messages[id]->mfnHook(cl.messages[id]->name, net_message.cursize, net_message.data);
};