/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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

#include "vguiwrap.hpp"
#include "App.hpp"
#include "SurfaceBaseImpl.hpp"
#include "IEngineSurface.h"

EXPOSE_SINGLE_INTERFACE(CVGuiWrap, IVGuiWrap, OGS_VGUIWRAP_INTERFACE_VERSION);

CVGuiWrap::CVGuiWrap() = default;
CVGuiWrap::~CVGuiWrap() = default;

/*bool*/ void CVGuiWrap::Startup(CreateInterfaceFn afnEngineFactory)
{
	// BP: We need this in order to be independent from the GAPI
	auto pEngineSurface{(IEngineSurface*)afnEngineFactory(ENGINE_SURFACE_VERSION, nullptr)};
	
	if(!pEngineSurface)
		return;
	
	static CApp App;
	
	vgui::App::getInstance()->start();
	
	mpRootPanel = std::make_unique<vgui::Panel>();
	
	vgui::App::getInstance()->setMouseArea(mpRootPanel.get());
	
	vgui::App::getInstance()->run();
	//return true;
};

void CVGuiWrap::Shutdown()
{
	vgui::App::getInstance()->stop();
};

/*
void CVGuiWrap::CallSurfaceProc()
{
};
*/

void CVGuiWrap::KeyEvent(KeyCode aeCode)
{
	switch()
	{
	case A:
		vgui::App::getInstance()->internalKeyPressed(aeCode, SurfaceBase* surfaceBase);
		break;
	case B:
		vgui::App::getInstance()->internalKeyTyped(aeCode, SurfaceBase* surfaceBase);
		break;
	case C:
		vgui::App::getInstance()->internalKeyReleased(aeCode, SurfaceBase* surfaceBase);
		break;
	};
};

void CVGuiWrap::MouseEvent(MouseCode aeCode)
{
	switch()
	{
	case A:
		vgui::App::getInstance()->internalMousePressed(aeCode, SurfaceBase* surfaceBase);
		break;
	case B:
		vgui::App::getInstance()->internalMouseDoublePressed(aeCode, SurfaceBase* surfaceBase);
		break;
	case C:
		vgui::App::getInstance()->internalMouseReleased(aeCode, SurfaceBase* surfaceBase);
		break;
	};
	//vgui::App::getInstance()->internalMouseWheeled(int delta, SurfaceBase* surfaceBase); // TODO
};

void CVGuiWrap::MouseMove(int anPosX, int anPosY)
{
	vgui::App::getInstance()->internalCursorMoved(anPosX, anPosY, SurfaceBase *surfaceBase);
};

void *CVGuiWrap::GetPanel() const
{
	return (void*)mpRootPanel.get();
};