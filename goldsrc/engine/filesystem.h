/// @file

#pragma once

//
// file IO
//

//struct IFileSystem;
//extern IFileSystem *gpFileSystem;

// returns the file size
// return -1 if file is not present
// the file should be in BINARY mode for stupid OSs that care
int FS_FileOpenRead(const char *path, int *hndl);

int FS_FileOpenWrite (const char *path);

void FS_FileClose (int handle);

void FS_FileSeek (int handle, int position);

int FS_FileRead (int handle, void *dest, int count);
int FS_FileWrite (int handle, const void *data, int count);

int	FS_FileTime (const char *path);

void FS_mkdir (const char *path);