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

namespace vgui2
{

void CPanel::Init(VPANEL vguiPanel, IClientPanel *panel)
{
};

void CPanel::SetPos(VPANEL vguiPanel, int x, int y)
{
};

void CPanel::GetPos(VPANEL vguiPanel, int &x, int &y)
{
};

void CPanel::SetSize(VPANEL vguiPanel, int wide, int tall)
{
};

void CPanel::GetSize(VPANEL vguiPanel, int &wide, int &tall)
{
};

void CPanel::SetMinimumSize(VPANEL vguiPanel, int wide, int tall)
{
};

void CPanel::GetMinimumSize(VPANEL vguiPanel, int &wide, int &tall)
{
};

void CPanel::SetZPos(VPANEL vguiPanel, int z)
{
};

int CPanel::GetZPos(VPANEL vguiPanel)
{
	return 0;
};

void CPanel::GetAbsPos(VPANEL vguiPanel, int &x, int &y)
{
};

void CPanel::GetClipRect(VPANEL vguiPanel, int &x0, int &y0, int &x1, int &y1)
{
};

void CPanel::SetInset(VPANEL vguiPanel, int left, int top, int right, int bottom)
{
};

void CPanel::GetInset(VPANEL vguiPanel, int &left, int &top, int &right, int &bottom)
{
};

void CPanel::SetVisible(VPANEL vguiPanel, bool state)
{
};

bool CPanel::IsVisible(VPANEL vguiPanel)
{
	return false;
};

void CPanel::SetParent(VPANEL vguiPanel, VPANEL newParent)
{
};

int CPanel::GetChildCount(VPANEL vguiPanel)
{
	return 0;
};

VPANEL CPanel::GetChild(VPANEL vguiPanel, int index)
{
	return 0;
};

VPANEL CPanel::GetParent(VPANEL vguiPanel)
{
	return 0;
};

void CPanel::MoveToFront(VPANEL vguiPanel)
{
};

void CPanel::MoveToBack(VPANEL vguiPanel)
{
};

bool CPanel::HasParent(VPANEL vguiPanel, VPANEL potentialParent)
{
	return false;
};

bool CPanel::IsPopup(VPANEL vguiPanel)
{
	return false;
};

void CPanel::SetPopup(VPANEL vguiPanel, bool state)
{
};

bool CPanel::Render_GetPopupVisible(VPANEL vguiPanel)
{
	return false;
};

void CPanel::Render_SetPopupVisible(VPANEL vguiPanel, bool state)
{
};

HScheme CPanel::GetScheme(VPANEL vguiPanel)
{
	return 0;
};

bool CPanel::IsProportional(VPANEL vguiPanel)
{
	return false;
};

bool CPanel::IsAutoDeleteSet(VPANEL vguiPanel)
{
	return false;
};

void CPanel::DeletePanel(VPANEL vguiPanel)
{
};

void CPanel::SetKeyBoardInputEnabled(VPANEL vguiPanel, bool state)
{
};

void CPanel::SetMouseInputEnabled(VPANEL vguiPanel, bool state)
{
};

bool CPanel::IsKeyBoardInputEnabled(VPANEL vguiPanel)
{
	return false;
};

bool CPanel::IsMouseInputEnabled(VPANEL vguiPanel)
{
	return false;
};

void CPanel::Solve(VPANEL vguiPanel)
{
};

const char *CPanel::GetName(VPANEL vguiPanel)
{
	return "";
};

const char *CPanel::GetClassName(VPANEL vguiPanel)
{
	return "";
};

void CPanel::SendMessage(VPANEL vguiPanel, KeyValues *params, VPANEL ifromPanel)
{
};

void CPanel::Think(VPANEL vguiPanel)
{
};

void CPanel::PerformApplySchemeSettings(VPANEL vguiPanel)
{
};

void CPanel::PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce = true)
{
};

void CPanel::Repaint(VPANEL vguiPanel)
{
};

VPANEL CPanel::IsWithinTraverse(VPANEL vguiPanel, int x, int y, bool traversePopups)
{
	return 0;
};

void CPanel::OnChildAdded(VPANEL vguiPanel, VPANEL child)
{
};

void CPanel::OnSizeChanged(VPANEL vguiPanel, int newWide, int newTall)
{
};

void CPanel::InternalFocusChanged(VPANEL vguiPanel, bool lost)
{
};

bool CPanel::RequestInfo(VPANEL vguiPanel, KeyValues *outputData)
{
	return false;
};

void CPanel::RequestFocus(VPANEL vguiPanel, int direction = 0)
{
};

bool CPanel::RequestFocusPrev(VPANEL vguiPanel, VPANEL existingPanel)
{
	return false;
};

bool CPanel::RequestFocusNext(VPANEL vguiPanel, VPANEL existingPanel)
{
	return false;
};

VPANEL CPanel::GetCurrentKeyFocus(VPANEL vguiPanel)
{
	return 0;
};

int CPanel::GetTabPosition(VPANEL vguiPanel)
{
	return 0;
};

SurfacePlat *CPanel::Plat(VPANEL vguiPanel)
{
	return nullptr;
};

void CPanel::SetPlat(VPANEL vguiPanel, SurfacePlat *Plat)
{
};

Panel *CPanel::GetPanel(VPANEL vguiPanel, const char *destinationModule)
{
	return nullptr;
};

bool CPanel::IsEnabled(VPANEL vguiPanel)
{
	return true;
};

void CPanel::SetEnabled(VPANEL vguiPanel, bool state)
{
};

void *CPanel::Client(VPANEL vguiPanel)
{
	return nullptr;
};

const char *CPanel::GetModuleName(VPANEL vguiPanel)
{
	return "";
};

}; // namespace vgui2