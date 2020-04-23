/*
 * This file is part of OGS Engine
 * Copyright (C) 2020 BlackPhrase
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

#include "vgui2/controls/CheckButton.h"

class CCvarToggleCheckButton : public vgui2::CheckButton
{
	DECLARE_CLASS_SIMPLE(CCvarToggleCheckButton, vgui2::CheckButton);
public:
	CCvarToggleCheckButton(vgui2::Panel *apParent, const char *asPanelName, const char *asText, const char *asCvarName);
	~CCvarToggleCheckButton();
	
	void SetSelected(bool abState) override;
	
	void Paint() override;
	
	void Reset() override;
	
	void ApplyChanges() override;
	void ApplySettings(KeyValues *apResourceData) override;
	
	bool HasBeenModified() override;
private:
	MESSAGE_FUNC(OnButtonChecked, "CheckButtonChecked");
	
	const char *msCvarName{""};
	
	bool mbStartValue{false};
};