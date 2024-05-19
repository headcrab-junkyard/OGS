/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021-2023 BlackPhrase
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
/// @brief voicetweak interface implementation

#include "quakedef.h"
#include "voicetweak.h"

qboolean gbInVoiceTweakMode = false;

float gVoiceTweakControlMap[3] = {0.0f}; // TODO

static int Voice_StartVoiceTweakMode()
{
	// TODO
	
	// TODO: something else?
	gbInVoiceTweakMode = true;
	return true;
};

static void Voice_EndVoiceTweakMode()
{
	// TODO: something else?
	gbInVoiceTweakMode = false;
};

static void Voice_SetControlFloat(VoiceTweakControl aeControl, float afValue)
{
	// TODO: check for voice tweak mode?
	
	// TODO
	gVoiceTweakControlMap[aeControl] = afValue;
};

static float Voice_GetControlFloat(VoiceTweakControl aeControl)
{
	// TODO
	return gVoiceTweakControlMap[aeControl];
};

static int Voice_GetSpeakingVolume()
{
	return 0; // TODO: voice_scale?
};

IVoiceTweak gVoiceTweak =
{
  Voice_StartVoiceTweakMode,
  Voice_EndVoiceTweakMode,

  Voice_SetControlFloat,
  Voice_GetControlFloat,

  Voice_GetSpeakingVolume
};