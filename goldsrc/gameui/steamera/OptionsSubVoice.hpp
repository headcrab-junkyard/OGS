/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2020 BlackPhrase
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

#include "vgui2/controls/PropertyPage.h"

using IVoiceTweak = struct IVoiceTweak_s;

class CCvarSlider;
class CCvarToggleCheckButton;

class COptionsSubVoice : public vgui2::PropertyPage
{
public:
	COptionsSubVoice(vgui2::Panel *apParent);
	~COptionsSubVoice();
	
	void OnResetData() override;
	void OnApplyChanges() override;
protected:
	void OnThink();
private:
	void OnCommand(const char *asCmd);
	
	void StartMicTest();
	void EndMicTest();
	
	void UseCurrentVoiceParams();
	void ResetVoiceParams();
private:
	IVoiceTweak *mpVoiceTweak{nullptr}; ///< Engine Voice Tweak API
	
	CCvarToggleCheckButton *mpVoiceEnableCheckButton{nullptr};
	
	vgui2::CheckButton *mpMicBoost{nullptr};
	
	vgui2::ImagePanel *mpMicMeter{nullptr};
	vgui2::ImagePanel *mpMicMeter2{nullptr};
	
	vgui2::Button *mpTestMicButton{nullptr};
	
	vgui2::Label *mpMicSliderLabel{nullptr};
	vgui2::Label *mpReceiveSliderLabel{nullptr};
	
	vgui2::Slider *mpMicVolume{nullptr};
	
	CCvarSlider *mpReceiveVolumeSlider{nullptr};
	
	// "Open mic" settings
	vgui2::Label *mpThresholdSliderLabel{nullptr};
	CCvarSlider *mpThresholdVolume{nullptr};
	CCvarToggleCheckButton *mpOpenMicEnableCheckButton{nullptr};
	
	float mfReceiveVolume{0.0f};
	
	int mnMicVolume{0};
	int mnVoiceThreshold{0};
	int mnReceiveSliderValue{0};
	
	bool mbVoiceEnabled{false};
	bool mbMicBoostEnabled{false};
	bool mbOpenMicEnabled{false};
};