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

#define LINK_ENTITY_TO_CLASS(mapClassName, dllClassName) \
	C_EXPORT void mapClassName(entvars_t *self){GetClassPtr((dllClassName*)self);}

template<typename T>
T *GetClassPtr(T *a)
{
	auto pEntVars{reinterpret_cast<entvars_t*>(a)};
	
	if(!pEntVars)
		pEntVars = gpEngine->pfnEntVarsOfEnt(gpEngine->pfnSpawn());
	
	a = reinterpret_cast<T*>(gpEngine->pfnGetEntPrivateData(gpEngine->pfnEntFromEntVars(pEntVars)));
	
	if(!a)
	{
		//
		//a = new (pEntVars) T;
		//a->pev = pEntVars;
		//
		/*gpEngine->pfnEntFromEntVars(pEntVars)->pvPrivateData =*/ auto pPrivateData{gpEngine->pfnAllocEntPrivateData(gpEngine->pfnEntFromEntVars(pEntVars), sizeof(T))};
		a = new (pPrivateData) T(self);
		//
	};
	
	return a;
};