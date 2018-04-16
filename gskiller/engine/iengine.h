/// @file

#pragma once

struct IEngine
{
	///
	virtual bool Load(bool dedicated, const char *basedir, const char *cmdline) = 0;
	
	///
	virtual void Unload() = 0;
	
	///
	virtual void Frame() = 0;
};

extern IEngine *gpEngine;