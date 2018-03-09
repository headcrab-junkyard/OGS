/// @file

#pragma once

#include "quakedef.h"
#include "IEngine.hpp"

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