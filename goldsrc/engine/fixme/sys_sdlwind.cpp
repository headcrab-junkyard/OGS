/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2021 BlackPhrase
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

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_syswm.h>
#include <stb_image.h>

#include "quakedef.h"
#include "igame.h"

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
	SDL_GLContext mGLContext;

	SDL_Window *mpWindow{nullptr};
	SDL_Cursor *mpCursor{nullptr};

	bool mbActiveApp{false};
};

CGame::CGame() = default;

CGame::~CGame() //= default;
{
	SDL_FreeCursor(mpCursor);
	
	SDL_FreeSurface(pWinIcon);
	stbi_image_free(pGameIcon);
	
	SDL_GL_DeleteContext(mGLContext); // TODO: unused?
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();
};

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
	// Init SDL2
	if(SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) // TODO: SDL_InitSubsystem?
			Sys_Error("Couldn't create SDL2 window! (%s)", SDL_GetError());
	
	// Setup window attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	SDL_DisplayMode CurrentDisplayMode;
	SDL_GetCurrentDisplayMode(0, &CurrentDisplayMode);
	
	SDL_WindowFlags WindowFlags{SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI}; // TODO: check if we should add opengl to flags
	
	if(!COM_CheckParm("-noontop"))
		WindowFlags |= SDL_WINDOW_ALWAYS_ON_TOP; //SDL_ALLOW_TOPMOST
	
	if(COM_CheckParm("-noborder"))
		WindowFlags |= SDL_WINDOW_BORDERLESS;
	
	if(COM_CheckParm("-fullscreen"))
		WindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP; // SDL_WINDOW_FULLSCREEN
	
	mpWindow = SDL_CreateWindow("OGS Engine Tech Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 600, WindowFlags); // TODO: title should be "Half-Life"

	if(!mpWindow)
		Sys_Error("Failed to create SDL Window! (%s)", SDL_GetError());
	
	// BP: hack to make some Windows stuff (dependent on window) work with SDL
#ifdef _WIN32
	SDL_SysWMinfo WindowInfo{};
	SDL_VERSION(&WindowInfo.version);
	SDL_GetWindowWMInfo(mpWindow, &WindowInfo);
	
	mainwindow = WindowInfo.win.window;
#endif
	
	mGLContext = SDL_GL_CreateContext(mpWindow);
	
	int r, g, b, a, depth;
	
	if(!SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r))
		Sys_Error("Failed to get GL RED size (%s)", SDL_GetError());
	if(!SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g))
		Sys_Error("Failed to get GL GREEN size (%s)", SDL_GetError());
	if(!SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b))
		Sys_Error("Failed to get GL BLUE size (%s)", SDL_GetError());
	if(!SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a))
		Sys_Error("Failed to get GL ALPHA size (%s)", SDL_GetError());
	if(!SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth))
		Sys_Error("Failed to get GL DEPTH size (%s)", SDL_GetError());
	
	Con_DPrintf("GL_SIZES: r:%d g:%d b:%d a:%d depth:%d\n", r, g, b, a, depth);
	
	SDL_GL_SetSwapInterval(1); // TODO: "-vsync"?
	
	auto req_format{STBI_rgb_alpha};
	int width, height, orig_format;
	auto pGameIcon{stbi_load(va("./%s/game.ico", com_gamedir), &width, &height, &orig_format, req_format)};
	
	if(!pGameIcon)
	{
		SDL_Log("Loading image failed: %s", stbi_failure_reason());
		exit(1);
	};
	
	// Set up the pixel format color masks for RGB(A) byte arrays.
	// Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (req_format == STBI_rgb) ? 8 : 0;
	rmask = 0xff000000 >> shift;
	gmask = 0x00ff0000 >> shift;
	bmask = 0x0000ff00 >> shift;
	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = (req_format == STBI_rgb) ? 0 : 0xff000000;
#endif

	int depth, pitch;
	if(req_format == STBI_rgb)
	{
		depth = 24;
		pitch = 3 * width; // 3 bytes per pixel * pixels per row
	}
	else // STBI_rgb_alpha (RGBA)
	{
		depth = 32;
		pitch = 4 * width;
	};
	
	auto pWinIcon{SDL_CreateRGBSurfaceFrom(static_cast<void*>(pGameIcon), width, height, depth, pitch, rmask, gmask, bmask, amask)};
	
	if(!pWinIcon)
	{
		SDL_Log("Creating surface failed: %s", SDL_GetError());
		stbi_image_free(pGameIcon);
		exit(1);
	};
	
	SDL_SetWindowIcon(mpWindow, pWinIcon);
	
	mpCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(mpCursor);
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
	return reinterpret_cast<void*>(mpWindow);
};

void *CGame::GetMainWindowAddress()
{
	return reinterpret_cast<void*>(mpWindow);
};

void CGame::GetWindowRect()
{
	// TODO
};