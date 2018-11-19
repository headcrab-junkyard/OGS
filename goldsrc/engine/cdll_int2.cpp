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
#include "cl_dll/IClientGame.hpp"

extern void *gpClientDLL;

extern cldll_func_t cl_funcs;

extern "C" qboolean LoadClientDLLClientGame()
{
	auto fnClientDLLFactory{Sys_GetFactory(gpClientDLL)};
	
	if(!fnClientDLLFactory)
		return false;
	
	auto pClientGame{(IClientGame*)fnClientDLLFactory(OGS_CLIENTGAME_INTERFACE_VERSION, nullptr)};
	
	if(!pClientGame)
		return false;
	
	Q_memset(&cl_funcs, 0, sizeof(cl_funcs));
	
	if(!pClientGame->Init(Sys_GetFactoryThis(), &cl_funcs))
		return false;
	
	return true;
};