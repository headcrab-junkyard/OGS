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

#include <windows.h>
#include "quakedef.h"
#include "icdaudio.h"

UINT wDeviceID;

extern "C" LONG CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(lParam != wDeviceID)
		return 1;

	// TODO
/*
	switch(wParam)
	{
	case MCI_NOTIFY_SUCCESSFUL:
		if(playing)
		{
			playing = false;
			if(playLooping)
				CDAudio_Play(playTrack, true);
		};
		break;

	case MCI_NOTIFY_ABORTED:
	case MCI_NOTIFY_SUPERSEDED:
		break;

	case MCI_NOTIFY_FAILURE:
		Con_DPrintf("MCI_NOTIFY_FAILURE\n");
		CDAudio_Stop();
		cdValid = false;
		break;

	default:
		Con_DPrintf("Unexpected MM_MCINOTIFY type (%i)\n", wParam);
		return 1;
	};
*/

	return 0;
};