/// @file

#include "quakedef.h"

netadr_t net_from;
sizebuf_t net_message;

void NET_Init()
{
};

void NET_Shutdown()
{
};

qboolean NET_GetPacket(netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
	return true;
};

void NET_SendPacket(netsrc_t sock, int length, void *data, netadr_t to)
{
};

qboolean NET_CompareAdr(netadr_t a, netadr_t b)
{
	return true;
};

char *NET_AdrToString(netadr_t a)
{
	return NULL;
};

qboolean NET_StringToAdr(char *s, netadr_t *a)
{
	return true;
};