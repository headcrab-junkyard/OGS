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

#pragma once

#include <memory>
#include "ivguiwrap.hpp"

using vgui::Panel;

class CSurfaceBaseImpl;

class CVGuiWrap final : public IVGuiWrap
{
public:
	CVGuiWrap();
	~CVGuiWrap();
	
	/*bool*/ void Startup() override;
	void Shutdown() override;
	
	//void CallSurfaceProc() override;
	
	void KeyEvent() override;
	void MouseEvent() override;
	void MouseMove(int anPosX, int anPosY) override;
	
	void *GetPanel() const override;
private:
	std::unique_ptr<CSurfaceBaseImpl> mpSurface;
	std::unique_ptr<vgui::Panel> mpRootPanel;
};