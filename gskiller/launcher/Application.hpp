/// @file

#pragma once

#include <cstdlib>
#include <cstring>

#include "interface.h"
#include "engine_launcher_api.h"

class CApplication
{
public:
	CApplication();
	~CApplication();
	
	int Run();
private:
	bool Init();
	void Shutdown();
	
	bool LoadFileSystemModule(const char *name);
	
	void *mpFSLib{nullptr};
	void *mpEngineLib{nullptr};
	
	CreateInterfaceFn mfnFSFactory{nullptr};
	
	IEngineAPI *mpEngine{nullptr};
	
	bool mbRestart{false};
protected:
	virtual bool PostInit(){return true;}
};