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

#include "quakedef.h"
#include "icdaudio.h"

// TODO: bgmvolume

void CD_Command_f()
{
	//if(COM_CheckParm("-nocdaudio") || COM_CheckParm("-nosound")) // TODO: <-- ADD THIS (Bugfix; Not present in GS)
	//return;

	// ...

	if(!stricmp(command, "on"))
	{
		g_CDAudio.m_bEnabled = true;
		return;
	};

	// ...
};

class CCDAudio final : public ICDAudio
{
public:
	CCDAudio();
	~CCDAudio();

	void Frame() override;

	void Pause() override;
	void Resume() override;

	void MP3_Init();
	void MP3_InitStream();

	float m_flVolume{ 0.0f }; // private?
	bool m_bEnabled{ false };
};

static CCDAudio g_CDAudio;
ICDAudio *cdaudio = &g_CDAudio;

CCDAudio::CCDAudio() = default;
CCDAudio::~CCDAudio() = default;

void CCDAudio::Frame()
{
	// This is set to TRUE when "cd on" went to the console
	if(!m_bEnabled)
		return;

	if(m_flVolume != bgmvolume.value)
	{
		if(m_flVolume)
		{
			m_flVolume = 0.0f;
			Pause();
		}
		else
		{
			m_flVolume = 1.0f;
			Resume();
		};

		Cvar_DirectSet(&bgmvolume, va("%f", m_flVolume)); // cvar_t bgmvolume is not registered, you should never call Cvar_DirectSet with an unregistered cvar!
	};

	// ...
};

void CCDAudio::MP3_Init(){
	// TODO
};

void CCDAudio::MP3_InitStream(int trackNum, bool looping){
	// TODO
};