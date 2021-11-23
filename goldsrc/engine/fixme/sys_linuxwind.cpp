/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2020 BlackPhrase
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
#include "igame.h"

//#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

class CGame final : public IGame
{
public:
	CGame();
	~CGame();

	void Init();
	void Shutdown();

	void CreateGameWindow() override;

	void PlayStartupVideos() override;
	void PlayVideoAndWait(const char *name, bool) override; // TODO: (unsigned long *, char)?

	void SleepUntilInput(int time) override;

	void SetActiveApp(bool abActive);
	bool IsActiveApp();

	void SetWindowXY(int x, int y);
	void SetWindowSize(int width, int height);

	void SetCursorVisible(bool abVisible);

	bool IsMultiplayer();

	void *GetMainWindow();
	void *GetMainWindowAddress();

	void GetWindowRect(); // TODO: args
private:
	bool mbActiveApp{false};
};

CGame::CGame() = default;
CGame::~CGame() = default;

void CGame::Init()
{
	// TODO
};

void CGame::Shutdown()
{
	// TODO
};

void CGame::CreateGameWindow()
{
	int width = 640, height = 480;

#ifdef GLX
	int i;
	XSetWindowAttributes attr;
	unsigned long mask;
	Window root;
	XVisualInfo *visinfo;
	bool fullscreen = true;
	//int MajorVersion, MinorVersion;
	//int actualWidth, actualHeight;
	int attrib[] =
	{ 
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None
	};
	int scrnum;
	
	// interpret command-line params

	// set vid parameters
	if((i = COM_CheckParm("-window")) != 0)
		fullscreen = false;

	if((i = COM_CheckParm("-width")) != 0)
		width = atoi(com_argv[i + 1]);

	if((i = COM_CheckParm("-height")) != 0)
		height = atoi(com_argv[i + 1]);
	
	if(!(dpy = XOpenDisplay(NULL)))
	{
		fprintf(stderr, "Error couldn't open the X display\n");
		exit(1);
	};
	
	scrnum = DefaultScreen(dpy);
	root = RootWindow(dpy, scrnum);
	
	// Get video mode list
	/*
	MajorVersion = MinorVersion = 0;
	if(!XF86VidModeQueryVersion(dpy, &MajorVersion, &MinorVersion))
		vidmode_ext = false;
	else
	{
		Con_Printf("Using XFree86-VidModeExtension Version %d.%d\n", MajorVersion, MinorVersion);
		vidmode_ext = true;
	};
	*/

	visinfo = qglXChooseVisual(dpy, scrnum, attrib);
	if(!visinfo)
	{
		fprintf(stderr, "qkHack: Error couldn't get an RGB, Double-buffered, Depth visual\n");
		exit(1);
	};

	/*
	if(vidmode_ext)
	{
		int best_fit, best_dist, dist, x, y;

		XF86VidModeGetAllModeLines(dpy, scrnum, &num_vidmodes, &vidmodes);

		// Are we going fullscreen?  If so, let's change video mode
		if(fullscreen)
		{
			best_dist = 9999999;
			best_fit = -1;

			for(i = 0; i < num_vidmodes; i++)
			{
				if(width > vidmodes[i]->hdisplay ||
				   height > vidmodes[i]->vdisplay)
					continue;

				x = width - vidmodes[i]->hdisplay;
				y = height - vidmodes[i]->vdisplay;
				dist = (x * x) + (y * y);
				if(dist < best_dist)
				{
					best_dist = dist;
					best_fit = i;
				}
			}

			if(best_fit != -1)
			{
				actualWidth = vidmodes[best_fit]->hdisplay;
				actualHeight = vidmodes[best_fit]->vdisplay;

				// change to the mode
				XF86VidModeSwitchToMode(dpy, scrnum, vidmodes[best_fit]);
				vidmode_active = true;

				// Move the viewport to top left
				XF86VidModeSetViewPort(dpy, scrnum, 0, 0);
			}
			else
				fullscreen = 0;
		};
	};
	*/

	// window attributes
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
	attr.event_mask = X_MASK;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	
	if(vidmode_active)
	{
		mask = CWBackPixel | CWColormap | CWSaveUnder | CWBackingStore |
		CWEventMask | CWOverrideRedirect;
		attr.override_redirect = True;
		attr.backing_store = NotUseful;
		attr.save_under = False;
	};

	win = XCreateWindow(dpy, root, 0, 0, width, height,
	                    0, visinfo->depth, InputOutput,
	                    visinfo->visual, mask, &attr);
	XMapWindow(dpy, win);

	if(vidmode_active)
	{
		XMoveWindow(dpy, win, 0, 0);
		XRaiseWindow(dpy, win);
		XWarpPointer(dpy, None, win, 0, 0, 0, 0, 0, 0);
		XFlush(dpy);
		// Move the viewport to top left
		XF86VidModeSetViewPort(dpy, scrnum, 0, 0);
	};

	XFlush(dpy);
#endif // GLX
};

void CGame::PlayStartupVideos()
{
	// TODO
};

void CGame::PlayVideoAndWait(const char *name, bool)
{
	// TODO
};

void CGame::SleepUntilInput(int time)
{
	// TODO
};

void CGame::SetActiveApp(bool abActive)
{
	// TODO
	mbActiveApp = abActive;
};

bool CGame::IsActiveApp()
{
	// TODO
	return mbActiveApp;
};

void CGame::SetWindowXY(int x, int y)
{
	// TODO
};

void CGame::SetWindowSize(int width, int height)
{
	// TODO
};

void CGame::SetCursorVisible(bool abVisible)
{
	// TODO
};

bool CGame::IsMultiplayer()
{
	// TODO
	return false;
};

void *CGame::GetMainWindow()
{
	return root;
};

void *CGame::GetMainWindowAddress()
{
	return root;
};

void CGame::GetWindowRect()
{
	// TODO
};