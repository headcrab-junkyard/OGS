/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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
/// @brief null ICDAudio interface implementation

#include "quakedef.h"
#include "icdaudio.h"

class CCDAudio /*Null*/ final : public ICDAudio
{
public:
	CCDAudio();
	~CCDAudio();

	int Init() override;
	void Shutdown() override;

	void Frame() override;

	void Pause() override;
	void Resume() override;

private:
	void Play(byte track, qboolean looping); // override
	void Stop();                             // override
};

int CCDAudio::Init()
{
	return 0;
};

void CCDAudio::Shutdown(){};

void CCDAudio::Frame() // TODO: was Update
{};

void CCDAudio::Play(byte track, qboolean looping){};

void CCDAudio::Stop(){};

void CCDAudio::Pause(){};

void CCDAudio::Resume(){};

#ifdef _WIN32
#include "winquake.h"
extern "C" LONG CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
};
#endif

// TODO: temp
ICDAudio *CreateCDAudio()
{
	return new CCDAudio();
};