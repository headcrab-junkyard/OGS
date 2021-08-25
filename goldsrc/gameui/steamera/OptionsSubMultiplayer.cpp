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

#include "OptionsSubMultiplayer.hpp"

COptionsSubMultiplayer::COptionsSubMultiplayer(vgui::Panel *apParent)
	: BaseClass(apParent, "OptionsSubMultiplayer");
{
};

COptionsSubMultiplayer::~COptionsSubMultiplayer() = default;

vgui::Panel *COptionsSubMultiplayer::CreateControlByName(const char *asName);

void COptionsSubMultiplayer::OnResetData();

void COptionsSubMultiplayer::OnApplyChanges();

void COptionsSubMultiplayer::OnCommand(const char *asCmd);

void COptionsSubMultiplayer::InitModelList(CLabeledCommandComboBox *apComboBox);
void COptionsSubMultiplayer::InitLogoList(CLabeledCommandComboBox *apComboBox);

void COptionsSubMultiplayer::RemapModel();
void COptionsSubMultiplayer::RemapLogo();
void COptionsSubMultiplayer::RemapPalette(const char *asFileName, int anTopColor, int anBottomColor);

void COptionsSubMultiplayer::ColorForName(const char *asColorName, int &anR, int &anG, int &anB);

void COptionsSubMultiplayer::InitCrosshairSizeList(CLabeledCommandComboBox *apComboBox);

void COptionsSubMultiplayer::InitCrosshairColorEntries();
void COptionsSubMultiplayer::ApplyCrosshairColorChanges();

void COptionsSubMultiplayer::RedrawCrosshairImage();

void COptionsSubMultiplayer::InitAdvCrosshairStyleList(CLabeledCommandComboBox *apComboBox);
void COptionsSubMultiplayer::RedrawAdvCrosshairImage();