/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2022 BlackPhrase
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

#include "quakedef.h"
#include "icdaudio.h"

//extern HWND mainwindow;
extern cvar_t bgmvolume;

void CD_Command_f()
{
	//if(COM_CheckParm("-nocdaudio") || COM_CheckParm("-nosound")) // TODO: <-- ADD THIS (Bugfix; Not present in GS)
	//return;

	// ...

	if(!stricmp(command, "on"))
	{
		g_CDAudio.mbEnabled = true;
		return;
	};

	// ...
};

/*
static void CD_f()
{
	char *command;
	int ret;
	int n;
	int startAddress;

	if(Cmd_Argc() < 2)
		return;

	command = Cmd_Argv(1);

	if(Q_strcasecmp(command, "on") == 0)
	{
		gpCDAudio->SetActive(true);
		return;
	};

	if(Q_strcasecmp(command, "off") == 0)
	{
		if(playing)
			CDAudio_Stop();
		gpCDAudio->SetActive(false);
		return;
	};

	if(Q_strcasecmp(command, "reset") == 0)
	{
		gpCDAudio->SetActive(true);
		if(playing)
			CDAudio_Stop();
		for(n = 0; n < 100; n++)
			remap[n] = n;
		CDAudio_GetAudioDiskInfo();
		return;
	};

	if(Q_strcasecmp(command, "remap") == 0)
	{
		ret = Cmd_Argc() - 2;
		if(ret <= 0)
		{
			for(n = 1; n < 100; n++)
				if(remap[n] != n)
					Con_Printf("  %u -> %u\n", n, remap[n]);
			return;
		};
		for(n = 1; n <= ret; n++)
			remap[n] = Q_atoi(Cmd_Argv(n + 1));
		return;
	};

	if(Q_strcasecmp(command, "close") == 0)
	{
		return;
	};

	if(!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if(!cdValid)
		{
			Con_Printf("No CD in player.\n");
			return;
		};
	};

	if(Q_strcasecmp(command, "play") == 0)
	{
		CDAudio_Play((byte)Q_atoi(Cmd_Argv(2)), false);
		return;
	};

	if(Q_strcasecmp(command, "loop") == 0)
	{
		CDAudio_Play((byte)Q_atoi(Cmd_Argv(2)), true);
		return;
	};

	if(Q_strcasecmp(command, "stop") == 0)
	{
		CDAudio_Stop();
		return;
	};

	if(Q_strcasecmp(command, "pause") == 0)
	{
		CDAudio_Pause();
		return;
	};

	if(Q_strcasecmp(command, "resume") == 0)
	{
		CDAudio_Resume();
		return;
	};

	if(Q_strcasecmp(command, "eject") == 0)
	{
		if(playing)
			CDAudio_Stop();
		cdValid = false;
		return;
	};

	if(Q_strcasecmp(command, "info") == 0)
	{
		Con_Printf("%u tracks\n", maxTrack);
		if(playing)
			Con_Printf("Currently %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		else if(wasPlaying)
			Con_Printf("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		Con_Printf("Volume is %f\n", cdaudio->m_flVolume);
		return;
	};
};
*/

class CCDAudio final : public ICDAudio
{
public:
	CCDAudio();
	~CCDAudio();
	
	int Init() override; // TODO: remove?
	void Shutdown() override; // TODO: remove?
	
	void Frame() override;
	
	void Pause() override;
	void Resume() override;
	
	void MP3_Init();
	void MP3_InitStream();
private:
	void Play(byte track, qboolean looping) /*override*/;
	void Stop() /*override*/;

	int GetAudioDiskInfo();
public: //private:
	float m_flVolume{0.0f}; // private?
	bool mbEnabled{false}; // TODO: m_bEnabled
private:
	bool mbInitialized{false};
	
	byte remap[100]{};
	byte cdrom{0};
	byte playTrack{0};
	byte maxTrack{0};
	
	bool cdValid{false};
	bool playing{false;
	bool wasPlaying{false};
	bool playLooping{false};
};

//static CCDAudio g_CDAudio;
//ICDAudio *cdaudio = &g_CDAudio;

// TODO: temp
ICDAudio *CreateCDAudio()
{
	return new CCDAudio();
};

CCDAudio::CCDAudio() = default;
CCDAudio::~CCDAudio() = default;

int CCDAudio::Init()
{
	//DWORD dwReturn;
	//MCI_OPEN_PARMS mciOpenParms;
	//MCI_SET_PARMS mciSetParms;

	if(cls.state == ca_dedicated)
		return -1;

	if(COM_CheckParm("-nocdaudio"))
		return -1;

	/*
	mciOpenParms.lpstrDeviceType = "cdaudio";
	if(dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE, (DWORD)(LPVOID)&mciOpenParms))
	{
		Con_Printf("CDAudio_Init: MCI_OPEN failed (%i)\n", dwReturn);
		return -1;
	};
	wDeviceID = mciOpenParms.wDeviceID;

	// Set the time format to track/minute/second/frame (TMSF).
	mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;

	if(dwReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&mciSetParms))
	{
		Con_Printf("MCI_SET_TIME_FORMAT failed (%i)\n", dwReturn);
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, (DWORD)nullptr);
		return -1;
	};
	*/

	for(int n = 0; n < 100; n++)
		remap[n] = n;

	mbInitialized = true;
	mbEnabled = true;

	//if(GetAudioDiskInfo())
	{
		//Con_Printf("CDAudio_Init: No CD in player.\n");
		//cdValid = false;
		//mbEnabled = false; // TODO: qw
	};

	//Cmd_AddCommand("cd", CD_f); // TODO

	//Con_Printf("CD Audio Initialized\n");
	
	MP3_Init();
	
	return 0;
};

void CCDAudio::Shutdown()
{
	if(!mbInitialized)
		return;

	Stop();

	//if(mciSendCommand(wDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD)nullptr))
		//Con_DPrintf("CDAudio_Shutdown: MCI_CLOSE failed\n");
};

void CCDAudio::Frame()
{
	// This is set to TRUE when "cd on" went to the console
	if(!mbEnabled)
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
		
		Cvar_SetValue("bgmvolume", m_flVolume);
		// TODO
		//Cvar_DirectSet(&bgmvolume, va("%f", m_flVolume)); // cvar_t bgmvolume is not registered, you should never call Cvar_DirectSet with an unregistered cvar!
	};
};

