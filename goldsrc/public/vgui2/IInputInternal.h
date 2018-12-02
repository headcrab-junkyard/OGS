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

// TODO: unused in gs?

#pragma once

#include "IInput.h"
/*
#include "vgui2/VGUI.h"
#include "vgui2/KeyCode.h"
#include "vgui2/MouseCode.h"
*/

const char VGUI_INPUTINTERNAL_INTERFACE_VERSION[] = "VGUI_InputInternal001";

namespace vgui2
{

using HInputContext = int;

const HInputContext DEFAULT_INPUT_CONTEXT = (HInputContext)~0;

struct IInputInternal : public IInput
{
	///
	virtual void RunFrame() = 0;
	
	///
	virtual void UpdateMouseFocus(int x, int y) = 0;
	
	///
	virtual void PanelDeleted(VPANEL nPanel) = 0;
	
	///
	virtual bool InternalCursorMoved(int x, int y) = 0;
	
	///
	virtual bool InternalMousePressed(MouseCode code) = 0;
	
	///
	virtual bool InternalMouseDoublePressed(MouseCode code) = 0;
	
	///
	virtual bool InternalMouseReleased(MouseCode code) = 0;
	
	///
	virtual bool InternalMouseWheeled(int nDelta) = 0;
	
	///
	virtual bool InternalKeyCodePressed(KeyCode code) = 0;
	
	///
	virtual void InternalKeyCodeTyped(KeyCode code) = 0;
	
	///
	virtual void InternalKeyTyped(wchar_t unichar) = 0;
	
	///
	virtual bool InternalKeyCodeReleased(KeyCode code) = 0;
	
	///
	virtual HInputContext CreateInputContext() = 0;
	
	///
	virtual void DestroyInputContext(HInputContext nContext) = 0;
	
	///
	virtual void AssociatePanelWithInputContext(HInputContext nContext, VPANEL nRoot) = 0;
	
	///
	virtual void ActivateInputContext(HInputContext nContext) = 0;
	
	///
	virtual VPANEL GetMouseCapture() = 0;
	
	///
	virtual bool IsChildOfModalPanel(VPANEL nPanel) = 0;
	
	///
	virtual void ResetInputContext(HInputContext nContext) = 0;
};

}; // namespace vgui2