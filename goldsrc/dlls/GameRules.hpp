/*
 * This file is part of OGS Engine
 * Copyright (C) 2019 BlackPhrase
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

#include "IGameRules.hpp"

class CGameRules : public IGameRules
{
public:
	CGameRules();
	~CGameRules();
	
	bool ShouldCollide(const CBaseEntity &lhs, const CBaseEntity &rhs) const override;
	
	bool HandleClientConnect(CBaseEntity *apEntity, const char *asName, const char *asAdr, char sRejectReason[128]) override;
	void HandleClientDisconnect(CBaseEntity *apEntity) override;
};