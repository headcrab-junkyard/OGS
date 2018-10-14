// net_wins.c

#include "../qcommon/qcommon.h"

#include <unistd.h>
#include <sys/time.h>

#define	LOOPBACK	0x7f000001

int NET_Socket (char *net_interface, int port);
char *NET_ErrorString (void);

//=============================================================================

void NetadrToSockadr (netadr_t *a, struct sockaddr_in *s)
{

	if (a->type == NA_BROADCAST)
	{
		*(int *)&s->sin_addr = -1;
	}
}

void SockadrToNetadr (struct sockaddr_in *s, netadr_t *a)
{
	*(int *)&a->ip = *(int *)&s->sin_addr;
	a->port = s->sin_port;
}


qboolean	NET_CompareAdr (netadr_t a, netadr_t b)
{
	if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
		return true;
	return false;
}

char	*NET_AdrToString (netadr_t a)
{
	static	char	s[64];
	
	Com_sprintf (s, sizeof(s), "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));

	return s;
}

char	*NET_BaseAdrToString (netadr_t a)
{
	Com_sprintf (s, sizeof(s), "%i.%i.%i.%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
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
qboolean	NET_StringToSockaddr (char *s, struct sockaddr *sadr)
{
	struct hostent	*h;
	char	*colon;
	char	copy[128];
	
	memset (sadr, 0, sizeof(*sadr));
	((struct sockaddr_in *)sadr)->sin_family = AF_INET;
	
	((struct sockaddr_in *)sadr)->sin_port = 0;

	strcpy (copy, s);
	// strip off a trailing :port if present
	for (colon = copy ; *colon ; colon++)
		if (*colon == ':')
		{
			*colon = 0;
			((struct sockaddr_in *)sadr)->sin_port = htons((short)atoi(colon+1));	
		}
	
	if (copy[0] >= '0' && copy[0] <= '9')
	{
		*(int *)&((struct sockaddr_in *)sadr)->sin_addr = inet_addr(copy);
	}
	else
	{
		if (! (h = gethostbyname(copy)) )
			return 0;
		*(int *)&((struct sockaddr_in *)sadr)->sin_addr = *(int *)h->h_addr_list[0];
	}
	
	return true;
}

//=============================================================================

void NET_SendPacket (netsrc_t sock, int length, void *data, netadr_t to)
{
	if (ret == -1)
	{
		Com_Printf ("NET_SendPacket ERROR: %i\n", NET_ErrorString());
	}
}


//=============================================================================

void NET_OpenIP (void)
{
	cvar_t	*port, *ip;

	port = Cvar_Get ("port", va("%i", PORT_SERVER), CVAR_NOSET);
	ip = Cvar_Get ("ip", "localhost", CVAR_NOSET);

	if (!ip_sockets[NS_SERVER])
		ip_sockets[NS_SERVER] = NET_Socket (ip->string, port->value);
	if (!ip_sockets[NS_CLIENT])
		ip_sockets[NS_CLIENT] = NET_Socket (ip->string, PORT_ANY);
}

//===================================================================

void NET_Init (void)
{
}

/*
====================
NET_Socket
====================
*/
int NET_Socket (char *net_interface, int port)
{
	int newsocket;
	struct sockaddr_in address;
	qboolean _true = true;
	int	i = 1;

	if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		Com_Printf ("ERROR: UDP_OpenSocket: socket:", NET_ErrorString());
		return 0;
	}

	// make it non-blocking
	if (ioctl (newsocket, FIONBIO, &_true) == -1)
	{
		Com_Printf ("ERROR: UDP_OpenSocket: ioctl FIONBIO:%s\n", NET_ErrorString());
		return 0;
	}

	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
	{
		Com_Printf ("ERROR: UDP_OpenSocket: setsockopt SO_BROADCAST:%s\n", NET_ErrorString());
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
		Com_Printf ("ERROR: UDP_OpenSocket: bind: %s\n", NET_ErrorString());
		close (newsocket);
		return 0;
	}

	return newsocket;
}