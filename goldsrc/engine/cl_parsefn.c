/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018-2019 BlackPhrase
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
#include "event_args.h"

#define MAX_EVENTS 1024


typedef struct event_s
{
	unsigned short id;
	const char *name;
	event_args_t args;
	float delay;
	//pfnEventHook fnHook;
} event_t;

event_t gEventQue[MAX_EVENTS];

void CL_QueEvent(event_args_t *apArgs, float delay)
{
	static int nLast = 0;
	
	if(nLast >= MAX_EVENTS)
		return;
	
	gEventQue[nLast].args.flags = apArgs->flags;
	gEventQue[nLast].args.entindex = apArgs->entindex;
	
	VectorCopy(apArgs->origin, gEventQue[nLast].args.origin);
	VectorCopy(apArgs->angles, gEventQue[nLast].args.angles);
	VectorCopy(apArgs->velocity, gEventQue[nLast].args.velocity);
	
	gEventQue[nLast].args.ducking = apArgs->ducking;
	
	gEventQue[nLast].args.fparam1 = apArgs->fparam1;
	gEventQue[nLast].args.fparam2 = apArgs->fparam2;
	
	gEventQue[nLast].args.iparam1 = apArgs->iparam1;
	gEventQue[nLast].args.iparam2 = apArgs->iparam2;
	
	gEventQue[nLast].args.bparam1 = apArgs->bparam1;
	gEventQue[nLast].args.bparam2 = apArgs->bparam2;
	
	gEventQue[nLast].delay = delay;
	
	nLast++;
};

void CL_ParseEvent()
{
	// TODO
	
	event_args_t args;
	
	int i = 0;
	int nNumberOfEvents = MSG_ReadByte();
	while(i < nNumberOfEvents)
	{
		++i;

		short nEventIndex = MSG_ReadShort();

		qboolean bHasEntsInPack = MSG_ReadOneBit(); // TODO

		short nPacketIndex = MSG_ReadShort(); // TODO

		Q_memset(&args, 0, sizeof(args));
		
		qboolean bHasEventArgs = MSG_ReadOneBit();
		if(bHasEventArgs)
			MSG_ReadEventArgs(&args);

		float fFireTime = 0.0f;

		qboolean bHasFireTime = MSG_ReadOneBit();
		if(bHasFireTime)
			fFireTime = MSG_ReadShort();

		CL_QueueEvent(0, nEventIndex, fFireTime, &args); // TODO: read flags
	};
};

void CL_FireEvents()
{
	// TODO
	
	//for(int i = 0; i < MAX_EVENTS; i++)
	{
		//gEventQue[i]
		
		//Con_Printf("CL_FireEvents: Unknown event %d:%s", currentevent.id, currentevent.name);
	};
};