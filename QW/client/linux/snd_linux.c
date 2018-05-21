
qboolean SNDDMA_Init()
{

	int rc;
    int fmt;
	int tmp;
    int i;
    char *s;
	struct audio_buf_info info;
	int caps;

	snd_inited = 0;

// open /dev/dsp, confirm capability to mmap, and get size of dma buffer

    audio_fd = open("/dev/dsp", O_RDWR);
    if (audio_fd < 0)
	{
		perror("/dev/dsp");
        Con_Printf("Could not open /dev/dsp\n");
		return 0;
	}

    rc = ioctl(audio_fd, SNDCTL_DSP_RESET, 0);
    if (rc < 0)
	{
		perror("/dev/dsp");
		Con_Printf("Could not reset /dev/dsp\n");
		close(audio_fd);
		return 0;
	}

	if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &caps)==-1)
	{
		perror("/dev/dsp");
        Con_Printf("Sound driver too old\n");
		close(audio_fd);
		return 0;
	}

	if (!(caps & DSP_CAP_TRIGGER) || !(caps & DSP_CAP_MMAP))
	{
		Con_Printf("Sorry but your soundcard can't do this\n");
		close(audio_fd);
		return 0;
	}

    if (ioctl(audio_fd, SNDCTL_DSP_GETOSPACE, &info)==-1)
    {   
        perror("GETOSPACE");
		Con_Printf("Um, can't do GETOSPACE?\n");
		close(audio_fd);
		return 0;
    }
    
	shm = &sn;
    shm->splitbuffer = 0;

// set sample bits & speed

    s = getenv("QUAKE_SOUND_SAMPLEBITS");
    if (s) shm->samplebits = atoi(s);
	else if ((i = COM_CheckParm("-sndbits")) != 0)
		shm->samplebits = atoi(com_argv[i+1]);
	if (shm->samplebits != 16 && shm->samplebits != 8)
    {
        ioctl(audio_fd, SNDCTL_DSP_GETFMTS, &fmt);
        if (fmt & AFMT_S16_LE) shm->samplebits = 16;
        else if (fmt & AFMT_U8) shm->samplebits = 8;
    }

    s = getenv("QUAKE_SOUND_SPEED");
    if (s) shm->speed = atoi(s);
	else if ((i = COM_CheckParm("-sndspeed")) != 0)
		shm->speed = atoi(com_argv[i+1]);
    else
    {
        for (i=0 ; i<sizeof(tryrates)/4 ; i++)
            if (!ioctl(audio_fd, SNDCTL_DSP_SPEED, &tryrates[i])) break;
        shm->speed = tryrates[i];
    }

    s = getenv("QUAKE_SOUND_CHANNELS");
    if (s) shm->channels = atoi(s);
	else if ((i = COM_CheckParm("-sndmono")) != 0)
		shm->channels = 1;
	else if ((i = COM_CheckParm("-sndstereo")) != 0)
		shm->channels = 2;
    else shm->channels = 2;

	shm->samples = info.fragstotal * info.fragsize / (shm->samplebits/8);
	shm->submission_chunk = 1;

// memory map the dma buffer

	shm->buffer = (unsigned char *) mmap(NULL, info.fragstotal
		* info.fragsize, PROT_WRITE, MAP_FILE|MAP_SHARED, audio_fd, 0);
	if (!shm->buffer)
	{
		perror("/dev/dsp");
		Con_Printf("Could not mmap /dev/dsp\n");
		close(audio_fd);
		return 0;
	}

	tmp = 0;
	if (shm->channels == 2)
		tmp = 1;
    rc = ioctl(audio_fd, SNDCTL_DSP_STEREO, &tmp);
    if (rc < 0)
    {
		perror("/dev/dsp");
        Con_Printf("Could not set /dev/dsp to stereo=%d", shm->channels);
		close(audio_fd);
        return 0;
    }
	if (tmp)
		shm->channels = 2;
	else
		shm->channels = 1;

    rc = ioctl(audio_fd, SNDCTL_DSP_SPEED, &shm->speed);
    if (rc < 0)
    {
		perror("/dev/dsp");
        Con_Printf("Could not set /dev/dsp speed to %d", shm->speed);
		close(audio_fd);
        return 0;
    }

    if (shm->samplebits == 16)
    {
        rc = AFMT_S16_LE;
        rc = ioctl(audio_fd, SNDCTL_DSP_SETFMT, &rc);
        if (rc < 0)
		{
			perror("/dev/dsp");
			Con_Printf("Could not support 16-bit data.  Try 8-bit.\n");
			close(audio_fd);
			return 0;
		}
    }
    else if (shm->samplebits == 8)
    {
        rc = AFMT_U8;
        rc = ioctl(audio_fd, SNDCTL_DSP_SETFMT, &rc);
        if (rc < 0)
		{
			perror("/dev/dsp");
			Con_Printf("Could not support 8-bit data.\n");
			close(audio_fd);
			return 0;
		}
    }
	else
	{
		perror("/dev/dsp");
		Con_Printf("%d-bit sound not supported.", shm->samplebits);
		close(audio_fd);
		return 0;
	}

// toggle the trigger & start her up

    tmp = 0;
    rc  = ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &tmp);
	if (rc < 0)
	{
		perror("/dev/dsp");
		Con_Printf("Could not toggle.\n");
		close(audio_fd);
		return 0;
	}
    tmp = PCM_ENABLE_OUTPUT;
    rc = ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &tmp);
	if (rc < 0)
	{
		perror("/dev/dsp");
		Con_Printf("Could not toggle.\n");
		close(audio_fd);
		return 0;
	}

	shm->samplepos = 0;

	snd_inited = 1;
	return 1;

}
