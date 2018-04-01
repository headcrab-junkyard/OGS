/// @file

#pragma once

#include <cstdio>
#include "IFileSystem.hpp"

class CFileSystem final : public IFileSystem
{
public:
	CFileSystem();
	~CFileSystem();
	
	void Mount() override;
	void Unmount() override;
	
	void AddSearchPath(const char *path, const char *alias) override;
	
	IFile *OpenFile(const char *asName) override;
	void CloseFile(IFile *apFile) override;
	
	int FileOpen(const char *path, const char *mode) override;
	void FileClose(int handle) override;
	
	void FileSeek(int handle, int position) override;
	
	int FileRead(int handle, void *dest, int count) override;
	int FileWrite(int handle, const void *data, int count) override;
	
	int GetFileTime(const char *path) const override;
private:
	static constexpr auto MAX_HANDLES{10};
	
	int findhandle();
	int filelength(FILE *f);

	// TODO: if WIN only?
	FILE *sys_handles[MAX_HANDLES]{nullptr};
};