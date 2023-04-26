/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021-2022 BlackPhrase
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
/// @brief portable code to mix sounds for snd_dma.c

#include "quakedef.h"

#ifdef _WIN32
#include "winquake.h"
#else
#define DWORD unsigned long
#endif

#define PAINTBUFFER_SIZE 512

// TODO: wrong place?
cvar_t room_delay = {"room_delay", "0"};
cvar_t room_feedback = {"room_feedback", "0.2"};
cvar_t room_dlylp = {"room_dlylp", "1.0"};
cvar_t room_size = {"room_size", "0"};
cvar_t room_refl = {"room_refl", "0.7"};
cvar_t room_rvblp = {"room_rvblp", "1.0"};
cvar_t room_left = {"room_left", "0"};
cvar_t room_lp = {"room_lp", "0"};
cvar_t room_mod = {"room_mod", "0"};
cvar_t room_type = {"room_type", "0"};
cvar_t waterroom_type = {"waterroom_type", "14"};
cvar_t room_off = {"room_off", "0", FCVAR_ARCHIVE};

portable_samplepair_t paintbuffer[PAINTBUFFER_SIZE];
int snd_scaletable[32][256];
int *snd_p, snd_linear_count, snd_vol;
short *snd_out;

void Snd_WriteLinearBlastStereo16();

//#if !id386 // TODO: check for non-Intel?
void Snd_WriteLinearBlastStereo16()
{
	int i;
	int val;

	for(i = 0; i < snd_linear_count; i += 2)
	{
		val = (snd_p[i] * snd_vol) >> 8;
		if(val > 0x7fff)
			snd_out[i] = 0x7fff;
		else if(val < (short)0x8000)
			snd_out[i] = (short)0x8000;
		else
			snd_out[i] = val;

		val = (snd_p[i + 1] * snd_vol) >> 8;
		if(val > 0x7fff)
			snd_out[i + 1] = 0x7fff;
		else if(val < (short)0x8000)
			snd_out[i + 1] = (short)0x8000;
		else
			snd_out[i + 1] = val;
	};
};
//#endif // !id386

void S_TransferStereo16(int endtime)
{
	int lpos;
	int lpaintedtime;
	DWORD *pbuf;
#ifdef _WIN32
	int reps;
	DWORD dwSize, dwSize2;
	DWORD *pbuf2;
	HRESULT hresult;
#endif

	snd_vol = volume.value * 256;

	snd_p = (int *)paintbuffer;
	lpaintedtime = paintedtime;

#ifdef _WIN32
	if(pDSBuf)
	{
		reps = 0;

		while((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, (void**)&pbuf, &dwSize,
		                                      (void**)&pbuf2, &dwSize2, 0)) != DS_OK)
		{
			if(hresult != DSERR_BUFFERLOST)
			{
				Con_Printf("S_TransferStereo16: DS::Lock Sound Buffer Failed\n");
				S_Shutdown();
				S_Startup();
				return;
			};

			if(++reps > 10000)
			{
				Con_Printf("S_TransferStereo16: DS: couldn't restore buffer\n");
				S_Shutdown();
				S_Startup();
				return;
			};
		};
	}
	else
#endif
	{
		pbuf = (DWORD *)shm->buffer;
	};

	while(lpaintedtime < endtime)
	{
		// handle recirculating buffer issues
		lpos = lpaintedtime & ((shm->samples >> 1) - 1);

		snd_out = (short *)pbuf + (lpos << 1);

		snd_linear_count = (shm->samples >> 1) - lpos;
		if(lpaintedtime + snd_linear_count > endtime)
			snd_linear_count = endtime - lpaintedtime;

		snd_linear_count <<= 1;

		// write a linear blast of samples
		Snd_WriteLinearBlastStereo16();

		snd_p += snd_linear_count;
		lpaintedtime += (snd_linear_count >> 1);
	};

#ifdef _WIN32
	if(pDSBuf)
		pDSBuf->lpVtbl->Unlock(pDSBuf, pbuf, dwSize, NULL, 0);
#endif
};

void S_TransferPaintBuffer(int endtime)
{
	int out_idx;
	int count;
	int out_mask;
	int *p;
	int step;
	int val;
	int snd_vol;
	DWORD *pbuf;
#ifdef _WIN32
	int reps;
	DWORD dwSize, dwSize2;
	DWORD *pbuf2;
	HRESULT hresult;
#endif

	if(shm->samplebits == 16 && shm->channels == 2)
	{
		S_TransferStereo16(endtime);
		return;
	};

	p = (int *)paintbuffer;
	count = (endtime - paintedtime) * shm->channels;
	out_mask = shm->samples - 1;
	out_idx = paintedtime * shm->channels & out_mask;
	step = 3 - shm->channels;
	snd_vol = volume.value * 256;

#ifdef _WIN32
	if(pDSBuf)
	{
		reps = 0;

		while((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, (void**)&pbuf, &dwSize,
		                                      (void**)&pbuf2, &dwSize2, 0)) != DS_OK)
		{
			if(hresult != DSERR_BUFFERLOST)
			{
				Con_Printf("S_TransferPaintBuffer: DS::Lock Sound Buffer Failed\n");
				S_Shutdown();
				S_Startup();
				return;
			};

			if(++reps > 10000)
			{
				Con_Printf("S_TransferPaintBuffer: DS: couldn't restore buffer\n");
				S_Shutdown();
				S_Startup();
				return;
			};
		};
	}
	else
