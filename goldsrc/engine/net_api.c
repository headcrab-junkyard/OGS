/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021 BlackPhrase
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
/// @brief net api implementation

#include "quakedef.h"
#include "net_api.h"

void NET_InitNetworking(){
	// TODO
};

void NET_Status(struct net_status_s *status){
	// TODO
};

void NET_SendRequest(int context, int request, int flags, double timeout, struct netadr_s *remote_address, net_api_response_func_t response){
	// TODO
};

void NET_CancelRequest(int context){
	// TODO
};

void NET_CancelAllRequests(){
	// TODO
};

net_api_t netapi =
{
  NET_InitNetworking,
  NET_Status,

  NET_SendRequest,
  NET_CancelRequest,
  NET_CancelAllRequests,

  NULL, //NET_AdrToString, // TODO
  NULL, //NET_CompareAdr, // TODO
  NET_StringToAdr,

  NULL, //Info_ValueForKey, // TODO
  Info_RemoveKey,
  Info_SetValueForKey
};