/// @file

#pragma once

#include "SurfaceBase.h"

class CSurfaceBaseImpl : public vgui::SurfaceBase
{
public:
	void setTitle(const char *asTitle) override;
	bool setFullscreenMode(int anWide, int anTall, int bpp) override;
	void setWindowedMode() override;
	void setAsTopMost(bool abState) override;
	bool hasFocus() override;
	bool isWithin(int x,int y) override;
	void GetMousePos(int &x, int &y) override;
};