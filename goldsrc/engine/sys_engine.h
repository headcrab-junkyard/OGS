/// @file

#pragma once

#include "quakedef.h"
#include "iengine.h"

class CEngine final : public IEngine
{
public:
	CEngine();
	~CEngine();
	
	bool Load(bool dedicated, char *basedir, char *cmdline) override;
	void Unload() override;
	
	void Frame() override;
private:
	quakeparms_t mParms{};
};