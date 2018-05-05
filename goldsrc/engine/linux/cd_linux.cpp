/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2018 Headcrab-Garage

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/// @file
/// @brief linux implementation of ICDAudio interface

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <linux/cdrom.h>

#include "quakedef.h"
#include "icdaudio.h"

static qboolean cdValid = false;
static qboolean	playing = false;
static qboolean	wasPlaying = false;

static qboolean playLooping = false;
static float	cdvolume;
static byte 	remap[100];
static byte		playTrack;
static byte		maxTrack;

static int cdfile = -1;
static char cd_dev[64] = "/dev/cdrom";

class CCDAudio final : public ICDAudio
{
public:
	CCDAudio();
	~CCDAudio();
	
	int Init() override;
	void Shutdown() override;
	
	void Frame() override; // Update in Quake
	
	void Pause() override;
	void Resume() override;
private:
	void Play(byte track, qboolean looping); // override
	void Stop(); // override
	
	void Eject();
	void CloseDoor();
	
	int GetAudioDiskInfo();
	
	bool mbInitialized{false};
	bool mbEnabled{true};
};

int CCDAudio::Init()
{
	int i;

	if (cls.state == ca_dedicated)
		return -1;

	if (COM_CheckParm("-nocdaudio"))
		return -1;

	if ((i = COM_CheckParm("-cddev")) != 0 && i < com_argc - 1)
	{
		strncpy(cd_dev, com_argv[i + 1], sizeof(cd_dev));
		cd_dev[sizeof(cd_dev) - 1] = 0;
	};

	if ((cdfile = open(cd_dev, O_RDONLY)) == -1)
	{
		Con_Printf("CDAudio_Init: open of \"%s\" failed (%i)\n", cd_dev, errno);
		cdfile = -1;
		return -1;
	};

	for (i = 0; i < 100; i++)
		remap[i] = i;
	mbInitialized = true;
	mbEnabled = true;

	if (GetAudioDiskInfo())
	{
		Con_Printf("CDAudio_Init: No CD in player.\n");
		cdValid = false;
	};

	Cmd_AddCommand ("cd", CD_f);

	Con_Printf("CD Audio Initialized\n");

	return 0;
};

void CCDAudio::Shutdown()
{
	if(!mbInitialized)
		return;
	Stop();
	close(cdfile);
	cdfile = -1;
};

void CCDAudio::Frame()
{
	struct cdrom_subchnl subchnl;
	static time_t lastchk;

	if (!mbEnabled)
		return;

	if (bgmvolume.value != cdvolume)
	{
		if (cdvolume)
		{
			Cvar_SetValue ("bgmvolume", 0.0);
			cdvolume = bgmvolume.value;
			Pause ();
		}
		else
		{
			Cvar_SetValue ("bgmvolume", 1.0);
			cdvolume = bgmvolume.value;
			Resume ();
		};
	};

	if (playing && lastchk < time(nullptr))
	{
		lastchk = time(nullptr) + 2; //two seconds between chks
		subchnl.cdsc_format = CDROM_MSF;
		if (ioctl(cdfile, CDROMSUBCHNL, &subchnl) == -1 )
		{
			Con_DPrintf("ioctl cdromsubchnl failed\n");
			playing = false;
			return;
		};
		if (subchnl.cdsc_audiostatus != CDROM_AUDIO_PLAY && subchnl.cdsc_audiostatus != CDROM_AUDIO_PAUSED)
		{
			playing = false;
			if (playLooping)
				Play(playTrack, true);
		};
	};
};

void CCDAudio::Play(byte track, qboolean looping)
{
	struct cdrom_tocentry entry;
	struct cdrom_ti ti;

	if (cdfile == -1 || !mbEnabled)
		return;
	
	if (!cdValid)
	{
		GetAudioDiskInfo();
		if (!cdValid)
			return;
	};

	track = remap[track];

	if (track < 1 || track > maxTrack)
	{
		Con_DPrintf("CDAudio: Bad track number %u.\n", track);
		return;
	};

	// don't try to play a non-audio track
	entry.cdte_track = track;
	entry.cdte_format = CDROM_MSF;
    if ( ioctl(cdfile, CDROMREADTOCENTRY, &entry) == -1 )
	{
		Con_DPrintf("ioctl cdromreadtocentry failed\n");
		return;
	};
	
	if (entry.cdte_ctrl == CDROM_DATA_TRACK)
	{
		Con_Printf("CDAudio: track %i is not audio\n", track);
		return;
	};

	if (playing)
	{
		if (playTrack == track)
			return;
		Stop();
	};

	ti.cdti_trk0 = track;
	ti.cdti_trk1 = track;
	ti.cdti_ind0 = 1;
	ti.cdti_ind1 = 99;

	if ( ioctl(cdfile, CDROMPLAYTRKIND, &ti) == -1 ) 
    {
		Con_DPrintf("ioctl cdromplaytrkind failed\n");
		return;
    };

	if ( ioctl(cdfile, CDROMRESUME) == -1 ) 
		Con_DPrintf("ioctl cdromresume failed\n");

	playLooping = looping;
	playTrack = track;
	playing = true;

	if (cdvolume == 0.0)
		Pause ();
};

