/// @file
/// @brief net api implementation

#include "quakedef.h"
#include "net_api.h"

void NET_InitNetworking()
{
	// TODO
};

void NET_Status(struct net_status_s *status)
{
	// TODO
};

void NET_SendRequest(int context, int request, int flags, double timeout, struct netadr_s *remote_address, net_api_response_func_t response)
{
	// TODO
};

void NET_CancelRequest(int context)
{
	// TODO
};

void NET_CancelAllRequests()
{
	// TODO
};

net_api_t netapi =
{
	NET_InitNetworking,
	NET_Status,
	
	NET_SendRequest,
	NET_CancelRequest,
	NET_CancelAllRequests,
	
	NET_AdrToString,
	NET_CompareAdr,
	NET_StringToAdr,
	
	Info_ValueForKey,
	Info_RemoveKey,
	Info_SetValueForKey
};