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

#include "interface.h"

const char OGS_CLIENTGAME_INTERFACE_VERSION[] = "OGSClientGame001";

struct IClientGame : public IBaseInterface
{
	/// Initialize the client game module
	/// @param afnEngineFactory - engine interface factory
	/// @param pcl_funcs - pointer to client game module exports structure
	/// @return true if success, false if something went wrong (interface query failures, etc)
	virtual bool Init(CreateInterfaceFn afnEngineFactory, cldll_func_t *pcl_funcs) = 0;
	
	/// Shutdown the module
	//virtual void Shutdown() = 0;
};