void CCDAudio::Pause()
{
	if(!mbEnabled)
		return;

	if(!playing)
		return;
	
/*
	DWORD dwReturn;
	MCI_GENERIC_PARMS mciGenericParms;
	
	mciGenericParms.dwCallback = (DWORD)mainwindow;
	if(dwReturn = mciSendCommand(wDeviceID, MCI_PAUSE, 0, (DWORD)(LPVOID)&mciGenericParms))
		Con_DPrintf("MCI_PAUSE failed (%i)", dwReturn);
*/
	
	wasPlaying = playing;
	playing = false;
};

void CCDAudio::Resume()
{
	if(!mbEnabled)
		return;

	if(!cdValid)
		return;

	if(!wasPlaying)
		return;
	
/*
	DWORD dwReturn;
	MCI_PLAY_PARMS mciPlayParms;
	
	mciPlayParms.dwFrom = MCI_MAKE_TMSF(playTrack, 0, 0, 0);
	mciPlayParms.dwTo = MCI_MAKE_TMSF(playTrack + 1, 0, 0, 0);
	mciPlayParms.dwCallback = (DWORD)mainwindow;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID)&mciPlayParms);
	if(dwReturn)
	{
		Con_DPrintf("CDAudio: MCI_PLAY failed (%i)\n", dwReturn);
		return;
	};
*/
	
	playing = true;
};

void CCDAudio::MP3_Init()
{
	// TODO
};

// TODO: is it renamed Play method?
void CCDAudio::MP3_InitStream(int trackNum, bool looping)
{
	// TODO
};

void CCDAudio::Play(byte track, qboolean looping)
{
	if(!mbEnabled)
		return;

	if(!cdValid)
	{
		GetAudioDiskInfo();
		if(!cdValid)
			return;
	};

	track = remap[track];

	if(track < 1 || track > maxTrack)
	{
		Con_DPrintf("CDAudio: Bad track number %u.\n", track);
		return;
	};

/*
	DWORD dwReturn;
	MCI_STATUS_PARMS mciStatusParms;
	
	// don't try to play a non-audio track
	mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
	mciStatusParms.dwTrack = track;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD)(LPVOID)&mciStatusParms);
	if(dwReturn)
	{
		Con_DPrintf("MCI_STATUS failed (%i)\n", dwReturn);
		return;
	};

	if(mciStatusParms.dwReturn != MCI_CDA_TRACK_AUDIO)
	{
		Con_Printf("CDAudio: track %i is not audio\n", track);
		return;
	};

	
	// get the length of the track to be played
	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
	mciStatusParms.dwTrack = track;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD)(LPVOID)&mciStatusParms);
	if(dwReturn)
	{
		Con_DPrintf("MCI_STATUS failed (%i)\n", dwReturn);
		return;
	};
*/

	if(playing)
	{
		if(playTrack == track)
			return;
		Stop();
	};

/*
	MCI_PLAY_PARMS mciPlayParms;
	
	mciPlayParms.dwFrom = MCI_MAKE_TMSF(track, 0, 0, 0);
	mciPlayParms.dwTo = (mciStatusParms.dwReturn << 8) | track;
	mciPlayParms.dwCallback = (DWORD)mainwindow;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM | MCI_TO, (DWORD)(LPVOID)&mciPlayParms);
	if(dwReturn)
	{
		Con_DPrintf("CDAudio: MCI_PLAY failed (%i)\n", dwReturn);
		return;
	};
*/

	playLooping = looping;
	playTrack = track;
	playing = true;

	if(m_flVolume == 0.0f)
		Pause();
};

void CCDAudio::Stop()
{
	if(!mbEnabled)
		return;

	if(!playing)
		return;
	
/*
	DWORD dwReturn;
	if(dwReturn = mciSendCommand(wDeviceID, MCI_STOP, 0, (DWORD)nullptr))
		Con_DPrintf("MCI_STOP failed (%i)", dwReturn);
*/
	
	wasPlaying = false;
	playing = false;
};

int CCDAudio::GetAudioDiskInfo()
{
	cdValid = false;
	
/*
	DWORD dwReturn;
	MCI_STATUS_PARMS mciStatusParms;
	
	mciStatusParms.dwItem = MCI_STATUS_READY;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD)(LPVOID)&mciStatusParms);
	if(dwReturn)
	{
		Con_DPrintf("CDAudio: drive ready test - get status failed\n");
		return -1;
	};

	if(!mciStatusParms.dwReturn)
	{
		Con_DPrintf("CDAudio: drive not ready\n");
		return -1;
	};

	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD)(LPVOID)&mciStatusParms);
	if(dwReturn)
	{
		Con_DPrintf("CDAudio: get tracks - status failed\n");
		return -1;
	};

	if(mciStatusParms.dwReturn < 1)
	{
		Con_DPrintf("CDAudio: no music tracks\n");
		return -1;
	};
	
	maxTrack = mciStatusParms.dwReturn;
*/
	
	cdValid = true;

	return 0;
};