#endif
	{
		pbuf = (DWORD *)shm->buffer;
	};

	if(shm->samplebits == 16)
	{
		short *out = (short *)pbuf;
		while(count--)
		{
			val = (*p * snd_vol) >> 8;
			p += step;
			if(val > 0x7fff)
				val = 0x7fff;
			else if(val < (short)0x8000)
				val = (short)0x8000;
			out[out_idx] = val;
			out_idx = (out_idx + 1) & out_mask;
		};
	}
	else if(shm->samplebits == 8)
	{
		unsigned char *out = (unsigned char *)pbuf;
		while(count--)
		{
			val = (*p * snd_vol) >> 8;
			p += step;
			if(val > 0x7fff)
				val = 0x7fff;
			else if(val < (short)0x8000)
				val = (short)0x8000;
			out[out_idx] = (val >> 8) + 128;
			out_idx = (out_idx + 1) & out_mask;
		};
	};

#ifdef _WIN32
	if(pDSBuf)
	{
		DWORD dwNewpos, dwWrite;
		int il = paintedtime;
		int ir = endtime - paintedtime;

		ir += il;

		pDSBuf->lpVtbl->Unlock(pDSBuf, pbuf, dwSize, NULL, 0);

		pDSBuf->lpVtbl->GetCurrentPosition(pDSBuf, &dwNewpos, &dwWrite);

		//		if ((dwNewpos >= il) && (dwNewpos <= ir))
		//			Con_Printf("%d-%d p %d c\n", il, ir, dwNewpos);
	};
#endif
};

/*
===============================================================================

CHANNEL MIXING

===============================================================================
*/

void SND_PaintChannelFrom8(channel_t *ch, sfxcache_t *sc, int endtime);
void SND_PaintChannelFrom16(channel_t *ch, sfxcache_t *sc, int endtime);

void S_PaintChannels(int endtime)
{
	int i;
	int end;
	channel_t *ch;
	sfxcache_t *sc;
	int ltime, count;

	while(paintedtime < endtime)
	{
		// if paintbuffer is smaller than DMA buffer
		end = endtime;
		if(endtime - paintedtime > PAINTBUFFER_SIZE)
			end = paintedtime + PAINTBUFFER_SIZE;

		// clear the paint buffer
		Q_memset(paintbuffer, 0, (end - paintedtime) * sizeof(portable_samplepair_t));

		// paint in the channels.
		ch = channels;
		for(i = 0; i < total_channels; i++, ch++)
		{
			if(!ch->sfx)
				continue;
			if(!ch->leftvol && !ch->rightvol)
				continue;
			sc = S_LoadSound(ch->sfx);
			if(!sc)
				continue;

			ltime = paintedtime;

			while(ltime < end)
			{ // paint up to end
				if(ch->end < end)
					count = ch->end - ltime;
				else
					count = end - ltime;

				if(count > 0)
				{
					if(sc->width == 1)
						SND_PaintChannelFrom8(ch, sc, count);
					else
						SND_PaintChannelFrom16(ch, sc, count);

					ltime += count;
				};

				// if at end of loop, restart
				if(ltime >= ch->end)
				{
					if(sc->loopstart >= 0)
					{
						ch->pos = sc->loopstart;
						ch->end = ltime + sc->length - ch->pos;
					}
					else
					{ // channel just stopped
						ch->sfx = NULL;
						break;
					};
				};
			};
		};

		// transfer out according to DMA format
		S_TransferPaintBuffer(end);
		paintedtime = end;
	};
};

void SND_InitScaletable()
{
	int i, j;

	for(i = 0; i < 32; i++)
		for(j = 0; j < 256; j++)
			snd_scaletable[i][j] = ((signed char)j) * i * 8;
};

//#if !id386 // TODO: check for non-Intel?
void SND_PaintChannelFrom8(channel_t *ch, sfxcache_t *sc, int count)
{
	int data;
	int *lscale, *rscale;
	unsigned char *sfx;
	int i;

	if(ch->leftvol > 255)
		ch->leftvol = 255;
	if(ch->rightvol > 255)
		ch->rightvol = 255;

	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
	sfx = (signed char *)sc->data + ch->pos;

	for(i = 0; i < count; i++)
	{
		data = sfx[i];
		paintbuffer[i].left += lscale[data];
		paintbuffer[i].right += rscale[data];
	};

	ch->pos += count;
};
//#endif // !id386

void SND_PaintChannelFrom16(channel_t *ch, sfxcache_t *sc, int count)
{
	int data;
	int left, right;
	int leftvol, rightvol;
	signed short *sfx;
	int i;

	leftvol = ch->leftvol;
	rightvol = ch->rightvol;
	sfx = (signed short *)sc->data + ch->pos;

	for(i = 0; i < count; i++)
	{
		data = sfx[i];
		left = (data * leftvol) >> 8;
		right = (data * rightvol) >> 8;
		paintbuffer[i].left += left;
		paintbuffer[i].right += right;
	};

	ch->pos += count;
};