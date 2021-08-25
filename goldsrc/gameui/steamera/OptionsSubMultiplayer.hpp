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
/// @brief Multiplayer options property page

#pragma once

#include <vgui/controls/PropertyPage.h>

class CLabeledCommandComboBox;
class CBitmapImagePanel;

class CCvarToggleCheckButton;
//class CCvarTextEntry;
class CCvarSlider;

class CrosshairImagePanel;
//class CMultiplayerAdvancedDialog;

class AdvancedCrosshairImagePanel;

class COptionsSubMultiplayer : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubMultiplayer, vgui::PropertyPage);
public:
	COptionsSubMultiplayer(vgui::Panel *apParent);
	~COptionsSubMultiplayer();
	
	virtual vgui::Panel *CreateControlByName(const char *asName);
protected:
	virtual void OnResetData();
	
	// Called when the OK/Apply button is pressed
	// Changed data should be written into document
	virtual void OnApplyChanges();
	
	virtual void OnCommand(const char *asCmd);
private:
	void InitModelList(CLabeledCommandComboBox *apComboBox);
	void InitLogoList(CLabeledCommandComboBox *apComboBox);
	
	void RemapModel();
	void RemapLogo();
	void RemapPalette(const char *asFileName, int anTopColor, int anBottomColor);
	
	void ColorForName(const char *asColorName, int &anR, int &anG, int &anB);
	
	void InitCrosshairSizeList(CLabeledCommandComboBox *apComboBox);
	
	void InitCrosshairColorEntries();
	void ApplyCrosshairColorChanges();
	
	// Called when the appearance of the crosshair has changed
	void RedrawCrosshairImage();
	
	void InitAdvCrosshairStyleList(CLabeledCommandComboBox *apComboBox);
	void RedrawAdvCrosshairImage();
private:
	char msModelName[128];
	char msLogoName[128];
	
	// Mod-specific general checkboxes
	vgui::Dar<CCvarToggleCheckButton*> mvCvarToggleCheckButtons;
	
	CCvarSlider *mpPrimaryColorSlider;
	CCvarSlider *mpSecondaryColorSlider;
	
	CCvarToggleCheckButton *mpHighQualityModelCheckBox;
	
	CBitmapImagePanel *mpModelImage;
	CLabeledCommandComboBox *mpModelList;
	
	vgui::ImagePanel *mpLogoImage;
	CLabeledCommandComboBox *mpLogoList;
	
	CLabeledCommandComboBox *mpCrosshairSize;
	
	CCvarToggleCheckButton *mpCrosshairTransclucencyCheckBox;
	
	vgui::ComboBox *mpCrosshairColorComboBox;
	
	CrosshairImagePanel *mpCrosshairImage;
	
	AdvancedCrosshairImagePanel *mpAdvCrosshairImage;
	
	CCvarSlider *mpAdvCrosshairRedSlider;
	CCvarSlider *mpAdvCrosshairGreenSlider;
	CCvarSlider *mpAdvCrosshairBlueSlider;
	CCvarSlider *mpAdvCrosshairScaleSlider;
	
	CLabeledCommandComboBox *mpAdvCrosshairStyle;
	
	// Client download filter
	vgui::ComboBox *mpDownloadFilterComboBox;
	
	int mnTopColor;
	int mnBottomColor;
	
	int mnLogoR;
	int mnLogoG;
	int mnLogoB;
};