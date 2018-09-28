/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "VGUI.h"

//#define CEGUI_STATIC

//#include "CEGUI/CEGUI.h"
//#include "CEGUI/RendererModules/OpenGL/GLRenderer.h"

// Hi, Windows!
#ifdef PostMessage
#undef PostMessage
#endif

namespace vgui2
{

///CEGUI::OpenGLRenderer *gpRenderer{nullptr};

bool CVGui::Init(CreateInterfaceFn *factoryList, int numFactories)
{
	// TODO
	
	// * CEGUI::OpenGLRenderer for r_gl
	// * CEGUI::OpenGL3Renderer for r_gl3

	///*gpRenderer = CEGUI::OpenGLRenderer::bootstrapSystem();
	
	///CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	//CEGUI::Window *myRoot = wmgr.createWindow("DefaultWindow", "root");
	//CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(myRoot);
	
	///CEGUI::FrameWindow *fWnd = static_cast<CEGUI::FrameWindow*>(wmgr.createWindow( "TaharezLook/FrameWindow", "testWindow" ));
	
	return true;
};

void CVGui::Shutdown()
{
	// TODO
	
	//CEGUI::System::destroy();
	//CEGUI::OpenGLRenderer::destroy(gpRenderer);
	///CEGUI::OpenGLRenderer::destroySystem();
};

void CVGui::Start()
{
	mbRunning = true;
};

void CVGui::Stop()
{
	mbRunning = false;
};

bool CVGui::IsRunning()
{
	return mbRunning;
};

void CVGui::RunFrame()
{
	///CEGUI::System::getSingleton().renderAllGUIContexts();
};

void CVGui::ShutdownMessage(unsigned int shutdownID)
{
};

VPANEL CVGui::AllocPanel()
{
	return 0;
};

void CVGui::FreePanel(VPANEL panel)
{
};

void CVGui::DPrintf(const char *format, ...)
{
};

void CVGui::DPrintf2(const char *format, ...)
{
};

void CVGui::SpewAllActivePanelNames()
{
};

HPanel CVGui::PanelToHandle(VPANEL panel)
{
	return 0;
};

VPANEL CVGui::HandleToPanel(HPanel index)
{
	return 0;
};

void CVGui::MarkPanelForDeletion(VPANEL panel)
{
};

void CVGui::AddTickSignal(VPANEL panel, int intervalMilliseconds)
{
};

void CVGui::RemoveTickSignal(VPANEL panel)
{
};

void CVGui::PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds)
{
};

HContext CVGui::CreateContext()
{
	return 0;
};

void CVGui::DestroyContext(HContext context)
{
};

void CVGui::AssociatePanelWithContext(HContext context, VPANEL pRoot)
{
};

void CVGui::ActivateContext(HContext context)
{
	//mContexts[context]->SetActive(true); // Activate()
};

void CVGui::SetSleep(bool state)
{
	mbSleeping = state;
};

bool CVGui::GetShouldVGuiControlSleep()
{
	return false;
};

void CVGui::DispatchMessages()
{
};

}; // namespace vgui2