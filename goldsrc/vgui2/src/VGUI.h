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
	bool Init(CreateInterfaceFn *factoryList, int numFactories) override;
	void Shutdown() override;

	void Start() override;
	void Stop() override;

	bool IsRunning() override;

	void RunFrame() override;

	void ShutdownMessage(unsigned int shutdownID) override;

	VPANEL AllocPanel() override;
	void FreePanel(VPANEL panel) override;

	void DPrintf(const char *format, ...) override;
	void DPrintf2(const char *format, ...) override;

	void SpewAllActivePanelNames() override;

	HPanel PanelToHandle(VPANEL panel) override;
	VPANEL HandleToPanel(HPanel index) override;

	void MarkPanelForDeletion(VPANEL panel) override;

	void AddTickSignal(VPANEL panel, int intervalMilliseconds = 0) override;
	void RemoveTickSignal(VPANEL panekl) override;

	void PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds = 0.0f) override;

	HContext CreateContext() override;
	void DestroyContext(HContext context) override;

	void AssociatePanelWithContext(HContext context, VPANEL pRoot) override;
	void ActivateContext(HContext context) override;

	void SetSleep(bool state) override;
	bool GetShouldVGuiControlSleep() override;
private:
	void DispatchMessages();
	
	bool mbRunning{false};
	bool mbSleeping{false};
};

}; // namespace vgui2