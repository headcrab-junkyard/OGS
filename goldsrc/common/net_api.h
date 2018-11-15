/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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

#pragma once

#include "netadr.h"

struct net_response_s;

typedef void (*net_api_response_func_t)(struct net_response_s *response);

typedef struct net_adrlist_s
{
} net_adrlist_t;

typedef struct net_response_s
{
} net_response_t;

typedef struct net_status_s
{
} net_status_t;

typedef struct net_api_s
{
	///
	void (*InitNetworking)();
	
	///
	void (*Status)(struct net_status_s *status);
	
	///
	void (*SendRequest)(int context, int request, int flags, double timeout, struct netadr_s *remote_address, net_api_response_func_t response);
	
	///
	void (*CancelRequest)(int context);
	
	///
	void (*CancelAllRequests)();
	
	///
	char *(*AdrToString)(struct netadr_s *a);
	
	///
	int (*CompareAdr)(struct netadr_s *a, struct netadr_s *b);
	
	///
	int (*StringToAdr)(char *s, struct netadr_s *a);
	
	///
	const char *(*ValueForKey)(const char *s, const char *key);
	
	///
	void (*RemoveKey)(char *s, const char *key);
	
	///
	void (*SetValueForKey)(char *s, const char *key, const char *value, int maxsize);
} net_api_t;

extern net_api_t netapi;