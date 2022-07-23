/// @file

#pragma once

class CEventDispatcher
{
public:
	CEventDispatcher(vgui::App *apApp) : mpApp(apApp){}

	void internalMouseWheeled(int delta, vgui::SurfaceBase *pSurface)
	{
		mpApp->internalMouseWheeled(delta, pSurface);
	};
	
	void internalMousePressed(int delta, vgui::SurfaceBase *pSurface)
	{
		mpApp->internalMousePressed(delta, pSurface);
	};
	
	void internalMouseDoublePressed(int delta, vgui::SurfaceBase *pSurface)
	{
		mpApp->internalMouseDoublePressed(delta, pSurface);
	};
	
	void internalMouseReleased(vgui::MouseCode aMouseCode, vgui::SurfaceBase *pSurface)
	{
		mpApp->internalMouseReleased(aMouseCode, pSurface);
	};
private:
	vgui::App *mpApp{nullptr};
};