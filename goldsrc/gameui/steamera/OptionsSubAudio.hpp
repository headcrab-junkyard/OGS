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

class CCvarSlider;
class CLabeledCommandComboBox;

class COptionsSubAudio : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubAudio, vgui2::PropertyPage);
public:
	COptionsSubAudio(vgui2::Panel *apParent);
	~COptionsSubAudio();
private:
	MESSAGE_FUNC();
	MESSAGE_FUNC();
	MESSAGE_FUNC();
	
	vgui2::ComboBox *mpSpeakerSetupCombo{nullptr};
	vgui2::ComboBox *mpSoundQualityCombo{nullptr};
	vgui2::ComboBox *mpCloseCaptionCombo{nullptr};
	
	CCvarSlider *mpSFXSlider{nullptr};
	CCvarSlider *mpMusicSlider{nullptr};
	
	bool mbRequireRestart{false};
};