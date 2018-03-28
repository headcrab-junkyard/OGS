/// @file

#include <cstring>
#include "FileSystem.hpp"

// TODO: temp
void Sys_Error(const char *msg, ...)
{
};

EXPOSE_SINGLE_INTERFACE(CFileSystem, IFileSystem, OGS_FILESYSTEM_INTERFACE_VERSION);

CFileSystem::CFileSystem() = default;
CFileSystem::~CFileSystem() = default;

void CFileSystem::Mount()
{
};

void CFileSystem::Unmount()
{
};

void CFileSystem::AddSearchPath(const char *path, const char *alias)
{
};

IFile *CFileSystem::OpenFile(const char *asName)
{
	//IFile *pFile = new CFile(asName);
	//mlstOpenHandles.push_back(pFile);
	return nullptr;
};

void CFileSystem::CloseFile(IFile *apFile)
{
	if(!apFile)
		return;
	
	//fclose(apFile);
};

int CFileSystem::FileOpen(const char *path, const char *mode)
{
	// FS_FileOpenRead
	/*
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
	
	*/
	//
	
	FILE *f{fopen(path, "wb")};
	
	if(!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	
	int i{findhandle()};
	
	sys_handles[i] = f;
	
	return i;
};

void CFileSystem::FileClose(int handle)
{
	if(!handle)
		return;

#ifdef _WIN32
	fclose(sys_handles[handle]);
	sys_handles[handle] = nullptr;
#else
	close(handle);
#endif
};

void CFileSystem::FileSeek(int handle, int position) // TODO: seek mode
{
#ifdef _WIN32
	fseek(sys_handles[handle], position, SEEK_SET);
#else
	lseek(handle, position, SEEK_SET);
#endif
};

int CFileSystem::FileRead(int handle, void *dest, int count)
{
#ifdef _WIN32
	return fread(dest, 1, count, sys_handles[handle]);
#else
	return read(handle, dest, count);
#endif
};

int CFileSystem::FileWrite(int handle, const void *data, int count)
{
#ifdef _WIN32
	return fwrite(data, 1, count, sys_handles[handle]);
#else
	return write(handle, src, count);
#endif
};

int CFileSystem::GetFileTime(const char *path) const
{
	FILE *f{fopen(path, "rb")};
	
	if(f)
	{
		fclose(f);
		return 1;
	};
	
	return -1;
};

int CFileSystem::findhandle()
{
	for(int i = 1; i < MAX_HANDLES; i++)
		if(!sys_handles[i])
			return i;
	
	Sys_Error("out of handles");
	return -1;
};

/*
================
filelength
================
*/
int CFileSystem::filelength(FILE *f)
{
	int pos = ftell(f);
	fseek(f, 0, SEEK_END);
	
	int end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
};