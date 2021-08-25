/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2020-2021 BlackPhrase
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
/// @brief Mouse Details, Part of OptionsDialog

#pragma once

#include <vgui/controls/PropertyPage.h>

class CCvarNegateCheckButton;
//class CKeyToggleCheckButton;
class CCvarToggleCheckButton;
class CCvarSlider;

namespace vgui
{
	class Label;
	class Panel;
};

class COptionsSubMouse : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubMouse, vgui2::PropertyPage);
public:
	COptionsSubMouse(vgui2::Panel *apParent);
	~COptionsSubMouse();
	
	void OnResetData() override;
	void OnApplyChanges() override;
protected:
	void ApplySchemeSettings(vgui2::IScheme *apScheme);
private:
	void UpdateSensitivityLabel();
	void UpdateJoystickPanels();
private:
	MESSAGE_FUNC_PTR(OnControlModified, "ControlModified", panel);
	MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel);
	MESSAGE_FUNC_PTR(OnCheckButtonChecked, "CheckButtonChecked", panel)
	{
		OnControlModified(panel);
	};
private:
	CCvarNegateCheckButton *mpReverseMouseCheckButton;
	
	CCvarToggleCheckButton *mpMouseFilterCheckButton;
	CCvarToggleCheckButton *mpJoystickCheckButton;
	CCvarToggleCheckButton *mpJoystickSouthpawCheckButton;
	CCvarToggleCheckButton *mpQuickInfoCheckButton;
	CCvarToggleCheckButton *mpReverseJoystickCheckButton;
	
	CCvarSlider *mpMouseSensitivitySlider;
	vgui::TextEntry *mpMouseSensitivityLabel;
	
	CCvarSlider *mpJoyYawSensitivitySlider;
	vgui::Label *mpJoyYawSensitivityPreLabel;
	
	CCvarSlider *mpJoyPitchSensitivitySlider;
	vgui::Label *mpJoyPitchSensitivityPreLabel;
};