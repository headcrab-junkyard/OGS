/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2018 BlackPhrase
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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include "vgui2/IVGUI.h"

namespace vgui2
{

class CVGui : public IVGui
{
public:
	bool Init(CreateInterfaceFn *factoryList, int numFactories);
	void Shutdown();

	void Start();
	void Stop();

	bool IsRunning();

	void RunFrame();

	void ShutdownMessage(unsigned int shutdownID);

	VPANEL AllocPanel();
	void FreePanel(VPANEL panel);

	void DPrintf(const char *format, ...);
	void DPrintf2(const char *format, ...);

	void SpewAllActivePanelNames();

	HPanel PanelToHandle(VPANEL panel);
	VPANEL HandleToPanel(HPanel index);

	void MarkPanelForDeletion(VPANEL panel);

	void AddTickSignal(VPANEL panel, int intervalMilliseconds = 0);
	void RemoveTickSignal(VPANEL panekl);

	void PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds = 0.0f);

	HContext CreateContext();
	void DestroyContext(HContext context);

	void AssociatePanelWithContext(HContext context, VPANEL pRoot);
	void ActivateContext(HContext context);

	void SetSleep(bool state);
	bool GetShouldVGuiControlSleep();
};

}; // namespace vgui2