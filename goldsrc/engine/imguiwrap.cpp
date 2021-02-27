/*
 * This file is part of OGS Engine
 * Copyright (C) 2018-2020 BlackPhrase
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
#include "imguiwrap.h"
#include "imgui.h"
#include "imgui/imgui_impl_qgl.h"

#ifdef OGS_USE_SDL
#	include "imgui_impl_sdl.h"
#else
#	ifdef _WIN32
#		include "imgui_impl_win32.h"
#	endif
#endif

extern HWND mainwindow;

qboolean ImGui_Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	auto io{ImGui::GetIO()};
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	
	// Setup Platform/Renderer bindings
#ifdef OGS_USE_SDL
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context); // TODO
#else

#ifdef _WIN32
	ImGui_ImplWin32_Init(mainwindow);
#endif

#endif // OGS_USE_SDL

	ImGui_ImplQGL_Init();
	return true;
};

void ImGui_Shutdown()
{
	// Cleanup
	ImGui_ImplQGL_Shutdown();
	
#ifdef OGS_USE_SDL
	ImGui_ImplSDL2_Shutdown();
#else

#ifdef _WIN32
	ImGui_ImplWin32_Shutdown();
#endif

#endif // OGS_USE_SDL

	ImGui::DestroyContext();
};

void ImGui_Frame()
{
	// Start the Dear ImGui frame
	ImGui_ImplQGL_NewFrame();

#ifdef OGS_USE_SDL
	ImGui_ImplSDL2_NewFrame(window);
#else

#ifdef _WIN32
	ImGui_ImplWin32_NewFrame();
#endif

#endif // OGS_USE_SDL

	ImGui::NewFrame();
	
	bool show_demo_window = true;
	if(show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	
	// Rendering
	ImGui::EndFrame();
	ImGui::Render();
	//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplQGL_RenderDrawData(ImGui::GetDrawData());
	//SDL_GL_SwapWindow(window);
};