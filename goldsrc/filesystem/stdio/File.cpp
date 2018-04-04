/// @file

#include "File.hpp"

CFile::CFile(const char *asName) : msName(asName), mpHandle(fopen(asName))
{
	assert(mpHandle);
	
	//if(!mpHandle)
		//std::runtime_error();
};

CFile::~CFile()
{
	if(mpHandle)
	{
		fflush(mpHandle);
		fclose(mpHandle);
	};
};

void CFile::Write(const void *data, int count)
{
	if(mpHandle)
		fwrite(data, sizeof(char), count, mpHandle);
};

int CFile::Read(void *dest, int count) const
{
	if(mpHandle)
		return fread(dest, sizeof(char), count, mpHandle);

	return 0;
};

void CFile::Printf(const char *text, ...)
{
	if(mpHandle)
		fputs(text, mpHandle);
};

int CFile::Seek(int position) const
{
	if(mpHandle)
		return fseek(mpHandle, position, SEEK_SET);

	return 0;
};

int CFile::Tell() const
{
	if(mpHandle)
		return ftell(mpHandle);

	return 0;
};

const char *CFile::GetName() const
{
	if(mpHandle)
		return msName;

	return "";
};

const char *CFile::GetExt() const
{
	return ""; // TODO
};

const char *CFile::GetPath() const
{
	return ""; // TODO
};

int CFile::GetTime() const
{
	return 0; // TODO
};

int CFile::GetSize() const
{
	return 0; // TODO
};