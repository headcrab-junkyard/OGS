/// @file

#pragma once

#include "IDedicatedExports.h"

class CDedicatedExports final : public IDedicatedExports
{
public:
	CDedicatedExports();
	~CDedicatedExports();
	
	void Sys_Printf(char *text) override;
};