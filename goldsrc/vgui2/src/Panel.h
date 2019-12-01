/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2019 BlackPhrase
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

#include "vgui2/IPanel.h"

namespace vgui2
{

// BP: Looks like this is called VPanelWrapper in the original VGUI2
class CPanelManager : public IPanel // TODO: CPanelController?
{
public:
	void Init(VPANEL vguiPanel, IClientPanel *panel) override;

	void SetPos(VPANEL vguiPanel, int x, int y) override;
	void GetPos(VPANEL vguiPanel, int &x, int &y) override;

	void SetSize(VPANEL vguiPanel, int wide, int tall) override;
	void GetSize(VPANEL vguiPanel, int &wide, int &tall) override;

	void SetMinimumSize(VPANEL vguiPanel, int wide, int tall) override;
	void GetMinimumSize(VPANEL vguiPanel, int &wide, int &tall) override;

	void SetZPos(VPANEL vguiPanel, int z) override;
	int GetZPos(VPANEL vguiPanel) override;

	void GetAbsPos(VPANEL vguiPanel, int &x, int &y) override;

	void GetClipRect(VPANEL vguiPanel, int &x0, int &y0, int &x1, int &y1) override;

	void SetInset(VPANEL vguiPanel, int left, int top, int right, int bottom) override;
	void GetInset(VPANEL vguiPanel, int &left, int &top, int &right, int &bottom) override;

	void SetVisible(VPANEL vguiPanel, bool state) override;
	bool IsVisible(VPANEL vguiPanel) override;

	void SetParent(VPANEL vguiPanel, VPANEL newParent) override;

	int GetChildCount(VPANEL vguiPanel) override;

	VPANEL GetChild(VPANEL vguiPanel, int index) override;
	VPANEL GetParent(VPANEL vguiPanel) override;

	void MoveToFront(VPANEL vguiPanel) override;
	void MoveToBack(VPANEL vguiPanel) override;

	bool HasParent(VPANEL vguiPanel, VPANEL potentialParent) override;

	bool IsPopup(VPANEL vguiPanel) override;
	void SetPopup(VPANEL vguiPanel, bool state) override;

	bool Render_GetPopupVisible(VPANEL vguiPanel) override;
	void Render_SetPopupVisible(VPANEL vguiPanel, bool state) override;

	HScheme GetScheme(VPANEL vguiPanel) override;

	bool IsProportional(VPANEL vguiPanel) override;
	bool IsAutoDeleteSet(VPANEL vguiPanel) override;

	void DeletePanel(VPANEL vguiPanel) override;

	void SetKeyBoardInputEnabled(VPANEL vguiPanel, bool state) override;
	void SetMouseInputEnabled(VPANEL vguiPanel, bool state) override;

	bool IsKeyBoardInputEnabled(VPANEL vguiPanel) override;
	bool IsMouseInputEnabled(VPANEL vguiPanel) override;

	void Solve(VPANEL vguiPanel) override;

	const char *GetName(VPANEL vguiPanel) override;
	const char *GetClassName(VPANEL vguiPanel) override;

	void SendMessage(VPANEL vguiPanel, KeyValues *params, VPANEL ifromPanel) override;

	void Think(VPANEL vguiPanel) override;

	void PerformApplySchemeSettings(VPANEL vguiPanel) override;

	void PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce = true) override;
	void Repaint(VPANEL vguiPanel) override;

	VPANEL IsWithinTraverse(VPANEL vguiPanel, int x, int y, bool traversePopups) override;

	void OnChildAdded(VPANEL vguiPanel, VPANEL child) override;
	void OnSizeChanged(VPANEL vguiPanel, int newWide, int newTall) override;

	void InternalFocusChanged(VPANEL vguiPanel, bool lost) override;

	bool RequestInfo(VPANEL vguiPanel, KeyValues *outputData) override;
	void RequestFocus(VPANEL vguiPanel, int direction = 0) override;
	bool RequestFocusPrev(VPANEL vguiPanel, VPANEL existingPanel) override;
	bool RequestFocusNext(VPANEL vguiPanel, VPANEL existingPanel) override;

	VPANEL GetCurrentKeyFocus(VPANEL vguiPanel) override;

	int GetTabPosition(VPANEL vguiPanel) override;

	SurfacePlat *Plat(VPANEL vguiPanel) override;
	void SetPlat(VPANEL vguiPanel, SurfacePlat *Plat) override;

	Panel *GetPanel(VPANEL vguiPanel, const char *destinationModule) override;

	bool IsEnabled(VPANEL vguiPanel) override;
	void SetEnabled(VPANEL vguiPanel, bool state) override;

	void *Client(VPANEL vguiPanel) override;

	const char *GetModuleName(VPANEL vguiPanel) override;
};

}; // namespace vgui2