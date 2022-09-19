/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2020-2022 BlackPhrase
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

#include <tier1/interface.h>

constexpr auto OGS_VGUIWRAP_INTERFACE_VERSION{"OGSVGuiWrap001"};

struct IVGuiWrap : public IBaseInterface
{
	/// Initialize and start the module
	virtual void Startup() = 0;
	
	/// Shutdown the module
	virtual void Shutdown() = 0;
	
	///
	//virtual void CallSurfaceProc() = 0;
	
	/// @return a pointer to the main vgui panel to be used by other panels that requires it (on the client dll module side)
	virtual void *GetPanel() const = 0;
};