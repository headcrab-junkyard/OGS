/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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

// TODO: Linux also should support IPX nowadays

#include "quakedef.h"

/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <errno.h>
*/

#ifdef _WIN32
#include "winquake.h"
#endif

/*
#if defined(sun)
#include <unistd.h>
#endif

#ifdef sun
#include <sys/filio.h>
#endif

#ifdef NeXT
#include <libc.h>
#endif
*/

#define	MAX_LOOPBACK	4

typedef struct
{
	byte	data[MAX_MSGLEN];
	int		datalen;
} loopmsg_t;

typedef struct
{
	loopmsg_t	msgs[MAX_LOOPBACK];
	int			get, send;
} loopback_t;

loopback_t loopbacks[2];
int ip_sockets[2];
int ipx_sockets[2];

netadr_t net_local_adr;

netadr_t net_from; // TODO
sizebuf_t net_message; // TODO
/*
#ifdef _WIN32
int net_socket;
#else
int net_socket;      // non blocking, for receives
int net_send_socket; // blocking, for sends
#endif
*/

#ifdef _WIN32
#define MAX_UDP_PACKET (MAX_MSGLEN * 2) // one more than msg + header
#else
#define MAX_UDP_PACKET 8192
#endif
byte net_message_buffer[MAX_UDP_PACKET];

#ifdef _WIN32
WSADATA winsockdata;
#endif

#ifndef _WIN32
int gethostname(char *, int);
int close(int);
#endif

//=============================================================================

//void NetadrToSockadr (netadr_t *a, struct sockaddr *s) // TODO: windows q2
void NetadrToSockadr(netadr_t *a, struct sockaddr_in *s)
{
	memset(s, 0, sizeof(*s));
	
	if (a->type == NA_BROADCAST)
	{
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_port = a->port;
		((struct sockaddr_in *)s)->sin_addr.s_addr = INADDR_BROADCAST;
	}
	else if (a->type == NA_IP)
	{
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&a->ip; // TODO: *(int *)&s->sin_addr = *(int *)&a->ip;
		((struct sockaddr_in *)s)->sin_port = a->port;
	}
#ifdef _WIN32
	else if (a->type == NA_IPX)
	{
		((struct sockaddr_ipx *)s)->sa_family = AF_IPX;
		memcpy(((struct sockaddr_ipx *)s)->sa_netnum, &a->ipx[0], 4);
		memcpy(((struct sockaddr_ipx *)s)->sa_nodenum, &a->ipx[4], 6);
		((struct sockaddr_ipx *)s)->sa_socket = a->port;
	}
	else if (a->type == NA_BROADCAST_IPX)
	{
		((struct sockaddr_ipx *)s)->sa_family = AF_IPX;
		memset(((struct sockaddr_ipx *)s)->sa_netnum, 0, 4);
		memset(((struct sockaddr_ipx *)s)->sa_nodenum, 0xff, 6);
		((struct sockaddr_ipx *)s)->sa_socket = a->port;
	};
#endif
}

//void SockadrToNetadr (struct sockaddr *s, netadr_t *a) // TODO: windows q2
void SockadrToNetadr(struct sockaddr_in *s, netadr_t *a)
{
	if (s->sa_family == AF_INET)
	{
		a->type = NA_IP;
		*(int *)&a->ip = ((struct sockaddr_in *)s)->sin_addr.s_addr; // TODO: = *(int *)&s->sin_addr;
		a->port = ((struct sockaddr_in *)s)->sin_port;
	}
#ifdef _WIN32
	else if (s->sa_family == AF_IPX)
	{
		a->type = NA_IPX;
		memcpy(&a->ipx[0], ((struct sockaddr_ipx *)s)->sa_netnum, 4);
		memcpy(&a->ipx[4], ((struct sockaddr_ipx *)s)->sa_nodenum, 6);
		a->port = ((struct sockaddr_ipx *)s)->sa_socket;
	};
#endif
}

