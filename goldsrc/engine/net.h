/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
 * Copyright (C) 2018, 2021, 2023 BlackPhrase
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
/// @brief engine's interface to the networking layer

#pragma once

/*
==============================================================

NET

==============================================================
*/

const int PORT_ANY = -1;

//#define MAX_MSGLEN 1400 // max length of a message
//#define PACKET_HEADER 10 // two ints and a short

#include "enums.h"
#include "netadr.h"

extern netadr_t net_local_adr;
extern netadr_t net_from; // address of who sent the packet
extern sizebuf_t net_message;

extern cvar_t hostname; // TODO
//extern int net_socket;

void NET_Init();
void NET_Shutdown();

void NET_Config(qboolean multiplayer);

qboolean NET_GetPacket(netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message);
void NET_SendPacket(netsrc_t sock, int length, void *data, netadr_t to);

qboolean NET_CompareAdr(netadr_t a, netadr_t b);
qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b);
qboolean NET_IsLocalAddress(netadr_t adr);
char *NET_AdrToString(netadr_t a);
//char		*NET_BaseAdrToString (netadr_t a);
qboolean NET_StringToAdr(char *s, netadr_t *a);
//void		NET_Sleep(int msec);

//============================================================================

#define OLD_AVG 0.99 // total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

#define MAX_LATENT 32

typedef struct
{
	qboolean fatal_error;

	netsrc_t sock;

	float last_received; // for timeouts

	// the statistics are cleared at each client begin, because
	// the server connecting process gives a bogus picture of the data
	float frame_latency; // rolling average
	float frame_rate;

	int drop_count; // dropped packets, cleared each level
	int good_count; // cleared each level

	netadr_t remote_address;
	int qport;

	// bandwidth estimator
	double cleartime; // if realtime > nc->cleartime, free to go
	double rate;      // seconds / byte

	// sequencing variables
	int incoming_sequence;
	int incoming_acknowledged;
	int incoming_reliable_acknowledged; // single bit

	int incoming_reliable_sequence; // single bit, maintained local

	int outgoing_sequence;
	int reliable_sequence;      // single bit
	int last_reliable_sequence; // sequence number of last send

	// reliable staging and holding areas
	sizebuf_t message; // writing buffer to send to server
	byte message_buf[MAX_MSGLEN];

	int reliable_length;
	byte reliable_buf[MAX_MSGLEN]; // unacked reliable message

	// time and size data to calculate bandwidth
	int outgoing_size[MAX_LATENT];
	double outgoing_time[MAX_LATENT];
} netchan_t;

extern int net_drop; // packets dropped before this one

void Netchan_Init();
void Netchan_Setup(netsrc_t sock, netchan_t *chan, netadr_t adr, int qport);

void Netchan_Transmit(netchan_t *chan, int length, byte *data);
void Netchan_OutOfBand(int net_socket, netadr_t adr, int length, byte *data);
void Netchan_OutOfBandPrint(int net_socket, netadr_t adr, const char *format, ...);
qboolean Netchan_Process(netchan_t *chan);

qboolean Netchan_CanPacket(netchan_t *chan); // TODO: ???
qboolean Netchan_CanReliable(netchan_t *chan);