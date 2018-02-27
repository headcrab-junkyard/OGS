/// @file

#pragma once

struct IGame
{
	///
	virtual bool Init() = 0;
	
	///
	virtual void Shutdown() = 0;
	
	///
	virtual void Frame(double frametime) = 0;
};