/*
===================
NET_CompareBaseAdr

Compares without the port
===================
*/
qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b)
{
	if(a.type != b.type)
		return false;
	
	if(a.type == NA_LOOPBACK)
		return TRUE;
	
	if(a.type == NA_IP)
	{
		if(a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
			return true;
		return false;
	};
	
	if(a.type == NA_IPX)
	{
		if((memcmp(a.ipx, b.ipx, 10) == 0))
			return true;
		return false;
	};
}

qboolean NET_CompareAdr(netadr_t a, netadr_t b)
{
	if(a.type != b.type)
		return false;
	
	if(a.type == NA_LOOPBACK)
		return TRUE;
	
	if(a.type == NA_IP)
	{
		if(a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
			return true;
		return false;
	};
	
	if(a.type == NA_IPX)
	{
		if((memcmp(a.ipx, b.ipx, 10) == 0) && a.port == b.port)
			return true;
		return false;
	};
}

char *NET_AdrToString(netadr_t a)
{
	static char s[64];

	if(a.type == NA_LOOPBACK)
		Q_snprintf(s, sizeof(s), "loopback");
	else if(a.type == NA_IP)
		Q_snprintf(s, sizeof(s), "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
		////sprintf(s, "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
	else
		Q_snprintf(s, sizeof(s), "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%i", a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9], ntohs(a.port));
	
	return s;
}

char *NET_BaseAdrToString(netadr_t a)
{
	static char s[64];

	sprintf(s, "%i.%i.%i.%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);

	return s;
}

/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
qboolean NET_StringToAdr(char *s, netadr_t *a)
{
	struct hostent *h;
	struct sockaddr_in sadr;
	char *colon;
	char copy[128];

	memset(&sadr, 0, sizeof(sadr));
	sadr.sin_family = AF_INET;

	sadr.sin_port = 0;

	strcpy(copy, s);
	// strip off a trailing :port if present
	for(colon = copy; *colon; colon++)
		if(*colon == ':')
		{
			*colon = 0;
			sadr.sin_port = htons((short)atoi(colon + 1)); // TODO: no casting to short for linux
		}

	if(copy[0] >= '0' && copy[0] <= '9')
	{
		*(int *)&sadr.sin_addr = inet_addr(copy);
	}
	else
	{
		if((h = gethostbyname(copy)) == 0) // TODO: if(!(h = gethostbyname(copy))) for linux
			return 0;
		*(int *)&sadr.sin_addr = *(int *)h->h_addr_list[0];
	}

	SockadrToNetadr(&sadr, a);

	return true;
}

qboolean NET_IsLocalAddress(netadr_t adr)
{
#ifdef _WIN32
	return adr.type == NA_LOOPBACK;
#else
	return NET_CompareAdr(adr, net_local_adr);
#endif
}

// Returns true if we can't bind the address locally--in other words,
// the IP is NOT one of our interfaces.
/*
qboolean NET_IsClientLegal(netadr_t *adr)
{
	struct sockaddr_in sadr;
	int newsocket;

#if 0
	if (adr->ip[0] == 127)
		return false; // no local connections period

	NetadrToSockadr (adr, &sadr);

	if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		Sys_Error ("NET_IsClientLegal: socket:", strerror(errno));

	sadr.sin_port = 0;

	if( bind (newsocket, (void *)&sadr, sizeof(sadr)) == -1) 
	{
		// It is not a local address
		close(newsocket);
		return true;
	}
	close(newsocket);
	return false;
#else
	return true;
#endif
}
*/

/*
=============================================================================

LOOPBACK BUFFERS FOR LOCAL PLAYER

=============================================================================
*/

qboolean NET_GetLoopPacket (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
	int		i;
	loopback_t	*loop;

	loop = &loopbacks[sock];

	if (loop->send - loop->get > MAX_LOOPBACK)
		loop->get = loop->send - MAX_LOOPBACK;

	if (loop->get >= loop->send)
		return false;

	i = loop->get & (MAX_LOOPBACK-1);
	loop->get++;

	memcpy (net_message->data, loop->msgs[i].data, loop->msgs[i].datalen);
	net_message->cursize = loop->msgs[i].datalen;

#ifdef _WIN32
	memset (net_from, 0, sizeof(*net_from));
	net_from->type = NA_LOOPBACK;
#else
	*net_from = net_local_adr;
#endif

	return true;

}

void NET_SendLoopPacket (netsrc_t sock, int length, void *data, netadr_t to)
{
	int		i;
	loopback_t	*loop;

	loop = &loopbacks[sock^1];

	i = loop->send & (MAX_LOOPBACK-1);
	loop->send++;

	memcpy (loop->msgs[i].data, data, length);
	loop->msgs[i].datalen = length;
}

//=============================================================================

qboolean NET_GetPacket(netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
	int ret;
	struct sockaddr_in from;
	int fromlen;
	int net_socket;
	int protocol;
	int err;

	if(NET_GetLoopPacket(sock, net_from, net_message))
		return true;
	
	fromlen = sizeof(from);
	ret = recvfrom(net_socket, net_message->data, net_message->maxsize, 0, (struct sockaddr *)&from, &fromlen);
	
#ifdef _WIN32
	SockadrToNetadr(&from, net_from);
#endif

	if(ret == -1)
	{
#ifdef _WIN32
		int err = WSAGetLastError();

		if(err == WSAEWOULDBLOCK)
			return false;
		if(err == WSAEMSGSIZE)
		{
			Con_Printf("Warning:  Oversize packet from %s\n",
			           NET_AdrToString(*net_from));
			return false;
		}

		Sys_Error("NET_GetPacket: %s", strerror(err));
#else
		if(errno == EWOULDBLOCK)
			return false;
		if(errno == ECONNREFUSED)
			return false;
		Sys_Printf("NET_GetPacket: %s\n", strerror(errno));
		return false;
#endif
	}

	net_message->cursize = ret;
	
#ifdef _WIN32
	if(ret == sizeof(net_message_buffer))
	{
		Con_Printf("Oversize packet from %s\n", NET_AdrToString(*net_from));
		return false;
	}
#else
	SockadrToNetadr(&from, net_from);
#endif

	return ret;
}

//=============================================================================

void NET_SendPacket(netsrc_t sock, int length, void *data, netadr_t to)
{
	int ret;
	struct sockaddr_in addr;
	int net_socket;

	if(to.type == NA_LOOPBACK)
	{
		NET_SendLoopPacket(sock, length, data, to);
		return;
	}
	
	if(to.type == NA_BROADCAST)
	{
		net_socket = ip_sockets[sock];
		if(!net_socket)
			return;
	}
	else if (to.type == NA_IP)
	{
		net_socket = ip_sockets[sock];
		if(!net_socket)
			return;
	}
	else if(to.type == NA_IPX)
	{
		net_socket = ipx_sockets[sock];
		if(!net_socket)
			return;
	}
	else if(to.type == NA_BROADCAST_IPX)
	{
		net_socket = ipx_sockets[sock];
		if(!net_socket)
			return;
	}
	else
		Sys_Error(/*ERR_FATAL,*/ "NET_SendPacket: bad address type");
	
	NetadrToSockadr(&to, &addr);

	ret = sendto(net_socket, data, length, 0, (struct sockaddr *)&addr, sizeof(addr));
	if(ret == -1)
	{
#ifdef _WIN32
		int err = WSAGetLastError();

		// wouldblock is silent
		if(err == WSAEWOULDBLOCK)
			return;
		
		// some PPP links dont allow broadcasts
		if((err == WSAEADDRNOTAVAIL) && ((to.type == NA_BROADCAST) || (to.type == NA_BROADCAST_IPX)))
			return;

#ifndef SWDS
		if(err == WSAEADDRNOTAVAIL)
			Con_DPrintf("NET_SendPacket Warning: %i\n", err);
		else
#endif
			Con_Printf("NET_SendPacket ERROR: %i\n", err);
#else
		if(errno == EWOULDBLOCK)
			return;
		if(errno == ECONNREFUSED)
			return;
		Sys_Printf("NET_SendPacket: %s\n", strerror(errno));
#endif
	}
}

//=============================================================================

/*
int UDP_OpenSocket(int port)
{
	int newsocket;
	struct sockaddr_in address;
#ifdef _WIN32
	unsigned long _true = true;
#else
	qboolean _true = true;
#endif
	int i;

	if((newsocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		Sys_Error("UDP_OpenSocket: socket:", strerror(errno));

#ifdef _WIN32
	if(ioctlsocket(newsocket, FIONBIO, &_true) == -1)
#else
	if(ioctl(newsocket, FIONBIO, (char *)&_true) == -1)
#endif
		Sys_Error("UDP_OpenSocket: ioctl FIONBIO:", strerror(errno));

	address.sin_family = AF_INET;
	//ZOID -- check for interface binding option
	if((i = COM_CheckParm("-ip")) != 0 && i < com_argc)
	{
		address.sin_addr.s_addr = inet_addr(com_argv[i + 1]);
		Con_Printf("Binding to IP Interface Address of %s\n",
		           inet_ntoa(address.sin_addr));
	}
	else
		address.sin_addr.s_addr = INADDR_ANY;

	if(port == PORT_ANY)
		address.sin_port = 0;
	else
		address.sin_port = htons((short)port);
	
	if(bind(newsocket, (void *)&address, sizeof(address)) == -1)
		Sys_Error("UDP_OpenSocket: bind: %s", strerror(errno));

	return newsocket;
}
*/

void NET_GetLocalAddress()
{
	char buff[512]; // TODO: MAXHOSTNAMELEN
	struct sockaddr_in address;
	int namelen;

	gethostname(buff, 512); // TODO: MAXHOSTNAMELEN
	buff[512 - 1] = 0; // TODO: MAXHOSTNAMELEN

	NET_StringToAdr(buff, &net_local_adr);

	namelen = sizeof(address);
	if(getsockname(net_socket, (struct sockaddr *)&address, &namelen) == -1)
		Sys_Error("NET_Init: getsockname:", strerror(errno));
	net_local_adr.port = address.sin_port;

	Con_Printf("IP address %s\n", NET_AdrToString(net_local_adr));
}

//=============================================================================

/*
====================
NET_Socket
====================
*/
int NET_IPSocket (char *net_interface, int port)
{
	int					newsocket;
	struct sockaddr_in	address;
	qboolean			_true = true;
	int					i = 1;
	int					err;

	if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		err = WSAGetLastError();
		if (err != WSAEAFNOSUPPORT)
			Com_Printf ("WARNING: UDP_OpenSocket: socket: %s", NET_ErrorString());
		return 0;
	}

	// make it non-blocking
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		Com_Printf ("WARNING: UDP_OpenSocket: ioctl FIONBIO: %s\n", NET_ErrorString());
		return 0;
	}

	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
	{
		Com_Printf ("WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString());
		return 0;
	}

	if (!net_interface || !net_interface[0] || !stricmp(net_interface, "localhost"))
		address.sin_addr.s_addr = INADDR_ANY;
	else
		NET_StringToSockaddr (net_interface, (struct sockaddr *)&address);

	if (port == PORT_ANY)
		address.sin_port = 0;
	else
		address.sin_port = htons((short)port);

	address.sin_family = AF_INET;

	if( bind (newsocket, (void *)&address, sizeof(address)) == -1)
	{
		Com_Printf ("WARNING: UDP_OpenSocket: bind: %s\n", NET_ErrorString());
		closesocket (newsocket);
		return 0;
	}

	return newsocket;
}

/*
====================
NET_OpenIP
====================
*/
void NET_OpenIP ()
{
	cvar_t	*ip;
	int		port;

	ip = Cvar_Get ("ip", "localhost", CVAR_NOSET);

	if (!ip_sockets[NS_SERVER])
	{
		port = Cvar_Get("ip_hostport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("hostport", "0", CVAR_NOSET)->value;
			if (!port)
			{
				port = Cvar_Get("port", va("%i", PORT_SERVER), CVAR_NOSET)->value;
			}
		}
		ip_sockets[NS_SERVER] = NET_IPSocket (ip->string, port);
		if (!ip_sockets[NS_SERVER] && isDedicated)
			Com_Error (ERR_FATAL, "Couldn't allocate dedicated server IP port");
	}

	// dedicated servers don't need client ports
	if (isDedicated)
		return;

	if (!ip_sockets[NS_CLIENT])
	{
		port = Cvar_Get("ip_clientport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("clientport", va("%i", PORT_CLIENT), CVAR_NOSET)->value;
			if (!port)
				port = PORT_ANY;
		}
		ip_sockets[NS_CLIENT] = NET_IPSocket (ip->string, port);
		if (!ip_sockets[NS_CLIENT])
			ip_sockets[NS_CLIENT] = NET_IPSocket (ip->string, PORT_ANY);
	}
}

/*
====================
IPX_Socket
====================
*/
#ifdef _WIN32
int NET_IPXSocket (int port)
{
	int					newsocket;
	struct sockaddr_ipx	address;
	int					_true = 1;
	int					err;

	if ((newsocket = socket (PF_IPX, SOCK_DGRAM, NSPROTO_IPX)) == -1)
	{
		err = WSAGetLastError();
		if (err != WSAEAFNOSUPPORT)
			Com_Printf ("WARNING: IPX_Socket: socket: %s\n", NET_ErrorString());
		return 0;
	}

	// make it non-blocking
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		Com_Printf ("WARNING: IPX_Socket: ioctl FIONBIO: %s\n", NET_ErrorString());
		return 0;
	}

	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&_true, sizeof(_true)) == -1)
	{
		Com_Printf ("WARNING: IPX_Socket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString());
		return 0;
	}

	address.sa_family = AF_IPX;
	memset (address.sa_netnum, 0, 4);
	memset (address.sa_nodenum, 0, 6);
	if (port == PORT_ANY)
		address.sa_socket = 0;
	else
		address.sa_socket = htons((short)port);

	if( bind (newsocket, (void *)&address, sizeof(address)) == -1)
	{
		Com_Printf ("WARNING: IPX_Socket: bind: %s\n", NET_ErrorString());
		closesocket (newsocket);
		return 0;
	}

	return newsocket;
}
#endif // _WIN32

