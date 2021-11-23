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

extern "C"
{
#include "quakedef.h"
#include "winquake.h"
}
#include "igame.h"

// TODO: should be located in winquake header
#define	WINDOW_STYLE (WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

const char WINDOW_CLASS_NAME[] = "WinQuake";

HWND mainwindow;

extern DEVMODE gdevmode;
extern qboolean vid_canalttab;
extern qboolean vid_wassuspended;
extern int window_x, window_y;

/*
cvar_t vid_width = {"vid_width", "640", FCVAR_ARCHIVE};
cvar_t vid_height = {"vid_height", "480", FCVAR_ARCHIVE};
cvar_t vid_fullscreen = {"vid_fullscreen", "false", FCVAR_ARCHIVE};
*/

extern "C" void ClearAllStates();
extern "C" void VID_UpdateWindowStatus();
extern "C" int MapKey(int key);
extern "C" LRESULT CDAudio_MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/****************************************************************************
*
* Function:     AppActivate
* Parameters:   fActive - True if app is activating
*
* Description:  If the application is activating, then swap the system
*               into SYSPAL_NOSTATIC mode so that our palettes will display
*               correctly.
*
****************************************************************************/
void AppActivate(BOOL fActive, BOOL minimize)
{
	MSG msg;
	HDC hdc;
	int i, t;
	static bool sound_active;

	ActiveApp = fActive;
	Minimized = minimize;

	// enable/disable sound on focus gain/loss
	if(!ActiveApp && sound_active)
	{
		S_BlockSound();
		sound_active = false;
	}
	else if(ActiveApp && !sound_active)
	{
		S_UnblockSound();
		sound_active = true;
	};

	if(fActive)
	{
		if(modestate == MS_FULLDIB)
		{
			IN_ActivateMouse();
			IN_HideMouse();
			if(vid_canalttab && vid_wassuspended)
			{
				vid_wassuspended = false;
				ChangeDisplaySettings(&gdevmode, CDS_FULLSCREEN);
				ShowWindow(mainwindow, SW_SHOWNORMAL);
			};
		}
		else if((modestate == MS_WINDOWED) /*&& _windowed_mouse.value*/ && key_dest == key_game)
		{
			IN_ActivateMouse();
			IN_HideMouse();
		};
	};

	if(!fActive)
	{
		if(modestate == MS_FULLDIB)
		{
			IN_DeactivateMouse();
			IN_ShowMouse();
			if(vid_canalttab)
			{
				ChangeDisplaySettings(NULL, 0);
				vid_wassuspended = true;
			};
		}
		else if((modestate == MS_WINDOWED) /*&& _windowed_mouse.value*/)
		{
			IN_DeactivateMouse();
			IN_ShowMouse();
		};
	};
};

// main window procedure
LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lRet = 1;
	int fwKeys, xPos, yPos, fActive, fMinimized, temp;
	extern unsigned int uiWheelMessage;

	if(uMsg == uiWheelMessage)
		uMsg = WM_MOUSEWHEEL;

#ifdef OGS_USE_IMGUI
	if(ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
#endif
	
	switch(uMsg)
	{
	case WM_KILLFOCUS:
		if(modestate == MS_FULLDIB)
			ShowWindow(mainwindow, SW_SHOWMINNOACTIVE);
		break;

	case WM_CREATE:
		break;

	case WM_MOVE:
		window_x = (int)LOWORD(lParam);
		window_y = (int)HIWORD(lParam);
		VID_UpdateWindowStatus();
		
		//if((modestate == MS_WINDOWED) && !in_mode_set && !Minimized)
			//VID_RememberWindowPos();
		
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		Key_Event(MapKey(lParam), true);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		Key_Event(MapKey(lParam), false);
		break;

	case WM_SYSCHAR:
		// keep Alt-Space from happening
		break;

	// this is complicated because Win32 seems to pack multiple mouse events into
	// one update sometimes, so we always check all states and look for events
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		temp = 0;

		if(wParam & MK_LBUTTON)
			temp |= 1;

		if(wParam & MK_RBUTTON)
			temp |= 2;

		if(wParam & MK_MBUTTON)
			temp |= 4;

		IN_MouseEvent(temp);

		break;

	// JACK: This is the mouse wheel with the Intellimouse
	// Its delta is either positive or neg, and we generate the proper
	// Event.
	case WM_MOUSEWHEEL:
		if((short)HIWORD(wParam) > 0)
		{
			Key_Event(K_MWHEELUP, true);
			Key_Event(K_MWHEELUP, false);
		}
		else
		{
			Key_Event(K_MWHEELDOWN, true);
			Key_Event(K_MWHEELDOWN, false);
		};
		break;

	case WM_SIZE:
		Minimized = false;

		if(!(wParam & SIZE_RESTORED))
		{
			if(wParam & SIZE_MINIMIZED)
				Minimized = true;
		};
		break;

	case WM_CLOSE:
		Sys_Quit();
		break;

	case WM_ACTIVATE:
		fActive = LOWORD(wParam);
		fMinimized = (BOOL)HIWORD(wParam);
		AppActivate(!(fActive == WA_INACTIVE), fMinimized);

		// fix the leftover Alt from any Alt-Tab or the like that switched us away
		ClearAllStates();

		break;

	case WM_DESTROY:
	{
		if(mainwindow)
			DestroyWindow(mainwindow);

		PostQuitMessage(0);
	};
	break;

	case MM_MCINOTIFY:
		lRet = CDAudio_MessageHandler(hWnd, uMsg, wParam, lParam);
		break;

	default:
		/* pass all unhandled messages to DefWindowProc */
		lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	};

	/* return 1 if handled message, 0 if not */
	return lRet;
};

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
	
	void Init();
	void Shutdown();

	void CreateGameWindow() override;

	void PlayStartupVideos() override;
	void PlayVideoAndWait(const char *name, bool) override;

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

CGame gGame;
IGame *gpGame{&gGame};

CGame::CGame() = default;

CGame::~CGame() //= default;
{
	AppActivate(false, false);
	
	if (mainwindow)
	{
		DestroyWindow (mainwindow);
		mainwindow = nullptr;
	};
	
	//UnregisterClass (WINDOW_CLASS_NAME, glw_state.hInstance); // TODO
};

void CGame::Init()
{
	// TODO
};

void CGame::Shutdown()
{
	// TODO
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
	return reinterpret_cast<void*>(mainwindow);
};

void *CGame::GetMainWindowAddress()
{
	return reinterpret_cast<void*>(mainwindow);
};

void CGame::GetWindowRect()
{
	// TODO
};