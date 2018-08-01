/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
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

#include "InputInternal.h"

namespace vgui2
{

void CInputInternal::RunFrame()
{
};

void CInputInternal::UpdateMouseFocus(int x, int y)
{
};

void CInputInternal::PanelDeleted(VPANEL panel)
{
};

bool CInputInternal::InternalCursorMoved(int x, int y)
{
	return false;
};

bool CInputInternal::InternalMousePressed(MouseCode code)
{
	return false;
};

bool CInputInternal::InternalMouseDoublePressed(MouseCode code)
{
	return false;
};

bool CInputInternal::InternalMouseReleased(MouseCode code)
{
	return false;
};

bool CInputInternal::InternalMouseWheeled(int delta)
{
	return false;
};

bool CInputInternal::InternalKeyCodePressed(KeyCode code)
{
	return false;
};

void CInputInternal::InternalKeyCodeTyped(KeyCode code)
{
};

void CInputInternal::InternalKeyTyped(wchar_t unichar)
{
};

bool CInputInternal::InternalKeyCodeReleased(KeyCode code)
{
	return false;
};

HInputContext CInputInternal::CreateInputContext()
{
	return 0;
};

void CInputInternal::DestroyInputContext(HInputContext context)
{
};

void CInputInternal::AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot)
{
};

void CInputInternal::ActivateInputContext(HInputContext context)
{
};

VPANEL CInputInternal::GetMouseCapture()
{
	return 0;
};

bool CInputInternal::IsChildOfModalPanel(VPANEL panel)
{
	return false;
};

void CInputInternal::ResetInputContext(HInputContext context)
{
};

}; // namespace vgui2