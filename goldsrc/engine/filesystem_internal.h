/// @file
/// @brief internal filesystem module wrapper

//
// file IO
//

#pragma once

#define FILESYSTEM_INTERNAL_H

#define FILESYSTEM_INVALID_HANDLE (FileHandle_t)0

typedef enum
{
	FILESYSTEM_SEEK_HEAD = 0,
	FILESYSTEM_SEEK_CURRENT,
	FILESYSTEM_SEEK_TAIL
	//...?
} FileSystemSeek_t;

enum
{
	FILESYSTEM_INVALID_FIND_HANDLE = -1
};

typedef enum
{
	FILESYSTEM_WARNING_QUIET = 0,      ///< Don't print anything
	FILESYSTEM_WARNING_REPORTUNCLOSED, ///< On shutdown, report names of files left unclosed
	FILESYSTEM_WARNING_REPORTUSAGE,    ///< Report number of times a file was opened/closed
	FILESYSTEM_WARNING_REPORTALLACCESS ///< Report all open/close events to console (SLOW!!!!)
} FileWarningLevel_t;


////////////
// TODO: here or somewhere else?????

void FileSystem_Init(char *basedir, void *filesystemFactory); // TODO: return bool/int?
void FileSystem_Shutdown();
////////////

/// returns the file size
/// return -1 if file is not present
/// the file should be in BINARY mode for stupid OSs that care
int FS_FileOpenRead(const char *path, int *hndl);

int FS_FileOpenWrite(const char *path);

void FS_FileClose(int handle);

void FS_FileSeek(int handle, int position);

int FS_FileRead(int handle, void *dest, int count);
int FS_FileWrite(int handle, const void *data, int count);

int	FS_FileTime(const char *path);

void FS_mkdir(const char *path);

// Dynamic Library Management
	
//void *FS_LoadLibrary(const char *name, const char *path);