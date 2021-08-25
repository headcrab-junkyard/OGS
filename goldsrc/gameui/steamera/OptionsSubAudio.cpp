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

#include "OptionsSubAudio.hpp"

COptionsSubAudio::COptionsSubAudio(vgui::Panel *apParent)
	: vgui::PropertyPage(apParent, nullptr)
{
	mpSFXSlider = ;
	mpMusicSlider = ;
	
	mpCloseCaptionCombo = ;
	
	mpSoundQualityCombo = ;
	
	mpSpeakerSetupCombo = ;
	
	mpSpokenLanguageCombo = new vgui::ComboBox();
	
	LoadControlSettings();
};

COptionsSubAudio::~COptionsSubAudio() = default;

void COptionsSubAudio::OnResetData()
{
	mbRequireRestart = false;
	
	mpSFXSlider->Reset();
	mpMusicSlider->Reset();
	
	// Reset the combo boxes
	
	
};

void COptionsSubAudio::OnApplyChanges()
{
	mpSFXSlider->ApplyChanges();
	mpMusicSlider->ApplyChanges();
};

void COptionsSubAudio::OnCommand(const char *asCmd)
{
};

bool COptionsSubAudio::RequiresRestart() const
{
	// Nothing in audio required a restart right now
	return false;
};

void COptionsSubAudio::OnControlModified()
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
};

void COptionsSubAudio::RunTestSpeakers()
{
};

void COptionsSubAudio::OpenThirdPartySoundCreditsDialog()
{
};