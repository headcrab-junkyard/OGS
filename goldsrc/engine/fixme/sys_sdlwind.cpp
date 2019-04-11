/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018-2019 BlackPhrase
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

#include <SDL.h>
#include <SDL_opengl.h>

#include "quakedef.h"
#include "igame.h"

class CGame final : public IGame
{
public:
	CGame();
	~CGame();

	void CreateGameWindow() override;

	void PlayStartupVideos() override;
	void PlayVideoAndWait(const char *name, bool) override;

	void SleepUntilInput(int time) override;
private:
	SDL_GLContext mGLContext;
};

CGame::CGame() = default;

CGame::~CGame() //= default;
{
	SDL_GL_DeleteContext(mGLContext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();
};

void CGame::CreateGameWindow()
{
	// Init SDL2
	if(SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
			Sys_Error("Couldn't create SDL2 window! (%s)", SDL_GetError());
	
	// Setup window attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	SDL_DisplayMode CurrentDisplayMode;
	SDL_GetCurrentDisplayMode(0, &CurrentDisplayMode);
	
	// TODO: fullscreen support
	
	SDL_WindowFlags WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	mainwindow = SDL_CreateWindow("OGS Engine Tech Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 600, WindowFlags);
	
	mGLContext = SDL_GL_CreateContext(mainwindow);
	
	SDL_GL_SetSwapInterval(1);
	
	// TODO: renderer
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