/// @file

#pragma once

#include "quakedef.h" // TODO: host
#include "iengine.h"

class CEngine final : public IEngine
{
public:
	CEngine();
	~CEngine();
	
	bool Load(bool dedicated, const char *basedir, const char *cmdline) override;
	void Unload() override;
	
	void Frame() override;
private:
	quakeparms_t mParms{};
};