/*
====================
NET_OpenIPX
====================
*/
void NET_OpenIPX ()
{
#ifdef _WIN32
	int		port;
	int		dedicated;

	dedicated = Cvar_VariableValue ("dedicated");

	if (!ipx_sockets[NS_SERVER])
	{
		port = Cvar_Get("ipx_hostport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("hostport", "0", CVAR_NOSET)->value;
			if (!port)
			{
				port = Cvar_Get("port", va("%i", PORT_SERVER), CVAR_NOSET)->value;
			}
		}
		ipx_sockets[NS_SERVER] = NET_IPXSocket (port);
	}

	// dedicated servers don't need client ports
	if (dedicated)
		return;

	if (!ipx_sockets[NS_CLIENT])
	{
		port = Cvar_Get("ipx_clientport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("clientport", va("%i", PORT_CLIENT), CVAR_NOSET)->value;
			if (!port)
				port = PORT_ANY;
		}
		ipx_sockets[NS_CLIENT] = NET_IPXSocket (port);
		if (!ipx_sockets[NS_CLIENT])
			ipx_sockets[NS_CLIENT] = NET_IPXSocket (PORT_ANY);
	}
#endif // _WIN32
}

/*
====================
NET_Config

A single player game will only use the loopback code
====================
*/
void NET_Config(qboolean multiplayer)
{
	int i;
	static qboolean old_config;

	if (old_config == multiplayer)
		return;

	old_config = multiplayer;

	if (!multiplayer)
	{
		// shut down any existing sockets
		for (i=0 ; i<2 ; i++)
		{
			if (ip_sockets[i])
			{
#ifdef _WIN32
				closesocket (ip_sockets[i]);
#else
				close(ip_sockets[i]);
#endif
				ip_sockets[i] = 0;
			}
			if (ipx_sockets[i])
			{
#ifdef _WIN32
				closesocket (ipx_sockets[i]);
#else
				close(ipx_sockets[i]);
#endif
				ipx_sockets[i] = 0;
			}
		}
	}
	else
	{	// open sockets
		if (! noudp->value) // TODO: win-only check
			NET_OpenIP ();
		if (! noipx->value) // TODO: win-only check
			NET_OpenIPX ();
	}
}

/*
====================
NET_Init
====================
*/
void NET_Init(int port)
{
#ifdef _WIN32
	WORD wVersionRequested;
	int r;

	wVersionRequested = MAKEWORD(1, 1);

	r = WSAStartup(wVersionRequested, &winsockdata);

	if(r)
		Sys_Error("Winsock initialization failed.");
	
	//Con_Printf("Winsock Initialized\n"); // TODO
#endif

	//
	// init the message buffer
	//
	net_message.maxsize = sizeof(net_message_buffer);
	net_message.data = net_message_buffer;

	//
	// determine my name & address
	//
	//NET_GetLocalAddress();

	Con_Printf("UDP Initialized\n");
}

/*
====================
NET_Shutdown
====================
*/
void NET_Shutdown()
{
	NET_Config(false); // close sockets
	
#ifdef _WIN32
	WSACleanup();
#endif
}