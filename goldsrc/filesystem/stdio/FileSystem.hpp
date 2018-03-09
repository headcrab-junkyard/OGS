/// @file

#pragma once

#include "IFileSystem.hpp"

class CFileSystem final : public IFileSystem
{
public:
	CFileSystem();
	~CFileSystem();
	
	void Mount() override;
	void Unmount() override;
};