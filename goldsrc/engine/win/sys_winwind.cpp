/*
 * This file is part of OGS Engine
 * Copyright (C) 1997-2001 Id Software, Inc.
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
#include "winquake.h"
#include "igame.h"

// TODO: should be located in winquake header
#define	WINDOW_STYLE (WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

const char WINDOW_CLASS_NAME[] = "WinQuake";

/*
cvar_t vid_width = {"vid_width", "640", FCVAR_ARCHIVE};
cvar_t vid_height = {"vid_height", "480", FCVAR_ARCHIVE};
cvar_t vid_fullscreen = {"vid_fullscreen", "false", FCVAR_ARCHIVE};
*/

extern "C" LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void CenterWindow(HWND hWndCenter, int width, int height, BOOL lefttopjustify)
{
	RECT rect;
	int CenterX, CenterY;

	CenterX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	CenterY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if(CenterX > CenterY * 2)
		CenterX >>= 1; // dual screens
	CenterX = (CenterX < 0) ? 0 : CenterX;
	CenterY = (CenterY < 0) ? 0 : CenterY;
	SetWindowPos(hWndCenter, NULL, CenterX, CenterY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_DRAWFRAME);
};

class CGame final : public IGame
{
public:
	CGame();
	~CGame();

	void CreateGameWindow() override;

	void PlayStartupVideos() override;
	void PlayVideoAndWait(const char *name, bool) override;

	void SleepUntilInput(int time) override;
};

CGame gGame;
IGame *gpGame{&gGame};

CGame::CGame() = default;

CGame::~CGame() //= default;
{
	if (mainwindow)
	{
		DestroyWindow (mainwindow);
		mainwindow = nullptr;
	};
	
	//UnregisterClass (WINDOW_CLASS_NAME, glw_state.hInstance); // TODO
};

/*
** VID_CreateWindow
*/
/*
qboolean VID_CreateWindow( int width, int height, qboolean fullscreen )
{
	

	// init all the gl stuff for the window
	if (!GLimp_InitGL ())
	{
		ri.Con_Printf( PRINT_ALL, "VID_CreateWindow() - GLimp_InitGL failed\n");
		return false;
	}

	

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);

	return true;
}
*/

void CGame::CreateGameWindow()
{
	WNDCLASS wc;
	RECT rect;
	int stylebits{WINDOW_STYLE};
	int x = 0, y = 0, w, h;
	int exstyle{0};
	bool fullscreen{false};
	
	// TODO
	w = 1280;
	h = 600;
	
	if(COM_CheckParm("-width"))
		w = Q_atoi(com_argv[COM_CheckParm("-width") + 1]);
	
	if(COM_CheckParm("-height"))
		h = Q_atoi(com_argv[COM_CheckParm("-height") + 1]);
	
	// Register the frame class
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)MainWndProc; // TODO: (WNDPROC)glw_state.wndproc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = nullptr; // TODO: glw_state.hInstance;/global_hInstance
    wc.hIcon         = 0;
    wc.hCursor       = LoadCursor (nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_GRAYTEXT; // TODO: nullptr in qw
    wc.lpszMenuName  = 0;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClass (&wc) )
		/*ri.*/Sys_Error (/*ERR_FATAL,*/ "Couldn't register window class");

	if (fullscreen)
	{
		exstyle = WS_EX_TOPMOST;
		stylebits = WS_POPUP | WS_VISIBLE;
	};

	rect.left = 0;
	rect.top = 0;
	rect.right  = w;
	rect.bottom = h;

	AdjustWindowRect (&rect, stylebits, FALSE); // TODO: AdjustWindowRectEx(&rect, WindowStyle, FALSE, 0); in qw

	w = rect.right - rect.left;
	h = rect.bottom - rect.top;
	
	if (!fullscreen)
	{
		x = GetSystemMetrics(SM_CXSCREEN) * 0.5;
		y = GetSystemMetrics(SM_CYSCREEN) * 0.5;
	};
	
	// Create the window
	mainwindow = CreateWindowEx (
		 exstyle, // ExWindowStyle
		 WINDOW_CLASS_NAME,
		 "OGS Engine Tech Demo",
		 stylebits, // WindowStyle
		 x, y, // rect.left, rect.top,
		 w, h,
		 nullptr,
		 nullptr,
		 nullptr, // TODO: glw_state.hInstance,/global_hInstance
		 nullptr);

	if (!mainwindow)
		/*ri.*/Sys_Error (/*ERR_FATAL,*/ "Couldn't create window");
	
	// Center and show the window
	//CenterWindow(mainwindow, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, false);
	CenterWindow(mainwindow, rect.right - rect.left, rect.bottom - rect.top, false);

	ShowWindow( mainwindow, SW_SHOW/*DEFAULT*/ );
	UpdateWindow( mainwindow );
	
	// TODO: gl initialization was here
	
	SetForegroundWindow( mainwindow );
	SetFocus( mainwindow );
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
	//MsgWaitForMultipleObjects(1, &tevent, FALSE, time, QS_ALLINPUT); // TODO
};