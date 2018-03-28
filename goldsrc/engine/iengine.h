/// @file

#pragma once

struct IEngine
{
	///
	virtual bool Load(bool dedicated, char *basedir, char *cmdline) = 0;
	
	///
	virtual void Unload() = 0;
	
	///
	virtual void Frame() = 0;
};