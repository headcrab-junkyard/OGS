/*
 * This file is part of OGS Engine
 * Copyright (C) 2017-2018 BlackPhrase
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
/// @brief dedicated launcher exports interface

#pragma once

#include "public/interface.h"

const char VENGINE_DEDICATEDEXPORTS_API_VERSION[] = "VENGINE_DEDICATEDEXPORTS_API_VERSION001";

struct IDedicatedExports : public IBaseInterface
{
	///
	virtual void Sys_Printf(const char *text) = 0;
};