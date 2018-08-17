/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief voicetweak interface implementation

#include "quakedef.h"
#include "ivoicetweak.h"

qboolean gbInVoiceTweakMode{ false };

float gVoiceTweakControlMap[3]{ 0.0f }; // TODO

static int Voice_StartVoiceTweakMode()
{
	// TODO
	gbInVoiceTweakMode = true;
	return 0;
};

static void Voice_EndVoiceTweakMode()
{
	// TODO
	gbInVoiceTweakMode = false;
};

static void Voice_SetControlFloat(VoiceTweakControl aeControl, float afValue)
{
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
	return 0; // TODO
};

IVoiceTweak gVoiceTweak =
{
  Voice_StartVoiceTweakMode,
  Voice_EndVoiceTweakMode,

  Voice_SetControlFloat,
  Voice_GetControlFloat,

  Voice_GetSpeakingVolume
};