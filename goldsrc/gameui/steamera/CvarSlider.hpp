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

#include "vgui2/controls/Slider.h"

class CCvarSlider : public vgui2::Slider
{
	DECLARE_CLASS_SIMPLE(CCvarSlider, vgui2::Slider);
public:
	CCvarSlider(vgui2::Panel *apParent, const char *asPanelName);
	CCvarSlider(vgui2::Panel *apParent, const char *asPanelName, const char *asCaption,
	float afMinValue, float afMaxValue, const char *asCvarName, bool abAllowOutOfRange = false);
	~CCvarSlider();
private:
	char msCvarName[64]{};
	
	float mfMinValue{0.0f};
	float mfMaxValue{0.0f};
	
	float mfCurrentValue{0.0f};
	
	bool mbAllowOutOfRange{false};
};