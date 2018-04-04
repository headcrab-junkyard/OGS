/// @file

#pragma once

#include <cstdio>
#include "IFile.hpp"

class CFile final : public IFile
{
public:
	CFile(const char *asName);
	~CFile();
	
	void Write(const void *data, int count) override;
	int Read(void *dest, int count) const override;

	void Printf(const char *text, ...) override;

	int Seek(int position) const override;

	int Tell() const override;

	const char *GetName() const override;
	const char *GetExt() const override;
	const char *GetPath() const override;

	int GetTime() const override;
	int GetSize() const override;
private:
	const char *msName{""};
	
	FILE *mpHandle{nullptr};
};