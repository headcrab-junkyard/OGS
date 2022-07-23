/// @file

#pragma once

#include <cstring>
#include "VGUI_App.h"

class CApp final : public vgui::App
{
public:
	CApp() : vgui::App(true){}
	
	void main(int argc, char *argv[]) override {}
};