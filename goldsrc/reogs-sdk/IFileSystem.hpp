/// @file

#pragma once

struct IFileSystem
{
	///
	virtual void Mount() = 0;
	
	///
	virtual void Unmount() = 0;
};