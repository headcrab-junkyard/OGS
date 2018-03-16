/// @file

#pragma once

#include "quakedef.h"
#include "iengine.h"

class CEngine final : public IEngine
{
public:
	CEngine();
	~CEngine();
	
	bool Init() override;
	void Shutdown() override;
	
	void Frame() override;
private:
	quakeparms_t mParms{};
};