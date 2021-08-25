/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021 BlackPhrase
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

#include <vgui_controls/Frame.h>

class CContentControlDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CContentControlDialog, vgui::Frame);
public:
	CContentControlDialog(vgui::Panel *apParent);
	~CContentControlDialog();
	
	/*virtual*/ void OnCommand(const char *asCmd) override;
	/*virtual*/ void OnClose() override;
	
	/*virtual*/ void Activate() override;
	
	void ResetPassword();
	void ApplyPassword();
	
	bool IsPasswordEnabled() const {return msGorePW[0] != '\0';}
	bool IsPasswordEnabledInDialog() const {return IsPasswordEnabled();}
protected:
	void WriteToken(const char *asToken);
	
	void Explain(const char *asMsg, ...);
	
	//bool CheckPassword(const char *asOldPassword, const char *asNewPassword, bool abEnableContentControl);
	
	bool EnablePassword(const char *asNewPassword);
	bool DisablePassword(const char *asOldPassword);
	
	void HashPassword(const char *asNewPassword, char *asHashBuf, int anMaxLen);
	
	void UpdateContentControlStatus();
protected:
	static constexpr auto MAX_GORE_PW{64};
	
	char msGorePW[MAX_GORE_PW]{};
	
	vgui::Label *mpStatusLabel{nullptr};
	
	vgui::Label *mpPasswordLabel{nullptr};
	vgui::Label *mpPassword2Label{nullptr};
	
	vgui::Label *mpExplainLabel{nullptr};
	
	vgui::TextEntry *mpPassword{nullptr};
	vgui::TextEntry *mpPassword2{nullptr};
	
	vgui::Button *mpOKButton{nullptr};
	
	bool mbDefaultPassword{false};
};