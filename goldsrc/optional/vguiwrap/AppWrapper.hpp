/// @file

#pragma once

// BP: this is a wrapper class for methods used by the original gs engine hw module

class CAppWrapper
{
public:
	CAppWrapper(vgui::App &aApp) : mApp(aApp){}
	
	void Reset()
	{
		mApp.reset();
	};
	
	void Init()
	{
		mApp.init();
	};
	
	void Start()
	{
		mApp.start();
	};
	
	void Stop()
	{
		mApp.stop();
	};
	
	void Run()
	{
		mApp.run();
	};
	
	void InternalTick()
	{
		mApp.internalTick();
	};
	
	void ExternalTick()
	{
		mApp.externalTick();
	};
	
	void PlatTick()
	{
		mApp.platTick();
	};
	
	void EnableBuildMode()
	{
		mApp.enableBuildMode();
	};
	
	long GetTimeMillis() const
	{
		return mApp.getTimeMillis();
	};
private:
	vgui::App &mApp;
};