void CCDAudio::Stop()
{
	if (cdfile == -1 || !mbEnabled)
		return;
	
	if (!playing)
		return;

	if ( ioctl(cdfile, CDROMSTOP) == -1 )
		Con_DPrintf("ioctl cdromstop failed (%d)\n", errno);

	wasPlaying = false;
	playing = false;
};

void CCDAudio::Pause()
{
	if (cdfile == -1 || !mbEnabled)
		return;

	if (!playing)
		return;

	if ( ioctl(cdfile, CDROMPAUSE) == -1 ) 
		Con_DPrintf("ioctl cdrompause failed\n");

	wasPlaying = playing;
	playing = false;
};

void CCDAudio::Resume()
{
	if (cdfile == -1 || !mbEnabled)
		return;
	
	if (!cdValid)
		return;

	if (!wasPlaying)
		return;
	
	if ( ioctl(cdfile, CDROMRESUME) == -1 ) 
		Con_DPrintf("ioctl cdromresume failed\n");
	playing = true;
};

static void CD_f ()
{
	char	*command;
	int		ret;
	int		n;

	if (Cmd_Argc() < 2)
		return;

	command = Cmd_Argv (1);

	if (Q_strcasecmp(command, "on") == 0)
	{
		mbEnabled = true;
		return;
	};

	if (Q_strcasecmp(command, "off") == 0)
	{
		if (playing)
			Stop();
		mbEnabled = false;
		return;
	};

	if (Q_strcasecmp(command, "reset") == 0)
	{
		mbEnabled = true;
		if (playing)
			Stop();
		for (n = 0; n < 100; n++)
			remap[n] = n;
		GetAudioDiskInfo();
		return;
	};

	if (Q_strcasecmp(command, "remap") == 0)
	{
		ret = Cmd_Argc() - 2;
		if (ret <= 0)
		{
			for (n = 1; n < 100; n++)
				if (remap[n] != n)
					Con_Printf("  %u -> %u\n", n, remap[n]);
			return;
		};
		for (n = 1; n <= ret; n++)
			remap[n] = Q_atoi(Cmd_Argv (n+1));
		return;
	};

	if (Q_strcasecmp(command, "close") == 0)
	{
		CloseDoor();
		return;
	};

	if (!cdValid)
	{
		GetAudioDiskInfo();
		if (!cdValid)
		{
			Con_Printf("No CD in player.\n");
			return;
		};
	};

	if (Q_strcasecmp(command, "play") == 0)
	{
		Play((byte)Q_atoi(Cmd_Argv (2)), false);
		return;
	};

	if (Q_strcasecmp(command, "loop") == 0)
	{
		Play((byte)Q_atoi(Cmd_Argv (2)), true);
		return;
	};

	if (Q_strcasecmp(command, "stop") == 0)
	{
		Stop();
		return;
	};

	if (Q_strcasecmp(command, "pause") == 0)
	{
		Pause();
		return;
	};

	if (Q_strcasecmp(command, "resume") == 0)
	{
		Resume();
		return;
	};

	if (Q_strcasecmp(command, "eject") == 0)
	{
		if (playing)
			Stop();
		Eject();
		cdValid = false;
		return;
	};

	if (Q_strcasecmp(command, "info") == 0)
	{
		Con_Printf("%u tracks\n", maxTrack);
		if (playing)
			Con_Printf("Currently %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		else if (wasPlaying)
			Con_Printf("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		Con_Printf("Volume is %f\n", cdvolume);
		return;
	};
};

void CCDAudio::Eject()
{
	if (cdfile == -1 || !mbEnabled)
		return; // no cd init'd

	if ( ioctl(cdfile, CDROMEJECT) == -1 ) 
		Con_DPrintf("ioctl cdromeject failed\n");
};

void CCDAudio::CloseDoor()
{
	if (cdfile == -1 || !mbEnabled)
		return; // no cd init'd

	if ( ioctl(cdfile, CDROMCLOSETRAY) == -1 ) 
		Con_DPrintf("ioctl cdromclosetray failed\n");
};

int CCDAudio::GetAudioDiskInfo()
{
	struct cdrom_tochdr tochdr;

	cdValid = false;

	if ( ioctl(cdfile, CDROMREADTOCHDR, &tochdr) == -1 ) 
    {
      Con_DPrintf("ioctl cdromreadtochdr failed\n");
	  return -1;
    };

	if (tochdr.cdth_trk0 < 1)
	{
		Con_DPrintf("CDAudio: no music tracks\n");
		return -1;
	};

	cdValid = true;
	maxTrack = tochdr.cdth_trk1;

	return 0;
};