/*
 * This file is part of OGS Engine
 * Copyright (C) 2019-2020 BlackPhrase
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

#include "engine.h"

#define LINK_ENTITY_TO_CLASS(mapClassName, dllClassName) \
	C_EXPORT void mapClassName(entvars_t *self){GetClassPtr(static_cast<dllClassName*>(self));}

class CBaseEntity;

using entvars_t = struct entvars_s;

extern CGameWorld *gpGameWorld;

template<typename T>
T *GetClassPtr(T *a)
{
	auto pEntVars{reinterpret_cast<entvars_t*>(a)};
	
	if(!pEntVars)
		pEntVars = gpEngine->pfnGetVarsOfEnt(gpEngine->pfnCreateEntity());
	
	a = reinterpret_cast<T*>(gpEngine->pfnPvEntPrivateData(gpEngine->pfnFindEntityByVars(pEntVars)));
	
	if(!a)
	{
		//
		//a = new (pEntVars) T;
		//a->pev = pEntVars;
		//
		/*gpEngine->pfnFindEntityByVars(pEntVars)->pvPrivateData =*/ auto pPrivateData{gpEngine->pfnPvAllocEntPrivateData(gpEngine->pfnFindEntityByVars(pEntVars), sizeof(T))};
		a = new (pPrivateData) T;
		// TODO: use constructors
		a->self = pEntVars;
		a->mpWorld = gpGameWorld;
		//
	};
	
	return a;
};

inline CBaseEntity *ToBaseEntity(edict_t *apEdict)
{
	auto pPrivateData{gpEngine->pfnPvEntPrivateData(apEdict)};
	
	if(!pPrivateData)
		return nullptr;
	
	auto pBaseEntity{reinterpret_cast<CBaseEntity*>(pPrivateData)};
	
	if(!pBaseEntity)
		return nullptr;
	
	return pBaseEntity;
};