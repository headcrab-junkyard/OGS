/*
 * This file is part of OGS Engine
 * Copyright (C) 2020-2021 BlackPhrase
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

#include "quakedef.h"

void COM_ClearCustomizationList(struct customization_s *pHead, qboolean bClearDecals)
{
	if(!pHead)
		return;
	
	customization_t *pEntry = pHead;
	customization_t *pNextEntry = NULL;
	
	while(pEntry->pNext != NULL)
	{
		// Skip decals if we shouldn't clear them
		if(pEntry->resource.type == t_decal && !bClearDecals)
			continue;
		
		pNextEntry = pEntry->pNext;
		free(pEntry); // TODO
		pEntry = pNextEntry;
	};
	
	free(pHead); // TODO
};

qboolean COM_CreateCustomization(struct customization_s *pListHead, struct resource_s *pResource, int nPlayerNum, int nFlags, struct customization_s **pCustomization, int *nLumps)
{
	if(!pListHead || !pResource)
		return false;
	
	customization_t *pList = pListHead;
	
	// Find a free slot
	while(pList->pNext)
		pList = pList->pNext;
	
	customization_t *pCust = (customization_t*)malloc(sizeof(customization_t)); // TODO
	
	Q_memset(pCust, 0, sizeof(customization_t));
	
	pCust->bInUse = false;
	
	pCust->resource = *pResource; // TODO
	pCust->resource.playernum = nPlayerNum;
	
	pCust->bTranslated = false;
	
	pCust->nUserData1 = 0;
	pCust->nUserData2 = 0;
	
	pCust->pInfo = NULL;
	pCust->pBuffer = NULL;
	
	pCust->pNext = NULL;
	
	pList->pNext = pCust;
	
	pCustomization = pCust; // TODO
	
	if(nLumps)
		*nLumps = 0; // TODO
	
	if(nFlags & FCUST_FROMHPAK)
	{
		// TODO
	};
	
	if(nFlags & FCUST_WIPEDATA)
	{
		// TODO
	};
	
	if(nFlags & FCUST_IGNOREINIT)
	{
		// TODO
	};
	
	return true;
};

int COM_SizeofResourceList(struct resource_s *pList, struct resourceinfo_s *pResInfo)
{
	// TODO: pResInfo purpose?
	
	if(!pList /*|| !pResInfo*/) // TODO
		return 0;
	
	int nCount = 1;
	
	while(pList->pNext != NULL)
	{
		pList = pList->pNext;
		++nCount;
	};
	
	return nCount;
};