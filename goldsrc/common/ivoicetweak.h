/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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
/// @brief voice tweaking interface

#pragma once

/// Access to the voice controls
typedef enum
{
	MicrophoneVolume = 0, ///< Controls the microphone volume value (Values: 0-1)
	OtherSpeakerScale,    ///< Scales how loud other players are (Values: 0-1)
	MicBoost              ///< # db gain to voice input (Values: 0-20? TODO)
} VoiceTweakControl;

typedef struct IVoiceTweak_s
{
	/// Turn the voice tweak mode on
	/// While in this mode, the user's voice is echoed back without sending to the server
	/// @return 0 on error
	int (*StartVoiceTweakMode)();
	
	/// Turn the voice tweak mode off
	void (*EndVoiceTweakMode)();
	
	/// Set control values
	void (*SetControlFloat)(VoiceTweakControl aeControl, float afValue);
	
	/// Get control values
	float (*GetControlFloat)(VoiceTweakControl aeControl);
	
	///
	int (*GetSpeakingVolume)();
} IVoiceTweak;